/* Main class to handle extra stuff:
 * 		Read timestamps;
 *		Create folders;
 * 		Save FileStorage stuf;
 */

#ifndef MISC_HPP
#define MISC_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/time.h>

#include "../vendor/lbp.hpp"
#include "../vendor/preprocessFace.hpp"
#include "../vendor/detectObject.hpp"

using namespace std;

namespace FACEANTISPOOF
{
    class misc
    {
        public:
            misc();
			~misc();
            string faceCascadeName, eyeCascadeName, eyeTreeCascadeName;
            cv::CascadeClassifier faceCascade, eyeCascade, eyeTreeCascade;
            unsigned int framesCount;
            unsigned int facesCount;
            void saveHist(cv::Mat& hist);
            void loadCascades();

        protected:
            typedef struct timeval TIME;
            bool success;
            string outputFolderPath, outputFolderName, outputVideoName;
            TIME t, ct;
            cv::FileStorage outputXMLFile;
            ofstream resultsFile;
            vector<int> comPressParam;
            void getTime();
            void tset();
            double tget(TIME *t0, TIME *t1);
            void createFolders();
            void saveHist(cv::Mat& hist, int frame);
    };
}

#endif
