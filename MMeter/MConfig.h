#pragma once

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

class MConfig
{
public:
	MConfig();
	void saveConfig();
	void loadConfig(std::string workPath);	

	
	bool getDebugOn() const {
		return _debugOn;
	}

	bool getFullDebugOn() const {
		return _fullDebugOn;
	}

	int getDigitMaxHeight() const {
		return _digitMaxHeight;
	}

	int getDigitMinHeight() const {
		return _digitMinHeight;
	}

	int getDigitYAlignment() const {
		return _digitYAlignment;
	}

	std::string getTrainingDataFilename() const {
		return _workPath + _trainingDataFilename;
	}

	float getOcrMaxDist() const {
		return _ocrMaxDist;
	}

	float getMinLine_IsPercentageOfMinDimentionOfImage() const{
		return _minLine_IsPercentageOfMinDimentionOfImage;
	}

	float getMaxLineGap() const {
		return _maxLineGap;
	}

	int getRotationDegrees() const {
		return _rotationDegrees;
	}

	int getCannyThreshold1Lines() const {
		return _cannyThreshold1Lines;
	}

	int getCannyThreshold2Lines() const {
		return _cannyThreshold2Lines;
	}

	int getCannyThreshold1Digits() const {
		return _cannyThreshold1Digits;
	}

	int getCannyThreshold2Digits() const {
		return _cannyThreshold2Digits;
	}

	std::vector<int> getTestsQuantity() {
		std::vector<int> res;
		std::string podstr = "";
		for (int i = 0; i < _testsQuantity.size(); i++)
		{
			if (_testsQuantity[i] != ' ')
			{
				podstr += _testsQuantity[i];
			}
			else
			{
				res.push_back(std::stoi(podstr));
				podstr = "";
			}
		}
		return res;
	}

	void setTestsQuantity(std::vector<int> VecStr) {
		std::string tq = "";

		for each (int item in VecStr)
		{
			tq += std::to_string(item) + " ";
		}
		
		_testsQuantity = tq;
	}


	~MConfig();

private:
	bool _debugOn;
	bool _fullDebugOn;
	int _rotationDegrees;
	float _ocrMaxDist;
	float _minLine_IsPercentageOfMinDimentionOfImage;
	float _maxLineGap;
	int _digitMinHeight;
	int _digitMaxHeight;
	int _digitYAlignment;
	int _cannyThreshold1Digits;
	int _cannyThreshold2Digits;
	int _cannyThreshold1Lines;
	int _cannyThreshold2Lines;
	std::string _trainingDataFilename;
	std::string _testsQuantity;

	std::string _workPath;
};

#endif /* CONFIG_H_ */
