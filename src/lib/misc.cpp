#include <FaceAntispoof/misc.hpp>

using namespace FACEANTISPOOF;

const int misc::NUM_TRAINING_EXAMPLES = 31500; /* (70 real faces + 35 masks) * 300 frames */
const int misc::NUM_FEATURES = 3776; /* Number of histograms columns */
const int misc::NUM_FACES_SAMPLES = 21000; /* Number of face histograms */
const int misc::NUM_MASKS_SAMPLES = 10500; /* Number of mask histograms */
const int misc::NUM_FRAMES = 300;
const int misc::NUM_FACES = 70;
const int misc::NUM_MASKS = 35;

misc::misc()
{
    this->faceCascadeName = "../misc/haarcascade_frontalface_alt.xml";
    this->eyeCascadeName = "../misc/haarcascade_lefteye_2splits.xml";
    this->eyeTreeCascadeName = "../misc/haarcascade_righteye_2splits.xml";
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
    command << "cd " << this->outputFolderPath << " && mkdir " << this->outputFolderName << " && cd " << this->outputFolderName << " && mkdir frames && mkdir faces && mkdir LBP && mkdir detectedFaces";
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
    std::string result;
    std::stringstream sstm;
    sstm << "Histogram_" << frame;
    result = sstm.str();
    // this->outputXMLFile << "Face" << frame;
    this->outputXMLFile << result << hist;
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

void misc::saveImage(cv::Mat &image, std::string folder, int count){
    std::string result;
    std::stringstream sstm;

    this->comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
    this->comPressParam.push_back(5);
    sstm << this->outputFolderPath << this->outputFolderName << folder << count << ".png";
    result = sstm.str();
    cv::imwrite(result, image, this->comPressParam);
}

void misc::readHist()
{
    std::string result;
    std::stringstream sstm;
    cv::Mat tmpImg(1, misc::NUM_FEATURES, CV_32FC1);

    /* Using half histograms for memory purposes */
    for (int i = 1; i <= NUM_FACES/2; i++)
    {
        sstm.clear();
        sstm << "../misc/Histograms/Color/Face/histograms" << i << ".xml";
        result = sstm.str();
        this->inputXMLFile.open(result, cv::FileStorage::READ);

        for (int count = 1; count <= misc::NUM_FRAMES; count++)
        {
            sstm.clear();
            sstm << "Histogram_" << count;
            result = sstm.str();
            this->inputXMLFile[result] >> tmpImg;
            this->Hists.push_back(tmpImg);
            // std::cout << "Faces i: " << i << " count: " << count << std::endl;
            // tmpImg.release();
        }
    }
    std::cout << "Faces hist size: " << this->Hists.size() << std::endl;

    /* Using half histograms for memory purposes */
    for (int i = 1; i <= misc::NUM_MASKS/2; i++)
    {
        sstm.clear();
        sstm << "../misc/Histograms/Color/Mask/histograms" << i << ".xml";
        result = sstm.str();
        this->inputXMLFile.open(result, cv::FileStorage::READ);

        for (int count = 1; count <= misc::NUM_FRAMES; count++)
        {
            sstm.clear();
            sstm << "Histogram_" << count;
            result = sstm.str();
            this->inputXMLFile[result] >> tmpImg;
            this->Hists.push_back(tmpImg);
            // std::cout << "Masks i: " << i << " count: " << count << std::endl;
            // tmpImg.release();
        }
    }
    std::cout << "Total size: " << this->Hists.size() << std::endl;
}

void misc::trainSvm()
{
    this->readHist();

    cv::Mat training_data(this->Hists.size(), misc::NUM_FEATURES, CV_32FC1);
    // cv::Mat training_data(this->Hists);
    cv::Mat class_labels(this->Hists.size(), 1, CV_32SC1);

    /* this->Hists.size() <= misc::NUM_TRAINING_EXAMPLES */
    for(unsigned int i = 0; i < this->Hists.size(); i++)
    {
        cv::Mat test;
        this->Hists[i].convertTo(test, CV_32FC1);
        training_data.push_back(test);
    }

    /* Set 1 for faces an 2 for masks */
    class_labels.rowRange(0, misc::NUM_FACES_SAMPLES/2).setTo(1);
    class_labels.rowRange(misc::NUM_FACES_SAMPLES/2, this->Hists.size()).setTo(2);

    cv::Ptr<ml::SVM> my_svm = ml::SVM::create();
    Ptr<ml::TrainData> tData = ml::TrainData::create(training_data, ml::ROW_SAMPLE, class_labels);
    std::cout << "Training SVM with " << training_data.rows << " samples!" << std::endl;
    // my_svm->trainAuto(tData);
    my_svm->train(tData);
    std::cout << "Saving SVM!" << std::endl;
    my_svm->save("../misc/svm_lbp_model.txt");
}

void misc::loadSvm()
{
    this->my_svm = ml::StatModel::load<ml::SVM>("../misc/svm_lbp_model.xml");
}

void misc::predictSvm(cv::Mat& hist)
{
    std::cout << this->my_svm->predict(hist) << std::endl;
}

