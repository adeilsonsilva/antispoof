#include <FaceAntispoof/base.hpp>

int main(int argc, char const *argv[])
{
    std::cout << "Usage: ./faceAntispoof videoname" << std::endl;

	if(argc < 2){ 
		std::cout << "Not enough arguments." << std::endl; 	
		exit(-1);
	}

    cv::Mat image, lbp, dst, hist;
	FACEANTISPOOF::data dataHandler(argv[1]);

	if(dataHandler.isLoaded()){
		while(dataHandler.get(image))
		{
			cvtColor(image, dst, CV_BGR2GRAY);
			libfacerec::olbp(dst, lbp);
			hist = libfacerec::spatial_histogram(lbp, 59);
			dataHandler.saveHist(hist);
            cv::imshow("Video", lbp);
			int c = cvWaitKey(1);
			if(char(c) == 's'){break;}
		}
        std::cout << "End of video!" << std::endl;
	}else{
		std::cout << "Error loading video file!" << std::endl;
	}

	return 0;
}
