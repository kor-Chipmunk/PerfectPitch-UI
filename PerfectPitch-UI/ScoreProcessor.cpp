#include "stdafx.h"

#include"ScoreProcessor.h"
#include"Accord.h"

using namespace std;

ScoreProcessor::ScoreProcessor(cv::Mat& image, Score* score, int* lineArr, int RL)
{
	this->image = image;
	this->score = score;
	this->lineArr = lineArr;
	this->interval = (double)(lineArr[4] - lineArr[0]) / (double)8;
	this->row = image.rows;
	this->col = image.cols;
	this->head_fill = cv::imread("Head_fill.jpg");
	this->head_empty = cv::imread("Head_empty.jpg");
	this->head_empty_line = cv::imread("Head_empty_line.jpg");
	this->RL = RL;

	this->Hofhead = head_fill.rows;
	this->Wofhead = head_fill.cols;
	a = 0; b = 0;

	for (int x = 0; x < Wofhead; x++)
	{
		if (head_fill.at<cv::Vec3b>(0, x)[0] == 0)
		{
			a = x;
			break;
		}
	}
	for (int y = 0; y < Hofhead; y++)
	{
		if (head_fill.at<cv::Vec3b>(y, 0)[0] == 0)
		{
			b = y;
			break;
		}
	}
}

bool ScoreProcessor::TemplateMatch(cv::Mat tem, int x, int y, double rate, int c)
{
	int count = 0;
	for (int i = 0; i < Wofhead; i++)
	{
		for (int j = 0; j < Hofhead; j++)
		{
			if (abs((int)image.at<cv::Vec3b>(y + j, x + i)[c] - (int)tem.at<cv::Vec3b>(j, i)[c])<250)
				count++;
		}
	}
	double MatchRate = (double)count / (double)(Wofhead*Hofhead);

	if (MatchRate >= rate) //rate이상 일치 시 detect
	{
		for (int i = 0; i < Wofhead; i++)
		{
			for (int j = 0; j < Hofhead; j++)
			{
				if (image.at<cv::Vec3b>(y + j, x + i)[c] == 0)
				{
					image.at<cv::Vec3b>(y + j, x + i)[c] = 255;
				}
			}
		}
		return true;
	}
	else return false;
}

void ScoreProcessor::DetectNote()
{
	Accords Notes;
	for (int x = 0; x < col - Wofhead; x++)
	{
		int count = 0;
		for (int y = b; y < row - (Hofhead - b); y++)
		{
			if (image.at<cv::Vec3b>(y, x)[0] == 0)
			{
				if (image.at<cv::Vec3b>(y + 1, x)[0] == 0)
				{
					//if (image.at<cv::Vec3b>(y + 2, x)[0] == 0)
					{
						if (image.at<cv::Vec3b>(y - b, x)[0] == 255)
						{
							if (image.at<cv::Vec3b>(y - b + Hofhead, x + Wofhead)[0] == 255)
							{
								if (image.at<cv::Vec3b>(y - b + Hofhead / 2, x + Wofhead / 2)[1] == 0)
								{
									if (TemplateMatch(head_fill,x, y - b, 0.90, 2))//head 차있는 음표 찾음
									{
										count++;
										int XofLine = 0;
										int Beat = DetectBeat(x,y);
										cout << "박자는" << Beat << " 음높이는" << DetectPitch(y+2) << endl;;
										TempNotes n(x, DetectPitch(y+2), Beat);
										Notes.pushlinAccord(n, x);
										y += Hofhead - 1;
									}
								}
								else
								{
									if (TemplateMatch(head_empty, x, y - b, 0.85, 0)) //head 비어있고 칸 안에 있는 음표 찾음
									{
										count++;
										int Beat = 2 * DetectBeat(x,y);
										cout << "빈음표 박자는" << Beat << " 음높이는" << DetectPitch(y+2) << endl;
										TempNotes n(x, DetectPitch(y + 2), Beat);
										Notes.pushlinAccord(n, x);
										y += Hofhead - 1;
									}
									else if (TemplateMatch(head_empty_line, x, y - b, 0.83, 0)) //head 비어있고 오선 위에 있는 음표 찾음
									{
										count++;
										int Beat = 2 * DetectBeat(x,y);
										cout << "빈음표 박자는" << Beat << " 음높이는" << DetectPitch(y+2) << endl;
										TempNotes n(x, DetectPitch(y + 2), Beat);
										Notes.pushlinAccord(n, x);
										y += Hofhead - 1;
									}
								}
							}
						}
					}
				}
			}
		}
		//if (count != 0)
		//x += Wofhead - 3;
		//이 주석처리를 지우면 시간이 매우 단축되나 인식률도 매우 떨어짐
		
	}
	for (int i = 0; i < Notes.Accord.size(); i++)
	{
		score->NewNote(RL, Notes.Accord[i]);
	}
}

bool ScoreProcessor::isShape_UP(int x, int y, int& XofLine, bool& isNotail)
{
	for (int i = -3; i < 4; i++)
	{
		if (image.at<cv::Vec3b>(y - 4 - b, x + Wofhead + i)[0] == 0 && image.at<cv::Vec3b>(y - 5 - b, x + Wofhead + i)[0] == 0) //위 경우
		{
			XofLine = x + Wofhead + i;
			isNotail = false;
			return true;
		}
		else if (image.at<cv::Vec3b>(y + Hofhead - b + 4, x + i)[0] == 0 && image.at<cv::Vec3b>(y + Hofhead - b + 5, x + i)[0] == 0) //아래 경우
		{
			XofLine = x + i;
			isNotail = false;
			return false;
		}
	}
	isNotail = true;
	return false;
}//y좌표를 3칸,4칸으로 탐색하면 못 찾음

int ScoreProcessor::DetectBeat(int x, int y)
{
	int XofLine = 0;
	bool isNotail = false;
	int countL = 0;
	int countR = 0;
	int count = 0;

	if (isShape_UP(x, y, XofLine, isNotail))
	{
		bool state = false;
		int Ryh = y - b - 4;
		while (Ryh >= 1)
		{
			if (image.at<cv::Vec3b>(Ryh - 1, XofLine)[0] == 0) //다음칸 0채널이 검정색이면 카운터 초기화하고 한칸더 올라감
				Ryh--;
			else
				break;
		}
		DetectBeat_sub(countR, XofLine, Ryh, 2, true);
		DetectBeat_sub(countL, XofLine, Ryh, -2, true);

		if (countR >= countL)
			count = countR;
		else
			count = countL;
		/*
		i=y-4-b부터 시작해서 i--하면서 탐색.
		*/
	}
	else
	{
		if (!isNotail) //꼬리가 아래로 내려갈 경우
		{
			int Ryl = y + Hofhead - b + 4;
			while (Ryl < image.rows - 1)
			{
				if (image.at<cv::Vec3b>(Ryl + 1, XofLine)[0] == 0)
					Ryl++;
				else
					break;
			}
			DetectBeat_sub(countR, XofLine, Ryl, 2, false);
			DetectBeat_sub(countL, XofLine, Ryl, -2, false);
			if (countR >= countL)
				count = countR;
			else
				count = countL;
		}

		else //박자는 온음표
		{
			//박자는 온음표
			return 32;
		}
	}

	if (count == 0)
		return 8;
	else if (count == 1)
		return 4;
	else if (count == 2)
		return 2;
	else if (count == 3)
		return 1;
	else
		return 0;
}//32분음표 = 1, 16분음표 = 2, 8분음표 = 4, 4분음표 = 8, 온음표 = 32 리턴

void ScoreProcessor::DetectBeat_sub(int& count, int XofLine, int Ry, int k, bool Up)
{
	int i = Ry;
	if (Up) //음표 꼬리가 위로 생긴 경우 : Ry부터 내려오면서 찾아야함
	{
		bool state = false;
		int whiteCount = 0;

		while (whiteCount < 5) //흰 칸을 5칸 연속 만나기 전까지 탐색
		{
			//오선 제거한 채널로 탐색
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //지금칸 검정
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] == 0)//아래칸도 검정
					whiteCount = 0;
				else //검->흰or초 탐색
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255) //검->흰 탐색
						count++;
					else //검->초 탐색
					{
						state = true;
					}
				}
			}
			else //지금칸 흰or초
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] == 255)//아래칸도 흰색
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255 && state) //state true이고 초->흰 탐색의 경우
					{
						whiteCount = 0;
						count++; //검->초->흰 인 경우 꼬리로 본다.
					}
					else
						whiteCount++;
				}
				else
					whiteCount = 0;
			}
			i++;
		}
	}
	else //음표 꼬리가 아래로 생긴 경우 : Ry부터 올라가면서 찾아야 함
	{
		bool state = false;
		int whiteCount = 0;

		while (whiteCount < 5) //흰 칸을 5칸 연속 만나기 전까지 탐색
		{
			//오선 제거한 채널로 탐색
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //지금칸 검정
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 0)//위칸도 검정
					whiteCount = 0;
				else //검->흰or초 탐색
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255) //검->흰 탐색
						count++;
					else //검->초 탐색
					{
						state = true;
					}
				}
			}
			else //지금칸 흰or초
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 255)//위칸도 흰색
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255 && state) //state true이고 초->흰 탐색의 경우
					{
						whiteCount = 0;
						count++; //검->초->흰 인 경우 꼬리로 본다.
					}
					else
						whiteCount++;
				}
				else
					whiteCount = 0;
			}
			i--;
		}
	}
}

int ScoreProcessor::DetectPitch(int y)
{
		double k = (double)(y - lineArr[0]) / (double)interval;
		int qu = (int)k;
		if (k - (double)qu >= 0.5)
		{
			qu++;
		}
		else if (k - (double)qu <= -0.5)
		{
			qu--;
		}
		return LUT[RL][qu + 6];
}