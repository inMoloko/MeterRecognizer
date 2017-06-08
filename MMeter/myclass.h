#pragma once



#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "MConfig.h"
#include "IndicationNumber.h"
#include "KNearestOcr.h"

namespace MMeter
{
	class myclass
	{
	public:
		myclass(char* imagePath, char* workPath);
		myclass(std::string imagePath, char* workPath);
		double sumX_Y();
		void process2();
		float getMeterCircleRadius();
		char* getMeterValue();
		std::string getAnswerByiNums(std::vector<std::vector<IndicationNumber>>& iNums);
		void findOtherCounters(std::vector<std::vector<IndicationNumber>>& iNums, KNearestOcr ocr);
		IndicationNumber getINumByRect(cv::Rect rect, KNearestOcr ocr);
		void findOtherCountersandGetiNums(std::vector<std::vector<int>>& rectDists, std::vector<std::vector<int>>& filteredRectDists, std::vector<std::vector<IndicationNumber>>& iNums, int average_width, KNearestOcr ocr);
		std::vector<std::vector<int>> filterRectDists(std::vector<std::vector<int>> rectDists);
		int getAveraveWidth(std::vector<std::vector<IndicationNumber>> iNums);
		std::vector<std::vector<int>> getRectDists(std::vector<std::vector<IndicationNumber>> iNums, int average_width);
		void readAllfilesAndLearn(KNearestOcr& ocr);
		void autoLearn();
		std::string iNumsAnalyse(std::vector<std::vector<IndicationNumber>>& iNums, std::vector<int>& rNumsQuality);
		void learn();
		std::vector<cv::Mat> getOutput();
		void configureLogging(const std::string & priority, bool toConsole);

		~myclass();

	private:
		void learnOcr();
		void rotate(double rotationDegrees);
		std::vector<cv::Rect> findCounterDigits(int cannyThreshld1 = 0, int cannyThreshld2 = 0);
		
		void findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
			std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result, float meterCircleRadius);
		float detectSkew();
		float detectSkew2();
		void drawLines(std::vector<cv::Vec2f>& lines);
		void drawLines(std::vector<cv::Vec4i>& lines, int xoff = 0, int yoff = 0);
		void drawLines(std::vector<cv::Vec4i>& lines, bool flag);
		void filterContours(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, std::vector<cv::Rect>& boundingBoxes,
			std::vector<std::vector<cv::Point> >& filteredContours, float meterCircleRadius);
		std::vector<std::vector<IndicationNumber>> findCountersandGetiNums(KNearestOcr ocr);
		void coutRecognizediNums(std::vector<std::vector<IndicationNumber> >& iNums);
		int getiNumAverageX(std::vector<IndicationNumber> iNum);

		double x;
		double y;
		MConfig _config;
		std::string _imagePath;
		std::string _imageFileName;
		std::string _workPath;		

		cv::Mat _img;
		cv::Mat _imgGray;
		cv::Mat _tmpGray;
		cv::Mat _imgEqualized;
		std::vector<cv::Mat> _digits;
		std::vector<cv::Mat> _numGrayImgages;
		//Config _config;
		bool _debugWindow;
		bool _debugSkew = true;
		bool _debugEdges = true;
		bool _debugDigits  = true;
		std::string _stringID = "";
	};
}
