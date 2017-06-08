#pragma once

#ifndef KNEARESTOCR_H_
#define KNEARESTOCR_H_

#include <vector>
#include <list>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "MConfig.h"

class KNearestOcr
{
public:
	KNearestOcr();
	KNearestOcr(const MConfig & config);
	virtual ~KNearestOcr();

	void learn(cv::Mat edges, int answer);

	int learn(const cv::Mat & img, std::string workPath, std::vector<int>& testsQuantity);
	int learn(const std::vector<cv::Mat> & images, std::string workPath);
	void saveTrainingData();
	bool is_empty(std::ifstream & pFile);
	bool loadTrainingData(bool isNotTraining);

	char recognize(const cv::Mat & img);
	std::string recognize(const std::vector<cv::Mat> & images);

private:
	cv::Mat prepareSample(const cv::Mat & img);
	void initModel();

	cv::Mat _samples;
	cv::Mat _responses;
	cv::Ptr<cv::ml::KNearest> _pModel;
	MConfig _config;
};

#endif /* KNEARESTOCR_H_ */

