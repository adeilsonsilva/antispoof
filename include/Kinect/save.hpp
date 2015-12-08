/* Main class to handle Kinect stuff:
 * 		Save video streams;
 *		Save images;
 */

#ifndef SAVE_HPP
#define SAVE_HPP

#include <Kinect/dependencies.hpp>

/* Class required includes */
#include <stdlib.h>
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
#include "liblzf/lzf_c.c"
#include <X11/Xlib.h>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/threading.h>
#include <libfreenect2/packet_pipeline.h>

using namespace std;

namespace KINECT
{
	class save
	{
		public:
			/* Loop control */
			int flag = 1;

			libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
			libfreenect2::FrameMap frames;

			struct __camera {
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
			};

			struct __multimodal_cam{

				int height, width;
				CAMERA *color, *depth, *ir;
			};

			struct __input {
				int fd;
				u_int32_t mode;

				FILE *f;
				char *filename;
			};

			typedef save::__camera CAMERA;
			typedef save::__multimodal_cam MULT_MODAL;
			typedef save::__input INPUT;
	};

}

#endif
