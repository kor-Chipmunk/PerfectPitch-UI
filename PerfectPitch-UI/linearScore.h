#pragma once
#include<vector>
#include<opencv2/opencv.hpp>

using namespace std;

class linearScore {
public:
	static int index;
	cv::Mat oneline[2];
	int fiveLine[2][5];
	linearScore(cv::Mat image, vector<int>& line, bool& Eoi);
	linearScore() {};
};