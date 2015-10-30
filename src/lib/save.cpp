#include <Kinect/save.hpp>

/* Load input */
INPUT *createInput(const char *device, u_int32_t mode, const char *filename) {
	INPUT *r = new INPUT;

	/* Open device */
	r->fd = open(device, O_RDONLY);

	/* Output info */
	r->mode = mode;
	r->filename = strdup(filename);

	r->f = fopen(filename, "w");

	return r;
}

/* Release input */
void releaseInput(INPUT *i) {
	fclose(i->f);

	free(i->filename);

	close(i->fd);
	delete i;
}

/* Dump input */
void *data(void *arg) {
	INPUT *input = (INPUT *) arg;
	struct input_event ev;
	Display *dpy;
	Window root, win;
	int coord[2], winX, winY;
	unsigned int mask;

	fwrite((const void *) &input->mode, sizeof(u_int32_t), 1, input->f);
	dpy = XOpenDisplay(NULL);

	/* Capture loop */
	for(;flag;) {
		read(input->fd, &ev, sizeof(struct input_event));

		/* Filter keyboard and mouse events */
		if(ev.type == 1 && (ev.value == 0 || ev.value == 1)) {
			fwrite((const void *) &ev, sizeof(struct input_event), 1, input->f);//std::cout<<"Teclado!! \n";
			if(ev.code == KEY_F12) {
				flag = 0;
				break;
			}
		}
		else if(ev.type == 2) {
			if(ev.code == 0 || ev.code == 1) {
				XQueryPointer(dpy, DefaultRootWindow(dpy), &root, &win, &coord[0], &coord[1], &winX, &winY, &mask);
				fwrite((const void *) &ev, sizeof(struct input_event), 1, input->f);//std::cout<<"Mouse :B \n";
				fwrite((const void *) coord, sizeof(int), 2, input->f);
			}
			if(ev.code == 8) {
				fwrite((const void *) &ev, sizeof(struct input_event), 1, input->f);
			}
		}
	}
}

/* Load camera and stuff */
CAMERA *createCam(int width, int height, int channels, u_int32_t compression, u_int32_t mode, const char *filename) {
	CAMERA *r = new CAMERA;

	/* Stuff */
	r->vSize = height*width*channels;
	r->compression = compression;
	r->mode = mode;
	r->filename = strdup(filename);

	return r;
}

/* Release camera and stuff */
void releaseCam(CAMERA *c) {
	/* Stuff */
	free(c->filename);

	/* Close camera */
	close(c->fd);
	delete c;
}

/* Disk writing thread */
void *write(void *arg) {
	CAMERA *camera = (CAMERA *) arg;
	int count = 0;

	for(;camera->vThreadFlag;) {
		if(camera->vFlag[count] == 2) {
			fwrite((const void *) &camera->vTime[count], sizeof(struct timeval), 1, camera->f);
			fwrite((const void *) &camera->vZipSize[count], sizeof(unsigned long int), 1, camera->f); 
			fwrite((const void *) camera->vZipBuf[count], sizeof(char), camera->vZipSize[count], camera->f);

			camera->vFlag[count] = 0;
			count = (count+1)%NBUF;
		}
	}
}

/* Compression thread */
void *compress(void *arg) {
	CAMERA *camera = (CAMERA *) arg;
	int count = 0;

	for(;camera->vThreadFlag;) {
		if(camera->vFlag[count] == 1) {
			if(camera->compression == ZLIB) {
				camera->vZipSize[count] = camera->vSize;
				compress2((Bytef *) camera->vZipBuf[count], &camera->vZipSize[count], (const Bytef *) camera->vBuf[count], camera->vSize, 1);
			}
			else {
				camera->vZipSize[count] = lzf_compress((const void *) camera->vBuf[count], camera->vSize, (void *) camera->vZipBuf[count], camera->vSize);
			}

			camera->vFlag[count] = 2;
			count = (count+1)%NBUF;
		}
	}
}

/* Video thread */
void *video(void *arg) {

	MULT_MODAL *mModal = (MULT_MODAL *) arg;

	CAMERA *camColor, *camDepth, *camIr;
	camColor = mModal->color;
	camDepth = mModal->depth;
	camIr = mModal->ir;

	int color_width,color_height, depth_width, depth_height;
	color_width = COLOR_WIDTH;
	color_height = COLOR_HEIGHT;
	depth_width = DEPTH_WIDTH;
	depth_height = DEPTH_HEIGHT;


	struct timeval t1, t2;
	pthread_t zipColor, zipDepth, zipIr, saveColor, saveDepth, saveIr;
	int count = 0;

	/* Alloc buffers */
	for(int i=0; i < NBUF; i++) {
		camColor->vBuf[i] = new unsigned char[camColor->vSize];
		camColor->vZipBuf[i] = new unsigned char[camColor->vSize];
		camColor->vFlag[i] = 0;

		camDepth->vBuf[i] = new unsigned char[camDepth->vSize];
		camDepth->vZipBuf[i] = new unsigned char[camDepth->vSize];
		camDepth->vFlag[i] = 0;
	
		camIr->vBuf[i] = new unsigned char[camIr->vSize];
		camIr->vZipBuf[i] = new unsigned char[camIr->vSize];
		camIr->vFlag[i] = 0;
	}

	/* Start zip thread */
	camColor->vThreadFlag = 1;
	camDepth->vThreadFlag = 1;
	camIr->vThreadFlag = 1;

	pthread_create(&zipColor, NULL, &compress, (void *)camColor);
	pthread_create(&zipDepth, NULL, &compress, (void *)camDepth);
	pthread_create(&zipIr, NULL, &compress, (void *)camIr);

	/* Open output COLOR file & start writing thread */
	camColor->f = fopen(camColor->filename, "w");
	fwrite((const void *) &camColor->mode, sizeof(u_int32_t), 1, camColor->f);
	fwrite((const void *) &camColor->compression, sizeof(u_int32_t), 1, camColor->f);
	fwrite((const void *) &color_width, sizeof(int), 1, camColor->f);
	fwrite((const void *) &color_height, sizeof(int), 1, camColor->f);
	pthread_create(&saveColor, NULL, &write, (void *)camColor);


	/* Open output DEPTH file & start writing thread */
	camDepth->f = fopen(camDepth->filename, "w");
	fwrite((const void *) &camDepth->mode, sizeof(u_int32_t), 1, camDepth->f);
	fwrite((const void *) &camDepth->compression, sizeof(u_int32_t), 1, camDepth->f);
	fwrite((const void *) &depth_width, sizeof(int), 1, camDepth->f);
	fwrite((const void *) &depth_height, sizeof(int), 1, camDepth->f);
	pthread_create(&saveDepth, NULL, &write, (void *)camDepth);

	/* Open output IR file & start writing thread */
	camIr->f = fopen(camIr->filename, "w");
	fwrite((const void *) &camIr->mode, sizeof(u_int32_t), 1, camIr->f);
	fwrite((const void *) &camIr->compression, sizeof(u_int32_t), 1, camIr->f);
	fwrite((const void *) &depth_width, sizeof(int), 1, camIr->f);
	fwrite((const void *) &depth_height, sizeof(int), 1, camIr->f);
	pthread_create(&saveIr, NULL, &write, (void *)camIr);

	/* Video loop */
	gettimeofday(&t1, NULL);

	int i=0;
	for(;flag;) {
		listener.waitForNewFrame(frames);
		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
		cv::Mat originalColor;

		cv::resize(cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data) , originalColor, cv::Size(COLOR_WIDTH,COLOR_HEIGHT) );
//	    cv::imshow("originalColor", originalColor);
	    cv::imshow("Depth", cv::Mat(depth->height, depth->width, CV_32FC1, depth->data)/ 4000.0f);
		cv::waitKey(1); 

		while((camColor->vFlag[count] != 0) || (camDepth->vFlag[count] != 0) || (camIr->vFlag[count] != 0) )std::cout<<"\t";
			//printf("\t%d Color, %d Depth, %d IR\n",camColor->vFlag[count], camDepth->vFlag[count], camIr->vFlag[count]);

		memcpy((void *) camColor->vBuf[count], originalColor.data, camColor->vSize);
		gettimeofday(&camColor->vTime[count], NULL);
		camColor->vFlag[count] = 1;

		memcpy((void *) camDepth->vBuf[count], depth->data, camDepth->vSize);
		gettimeofday(&camDepth->vTime[count], NULL);
		camDepth->vFlag[count] = 1;

		memcpy((void *) camIr->vBuf[count], ir->data, camIr->vSize);
		gettimeofday(&camIr->vTime[count], NULL);
		camIr->vFlag[count] = 1;

		count=(count+1)%NBUF;

		i++;
		listener.release(frames);
	}

	gettimeofday(&t2, NULL);

	cout << camDepth->filename << " " << i/(t2.tv_sec + t2.tv_usec/1000000.0 - t1.tv_sec - t1.tv_usec/1000000.0) << endl;

	/* Release buffers */
	for(int i=0; i < NBUF; i++) {
		while(camDepth->vFlag[i] != 0)std::cout<<"\t";
		delete[] camDepth->vBuf[i];
		delete[] camDepth->vZipBuf[i];

		while(camIr->vFlag[i] != 0)std::cout<<"\t";
		delete[] camIr->vBuf[i];
		delete[] camIr->vZipBuf[i];

		while(camColor->vFlag[i] != 0)std::cout<<"\t";
		delete[] camColor->vBuf[i];
		delete[] camColor->vZipBuf[i];
	}

	/* Stop zip threads */
	camColor->vThreadFlag = 0;
	camDepth->vThreadFlag = 0;
	camIr->vThreadFlag = 0;
	/* Close file */
	fclose(camColor->f);
	fclose(camDepth->f);
	fclose(camIr->f);
}

/* Magic function */
int main(int argc, char **argv) {

  std::string program_path(argv[0]);
  size_t executable_name_idx = program_path.rfind("Protonect");

  std::string binpath = "/";

  if(executable_name_idx != std::string::npos)
  {
    binpath = program_path.substr(0, executable_name_idx);
  }

  libfreenect2::Freenect2 freenect2;

  libfreenect2::Freenect2Device *dev = 0;

  if(freenect2.enumerateDevices() == 0)
  {
    std::cout << "no device connected!" << std::endl;
    return -1;
  }

  std::string serial = freenect2.getDefaultDeviceSerialNumber();

  dev = freenect2.openDevice(serial);

	if(dev == 0)
	{
		std::cout << "failure opening device!" << std::endl;
		return -1;
	}

	dev->setColorFrameListener(&listener);
	dev->setIrAndDepthFrameListener(&listener);
	dev->start();

	std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
	std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
//////////////////////
	CAMERA *depthCamera, *colorCamera, *irCamera;
	MULT_MODAL mModal;
	INPUT *keyboardInput, *mouseInput;
	pthread_t depth, color, ir, keyboard, mouse;

	/* 1920 1080 */ 
	colorCamera = createCam(COLOR_WIDTH, COLOR_HEIGHT, 4, LIBLZF, VIDEO_RGB_24B, "color.log");
	/* 960 540 */
	depthCamera = createCam( DEPTH_WIDTH, DEPTH_HEIGHT, 4, LIBLZF,VIDEO_DIR_24B, "depth.log");
	irCamera = createCam( DEPTH_WIDTH, DEPTH_HEIGHT, 5, LIBLZF,VIDEO_IR_24B, "ir.log");
	keyboardInput = createInput("/dev/input/event2", DATA_KEYBOARD, "keyboard.log");
	mouseInput = createInput("/dev/input/event4", DATA_MOUSE, "mouse.log");

	mModal.color = colorCamera;
	mModal.depth = depthCamera;
	mModal.ir = irCamera;
	pthread_create(&depth, NULL, &video, (void *) &mModal);
//	pthread_create(&color, NULL, &video, (void *) colorCamera);
	pthread_create(&keyboard, NULL, &data, (void *) keyboardInput);
	pthread_create(&mouse, NULL, &data, (void *) mouseInput);

	pthread_join(keyboard, NULL);
	pthread_cancel(mouse);
	pthread_join(depth, NULL);
//	pthread_join(color, NULL);

	dev->stop();
	dev->close();

//	releaseCam(colorCamera);
	releaseCam(depthCamera);
	releaseInput(keyboardInput);
	releaseInput(mouseInput);

}

