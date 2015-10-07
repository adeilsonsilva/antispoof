/* CPP include files */
#include <iostream>
#include <string>

/* OpenCV include files */
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv/cv.h>

/* App include files */
#include "data.hpp"
#include "misc.hpp"
#include "../../vendor/lbp.hpp"
#include "../../vendor/helper.hpp"

/* Extra include files */
#include <sys/time.h>
#include <fstream>
#include <ctime>
#include <cmath>

/* Flags */
#define KINECT 0
#define WRITEVIDEO 1
#define M_WIDTH 960
#define M_HEIGHT 540
