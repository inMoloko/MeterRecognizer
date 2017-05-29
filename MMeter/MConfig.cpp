#include "MConfig.h"

#include <opencv2/highgui/highgui.hpp>


MConfig::MConfig() :
	_debugOn(false), _fullDebugOn(false), _rotationDegrees(0), _ocrMaxDist(5e5), _digitMinHeight(20), _digitMaxHeight(
		90), _digitYAlignment(10), _cannyThreshold1Digits(50), _cannyThreshold2Digits(
			100), _cannyThreshold1Lines(100), _cannyThreshold2Lines(200), _trainingDataFilename("trainctr.yml"), 
			_minLine_IsPercentageOfMinDimentionOfImage(10), _maxLineGap(5), _testsQuantity("0 0 0 0 0 0 0 0 0 0"){
}


MConfig::~MConfig()
{
}

void MConfig::saveConfig() {
	cv::FileStorage fs(_workPath + "config.yml", cv::FileStorage::WRITE);
	fs << "debugOn" << _debugOn;
	fs << "fullDebugOn" << _fullDebugOn;
	fs << "rotationDegrees" << _rotationDegrees;
	fs << "cannyThreshold1Lines" << _cannyThreshold1Lines;
	fs << "cannyThreshold2Lines" << _cannyThreshold2Lines;
	fs << "cannyThreshold1Digits" << _cannyThreshold1Digits;
	fs << "cannyThreshold2Digits" << _cannyThreshold2Digits;
	fs << "digitMinHeight" << _digitMinHeight;
	fs << "digitMaxHeight" << _digitMaxHeight;
	fs << "digitYAlignment" << _digitYAlignment;
	fs << "ocrMaxDist" << _ocrMaxDist;
	fs << "minLine_IsPercentageOfMinDimentionOfImage" << _minLine_IsPercentageOfMinDimentionOfImage;
	fs << "maxLineGap" << _maxLineGap;
	fs << "trainingDataFilename" << _trainingDataFilename;
	fs << "testsQuantity" << _testsQuantity;
	fs.release();
}

void MConfig::loadConfig(std::string workPath) {
	_workPath = workPath;

	cv::FileStorage fs(_workPath + "config.yml", cv::FileStorage::READ);
	if (fs.isOpened()) {
		fs["debugOn"] >> _debugOn;
		fs["fullDebugOn"] >> _fullDebugOn;
		fs["rotationDegrees"] >> _rotationDegrees;
		fs["cannyThreshold1Lines"] >> _cannyThreshold1Lines;
		fs["cannyThreshold2Lines"] >> _cannyThreshold2Lines;
		fs["cannyThreshold1Digits"] >> _cannyThreshold1Digits;
		fs["cannyThreshold2Digits"] >> _cannyThreshold2Digits;
		fs["digitMinHeight"] >> _digitMinHeight;
		fs["digitMaxHeight"] >> _digitMaxHeight;
		fs["digitYAlignment"] >> _digitYAlignment;
		fs["ocrMaxDist"] >> _ocrMaxDist;
		fs["minLine_IsPercentageOfMinDimentionOfImage"] >> _minLine_IsPercentageOfMinDimentionOfImage;
		fs["maxLineGap"] >> _maxLineGap;
		fs["trainingDataFilename"] >> _trainingDataFilename;
		fs["testsQuantity"]  >> _testsQuantity;
		fs.release();
	}
	else {
		//Если файла конфигурации нет, то он создается с дефолтовыми значениями
		saveConfig();
	}
}
