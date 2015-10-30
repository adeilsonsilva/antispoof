#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <errno.h>
#include <zlib.h>
#include "liblzf/lzf_c.c"
#include <X11/Xlib.h>
#include <linux/input.h>

#define COLOR_WIDTH 768
#define COLOR_HEIGHT 432
#define DEPTH_HEIGHT 424
#define DEPTH_WIDTH 512

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/threading.h>
#include <libfreenect2/packet_pipeline.h>
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;

/* Camera stuff */
#define NBUF 4
enum PixelFormat { YUYV = 0x56595559, INVZ = 0x5a564e49, INZI = 0x495a4e49, INVR = 0x52564e49, INRI = 0x49524e49, INVI = 0x49564e49, RELI = 0x494c4552 };
enum CompressionFormat { NONE = 0x00000000, ZLIB = 0x00000001, LIBLZF = 0x00000002 };
enum Mode { VIDEO_RGB_24B = 0x00000000, VIDEO_YUYV_16B = 0x00000001, VIDEO_IR_24B = 0x00000002, VIDEO_DEPTH_16B = 0x00000003, VIDEO_DIR_24B = 0x00000004, DATA_MOUSE = 0x00000005, DATA_KEYBOARD = 0x00000006};

libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
libfreenect2::FrameMap frames;

/* Loop control */
int flag = 1;

typedef struct __camera {
	/* v4l */
	int fd;
	struct v4l2_format v4l2Format;
	struct v4l2_requestbuffers requestBuffers;
	__u32 buffercount;
	v4l2_buffer *buffers;

	/* video */
	unsigned char *vBuf[NBUF], *vZipBuf[NBUF];
	int vFlag[NBUF], vThreadFlag;
	unsigned long int vSize, vZipSize[NBUF];
	struct timeval vTime[NBUF];
	u_int32_t compression, mode;

	/* file */
	FILE *f;
	char *filename;
} CAMERA;

typedef struct __multimodal_cam{

	int height, width;
	CAMERA *color, *depth, *ir;
}MULT_MODAL;

typedef struct __input {
	int fd;
	u_int32_t mode;

	FILE *f;
	char *filename;
} INPUT;