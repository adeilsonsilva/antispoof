#include <opencv2/opencv.hpp>
#include <string>

/* Main class to handle data related stuff:
 * 		Read video files;
 *		Read images;
 *		Save video files;
 */

using namespace std;

namespace FACEANTISPOOF
{
	class data{
		public:
			data(string filename);
			~data();
			bool get(cv::Mat& image);
			bool isLoaded();

		private:
			cv::VideoCapture videoFile;
			bool loaded;
	};
}