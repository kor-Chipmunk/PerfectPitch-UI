#include "stdafx.h"
#include"Pretreatment.h"

void Pretreatment::Binarization(cv::Mat& image, uchar threshold)
{
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (image.at<cv::Vec3b>(y, x)[0] > threshold
				|| image.at<cv::Vec3b>(y, x)[1] > threshold
				|| image.at<cv::Vec3b>(y, x)[2] > threshold) {
				image.at<cv::Vec3b>(y, x)[0] = 255;
				image.at<cv::Vec3b>(y, x)[1] = 255;
				image.at<cv::Vec3b>(y, x)[2] = 255;
			}
			else {
				image.at<cv::Vec3b>(y, x)[0] = 0;
				image.at<cv::Vec3b>(y, x)[1] = 0;
				image.at<cv::Vec3b>(y, x)[2] = 0;
			}
		}
	}
}

cv::Mat Pretreatment::Erode(const cv::Mat& image, int kSize) //���� ���������� ������� ����
{
	cv::Mat result(image.size(), image.type());
	cv::Rect imgRect(0, 0, image.cols, image.rows);
	int uc = image.cols;
	int ur = image.rows;

	for (int j = 0; j < ur; j++) { //��� ��
		for (int i = 0; i < uc; i++) { //��� ��

			cv::Rect tmpRect(i - kSize / 2, j - kSize / 2, kSize, kSize);//����ũ
			cv::Rect in = imgRect & tmpRect;
			uchar min = 255;
			for (int k = in.x; k < in.x + in.width; k++) {
				for (int l = in.y; l < in.y + in.height; l++) {
					if (image.at<uchar>(l, k) < min)
						min = image.at<uchar>(l, k);
				}
			}
			result.at<uchar>(j, i) = min;
		}
	}
	return result;
}

cv::Mat Pretreatment::Dilate(const cv::Mat& image) //���� ���������� ������� ����
{
	cv::Mat result(image.size(), image.type());
	cv::Rect imgRect(0, 0, image.cols, image.rows);
	int uc = image.cols;
	int ur = image.rows;

	for (int j = 0; j < ur; j++) { //��� ��
		for (int i = 0; i < uc; i++) { //��� ��
			cv::Rect tmpRect(i - 1, j - 1, 3, 3);
			cv::Rect in = imgRect & tmpRect;
			uchar max = 0;

			for (int k = in.x; k < in.x + in.width; k++) {
				for (int l = in.y; l < in.y + in.height; l++) {
					if (image.at<uchar>(l, k) > max)
						max = image.at<uchar>(l, k);
				}
			}
			result.at<uchar>(j, i) = max;
		}
	}
	return result;
}

void Pretreatment::DetectLine(cv::Mat& image, vector<int>& lineArr)
{
	int row = image.rows;
	int col = image.cols;
	int* hist = new int[row];
	for (int i = 0; i < row; i++)
		hist[i] = 0;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (image.at<cv::Vec3b>(y, x)[0] != 255)
				hist[y]++;
		}
	}

	for (int i = 0; i < row; i++)
	{
		if (hist[i] > col*0.55)//55% �Ӱ谪
		{
			if (i > 0 && hist[i - 1])
			{
				//i�� ��������
				lineArr.push_back(i); //lineArr�� ���� ���̵��� push�ص�
				for (int j = 0; j < col; j++)
				{
					image.at<cv::Vec3b>(i, j)[1] = 200; //���� ã���� �ʷϻ����� �ٲ�
				}
			}
		}
	}
}

void Pretreatment::RemoveDup(vector<int>& lineArr) //������ �β����� ���� ���� ���, ��� ���� ����� �����ϴ� �Լ�. (���� �� �۵����� ����)
//ex 1 2 3 6 7 10 11 �� ����ִ� ���, 2 7 11 �� ����� �����.
{
	int size = lineArr.size();
	for (int i = 0; i < size - 1; i++)
	{
		int j = i;
		for (; j < size - 1;)
		{
			if (lineArr[j + 1] - lineArr[j] == 1) //��ĭ�� 1 ���̳��� j�̵�
				j++;
			else break; //���� ���¿��� j�� ��ġ�ºκ��� ���� ������ ��
		}
		//i�� ��ġ�� ������ ���� �����ε���, j�� ���� ������ �ε��� �� 1 2 3 5 6 �� ��� ���� i=0, j=2�� ��Ȳ��
		int count = 0;
		if (count < j - i)
		{
			vector<int>::iterator it = lineArr.begin();
			while (count < j - i && it < lineArr.end() - i)
			{
				it = lineArr.begin();
				it += i;
				lineArr.erase(it); //lineArr[i]�� ����� �ڿ��ִ� �͵��� ��ĭ�� ���
				size--;
				count++;

				if (count >= j - i || it >= lineArr.end() - i)
					break;
				it = lineArr.begin();
				it += j - 1;
				lineArr.erase(it);//lineArr[j-1]�� ����� �ڿ��ִ� �͵��� ��ĭ�� ���
				size--;
				count++;
			}
		}
	}
}

void Pretreatment::DeterScale(cv::Mat image) //����� ���� �� LUP �����
{

}