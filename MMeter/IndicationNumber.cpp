#include "IndicationNumber.h"

IndicationNumber::IndicationNumber(int stageID, cv::Mat numGrayImg, cv::Mat numEdge, cv::Rect numAlignedBoundingBox, char recognized)
{
	_stageID = stageID;
	_numGrayImg = numGrayImg;
	_numEdge = numEdge;
	_numAlignedBoundingBox = numAlignedBoundingBox;
	_recognized = recognized;
}
