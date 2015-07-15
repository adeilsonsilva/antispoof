#include <FaceAntispoof/data.hpp>

using namespace FACEANTISPOOF;

data::data(string filename)
{
	this->videoFile.open(filename);
	this->loaded = this->videoFile.isOpened();
}

data::~data()
{
	this->videoFile.release();
}

bool data::get(cv::Mat& image)
{
	return this->videoFile.read(image);
}

bool data::isLoaded(){
	return this->loaded;
}