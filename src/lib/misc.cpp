#include <FaceAntispoof/misc.hpp>

using namespace FACEANTISPOOF;

misc::misc()
{
    this->faceCascadeName = "../misc/haarcascade_frontalface_alt.xml";
    this->eyeCascadeName = "../misc/haarcascade_eye.xml";
    this->eyeTreeCascadeName = "../misc/haarcascade_eye_tree_eyeglasses.xml";
    this->outputFolderPath = "../out/";
    this->getTime();
    this->createFolders();
    this->loadCascades();
    this->outputXMLFile.open(this->outputFolderPath + this->outputFolderName + "/histograms.xml", cv::FileStorage::WRITE);
    this->outputXMLFile << "Data" << this->outputFolderName;
    this->framesCount = 0;
    this->facesCount = 0;
}

misc::~misc()
{
    this->outputXMLFile.release();
    this->resultsFile.close();
}

void misc::getTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[100];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,100,"%d-%m-%Y_%I:%M:%S",timeinfo);
    std::string str(buffer);

    this->outputFolderName = str;
}

void misc::tset()
{
    gettimeofday(&this->t, NULL);
}

double misc::tget(TIME *t0, TIME *t1)
{
	return t1->tv_sec-t0->tv_sec+(t1->tv_usec-t0->tv_usec)*0.000001;
}

void misc::createFolders()
{
    std::string commandStr, resultsName;
	const char* commandStr_C;
	std::stringstream command, videoPath, resultsPath;
	char resp;

	std::cout << "Results will be saved at: /antispoofing/out/" << this->outputFolderName << std::endl;
	command << "cd " << this->outputFolderPath << " && mkdir " << this->outputFolderName << " && cd " << this->outputFolderName << " && mkdir frames && mkdir faces && mkdir detectedFaces";
	commandStr = command.str();
	commandStr_C = commandStr.c_str();
	if(std::system(commandStr_C) != 0){
       std::cout << "The folder " << this->outputFolderName << " already exists! Overwrite? [y/n]: ";
       std::cin >> resp;
        if(resp == 'n' || resp == 'N'){
            this->success = false;
            exit(1);
        }
	}

	videoPath << this->outputFolderPath << this->outputFolderName << "/video.avi";
	this->outputVideoName = videoPath.str();
	resultsPath << this->outputFolderPath << this->outputFolderName << "/results.txt";
	resultsName = resultsPath.str();
	this->resultsFile.open(resultsName.c_str());
    this->success = true;
}

/* Wrapper to avoid bad memory handling from FileStorage Class
 * this->outputXMLFile << "Frame" << this->frame;
 * LINE ABOVE DOES NOT WORK
 */
void misc::saveHist(cv::Mat& hist)
{
    this->saveHist(hist, this->framesCount);
}

void misc::saveHist(cv::Mat& hist, int frame)
{
    this->outputXMLFile << "Face" << frame;
    this->outputXMLFile << "Histograma" << hist;
}

/* Vendor function */
void misc::loadCascades()
{
    /* Load the Face Detection cascade classifier xml file. */
    try {   /* Surround the OpenCV call by a try/catch block so we can give a useful error message! */
        this->faceCascade.load(this->faceCascadeName);
    } catch (cv::Exception &e) {}
    if ( this->faceCascade.empty() ) {
        cerr << "ERROR: Could not load Face Detection cascade classifier [" << this->faceCascadeName << "]!" << endl;
        exit(1);
    }

    /* Load the Eye Detection cascade classifier xml file. */
    try {   /* Surround the OpenCV call by a try/catch block so we can give a useful error message! */
        this->eyeCascade.load(this->eyeCascadeName);
    } catch (cv::Exception &e) {}
    if ( this->eyeCascade.empty() ) {
        cerr << "ERROR: Could not load 1st Eye Detection cascade classifier [" << this->eyeCascadeName << "]!" << endl;
        exit(1);
    }

    /* Load the Eye Detection cascade classifier xml file. */
    try {   /* Surround the OpenCV call by a try/catch block so we can give a useful error message! */
        this->eyeTreeCascade.load(this->eyeTreeCascadeName);
    } catch (cv::Exception &e) {}
    if ( this->eyeTreeCascade.empty() ) {
        cerr << "Could not load 2nd Eye Detection cascade classifier [" << this->eyeTreeCascadeName << "]." << endl;
        /* Dont exit if the 2nd eye detector did not load, because we have the 1st eye detector at least.
         * exit(1);
         */
    }
}
