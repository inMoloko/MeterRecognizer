#pragma once

//#ifdef MMeter_EXPORTS
//#define MMeter_API __declspec(dllexport) 
//#else
//#define MMeter_API __declspec(dllimport)
//#endif

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "MConfig.h"

namespace MMeter
{
	class myclass
	{
	public:
		myclass(char* imagePath, char* workPath);
		double sumX_Y();
		void process();
		void process2();
		float getMeterCircleRadius();
		char* getMeterValue();
		void learn();
		std::vector<cv::Mat> getOutput();
		void configureLogging(const std::string & priority, bool toConsole);

		~myclass();

	private:
		void learnOcr();
		void rotate(double rotationDegrees);
		void findCounterDigits();
		
		void findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
			std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result, float meterCircleRadius);
		float detectSkew();
		void drawLines(std::vector<cv::Vec2f>& lines);
		void drawLines(std::vector<cv::Vec4i>& lines, int xoff = 0, int yoff = 0);
		cv::Mat cannyEdges();
		void filterContours(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, std::vector<cv::Rect>& boundingBoxes,
			std::vector<std::vector<cv::Point> >& filteredContours, float meterCircleRadius);

		double x;
		double y;
		MConfig _config;
		std::string _imagePath;
		std::string _imageFileName;
		std::string _workPath;		

		cv::Mat _img;
		cv::Mat _imgGray;
		std::vector<cv::Mat> _digits;
		//Config _config;
		bool _debugWindow;
		bool _debugSkew = true;
		bool _debugEdges = true;
		bool _debugDigits  = true;
	};
}
