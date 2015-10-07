#include <FaceAntispoof/misc.hpp>

using namespace FACEANTISPOOF;

misc::misc()
{
    this->outputFolderPath = "../out/";    
    this->getTime();	    
    this->createFolders();
    this->outputXMLFile.open(this->outputFolderPath + this->outputFolderName + "/histograms.xml", cv::FileStorage::WRITE);
    this->outputXMLFile << "Data" << this->outputFolderName;
    this->framesCount = 0;
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
	command << "cd " << this->outputFolderPath << " && mkdir " << this->outputFolderName << " && cd " << this->outputFolderName << " && mkdir frames";
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
void misc::saveHist(cv::Mat& hist){
    this->saveHist(hist, this->framesCount);
}

void misc::saveHist(cv::Mat& hist, int frame){
    this->outputXMLFile << "Frame" << frame;
    this->outputXMLFile << "Histograma" << hist;
}
