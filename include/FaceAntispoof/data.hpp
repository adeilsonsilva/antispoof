/* Main class to handle data related stuff:
 * 		Read video files;
 *		Read images;
 *		Save video files;
 * 		Save FileStorage stuf;
 */

#ifndef DATA_HPP
#define DATA_HPP

#include "misc.hpp"

using namespace std;

namespace FACEANTISPOOF
{
	class data : public misc 
    {
		public:
			data(string filename);
			~data();
			bool get(cv::Mat& outputImage);
			bool isLoaded();
            void saveImage(cv::Mat &image, std::string path, int imgCount);

		private:
			string outputFilePath;
			cv::VideoCapture inputVideoFile;
            cv::VideoWriter outputVideoFile;
			bool loaded;
	};
}

#endif
