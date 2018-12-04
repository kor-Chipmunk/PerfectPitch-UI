#pragma once
#include<iostream>
#include<opencv2\opencv.hpp>
#include"Score.h"
#include"string"

using namespace std;

class ScoreProcessor
{
	/*
	static int RightRecurIndex;
	static int LeftRecurIndex;
	static bool jumpState;
	static int jumpIndex;
	*/

	Accords Notes;
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
	cv::Mat Point;
	cv::Mat Sharp;
	cv::Mat Flat;
	cv::Mat Natural;
	cv::Mat Rest_4;
	cv::Mat Rest_8;
	int Hofhead;
	int Wofhead;
	int HofPoint;
	int WofPoint;
	int HofSFN;
	int WofSFN;
	int HofRest_4;
	int WofRest_4;
	int HofRest_8;
	int WofRest_8;
	int a;
	int b;
	int aofRest_4;
	int bofRest_4;
	int aofRest_8;
	int bofRest_8;
	//int LUT_linear[2][20] = {
	//	{41,39,37,36,34,32,31,29,27,25,24,22,20,19,17,15,13,12,10,8},
	//	{20,19,17,15,13,12,10,8,7,5,3,1,0,-2,-4,-5,-7,-9,-11,-12} };
	int LUT_linear[2][20] = {
		{ 41,39,37,36,34,32,30,29,27,25,24,22,20,18,17,15,13,12,10,8 },
	{ 20,18,17,15,13,12,10,8,6,5,3,1,0,-2,-4,-6,-7,-9,-11,-12 } };
	int LUT_original[2][20];

	bool TemplateMatch(cv::Mat tem, int x, int y, int width, int height, double rate, int p, int c);
	int DetectBeat(int x, int y);
	bool isShape_UP(int x, int y, int&XofLine, bool& isNotail);
	void DetectBeat_sub(int& count, int XofLine, int Ry, int k, bool Up);
	int DetectPitch(double y);
	bool DetectPoint(int x, int y, int p, int c);
	int Accidental(int x, int y);
	void RevisePitch(double y, int Case);
	void DetectScale();
public:
	ScoreProcessor(cv::Mat& image, Score* score, int* lineArr, int LUT_original[][20], int RL);
	void DetectNote();
};