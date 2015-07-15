#include <FaceAntispoof/base.hpp>

int main(int argc, char const *argv[])
{
	cv::Mat image, im;
	FACEANTISPOOF::data dataHandler("video.avi");

	if(dataHandler.isLoaded()){
		while(dataHandler.get(image))
		{
			cv::imshow("Depth Video", image);
			int c = cvWaitKey(1);
			if(char(c) == 's'){break;}
		}
	}else{
		std::cout << "Error loading video file!" << std::endl;
	}

	return 0;
}