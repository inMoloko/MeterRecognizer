//http://stackoverflow.com/questions/28035484/opencv-3-knn-implementation
#include "stdafx.h"
#include "KNearestOcr.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/ml/ml.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <exception>
#include <fstream>


KNearestOcr::KNearestOcr()
{
}


KNearestOcr::KNearestOcr(const MConfig & config) :
	_config(config) {
}

KNearestOcr::~KNearestOcr() {
}

/**
* Обучение одной цифры
*/
void KNearestOcr::learn(cv::Mat edges, int answer) {
	_responses.push_back(cv::Mat(1, 1, CV_32F, answer));
	_samples.push_back(prepareSample(edges));
}

int KNearestOcr::learn(const cv::Mat & img, std::string workPath, std::vector<int>& testsQuantity) {
	cv::imshow("Learn", img);
	int key = cv::waitKey(0);
	if (key >= '0' && key <= '9') {
		
		cv::Mat edges;
		cv::Canny(img, edges, _config.getCannyThreshold1Digits(), _config.getCannyThreshold2Digits());
		_responses.push_back(cv::Mat(1, 1, CV_32F, (float)key - '0'));
		_samples.push_back(prepareSample(edges));
		
		testsQuantity[key - '0']++;
		_config.setTestsQuantity(testsQuantity);

		cv::imwrite(workPath + "learning\\" + std::to_string(key - '0') + "\\" + std::to_string(testsQuantity[key - '0']) + ".jpg", img);
		_config.saveConfig();
	}
	return key;
}

/**
* Обучение вектора цифр
*/
int KNearestOcr::learn(const std::vector<cv::Mat>& images, std::string workPath) {
	int key = 0; 
	std::vector<int> tq = _config.getTestsQuantity();

	for (std::vector<cv::Mat>::const_iterator it = images.begin();
	it < images.end() && key != 's' && key != 'q'; ++it) {
		key = learn(*it, workPath, tq);
	}
	return key;
}

/**
* Сохранение данных для тренировки
*/
void KNearestOcr::saveTrainingData() {
	cv::FileStorage fs(_config.getTrainingDataFilename(), cv::FileStorage::WRITE);
	fs << "samples" << _samples;
	fs << "responses" << _responses;
	fs.release();
}
bool KNearestOcr::is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}
/**
* Загружаем данные для тернировки и инициализируем модель
*/
bool KNearestOcr::loadTrainingData(bool isNotTraining) {
	cv::FileStorage fs(_config.getTrainingDataFilename(), cv::FileStorage::READ);
	if (fs.isOpened()) {
		fs["samples"] >> _samples;
		fs["responses"] >> _responses;
		fs.release();

		if (isNotTraining)
			initModel();
	}
	else {
		return false;
	}
	return true;
}

/**
* Разпознование одной цифры
*/
char KNearestOcr::recognize(const cv::Mat& img) {

	log4cpp::Category& rlog = log4cpp::Category::getRoot();
	char cres = '?';
	try {
		if (!_pModel) {
			throw std::runtime_error("Model is not initialized");
		}
		cv::Mat results, neighborResponses, dists;
		float result = _pModel->findNearest(prepareSample(img), 2, results, neighborResponses, dists);

		if (0 == int(neighborResponses.at<float>(0, 0) - neighborResponses.at<float>(0, 1))
			&& dists.at<float>(0, 0) < _config.getOcrMaxDist()) {
			cres = '0' + (int)result;
		}

		rlog << log4cpp::Priority::DEBUG << "results: " << results;
		rlog << log4cpp::Priority::DEBUG << "neighborResponses: " << neighborResponses;
		rlog << log4cpp::Priority::DEBUG << "dists: " << dists;
	}
	catch (std::exception & e) {
		rlog << log4cpp::Priority::ERROR << e.what();
	}
	return cres;
}

/**
* Распознование вектора цифр
*/
std::string KNearestOcr::recognize(const std::vector<cv::Mat>& images) {
	std::string result;
	for (std::vector<cv::Mat>::const_iterator it = images.begin();
	it != images.end(); ++it) {
		result += recognize(*it);
	}
	return result;
}

/**
* Подготовка картинки цифры как примера для модели
*/
cv::Mat KNearestOcr::prepareSample(const cv::Mat& img) {
	cv::Mat roi, sample;
	cv::resize(img, roi, cv::Size(10, 10));
	roi.reshape(1, 1).convertTo(sample, CV_32F);
	return sample;
}

/**
* Инициализирование модели
*/
void KNearestOcr::initModel() {
	_pModel = cv::ml::KNearest::create();
	_pModel->train(_samples, 0, _responses);
}
