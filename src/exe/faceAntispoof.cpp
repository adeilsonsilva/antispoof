#include <FaceAntispoof/base.hpp>

int main(int argc, char const *argv[])
{

    std::cout << "Press 'q' to quit!" << std::endl;

    if(argc < 2){
        std::cout << "Not enough arguments." << std::endl;
        std::cout << "Usage: ./faceAntispoof videoname" << std::endl;
        exit(-1);
    }

    FACEANTISPOOF::data dataHelper(argv[1]);

    if(dataHelper.isLoaded()){
        dataHelper.loadSvm();
        while(dataHelper.getImage())
        {
            dataHelper.detectFace();
            dataHelper.getLBP();
            dataHelper.predict();
            dataHelper.showFaces();
            int c = cvWaitKey(1);
            if(char(c) == 'q'){break;}
        }
        std::cout << "End of video!" << std::endl;
    }else{
        std::cout << "Error loading video file!" << std::endl;
    }

    // FACEANTISPOOF::data dataHelper;
    // dataHelper.trainSvm();

    return 0;
}
