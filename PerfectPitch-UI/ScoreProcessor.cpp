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

	if (MatchRate >= rate) //rate�̻� ��ġ �� detect
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
									if (TemplateMatch(head_fill,x, y - b, 0.90, 2))//head ���ִ� ��ǥ ã��
									{
										count++;
										int XofLine = 0;
										int Beat = DetectBeat(x,y);
										cout << "���ڴ�" << Beat << " �����̴�" << DetectPitch(y+2) << endl;;
										TempNotes n(x, DetectPitch(y+2), Beat);
										Notes.pushlinAccord(n, x);
										y += Hofhead - 1;
									}
								}
								else
								{
									if (TemplateMatch(head_empty, x, y - b, 0.85, 0)) //head ����ְ� ĭ �ȿ� �ִ� ��ǥ ã��
									{
										count++;
										int Beat = 2 * DetectBeat(x,y);
										cout << "����ǥ ���ڴ�" << Beat << " �����̴�" << DetectPitch(y+2) << endl;
										TempNotes n(x, DetectPitch(y + 2), Beat);
										Notes.pushlinAccord(n, x);
										y += Hofhead - 1;
									}
									else if (TemplateMatch(head_empty_line, x, y - b, 0.83, 0)) //head ����ְ� ���� ���� �ִ� ��ǥ ã��
									{
										count++;
										int Beat = 2 * DetectBeat(x,y);
										cout << "����ǥ ���ڴ�" << Beat << " �����̴�" << DetectPitch(y+2) << endl;
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
		//�� �ּ�ó���� ����� �ð��� �ſ� ����ǳ� �νķ��� �ſ� ������
		
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
		if (image.at<cv::Vec3b>(y - 4 - b, x + Wofhead + i)[0] == 0 && image.at<cv::Vec3b>(y - 5 - b, x + Wofhead + i)[0] == 0) //�� ���
		{
			XofLine = x + Wofhead + i;
			isNotail = false;
			return true;
		}
		else if (image.at<cv::Vec3b>(y + Hofhead - b + 4, x + i)[0] == 0 && image.at<cv::Vec3b>(y + Hofhead - b + 5, x + i)[0] == 0) //�Ʒ� ���
		{
			XofLine = x + i;
			isNotail = false;
			return false;
		}
	}
	isNotail = true;
	return false;
}//y��ǥ�� 3ĭ,4ĭ���� Ž���ϸ� �� ã��

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
			if (image.at<cv::Vec3b>(Ryh - 1, XofLine)[0] == 0) //����ĭ 0ä���� �������̸� ī���� �ʱ�ȭ�ϰ� ��ĭ�� �ö�
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
		i=y-4-b���� �����ؼ� i--�ϸ鼭 Ž��.
		*/
	}
	else
	{
		if (!isNotail) //������ �Ʒ��� ������ ���
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

		else //���ڴ� ����ǥ
		{
			//���ڴ� ����ǥ
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
}//32����ǥ = 1, 16����ǥ = 2, 8����ǥ = 4, 4����ǥ = 8, ����ǥ = 32 ����

void ScoreProcessor::DetectBeat_sub(int& count, int XofLine, int Ry, int k, bool Up)
{
	int i = Ry;
	if (Up) //��ǥ ������ ���� ���� ��� : Ry���� �������鼭 ã�ƾ���
	{
		bool state = false;
		int whiteCount = 0;

		while (whiteCount < 5) //�� ĭ�� 5ĭ ���� ������ ������ Ž��
		{
			//���� ������ ä�η� Ž��
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //����ĭ ����
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] == 0)//�Ʒ�ĭ�� ����
					whiteCount = 0;
				else //��->��or�� Ž��
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255) //��->�� Ž��
						count++;
					else //��->�� Ž��
					{
						state = true;
					}
				}
			}
			else //����ĭ ��or��
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] == 255)//�Ʒ�ĭ�� ���
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255 && state) //state true�̰� ��->�� Ž���� ���
					{
						whiteCount = 0;
						count++; //��->��->�� �� ��� ������ ����.
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
	else //��ǥ ������ �Ʒ��� ���� ��� : Ry���� �ö󰡸鼭 ã�ƾ� ��
	{
		bool state = false;
		int whiteCount = 0;

		while (whiteCount < 5) //�� ĭ�� 5ĭ ���� ������ ������ Ž��
		{
			//���� ������ ä�η� Ž��
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //����ĭ ����
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 0)//��ĭ�� ����
					whiteCount = 0;
				else //��->��or�� Ž��
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255) //��->�� Ž��
						count++;
					else //��->�� Ž��
					{
						state = true;
					}
				}
			}
			else //����ĭ ��or��
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 255)//��ĭ�� ���
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255 && state) //state true�̰� ��->�� Ž���� ���
					{
						whiteCount = 0;
						count++; //��->��->�� �� ��� ������ ����.
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