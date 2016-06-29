#include <Kinect/load.hpp>
#include "liblzf/lzf_d.c"

using namespace KINECT;

typedef load::__data DATA;
typedef load::__stream STREAM;

load::load()
{
	// this->dir = this->createStream("depth.log");
	// this->ir = this->createStream("ir.log");
	// this->color = this->createStream((char*)"color.log");
	// this->keyboard = this->createData("keyboard.log");
	// this->mouse = this->createData("mouse.log");
	// this->flag_dir = this->streamNext(dir);
	// this->flag_ir = this->streamNext(ir);
	// this->flag_color = this->streamNext(this->color);
	// this->flag_keyboard = this->dataNext(this->keyboard);
	// this->flag_mouse = this->dataNext(mouse);
}

load::~load()
{
	// this->releaseStream(ir);
	// this->releaseStream(dir);
	this->releaseStream(color);
}

/* Create data stream */
DATA * load::createData(char *filename) {
	DATA *r;

	r = new DATA;
	r->f = fopen(filename, "r");

	fread(&r->mode, sizeof(u_int32_t), 1, r->f);

	return r;
}

/* Release data stream */
void load::releaseData(DATA *d) {
	fclose(d->f);
	delete d;
}

/* Next data event */
bool load::dataNext(DATA *d) {
	if(fread((void *) &d->ev, sizeof(struct input_event), 1, d->f) != 1)
		return false;
	if(d->ev.type == 2 && (d->ev.code == 0 || d->ev.code == 1))
		fread((void *) d->coord, sizeof(int), 2, d->f);
}

/* Create video stream */
STREAM * load::createStream(char *filename) {
	STREAM *r;

	r = new STREAM;
	r->f = fopen(filename, "r");

	if(r->f == NULL){
		std::cout << "Error loading kinect log." << std::endl;
		exit(-1);
	}

	fread(&r->mode, sizeof(u_int32_t), 1, r->f);
	switch(r->mode) {
		case VIDEO_RGB_24B:
		case VIDEO_YUYV_16B:
		case VIDEO_DEPTH_16B:
		case VIDEO_IR_24B:
		case VIDEO_DIR_24B:
			fread(&r->compression, sizeof(u_int32_t), 1, r->f);
			fread(&r->width, sizeof(int), 1, r->f);
			fread(&r->height, sizeof(int), 1, r->f);
			switch(r->mode) {
				case VIDEO_YUYV_16B:
					r->size = r->width*r->height*2;
					r->tmp.create(r->height, r->width, CV_8UC2);
					r->color.create(r->height, r->width, CV_8UC3);
					break;
				case VIDEO_DEPTH_16B:
					r->size = r->width*r->height*2;
					r->depth.create(r->height, r->width, CV_16U);
					break;
				case VIDEO_RGB_24B:
					r->size = r->width*r->height*4;
					r->color.create(r->height, r->width, CV_8UC4);
					break;
				case VIDEO_DIR_24B:
					r->size = r->width*r->height*8;
					r->depth.create(r->height, r->width, CV_32FC1);
//					r->infrared.create(r->height, r->width, CV_8U);
					break;
				case VIDEO_IR_24B:
					r->size = r->width*r->height*8;
					r->infrared.create(r->height, r->width, CV_32FC1);
					break;
			}
			r->buf1 = new unsigned char[r->size];
			r->buf2 = new unsigned char[r->size];
			break;
	}

	return r;
}

/* Release video stream */
void load::releaseStream(STREAM *s) {
	switch(s->mode) {
		case VIDEO_RGB_24B:
			s->color.release();
			break;
		case VIDEO_YUYV_16B:
			s->tmp.release();
			s->color.release();
			break;
		case VIDEO_DEPTH_16B:
			s->depth.release();
			break;
		case VIDEO_DIR_24B:
			s->depth.release();
//			s->infrared.release();
			break;
		case VIDEO_IR_24B:
			s->infrared.release();
			break;
	}
	fclose(s->f);
	delete s;
}

/* Next video frame */
bool load::streamNext(STREAM *s) {

	/* Read timestamp */
	if(fread(&s->t, sizeof(struct timeval), 1, s->f) != 1)
		return false;
	/* Read and decompress next frame */
	switch(s->compression) {
		case NONE:
			if(fread(s->buf1, sizeof(char), s->size, s->f) != s->size)
				return false;
			break;
		case ZLIB:
			/* NOT IMPLEMENTED YET */
			break;
		case LIBLZF:
			unsigned long int csize, nsize;
			if(fread(&csize, sizeof(unsigned long int), 1, s->f) != 1)
				return false;
//std::cout<<fread(s->buf2, sizeof(char), csize, s->f)<<" <- NUMBER -> "<<csize<<endl;
			if(fread(s->buf2, sizeof(char), csize, s->f) != csize){
				return false;}

			switch(s->compression) {
				case ZLIB:
					break;
				case LIBLZF:
					nsize = lzf_decompress(s->buf2, csize, s->buf1, s->size);
					break;
			}
	}


	/* Convert frame to OpenCV */
	switch(s->mode) {
		case VIDEO_RGB_24B:
			s->color = cv::Mat(s->height, s->width, CV_8UC4, s->buf1);
			break;
		case VIDEO_YUYV_16B:
			for(int j = 0 ; j < s->height ; j++){
				int step16 = s->width*2*j;
				for(int i = 0 ; i < s->width ; i++){
					int pixel16 = step16 + 2*i;
					s->tmp.at<u_int16_t>(j,i) = *(u_int16_t *)(s->buf1 + pixel16);
				}
			}
			cvtColor(s->tmp,s->color,COLOR_YUV2BGR_YUYV);

			break;
		case VIDEO_DEPTH_16B:
			/* NOT IMPLEMENTED YET */
			break;
		case VIDEO_DIR_24B:
			s->depth = cv::Mat(s->height, s->width, CV_32FC1, s->buf1)/ 4500.0f;
			break;
		case VIDEO_IR_24B:
			s->infrared = cv::Mat(s->height, s->width, CV_32FC1, s->buf1) / 20000.0f;
			break;
	}

	return true;
}

Mat load::getFrame() {

	struct timeval now;
	gettimeofday(&now, NULL);
	Mat frame;

	if(this->flag_color || this->flag_keyboard) {
		/* Who comes first? */
		// struct timeval t[4];
		// t[0] = (this->flag_dir ? dir->t : now);
		// t[1] = (this->flag_color ? color->t : now);
		// t[2] = (this->flag_ir ? ir->t : now);
		// t[3] = (this->flag_keyboard ? keyboard->ev.time : now);
		// t[4] = (this->flag_mouse ? mouse->ev.time : now);
		// int min=0;
		// for(int i=1; i < 4; i++)
		// 	if(t[i].tv_sec < t[min].tv_sec || (t[i].tv_sec == t[min].tv_sec && t[i].tv_usec < t[min].tv_usec))
		// 		min = i;

		frame = this->color->color;
		this->flag_color = this->streamNext(this->color);
		return frame;
	}
	return Mat();
}
