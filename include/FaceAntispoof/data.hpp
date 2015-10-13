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
            cv::Mat image, face, faceLBP, faceHist;
            cv::Mat detectedFace;
			bool getImage();
			bool isLoaded();
            void saveFrame();
            void saveFace();
            void saveDetectedFace();
            void detectFace();
            void getLBP();
            void showFaces();

		private:
			string outputFilePath;
			cv::VideoCapture inputVideoFile;
            cv::VideoWriter outputVideoFile;
			bool loaded;
            void saveImage();
	};
}

#endif
