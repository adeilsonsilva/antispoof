#include <FaceAntispoof/data.hpp>

using namespace FACEANTISPOOF;

data::data(string filename)
{
    std::cout << filename << std::endl;
	this->inputVideoFile.open(filename);
	this->loaded = this->inputVideoFile.isOpened();
    this->model = cv::face::createLBPHFaceRecognizer();
}

data::data()
{
    std::cout << "filename" << std::endl;
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
    if(this->isLoaded()){
    	if(this->inputVideoFile.read(this->image)){
            this->framesCount++;
            this->saveFrame();
            return true;
        }
    }else{
        return this->getFromKinect();
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

    // std::vector<Rect> faces;
    // cv::Mat frame_gray;

    // cvtColor( this->image, frame_gray, CV_BGR2GRAY );
    // equalizeHist( frame_gray, frame_gray );

    // //-- Detect faces
    // this->faceCascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );
    // processedFace = frame_gray(faces[0]);

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

void data::getLBP(bool vendor)
{
    if(!this->face.empty()){
        if(vendor)
        {
            lbpHandler.getImage(this->face, this->faceLBP);
            this->faceHist = lbpHandler.describe(this->faceLBP);
            this->saveFaceLBP();
        }else{
            this->userLabels.push_back(misc::LABEL_1);
            this->userImages.push_back(this->face);
            this->model->train(userImages, this->userLabels);
            this->userImages.clear();
            this->userLabels.clear();
            // this->faceHist = this->model->getMatVector("histograms");
        }
        this->saveHist(this->faceHist, this->facesCount);
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
        // cv::imshow("Detected Face", this->detectedFace);
        cv::imshow("Normalized Face", this->face);
        cv::imshow("LBP Normalized Face", this->faceLBP);
    }
}

void data::predict()
{
    int type;

    if(!this->face.empty()){
        this->faceHist.convertTo(this->faceHist, CV_32FC1);
        type = this->predictSvm(this->faceHist);

        switch(type){
            case 1:
                std::cout << "Face!" << std::endl;
                break;
            case 2:
                std::cout << "Mask!" << std::endl;
                break;
            default:
                std::cout << "Unkown!" << std::endl;
                break;
        }
    }
}

bool data::getFromKinect()
{
    this->image = this->kinectBuffer.getFrame();
    if(!this->image.empty()){
        this->framesCount++;
        this->saveFrame();
        return true;
    }
    return false;
}
