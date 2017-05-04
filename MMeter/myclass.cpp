#include <ctime>
#include <string>
#include <list>
#include <iostream>
#include "myclass.h"
#include "MConfig.h"
#include "KNearestOcr.h"
#include <windows.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Helpers.h"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/Priority.hh>
#include <fstream>
#include "IndicationNumber.h"


namespace MMeter
{
	/**
	* Сортировка прямоугольников по X
	*/

	class sortRectByX {
	public:
		bool operator()(cv::Rect const & a, cv::Rect const & b) const {
			return a.x < b.x;
		}
	};

	class sortRectByXY {
	public:
		bool operator()(std::vector<cv::Point> const & a, std::vector<cv::Point> const & b) const {

			cv::Point2f a_center;
			float a_radius;
			cv::Point2f b_center;
			float b_radius;
			cv::minEnclosingCircle(a, a_center, a_radius);
			cv::minEnclosingCircle(b, b_center, b_radius);

			return a_radius > b_radius;
		}
	};

	myclass::~myclass()
	{
	}
	
	myclass::myclass(char* imagePath, char* workPath)
	{
		std::string str(imagePath);
		_imagePath = str;
		_imageFileName = Helpers::getFileName(_imagePath);

		std::string str1(workPath);
		_workPath = str1;		
	}

	double myclass::sumX_Y()
	{
		return x + y;
	}

	std::vector<cv::Mat> myclass::getOutput() {
		return _digits;
	}

	char* myclass::getMeterValue() {
		_config.loadConfig(_workPath);
		configureLogging("INFO", true);
		log4cpp::Category& rlog = log4cpp::Category::getRoot();
		rlog << log4cpp::Priority::INFO << "Start!";
		_digits.clear();
		_img = cv::imread(_imagePath.c_str());
		cvtColor(_img, _imgGray, CV_BGR2GRAY);
		//cv::equalizeHist(_imgGray, _imgGray);
		
		//rotate(_config.getRotationDegrees());
		
		float skew_deg = detectSkew2(); //получение угла повора картинки в градусах
		rotate(skew_deg); // попорот картинки
		std::vector<std::vector<IndicationNumber>> iNums = findCountersandGetiNums(); // итеративный поиск контуров и упорядочивание их
		std::vector<std::vector<char>> rNums = fullRecognize(iNums); //распознование каждого контура
		std::string answer = rNumsAnalyse(rNums); // выборка из всех полученных результатов. генерация лучшего из них
		std::cout << answer;
		char * writablee = new char[answer.size() + 1];
		std::copy(answer.begin(), answer.end(), writablee);
		writablee[answer.size()] = '\0';
		return writablee;



		if (_config.getFullDebugOn())
		{
			cv::imwrite(_workPath + _imageFileName + "_getMeterValue_lines.jpg", _img);
			cv::imwrite(_workPath + _imageFileName + "_getMeterValue_rotated.jpg", _imgGray);
			cv::imwrite(_workPath + _imageFileName + "_getMeterValue_equalizeHist.jpg", _imgEqualized);
		}
		findCounterDigits();
		//findCounterDigitsManyParams();
		KNearestOcr ocr(_config);
		
		if (!ocr.loadTrainingData()) {
			std::cout << "Failed to load OCR training data\n"; 
			return "Error: не удалось загрузить данные для тренировки";
		}
		
		std::cout << "OCR training data loaded.\n";
		std::cout << "<q> to quit.\n";
		std::string result = ocr.recognize(_digits);
		std::cout << result;

	
		char * writable = new char[result.size() + 1];
		std::copy(result.begin(), result.end(), writable);
		writable[result.size()] = '\0';
		return writable;
	}


	void myclass::learn() {

		_config.loadConfig(_workPath);
		configureLogging("INFO", true);
		log4cpp::Category& rlog = log4cpp::Category::getRoot();
		rlog << log4cpp::Priority::INFO << "Start!";
		_digits.clear();
		_img = cv::imread(_imagePath.c_str());
		cvtColor(_img, _imgGray, CV_BGR2GRAY);

		rotate(_config.getRotationDegrees());

		float skew_deg = detectSkew2();
		rotate(skew_deg);
		if (_config.getFullDebugOn())
		{
			cv::imwrite(_workPath + _imageFileName + "getMeterValue_lines.jpg", _img);
			cv::imwrite(_workPath + _imageFileName + "getMeterValue_rotated.jpg", _imgGray);
		}
		findCounterDigits();


		KNearestOcr ocr(_config);
		if (!ocr.loadTrainingData()) {
			std::cout << "Failed to load OCR training data\n";
			return;
		}
		std::cout << "OCR training data loaded.\n";
		std::cout << "<q> to quit.\n";
		int key = 0;
		key = ocr.learn(_digits);
		std::cout << std::endl;

		if (key == 'q' || key == 's') {
			std::cout << "Quit\n";
			//break;
		}

		ocr.saveTrainingData();
	}

	std::string myclass::rNumsAnalyse(std::vector<std::vector<char>> rNums) {
		std::string result = "";
		for each (std::vector<char> rNum in rNums)
		{
			//счетчик частоты появления цифры
			int cur[10] = { 0,0,0,0,0,0,0,0,0,0 };

			for (int i = 0; i < rNum.size(); i++)
			{
				if (rNum[i] != '?') cur[rNum[i] - '0']++;
			}
			int maxV = 0;
			int maxIt = -1;
			//поиск самой часто встречающейся цифры
			for (int i = 0; i < 10; i++)
			{
				if (cur[i] > maxV)
				{
					maxV = cur[i];
					maxIt = i;
				}
			}
			//выводим тольно
			if (maxIt != -1) result += maxIt + '0';
		}
		return result;
	}

	//При дальнейших расчетах надо учитывать, что мы не поворачиваем окружность и для анализа взаимного расположения окружности и 
	float myclass::getMeterCircleRadius() {

		return min(_img.rows / 4, _img.cols / 4);

		int sensitivity = 100;
		int low_b = 0, low_g = 0, low_r = 255 - sensitivity;
		int high_b = 255, high_g = sensitivity, high_r = 255;


		_config.loadConfig(_workPath);	
		configureLogging("INFO", true);
		log4cpp::Category& rlog = log4cpp::Category::getRoot();

		rlog << log4cpp::Priority::INFO << "Start findMeterCircle";
		cv::Mat _img = cv::imread(_imagePath.c_str());

		cv::Mat _imgHSV;
		cvtColor(_img, _imgHSV, CV_BGR2HSV);
		cv::Mat frame_threshold;
		cv::inRange(_imgHSV, cv::Scalar(low_b, low_g, low_r), cv::Scalar(high_b, high_g, high_r), frame_threshold);
		if (_config.getFullDebugOn())
		{
			cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_inRange.jpg", frame_threshold);
		}
		cv::erode(frame_threshold, frame_threshold, NULL, cv::Point(-1, -1), 2);
		if (_config.getFullDebugOn())
			cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_erode.jpg", frame_threshold);
		cv::dilate(frame_threshold, frame_threshold, NULL, cv::Point(-1, -1), 2);
		if (_config.getFullDebugOn())
			cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_dilate.jpg", frame_threshold);		
		//GaussianBlur(gray, gray, cv::Size(33, 33), 8, 8);
		if (_config.getFullDebugOn()) {
			cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_gray.jpg", frame_threshold);
		}

		cv::Mat edges;
		// detect edges
		cv::Canny(frame_threshold, edges, _config.getCannyThreshold1Lines(), _config.getCannyThreshold2Lines());
		
		
		if (_config.getFullDebugOn()) {
			cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_edges.jpg", edges);
		}
		
		std::vector<std::vector<cv::Point> > contours, filteredContours;
		std::vector<cv::Rect> boundingBoxes;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		if (contours.size() > 0)
		{
			std::sort(contours.begin(), contours.end(), sortRectByXY());
			cv::Point2f center;
			float radius;
			cv::minEnclosingCircle(contours[0], center, radius);
			cv::circle(_img, center, radius, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
			if (_config.getDebugOn()) {
				cv::imwrite(_workPath + _imageFileName + "_findMeterCircle_result.jpg", _img);
			}

			return min(radius, _img.rows / 2, _img.cols / 2);
		}
		return min(_img.rows / 2, _img.cols / 2);
	}

	/*
	void myclass::process() {		
		_config.loadConfig();
		configureLogging("INFO", true);
		log4cpp::Category& rlog = log4cpp::Category::getRoot();

		rlog << log4cpp::Priority::INFO << "Start!";


		_digits.clear();


		//std::string path = "C:\\Users\\gudim\\Documents\\Visual Studio 2015\\Projects\\TestOpenCV\\TestOpenCV\\bin\\Debug\\Sources\\img_0524.jpg";

		_img = cv::imread(_path.c_str());

		// convert to gray
		cvtColor(_img, _imgGray, CV_BGR2GRAY);


		//int low_r = 6, low_g = 86, low_b = 29;
		//int high_r = 255, high_g = 255, high_b = 64;
		int sensitivity = 100;
		int low_b = 0, low_g = 0, low_r = 255 - sensitivity ;
		int high_b = 255, high_g = sensitivity, high_r = 255;
		//int low_b = 29, low_g = 86, low_r = 6;
		//int high_b = 64, high_g = 255, high_r = 255;



		cv::Mat white;
		//cvtColor(_img, white, CV_BGR2HSV);
		cvtColor(_img, white, CV_BGR2HSV);
		cv::imwrite(_path + "_circle_white1.jpg", white);
		cv::Mat frame_threshold;
		cv::inRange(white, cv::Scalar(low_b, low_g, low_r), cv::Scalar(high_b, high_g, high_r), frame_threshold);
		cv::imwrite(_path + "_circle_white2.jpg", white);
		cv::imwrite(_path + "_circle_white3.jpg", frame_threshold);
		cv::erode(frame_threshold, frame_threshold, NULL, cv::Point(-1, -1),2);
		cv::imwrite(_path + "_circle_white4.jpg", frame_threshold);
		cv::dilate(frame_threshold, frame_threshold, NULL, cv::Point(-1, -1), 2);
		cv::imwrite(_path + "_circle_white5.jpg", frame_threshold);

		_imgGray = frame_threshold;
	
		cv::Mat gray;
		cvtColor(_img, gray, CV_BGR2GRAY);
		GaussianBlur(gray, gray, cv::Size(33, 33), 8, 8);

		cv::imwrite(_path + "_circle_prepare.jpg", gray);

		
		//http://www.pyimagesearch.com/2015/09/14/ball-tracking-with-opencv/

		
		std::vector<cv::Vec3f> circles;
		cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 2,
			gray.rows/2 , // change this value to detect circles with different distances to each other
			30, 10, min(gray.rows / 4, gray.cols / 4), min(gray.rows / 2, gray.cols / 2) // change the last two parameters
						   // (min_radius & max_radius) to detect larger circles
			);

		

		for (size_t i = 0; i < circles.size(); i++)
		{
			cv::Vec3i c = circles[i];
			cv::circle(gray, cv::Point(c[0], c[1]), c[2], cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
			cv::circle(gray, cv::Point(c[0], c[1]), 2, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
		}
		cv::imwrite(_path + "_circle.jpg", gray);

		//cv::imwrite(_path + "_0_gray.jpg", _imgGray);

		// initial rotation to get the digits up
		rotate(_config.getRotationDegrees());
		//rotate(0);		

		// detect and correct remaining skew (+- 30 deg)
		float skew_deg = detectSkew();
		rotate(skew_deg);
		cv::imwrite(_path + "_1_lines.jpg", _img);

		cv::imwrite(_path + "_2_rotated.jpg", _imgGray);

		

		findCounterDigits();

		//cv::imwrite(_path + "_5_rectangle.jpg", _imgGray);

		//cv::imwrite(_path + "_gidits.jpg", _img);
		//cv::imwrite(_path + "_giditsGray.jpg", _imgGray);
		//learnOcr();

		//process2();
	}*/

	
	void myclass::process2() {

		_config.loadConfig(_workPath);
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		std::ofstream outfile("C:\\Temp\\test.txt");
		outfile << "my directory is " << std::string(buffer).substr(0, pos) << "\n";
		outfile << "getTrainingDataFilename " << _config.getTrainingDataFilename() << "\n";
		outfile << "my text here!" << std::endl;
		outfile.close();
	}
	/*
		KNearestOcr ocr(config);
		if (!ocr.loadTrainingData()) {
			std::cout << "Failed to load OCR training data\n";
			return;
		}
		std::cout << "OCR training data loaded.\n";
		std::cout << "<q> to quit.\n";
		std::string result = ocr.recognize(_digits);
		std::cout << result;

	}
	*/

	 void  myclass::learnOcr() {
		 MConfig config;
		 config.loadConfig(_workPath);
		 configureLogging("INFO", true);
		 log4cpp::Category& rlog = log4cpp::Category::getRoot();

		 rlog << log4cpp::Priority::INFO << "Start process2!";

		 KNearestOcr ocr(config);
		 if (!ocr.loadTrainingData()) {
			 std::cout << "Failed to load OCR training data\n";
			 return;
		 }
		 std::cout << "OCR training data loaded.\n";
		 std::cout << "<q> to quit.\n";
		 int key = 0;
		 key = ocr.learn(_digits);
		 std::cout << std::endl;

		 if (key == 'q' || key == 's') {
			 std::cout << "Quit\n";
			 //break;
		 }

		 ocr.saveTrainingData();
	}

	void myclass::configureLogging(const std::string & priority = "INFO", bool toConsole = false) {
		log4cpp::Appender *fileAppender = new log4cpp::FileAppender("default", _workPath + "meter.log");
		log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
		layout->setConversionPattern("%d{%d.%m.%Y %H:%M:%S} %p: %m%n");
		fileAppender->setLayout(layout);
		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.setPriority(log4cpp::Priority::getPriorityValue(priority));
		root.addAppender(fileAppender);
		if (toConsole) {
			log4cpp::Appender *consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
			consoleAppender->setLayout(new log4cpp::SimpleLayout());
			root.addAppender(consoleAppender);
		}
	}


	/**
	* Поворот картинки
	*/
	void myclass::rotate(double rotationDegrees) {
		cv::Mat M = cv::getRotationMatrix2D(cv::Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
		cv::Mat img_rotated;
		cv::warpAffine(_imgGray, img_rotated, M, _imgGray.size());
		_imgGray = img_rotated;
		if (_debugWindow) {
			cv::warpAffine(_img, img_rotated, M, _img.size());
			_img = img_rotated;
		}
	}
	/**
	* Отрисока линий на картинке
	*/
	void myclass::drawLines(std::vector<cv::Vec2f>& lines) {
		// draw lines
		for (size_t i = 0; i < lines.size(); i++) {
			float rho = lines[i][0];
			float theta = lines[i][1];
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
			cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
			cv::line(_img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
		}
	}
	void myclass::drawLines(std::vector<cv::Vec4i>& lines, int xoff, int yoff) {
		for (size_t i = 0; i < lines.size(); i++) {
			cv::line(_img, cv::Point(lines[i][0] + xoff, lines[i][1] + yoff),
				cv::Point(lines[i][2] + xoff, lines[i][3] + yoff), cv::Scalar(255, 0, 0), 1);
		}
	}
	void myclass::drawLines(std::vector<cv::Vec4i>& lines, bool flag)
		{
			// draw lines
			for (size_t i = 0; i < lines.size(); i++)
			{
				line(_img,
					cv::Point(lines[i][0], lines[i][1]),
					cv::Point(lines[i][2], lines[i][3]),
					cv::Scalar(0, 0, 255), 2);
			}
		}
	/**
	* Определение поворота картинки (+- 30 градусов) по горизонтали.
	*/
	float myclass::detectSkew() {
		log4cpp::Category& rlog = log4cpp::Category::getRoot();

		cv::Mat edges;
		cv::Canny(_imgGray, edges, _config.getCannyThreshold1Lines() , _config.getCannyThreshold2Lines());
		cv::imwrite(_workPath + _imageFileName + "_getMeterValue_lines_edges.jpg", edges);


		// находим линии
		std::vector<cv::Vec2f> lines;
		cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);


		// фильтруем линии и находим среднее
		std::vector<cv::Vec2f> filteredLines;
		float theta_min = 60.f * CV_PI / 180.f;
		float theta_max = 120.f * CV_PI / 180.0f;
		float theta_avr = 0.f;
		float theta_deg = 0.f;
		int sz = lines.size();
		for (size_t i = 0; i < lines.size(); i++)
		{
			float theta = lines[i][1];
			if (theta >= theta_min && theta <= theta_max )
			{
				filteredLines.push_back(lines[i]);
				theta_avr += theta;
			}
		}
		if (filteredLines.size() > 0) {
			theta_avr /= filteredLines.size();
			theta_deg = (theta_avr / CV_PI * 180.f) - 90;
			rlog.info("detectSkew: %.1f deg", theta_deg);
		}
		else {
			rlog.warn("failed to detect skew");
		}

		if (_debugSkew) {
			drawLines(filteredLines);
		}

		return theta_deg;
	}
	float myclass::detectSkew2()
	{
		log4cpp::Category& rlog = log4cpp::Category::getRoot();

		cv::Mat edges;
		cv::Canny(_imgGray, edges, _config.getCannyThreshold1Lines(), _config.getCannyThreshold2Lines());
		cv::imwrite(_workPath + _imageFileName + "_getMeterValue_lines_edges.jpg", edges);

		// находим линии
		std::vector<cv::Vec4i> lines;
		cv::Size s = _img.size();
		int minLineSize = min(s.height, s.width) *  _config.getMinLine_IsPercentageOfMinDimentionOfImage() / 100;
		HoughLinesP(edges, lines, 1, CV_PI / 180.f, 140, minLineSize, _config.getMaxLineGap());// нужно подумать насчет Gap

		// фильтруем линии
		std::vector<cv::Vec4i> filteredLines;
		float min_horise_degree_in_radians = 30.f * CV_PI / 180.f;
		float angle_avr = 0.f;
		float angle_deg = 0.f;
		for (size_t i = 0; i < lines.size(); i++)
		{		//		  x1		  y1			x2			y2  http://docs.opencv.org/3.2.0/dd/d1a/group__imgproc__feature.html#ga8618180a5948286384e3b7ca02f6feeb
				//(lines[i][0], lines[i][1]) (lines[i][2], lines[i][3])
			float x1 = lines[i][0]; float x2 = lines[i][2]; float y1 = lines[i][1]; float y2 = lines[i][3];

			//угловой коэффициент == тангенсу угла наклона прямой.
			float angle_koeff = (y2 - y1) / (x2 - x1);
			//угол поворота прямой
			float angle = atan(angle_koeff);

			if (fabs((double)angle) < min_horise_degree_in_radians)
			{
				filteredLines.push_back(lines[i]);
				angle_avr += angle;
			}
		}

		if (filteredLines.size() > 0) {
			angle_avr /= filteredLines.size();
			angle_deg = (angle_avr * 180.f / CV_PI);// -90;
			rlog.info("detectSkew: %.1f deg", angle_deg);
		}
		else
		{
			rlog.warn("failed to detect skew");
		}
		drawLines(filteredLines, true);
		cv::imwrite(_workPath + _imageFileName + "_getMeterValue_lines.jpg", _img);
		return angle_deg;
	}

	/*
	http://stackoverflow.com/questions/4292249/automatic-calculation-of-low-and-high-thresholds-for-the-canny-operation-in-open
	https://habrahabr.ru/post/114589/
	*/

	/**
	* Поиск прямоугольников, отсортированных по Y
	*/
	void myclass::findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
		std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result, float meterCircleRadius) {
		std::vector<cv::Rect>::const_iterator it = begin;
		cv::Rect start = *it;
		++it;
		result.push_back(start);
		for (; it != end; ++it) {		
			if (abs(start.y - it->y) <  start.height * 0.4/* _config.getDigitYAlignment()*/
				&& abs(start.height - it->height) <  start.height * 0.2
				&& abs(start.width - it->width) <  start.width * 0.2) {
				result.push_back(*it);
			}
		}
	}

	/**
	* Фильтрация контуров по размеру ограничивающих прямоугольников

	Проход по иерархии
	http://stackoverflow.com/questions/20492152/how-to-find-no-of-inner-holes-using-cvfindcontours-and-hierarchy
	*/
	void myclass::filterContours(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy,
		std::vector<cv::Rect>& boundingBoxes, std::vector<std::vector<cv::Point> >& filteredContours, float meterCircleRadius) {
		// filter contours by bounding rect size

		/*_config.getDigitMinHeight()*/
		/*_config.getDigitMaxHeight()*/
		for (size_t i = 0; i < contours.size(); i ++) {
			cv::Rect bounds = cv::boundingRect(contours[i]);
			if ( bounds.height > 2*meterCircleRadius*0.02 && bounds.height < 2*meterCircleRadius * 0.35 
				&& bounds.width >  2*meterCircleRadius * 0.02 && bounds.width < bounds.height*0.5 && bounds.width>bounds.height*0.2)
		/*	if (
				bounds.height > 50 
				&&
				bounds.height <= 150
				&& bounds.width > 50 && bounds.width < bounds.height
				) */
			{
				boundingBoxes.push_back(bounds);
				filteredContours.push_back(contours[i]);
			}
		}
	}

	std::vector<std::vector<IndicationNumber>> myclass::findCountersandGetiNums(){
		const int n = 5; //кол-во итераций
		//коэффициенты для Canny
		int thrs1[n] = { 100,80,50,30,90 }; 
		int thrs2[n] = { 200,120,100,80,160 };
		std::vector<std::vector<IndicationNumber>> iNums;
		
		for (int i = 0; i < n; i++)
		{
			_digits.clear();
			_numGrayImgages.clear();
			
			_stringID = std::to_string(i); //не забудь инициализировать _stringID для логирования картинок
			std::vector<cv::Rect> rects = findCounterDigits(thrs1[i], thrs2[i]); // поиск контуров - прямоугольников
			std::vector<int> averages;

			for (int k = 0; k < rects.size(); k++) 
			{
				int existRectID = -1;
				//ищем уже добавленный iNum
				for (int j = 0; j < iNums.size(); j++)
				{
					int average_x = getiNumAverageX(iNums[j]);
					if (abs(average_x - rects[k].x) < 7) //sdasd
						existRectID = j;
				}
				//если такого iNum в списке iNums еще нет
				if (existRectID != -1) 
					iNums[existRectID].push_back(IndicationNumber(i, _numGrayImgages[k], _digits[k], rects[k]));
				else
				//устанавливаем новый на свое место так, чтобы сохранялась упорядоченность по х
				{
					std::vector<IndicationNumber> newINum;
					newINum.push_back(IndicationNumber(i, _numGrayImgages[k], _digits[k], rects[k]));

					int cnt = 0;
					for (cnt = 0; cnt < iNums.size() ; cnt++)
					{
						int average_x = getiNumAverageX(iNums[cnt]);

						if (rects[k].x <= average_x) break;
					}
					iNums.insert(iNums.begin() + cnt, newINum);
				}
			}
 		}
		//распознаем и выводим результат в дебаге
		if (_config.getDebugOn()) coutRecognizediNums(iNums);
		return iNums;
	}

	std::vector<std::vector<char>> myclass::fullRecognize(std::vector<std::vector<IndicationNumber> > iNums) {
		KNearestOcr ocr(_config);

		if (!ocr.loadTrainingData()) 
			std::cout << "Failed to load OCR training data\n";
		else
			std::cout << "OCR training data loaded.\n";
	
		std::vector<std::vector<char>> rNums;
		for each (std::vector<IndicationNumber> iNum in iNums)
		{
			std::vector<char> rNum;
			for each (IndicationNumber inum in iNum)
			{
				rNum.push_back(ocr.recognize(inum._numEdge));
			}
			rNums.push_back(rNum);
		}
		return rNums;
	}

	void myclass::coutRecognized(std::vector<std::vector<char>> rNums) {
		for each (std::vector<char> rNum in rNums)
		{
			for each (char rnum in rNum)
			{
				std::cout << rnum;
			}
			std::cout << std::endl;
		}
	}

	void myclass::coutRecognizediNums(std::vector<std::vector<IndicationNumber> > iNums) {

		KNearestOcr ocr(_config);

		if (!ocr.loadTrainingData())
			std::cout << "Failed to load OCR training data\n";
		else
			std::cout << "OCR training data loaded.\n";

		int i = 0;
		for each (std::vector<IndicationNumber> iNum in iNums)
		{
			for each (IndicationNumber inum in iNum)
			{
				std::cout << ocr.recognize( inum._numEdge) << "  ";
				cv::imwrite(_workPath + _imageFileName + std::to_string(i) + "_IndicationNumber.jpg", inum._numEdge);
				i++;
			}
			std::cout << std::endl;
		}
	}

	int myclass::getiNumAverageX(std::vector<IndicationNumber> iNum) {
		int average_x = 0;
		for each (IndicationNumber inum in iNum)
		{
			average_x += inum._numAlignedBoundingBox.x;
		}
		average_x /= iNum.size();
		return average_x;
	}
	/**
	* Поиск контуров цифр
	*/
	std::vector<cv::Rect> myclass::findCounterDigits(int cannyThreshld1, int cannyThreshld2) {
		//log4cpp::Category& rlog = log4cpp::Category::getRoot();

		// edge image
		cv::Mat edges;
		cv::Mat curGray = _imgGray.clone();

		if (cannyThreshld1 == 0 && cannyThreshld2 == 0)
			cv::Canny(curGray, edges, _config.getCannyThreshold1Digits(), _config.getCannyThreshold2Digits());
		else 
			cv::Canny(curGray, edges, cannyThreshld1, cannyThreshld2);

		if (_config.getFullDebugOn()) {
			//cv::imshow("edges", edges);
			cv::imwrite(_workPath + _imageFileName
				+ "_" + _stringID + "_" +  
				"_getMeterValue_edges.jpg", edges);
		}		

		cv::Mat img_ret = edges.clone();

		// find contours in whole image
		std::vector<std::vector<cv::Point> > contours, filteredContours;
		std::vector<cv::Rect> boundingBoxes;
		std::vector<cv::Vec4i> hierarchy;
		//cv::findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//cv::findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		//cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	


		float radius = getMeterCircleRadius();
		// filter contours by bounding rect size
		filterContours(contours, hierarchy, boundingBoxes, filteredContours, radius);

		//rlog << log4cpp::Priority::INFO << "number of filtered contours: " << filteredContours.size();

		// find bounding boxes that are aligned at y position
		std::vector<cv::Rect> alignedBoundingBoxes, tmpRes;
		for (std::vector<cv::Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
			tmpRes.clear();
			findAlignedBoxes(ib, boundingBoxes.end(), tmpRes, radius);

			if (tmpRes.size() >= 2)
			{
				cv::Mat _imgtemp = curGray.clone();

				for (int i = 0; i < tmpRes.size(); ++i)
				{
					cv::rectangle(_imgtemp, tmpRes[i], cv::Scalar(255), 2);
				}				
				//cv::imshow("Show boxes", _imgtemp);
				int key = cv::waitKey(0);				

			}


			if (tmpRes.size()>=4 && tmpRes.size()<=8 && 
				(alignedBoundingBoxes.size() == 0 ||
				tmpRes.size() > alignedBoundingBoxes.size() && (tmpRes[0].height -alignedBoundingBoxes[0].height)>-alignedBoundingBoxes[0].height*0.2 ||
				tmpRes.size() <= alignedBoundingBoxes.size() && (tmpRes[0].height - alignedBoundingBoxes[0].height)>alignedBoundingBoxes[0].height*0.2))			 
				alignedBoundingBoxes = tmpRes;
			
		}
		//rlog << log4cpp::Priority::INFO << "max number of alignedBoxes: " << alignedBoundingBoxes.size();

		// sort bounding boxes from left to right
		std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());

		if (_config.getFullDebugOn()) {
			// draw contours
			cv::Mat cont = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
			cv::drawContours(cont, filteredContours, -1, cv::Scalar(300,0,0));
			//cv::imshow("contours", cont);
			cv::imwrite(_workPath + _imageFileName
				+ "_" + _stringID + "_" +
				"_getMeterValue_contours.jpg", cont);
		}

		// cut out found rectangles from edged image
		for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
			cv::Rect roi = alignedBoundingBoxes[i];
			_digits.push_back(img_ret(roi));
			_numGrayImgages.push_back(curGray(roi));
			
			if (_config.getFullDebugOn()) {
				cv::rectangle(curGray, roi, cv::Scalar(255), 2);
			}
		}
		if (_config.getFullDebugOn())
			cv::imwrite(_workPath + _imageFileName
				+ "_" + _stringID + "_" +
				"_getMeterValue_alignedBoundingBoxes.jpg", curGray);

		return alignedBoundingBoxes;
	}

}
