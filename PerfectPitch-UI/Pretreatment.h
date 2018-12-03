#pragma once

#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>

using namespace std;

class Pretreatment
{
public:
	static void Binarization(cv::Mat& image, uchar threshold);
	static cv::Mat Erode(const cv::Mat& image, int kSize);
	static cv::Mat Dilate(const cv::Mat& image);
	static void DetectLine(cv::Mat& image, vector<int>& lineArr);
	static void RemoveDup(vector<int>& lineArr);
	static void DeterScale(cv::Mat image);
	static void MakeLUP()
	{
		int LUP[100][2] = { 0, };
		/*
		
		for문 돌면서 채우는데
		LUP[1] = 1

		*/
	}
};