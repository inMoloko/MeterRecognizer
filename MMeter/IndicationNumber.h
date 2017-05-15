#pragma once
#include <opencv2/imgproc/imgproc.hpp>

class IndicationNumber
{
public:
	IndicationNumber(int stageID, cv::Mat numGrayImg, cv::Mat numEdge, cv::Rect numAlignedBoundingBox, char recognized);
	
	int _stageID;
	cv::Mat _numGrayImg;
	cv::Mat _numEdge;
	cv::Rect _numAlignedBoundingBox;
	char _recognized;
};

