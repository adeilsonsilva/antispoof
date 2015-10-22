#include <FaceAntispoof/data.hpp>

using namespace FACEANTISPOOF;

data::data(string filename)
{
	this->inputVideoFile.open(filename);
	this->loaded = this->inputVideoFile.isOpened();
}

data::data()
{
    this->loaded = false;
}

data::~data()
{
    string finalText = cv::format("\n\nFrames Count: %d || Face Count: %d", this->framesCount, this->facesCount);
    if(this->resultsFile.is_open()){
        this->resultsFile << finalText;
    }
	this->inputVideoFile.release();
}

bool data::getImage()
{
	if(this->inputVideoFile.read(this->image)){
        this->framesCount++;
        this->saveFrame();
        return true;
    }
    return false;
}

bool data::isLoaded(){
	return (this->loaded && this->success);
}

void data::detectFace()
{
    /* Find a face and preprocess it to have a standard size and contrast & brightness. */
    cv::Rect faceRect;  /* Position of detected face. */
    cv::Rect searchedLeftEye, searchedRightEye; /* Top-left and top-right regions of the face, where eyes were searched. */
    cv::Point leftEye, rightEye;    /* Position of the detected eyes. */
    cv::Mat processedFace;

    processedFace = vendor::getPreprocessedFace(this->image, 50, this->faceCascade, this->eyeCascade, this->eyeTreeCascade, true, &faceRect, &leftEye, &rightEye, &searchedLeftEye, &searchedRightEye);

    if(!processedFace.empty()){
        this->face = processedFace;
        this->detectedFace = this->image(faceRect);
        this->facesCount++;
        this->saveFace();
        this->saveDetectedFace();
    }else{
        this->face.release();
        this->detectedFace.release();
    }
}

void data::getLBP()
{
    if(!this->face.empty()){
        libfacerec::olbp(this->face, this->faceLBP);
        this->faceHist = libfacerec::spatial_histogram(this->faceLBP, 59);
        this->saveHist(this->faceHist, this->facesCount);
        this->saveFaceLBP();
    }
}

void data::saveFrame(){
    this->saveImage(this->image, "/frames/", this->framesCount);
}

void data::saveDetectedFace(){
    this->saveImage(this->detectedFace, "/detectedFaces/", this->facesCount);
}

void data::saveFace(){
    this->saveImage(this->face, "/faces/", this->facesCount);
}

void data::saveFaceLBP(){
    this->saveImage(this->faceLBP, "/LBP/", this->facesCount);
}

void data::showFaces()
{
    cv::imshow("Video Stream", this->image);
    if(!this->face.empty()){
        cv::imshow("Detected Face", this->detectedFace);
        cv::imshow("Normalized Face", this->face);
        cv::imshow("LBP Normalized Face", this->faceLBP);
    }
}

