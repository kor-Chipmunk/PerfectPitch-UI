#include "stdafx.h"

#include"ScoreProcessor.h"
#include"Accord.h"

using namespace std;


int Max(int a = 0, int b = 0, int c = 0, int d = 0, int e = 0, int f = 0, int g = 0, int h = 0)
{
	int arr[8] = { a,b,c,d,e,f,g,h };
	int max = a;
	for (int i = 0; i < 8; i++)
		if (arr[i] > max)
			max = arr[i];
	return max;
}
int Min(int a = 20000, int b = 20000, int c = 20000, int d = 20000, int e = 20000, int f = 20000, int g = 20000, int h = 20000)
{
	int arr[8] = { a,b,c,d,e,f,g,h };
	int min = a;
	for (int i = 0; i < 8; i++)
		if (arr[i] < min)
			min = arr[i];
	return min;
}

ScoreProcessor::ScoreProcessor(cv::Mat& image, Score* score, int* lineArr, int LUT_original[][20], int RL)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			this->LUT_original[j][i] = LUT_original[j][i];
		}
	}

	this->image = image;
	this->score = score;
	this->lineArr = lineArr;
	this->interval = (double)(lineArr[4] - lineArr[0]) / (double)8;
	this->row = image.rows;
	this->col = image.cols;
	this->head_fill = cv::imread("Head_fill.jpg");
	this->head_empty = cv::imread("Head_empty.jpg");
	this->head_empty_line = cv::imread("Head_empty_line.jpg");
	this->Sharp = cv::imread("Sharp.jpg");
	this->Flat = cv::imread("Flat.jpg");
	this->Point = cv::imread("Point.jpg");
	this->Natural = cv::imread("Natural.jpg");
	this->Rest_4 = cv::imread("Rest_4.jpg");
	this->Rest_8 = cv::imread("Rest_8.jpg");
	this->RL = RL;

	this->Hofhead = head_fill.rows;
	this->Wofhead = head_fill.cols;
	this->HofPoint = Point.rows;
	this->WofPoint = Point.cols;
	this->HofSFN = Sharp.rows;
	this->WofSFN = Sharp.cols;
	this->HofRest_4 = Rest_4.rows;
	this->WofRest_4 = Rest_4.cols;
	this->HofRest_8 = Rest_8.rows;
	this->WofRest_8 = Rest_8.cols;

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
	for (int x = 0; x < WofRest_4; x++)
	{
		if (Rest_4.at<cv::Vec3b>(0, x)[0] == 0)
		{
			aofRest_4 = x;
			break;
		}
	}
	for (int y = 0; y < HofRest_4; y++)
	{
		if (Rest_4.at<cv::Vec3b>(y, 0)[0] == 0)
		{
			bofRest_4 = y;
			break;
		}
	}
	for (int x = 0; x < WofRest_8; x++)
	{
		if (Rest_8.at<cv::Vec3b>(0, x)[0] == 0)
		{
			aofRest_4 = x;
			break;
		}
	}
	for (int y = 0; y < HofRest_8; y++)
	{
		if (Rest_8.at<cv::Vec3b>(y, 0)[0] == 0)
		{
			bofRest_8 = y;
			break;
		}
	}
	DetectScale();
}

bool ScoreProcessor::TemplateMatch(cv::Mat tem, int x, int y, int w, int h, double rate, int p, int c)
{
	if (x < 0 || y < 0)
		return false;
	if (y + h >= row || x + w >= col)
		return false;
	int count = 0;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (abs((int)image.at<cv::Vec3b>(y + j, x + i)[p] - (int)tem.at<cv::Vec3b>(j, i)[p]) < 220)
				count++;
		}
	}
	double MatchRate = (double)count / (double)(w*h);
	if (MatchRate >= rate) //rate�̻� ��ġ �� detect
	{
		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
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
	for (int x = 0; x < col - Max(Wofhead, WofRest_4, WofRest_8); x++) //��ģ���� ���߿� ����ū�� ã�Ƽ� �װɷ� ������
	{
		int count = 0;
		for (int y = Max(b, bofRest_4, bofRest_8); y < row - Max(Hofhead - b, HofRest_4 - bofRest_4, HofRest_8 - bofRest_8); y++) //�̰͵�..
		{
			if (image.at<cv::Vec3b>(y, x)[0] == 0)
			{
				if (image.at<cv::Vec3b>(y + 1, x)[0] == 0)
				{
					if (image.at<cv::Vec3b>(y + 2, x)[0] == 0)
					{
						if (image.at<cv::Vec3b>(y - b, x)[0] == 255)
						{
							if (image.at<cv::Vec3b>(y - b + Hofhead, x + Wofhead)[0] == 255)
							{
								if (image.at<cv::Vec3b>(y - b + Hofhead / 2, x + Wofhead / 2)[1] == 0)
								{
									if (TemplateMatch(head_fill, x, y - b, Wofhead, Hofhead, 0.90, 2, 2))//head ���ִ� ��ǥ ã��
									{
										int XofLine = 0;
										int Beat = DetectBeat(x, y);
										if (DetectPoint(x, y, 2, 2))
										{
											Beat *= 3;
											Beat /= 2;
										}
										RevisePitch((double)y - 1.5, Accidental(x, y));
										cout << "���ڴ�" << Beat << " �����̴� " << DetectPitch((double)y - 1.5) << endl;;
										TempNotes n(x, y, DetectPitch((double)y - 1.5), Beat);
										Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
										y += Hofhead - 1;
									}
								}
								else
								{
									if (TemplateMatch(head_empty, x, y - b, Wofhead, Hofhead, 0.85, 2, 2)) //head ����ְ� ĭ �ȿ� �ִ� ��ǥ ã��
									{
										int Beat = 2 * DetectBeat(x, y);
										if (DetectPoint(x, y, 2, 2))
										{
											Beat *= 3;
											Beat /= 2;
										}
										RevisePitch((double)y - 1.5, Accidental(x, y));

										cout << "���ڴ�" << Beat << " �����̴� " << DetectPitch((double)y - 1.5) << endl;
										TempNotes n(x, y, DetectPitch((double)y - 1.5), Beat);
										Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
										y += Hofhead - 1;
									}
									else if (TemplateMatch(head_empty_line, x, y - b, Wofhead, Hofhead, 0.83, 2, 2)) //head ����ְ� ���� ���� �ִ� ��ǥ ã��
									{
										count++;
										int Beat = 2 * DetectBeat(x, y);
										if (DetectPoint(x, y, 2, 2))
										{
											Beat *= 3;
											Beat /= 2;
										}
										RevisePitch((double)y - 1.5, Accidental(x, y));
										cout << "���ڴ�" << Beat << " �����̴�" << DetectPitch((double)y - 1.5) << endl;
										TempNotes n(x, y, DetectPitch((double)y - 1.5), Beat);
										Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
										y += Hofhead - 1;
									}
								}
							}
						}

						if (image.at<cv::Vec3b>(y - bofRest_4, x)[0] == 255)
						{
							if (TemplateMatch(Rest_4, x, y - bofRest_4, WofRest_4, HofRest_4, 0.85, 1, 0))
							{
								cout << "4�н�ǥ ã��" << endl;
								int Beat = 8;
								if (DetectPoint(x, y, 2, 0))
									Beat = 12;
								TempNotes n(x, y, 100, Beat);
								Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
								y += HofRest_4 - 1;
							}
						}
						if (image.at<cv::Vec3b>(y - bofRest_8, x)[0] == 255)
						{
							if (TemplateMatch(Rest_8, x, y - bofRest_8, WofRest_8, HofRest_8, 0.9, 1, 0))
							{
								cout << "8�н�ǥ ã��" << endl;
								int Beat = 4;
								if (DetectPoint(x, y, 2, 0))
									Beat = 6;
								TempNotes n(x, y, 100, Beat);
								Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
								y += HofRest_8 - 1;
							}
						}
						if (image.at<cv::Vec3b>(0, x + 3)[0] == 0)
						{
							int count3 = 0;
							for (int k = 1; k < Hofhead * 2; k++)
							{
								if (image.at<cv::Vec3b>(k, x + 3)[0] == 0)
									count3++;
							}
							if (count3 >= Hofhead * 2 - 3) //////////�ǳʶٱ� ã��
							{
								TempNotes n(x, y, -102, 1);
								Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
								x += 3;
								///push -100
							}
						}
						if (abs(y - lineArr[0]) < 3)
						{
							int count3 = 0;
							for (int k = 0; k < (int)interval - 3; k++)
							{
								for (int l = lineArr[0] + 3; l < lineArr[4] - 3; l++)
								{
									if (image.at<cv::Vec3b>(l, x + k)[0] == 0) ///������ǥ �߰�
										count3++;
								}
							}
							if (count3 == ((int)interval - 3)*(lineArr[4] - lineArr[0] - 6))
							{


								//������ǥ���� �� Ȯ��
								///push -101
								if (lineArr[1] - (int)interval - 2 >= 0 && lineArr[1] - (int)interval - 2 < row)
								{
									if (x + Wofhead + interval + 2 < col)
									{
										if (image.at<cv::Vec3b>(lineArr[1] + (int)interval - 2, x + Wofhead + interval + 2)[0] == 0) //������ ������ǥ
										{
											TempNotes n(x, y, -100, 1);
											Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
											x += 6;
										}
									}
									if (x - Wofhead >= 0)
									{
										if (image.at<cv::Vec3b>(lineArr[1] + (int)interval - 2, x - Wofhead)[0] == 0) //�������λ��� ������ǥ
										{
											TempNotes n(x, y, -101, 1);
											Notes.pushlinAccord(image, n, x, Wofhead, Hofhead);
											x += 6;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	///////////////�ӽó�Ʈ�� �ӽþǺ��� Ǫ��////////////////////////

	for (int i = 0; i < Notes.Accord.size(); i++)
		score->NewNote(RL, Notes.Accord[i]);
}


bool ScoreProcessor::isShape_UP(int x, int y, int& XofLine, bool& isNotail)
{
	for (int i = -5; i < 6; i++) //�Ʒ��� ������ ���ʺ��� Ž��(�ٱ���->����)
	{
		int Hcount = 0;
		int count = 0;
		for (int j = 4; j < 2 * Hofhead; j++)
		{
			if (y + Hofhead - b + j < row)
			{
				Hcount++;
				if (image.at<cv::Vec3b>(y + Hofhead - b + j, x + i)[0] == 0)
				{
					/*
					image.at<cv::Vec3b>(y + Hofhead - b + j, x + i)[0] = 255;//////////����� ����
					image.at<cv::Vec3b>(y + Hofhead - b + j, x + i)[1] = 128;
					image.at<cv::Vec3b>(y + Hofhead - b + j, x + i)[2] = 0;
					*/
					count++;
				}
				else break;
			}
		}
		if (count == Hcount)
		{
			XofLine = x + i;
			isNotail = false;
			return false;
		}

	}
	for (int i = 6; i > -4; i--) //���� ��� �����ʺ��� Ž��(�ٱ���->����)
	{
		int Hcount = 0;
		int count = 0;
		for (int j = 4; j < 2 * Hofhead; j++)
		{
			if (y - b - j >= 0)
			{
				Hcount++;
				if (image.at<cv::Vec3b>(y - b - j, x + Wofhead + i)[0] == 0)
				{
					count++;
					/*
					image.at<cv::Vec3b>(y - b - j, x + Wofhead+ i)[0] = 255;///////////////////////����� ����
					image.at<cv::Vec3b>(y - b - j, x + Wofhead+ i)[1] = 128;
					image.at<cv::Vec3b>(y - b - j, x + Wofhead+ i)[2] = 0;
					*/
				}
				else break;
			}
		}
		if (count == Hcount)
		{
			XofLine = x + Wofhead + i;
			isNotail = false;
			return true;
		}
	}
	isNotail = true;
	return false; //������ ���� ��� ����ǥ���� ����

}

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
		DetectBeat_sub(countR, XofLine, Ryh, 1, true);
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
			DetectBeat_sub(countL, XofLine, Ryl, -1, false);
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

		while (whiteCount < 8) //�� ĭ�� 8ĭ ���� ������ ������ Ž��
		{
			//���� ������ ä�η� Ž��
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //����ĭ ����
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] == 0)//�Ʒ�ĭ�� ����
				{
					whiteCount = 0;
					//cout << "��->��, state:" << (int)state << endl;
				}
				else //��->��or�� Ž��
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255) //��->�� Ž��
					{
						count++;
						//image.at<cv::Vec3b>(i - 1, XofLine + k)[1] = 255;/////////////////
						//image.at<cv::Vec3b>(i, XofLine + k)[1] = 255;//////////////�����������
						//cout << "��->��, state:" << (int)state << " count!" << endl;
					}
					else //��->�� Ž��
					{
						state = true;
						//cout << "��->��, state:" << (int)state << endl;
					}
				}
			}
			else //����ĭ ��or��
			{
				if (image.at<cv::Vec3b>(i + 1, XofLine + k)[1] != 0)//�Ʒ�ĭ�� ���/�ʷ�
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i + 1, XofLine + k)[0] == 255 && state) //state true�̰� ��->�� Ž���� ���
					{
						state = false;
						whiteCount = 0;
						count++; //��->��->�� �� ��� ������ ����.
						//image.at<cv::Vec3b>(i - 1, XofLine + k)[1] = 255;/////////////////
						//image.at<cv::Vec3b>(i, XofLine + k)[1] = 255;//////////////�����������
						//cout << "��->��->��, state:" << (int)state << " count!" << endl;
					}
					else
					{
						whiteCount++;
						//cout << "��->��, state:" << (int)state << endl;
					}
				}
				else
				{
					whiteCount = 0;
					//cout << "��->��, state:" << (int)state << endl;
				}
			}
			///////////////////////
			image.at<cv::Vec3b>(i, XofLine + k)[2] = 200;
			//////////////////////������� ���� ����ȭ(����������)
			i++;
		}
	}
	else //��ǥ ������ �Ʒ��� ���� ��� : Ry���� �ö󰡸鼭 ã�ƾ� ��
	{
		bool state = false;
		int whiteCount = 0;

		while (whiteCount < 8) //�� ĭ�� 8ĭ ���� ������ ������ Ž��
		{
			//���� ������ ä�η� Ž��
			if (image.at<cv::Vec3b>(i, XofLine + k)[1] == 0) //����ĭ ����
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 0)//��ĭ�� ����
				{
					whiteCount = 0;
					//cout << "��->��, state:" << (int)state << endl;
				}
				else //��->��or�� Ž��
				{
					whiteCount = 0;
					if (image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255) //��->�� Ž��
					{
						state = false;
						count++;
						//image.at<cv::Vec3b>(i, XofLine + k)[1] = 255;//////////////�����������
						//image.at<cv::Vec3b>(i + 1, XofLine + k)[1] = 255;/////////////////
						//cout << "��->��, state:" << (int)state << " count!" << endl;
					}
					else //��->�� Ž��
					{
						state = true;
						//cout << "��->��, state:" << (int)state << endl;
					}
				}
			}
			else //����ĭ ��or��
			{
				if (image.at<cv::Vec3b>(i - 1, XofLine + k)[1] == 255)//��ĭ�� ���
				{
					if (image.at<cv::Vec3b>(i, XofLine + k)[0] == 0 && image.at<cv::Vec3b>(i - 1, XofLine + k)[0] == 255 && state) //state true�̰� ��->�� Ž���� ���
					{
						state = false;
						whiteCount = 0;
						count++; //��->��->�� �� ��� ������ ����.
						//image.at<cv::Vec3b>(i, XofLine + k)[1] = 255;//////////////�����������
						//image.at<cv::Vec3b>(i + 1, XofLine + k)[1] = 255;/////////////////
						//cout << "��->��->��, state:" << (int)state <<" count!"<<endl;
					}
					else
					{
						whiteCount++;
						//cout << "��->��, state:" << (int)state << endl;
					}
				}
				else
				{
					whiteCount = 0;
					//cout << "��->��, state:" << (int)state << endl;
				}
			}
			///////////////////////
			image.at<cv::Vec3b>(i, XofLine + k)[2] = 200;
			//////////////////////������� ���� ����ȭ(����������)
			i--;
		}
	}
}

int ScoreProcessor::DetectPitch(double y)
{
	double k = (double)(y - (double)lineArr[0]) / (double)interval;
	int qu = (int)k;
	if (k - (double)qu >= 0.5)
	{
		qu++;
	}
	else if (k - (double)qu <= -0.5)
	{
		qu--;
	}
	return LUT_linear[RL][qu + 6];
}
void ScoreProcessor::RevisePitch(double y, int Case)
{
	if (Case != -1)
	{
		double k = (double)(y - (double)lineArr[0]) / (double)interval;
		int qu = (int)k;
		if (k - (double)qu >= 0.5)
		{
			qu++;
		}
		else if (k - (double)qu <= -0.5)
		{
			qu--;
		}
		switch (Case)
		{
		case 0: //��
			//LUT_linear[RL][qu + 6]++;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 20; j++) {
					if (j % 7 == (qu + 6) % 7)
						LUT_linear[i][j]++;
				}
			}
			break;
		case 1: //���ڸ�
			//LUT_linear[RL][qu + 6] = LUT_original[RL][qu + 6];
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 20; j++) {
					if (j % 7 == (qu + 6) % 7)
						LUT_linear[i][j] = LUT_original[i][j];
				}
			}
			break;
		case 2: //�÷�
			//LUT_linear[RL][qu + 6]--;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 20; j++) {
					if (j % 7 == (qu + 6) % 7)
						LUT_linear[i][j]--;
				}
			}
			break;
		}
	}
}

bool ScoreProcessor::DetectPoint(int x, int y, int p, int c)
{
	for (int i = x + 25; i < x + 35; i++)
	{
		for (int j = y - 16; j < y + 5; j++)
		{
			if (TemplateMatch(Point, i, j, WofPoint, HofPoint, 0.95, p, c))
				return true;
		}
	}
	return false;
}

int ScoreProcessor::Accidental(int x, int y)
{
	for (int i = x - Wofhead * 3 / 2; i < x - Wofhead; i++)
	{
		for (int j = y - Hofhead * 2; j < y - Hofhead * 3 / 2; j++)
		{
			if (TemplateMatch(Sharp, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				return 0;
			else if (TemplateMatch(Natural, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				return 1;
			else if (TemplateMatch(Flat, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				return 2;
		}
	}
	return -1;
}

void ScoreProcessor::DetectScale() {
	int num = 0;
	for (; num < col; num++) {
		if (image.at<cv::Vec3b>(lineArr[0], num)[0] == 0)
			break;
	}

	int Case = -1;
	for (int i = num + Wofhead * 4; i < num + Wofhead * 11; i++)
	{
		for (int j = lineArr[0] - 3 * Hofhead; j < lineArr[0] + Hofhead; j++)
		{
			if (Case == -1)
			{
				if (TemplateMatch(Sharp, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				{
					Case = 0;
					RevisePitch((double)j + 2 * Hofhead - 1.5, Case);
					i += WofSFN - 1;
					break;
				}
				else if (TemplateMatch(Flat, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				{
					Case = 2;
					RevisePitch((double)j + 2 * Hofhead - 1.5, Case);
					i += WofSFN - 1;
					break;
				}
			}
			else if (Case == 0)
			{
				if (TemplateMatch(Sharp, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				{
					RevisePitch((double)j + 2 * Hofhead - 1.5, Case);
					i += WofSFN - 1;
					break;
				}
			}
			else if (Case == 2)
			{
				if (TemplateMatch(Flat, i, j, WofSFN, HofSFN, 0.95, 1, 1))
				{
					RevisePitch((double)j + 2 * Hofhead - 1.5, Case);
					i += WofSFN - 1;
					break;
				}
			}
		}
	}
}