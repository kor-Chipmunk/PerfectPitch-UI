#pragma once
#include<iostream>
#include<opencv2\opencv.hpp>
#include"Score.h"

using namespace std;

class ScoreProcessor
{
	int RL;
	Score* score;
	cv::Mat image;
	int* lineArr;
	double interval;
	int row;
	int col;
	cv::Mat head_fill;
	cv::Mat head_empty;
	cv::Mat head_empty_line;
	int Hofhead;
	int Wofhead;
	int a;
	int b;
	int LUT[2][20] = { {29,27,25,24,22,20,18,17,15,13,12,10,8,6,5,3,1,0,-2,-4},
						{8,6,5,3,1,0,-2,-4,-6,-7,-9,-11,-12,-14,-16,-18,-19,-21,-23,-24} };


	bool TemplateMatch(cv::Mat tem, int x, int y, double rate, int c);
	int DetectBeat(int x, int y);
	bool isShape_UP(int x, int y, int&XofLine, bool& isNotail);
	void DetectBeat_sub(int& count, int XofLine, int Ry, int k, bool Up);
	int DetectPitch(int y);
public:
	ScoreProcessor(cv::Mat& image, Score* score, int* lineArr, int RL);
	void DetectNote();
};