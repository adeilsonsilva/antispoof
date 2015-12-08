/* Main class to handle Kinect stuff:
 * 		Load video streams;
 *		Read images;
 */

#ifndef LOAD_HPP
#define LOAD_HPP

#include <Kinect/dependencies.hpp>

using namespace cv;
using namespace std;

namespace KINECT
{
	class load
	{
		public:

			struct __stream {
				FILE *f;
				u_int32_t mode, compression;
				int width, height, size;
				Mat color, depth, infrared, tmp;
				unsigned char *buf1, *buf2;
				struct timeval t;
			};

			struct __data {
				FILE *f;
				u_int32_t mode;
				struct input_event ev;
				int coord[2];
			};

			typedef load::__data DATA;
			typedef load::__stream STREAM;

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
