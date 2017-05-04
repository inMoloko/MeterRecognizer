#include "IndicationNumber.h"

IndicationNumber::IndicationNumber(int stageID, cv::Mat numGrayImg, cv::Mat numEdge, cv::Rect numAlignedBoundingBox)
{
	_stageID = stageID;
	_numGrayImg = numGrayImg;
	_numEdge = numEdge;
	_numAlignedBoundingBox = numAlignedBoundingBox;
}


IndicationNumber::~IndicationNumber()
{
}
