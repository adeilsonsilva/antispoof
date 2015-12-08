/* CPP include files */
#include <iostream>
#include <stdio.h>

/* OpenCV include files */
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* App include files */
#include <zlib.h>
#include <liblzf/lzf.h>

/* Extra include files */
#include <linux/input.h>

/* Flags */
#define COLOR_WIDTH 768
#define COLOR_HEIGHT 432
#define DEPTH_HEIGHT 424
#define DEPTH_WIDTH 512

/* Enums */
enum CompressionFormat { NONE = 0x00000000, ZLIB = 0x00000001, LIBLZF = 0x00000002 };
enum Mode { VIDEO_RGB_24B = 0x00000000, VIDEO_YUYV_16B = 0x00000001, VIDEO_IR_24B = 0x00000002, VIDEO_DEPTH_16B = 0x00000003, VIDEO_DIR_24B = 0x00000004, DATA_MOUSE = 0x00000005, DATA_KEYBOARD = 0x00000006};
enum PixelFormat { YUYV = 0x56595559, INVZ = 0x5a564e49, INZI = 0x495a4e49, INVR = 0x52564e49, INRI = 0x49524e49, INVI = 0x49564e49, RELI = 0x494c4552 };

/* Camera stuff */
#define NBUF 4
