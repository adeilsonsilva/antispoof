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

using namespace std;

namespace FACEANTISPOOF
{
    class misc
    {
        public:
            misc();
			~misc();
            void saveHist(cv::Mat& hist);
            unsigned int framesCount;

        protected:
            typedef struct timeval TIME;
            void saveHist(cv::Mat& hist, int frame);
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
    };
}

#endif
