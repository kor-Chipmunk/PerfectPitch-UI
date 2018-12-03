#include"stdafx.h"

#include"linearScore.h"

linearScore::linearScore(cv::Mat image, vector<int>& line, bool& EoI) {
	for (int i = 0; i < 5; i++) {
		fiveLine[0][i] = line.at(index * 10 + i);
		fiveLine[1][i] = line.at(index * 10 + 5 + i);
	}
	cv::Mat right(line.at(index * 10 + 4) - line.at(index * 10) + (line.at(index*10+5) - line.at(index*10+4)), image.cols, CV_8UC3);
	cv::Mat left(line.at(index * 10 + 9) - line.at(index * 10 + 5) + (line.at(index*10+5) - line.at(index*10+4)), image.cols, CV_8UC3);

	oneline[0] = right;
	oneline[1] = left;

	for (int i = 0; i < right.rows; i++) {
		for (int j = 0; j < right.cols; j++) {
			oneline[0].at<cv::Vec3b>(i, j)[0] = image.at<cv::Vec3b>(line.at(index * 10) - (line.at(index*10+5) - line.at(index * 10 + 4)) / 2 + i, j)[0];
			oneline[0].at<cv::Vec3b>(i, j)[1] = image.at<cv::Vec3b>(line.at(index * 10) - (line.at(index * 10 + 5) - line.at(index * 10 + 4)) / 2 + i, j)[1];
			oneline[0].at<cv::Vec3b>(i, j)[2] = image.at<cv::Vec3b>(line.at(index * 10) - (line.at(index * 10 + 5) - line.at(index * 10 + 4)) / 2 + i, j)[2];
		}
	}
	for (int i = 0; i < left.rows; i++) {
		for (int j = 0; j < left.cols; j++) {
			oneline[1].at<cv::Vec3b>(i, j)[0] = image.at<cv::Vec3b>(line.at(index * 10 + 5) - (line.at(index * 10 + 5) - line.at(index * 10 + 4)) / 2 + i, j)[0];
			oneline[1].at<cv::Vec3b>(i, j)[1] = image.at<cv::Vec3b>(line.at(index * 10 + 5) - (line.at(index * 10 + 5) - line.at(index * 10 + 4)) / 2 + i, j)[1];
			oneline[1].at<cv::Vec3b>(i, j)[2] = image.at<cv::Vec3b>(line.at(index * 10 + 5) - (line.at(index * 10 + 5) - line.at(index * 10 + 4)) / 2 + i, j)[2];
		}
	}
	
	int k = line.at(index * 10) - (line.at(index * 10 + 5) - line.at(index * 10 + 4))/2;
	int l = line.at(index * 10 + 5) - (line.at(index * 10 + 5) - line.at(index * 10 + 4))/2;
	for (int i = 0; i < 5; i++)
	{
		line.at(index * 10 + i) -= k;
		line.at(index * 10 + 5 + i) -= l;
	}

	index++;
	if (index*10+5>=line.size())
	{
		EoI = true;
	}
}