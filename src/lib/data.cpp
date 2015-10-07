#include <FaceAntispoof/data.hpp>

using namespace FACEANTISPOOF;

data::data(string filename)
{
	this->inputVideoFile.open(filename);
	this->loaded = this->inputVideoFile.isOpened();
}

data::~data()
{
	this->inputVideoFile.release();
}

bool data::get(cv::Mat& outputImage)
{
	if(this->inputVideoFile.read(outputImage)){
        this->framesCount++;
        return true;
    }
    return false;
}

bool data::isLoaded(){
	return (this->loaded && this->success);
}

void data::saveImage(cv::Mat &image, std::string path, int imgCount){
    std::string result;
    std::stringstream sstm;

	this->comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
    this->comPressParam.push_back(5); 
    sstm << this->outputFolderPath << this->outputFolderName << path << imgCount << ".png";
    result = sstm.str();
    cv::imwrite(result, image, this->comPressParam);
}
