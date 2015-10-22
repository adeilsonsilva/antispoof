/* Main class to handle extra stuff:
 * 		Read timestamps;
 *		Create folders;
 * 		Save FileStorage stuf;
 */

#ifndef MISC_HPP
#define MISC_HPP

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
 #include <opencv2/highgui.hpp>
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
            vector<cv::Mat> Hists;
            unsigned int framesCount;
            unsigned int facesCount;
            void saveHist(cv::Mat& hist);
            void loadCascades();
            void readHist();
            void trainSvm();

        protected:
            typedef struct timeval TIME;
            bool success;
            string outputFolderPath, outputFolderName, outputVideoName;
            TIME t, ct;
            cv::FileStorage outputXMLFile, inputXMLFile;
            cv::FileNode node;
            ofstream resultsFile;
            vector<int> comPressParam;
            void getTime();
            void tset();
            double tget(TIME *t0, TIME *t1);
            void createFolders();
            void saveHist(cv::Mat& hist, int frame);
            void saveImage(cv::Mat &image, std::string folder, int count);
            static const int NUM_TRAINING_EXAMPLES;
            static const int NUM_FEATURES;
            static const int NUM_FACES;
            static const int NUM_MASKS;
    };
}

#endif
