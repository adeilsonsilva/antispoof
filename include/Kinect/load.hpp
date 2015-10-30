/* Main class to handle Kinect stuff:
 * 		Load video streams;
 *		Read images;
 */

#ifndef LOAD_HPP
#define LOAD_HPP

#include <iostream>
#include <stdio.h>
#include <zlib.h>
#include "liblzf/lzf.h"
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <linux/input.h>

using namespace cv;
using namespace std;

enum CompressionFormat { NONE = 0x00000000, ZLIB = 0x00000001, LIBLZF = 0x00000002 };
enum Mode { VIDEO_RGB_24B = 0x00000000, VIDEO_YUYV_16B = 0x00000001, VIDEO_IR_24B = 0x00000002, VIDEO_DEPTH_16B = 0x00000003, VIDEO_DIR_24B = 0x00000004, DATA_MOUSE = 0x00000005, DATA_KEYBOARD = 0x00000006};

typedef struct __stream {
	FILE *f;
	u_int32_t mode, compression;
	int width, height, size;
	Mat color, depth, infrared, tmp;
	unsigned char *buf1, *buf2;
	struct timeval t;
} STREAM;

typedef struct __data {
	FILE *f;
	u_int32_t mode;
	struct input_event ev;
	int coord[2];
} DATA;

namespace KINECT
{
	class load
	{
		public:
			load();
			~load();
			STREAM *dir;
			STREAM *ir;
			STREAM *color;
			DATA *keyboard;
			DATA *mouse;
			bool flag_dir, flag_ir, flag_color, flag_keyboard, flag_mouse;
			DATA * createData(char *filename);
			void releaseData(DATA *d);
			bool dataNext(DATA *d);
			STREAM *createStream(char *filename);
			void releaseStream(STREAM *s);
			bool streamNext(STREAM *s);
			Mat getFrame();
	};

}

#endif