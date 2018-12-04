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

cv::Mat Pretreatment::Erode(const cv::Mat& image, int kSize) //현재 버젼에서는 사용하지 않음
{
	cv::Mat result(image.size(), image.type());
	cv::Rect imgRect(0, 0, image.cols, image.rows);
	int uc = image.cols;
	int ur = image.rows;

	for (int j = 0; j < ur; j++) { //모든 행
		for (int i = 0; i < uc; i++) { //모든 열

			cv::Rect tmpRect(i - kSize / 2, j - kSize / 2, kSize, kSize);//마스크
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

cv::Mat Pretreatment::Dilate(const cv::Mat& image) //현재 버젼에서는 사용하지 않음
{
	cv::Mat result(image.size(), image.type());
	cv::Rect imgRect(0, 0, image.cols, image.rows);
	int uc = image.cols;
	int ur = image.rows;

	for (int j = 0; j < ur; j++) { //모든 행
		for (int i = 0; i < uc; i++) { //모든 열
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
		if (hist[i] > col*0.55)//55% 임계값
		{
			if (i > 0 && hist[i - 1])
			{
				//i가 오선높이
				lineArr.push_back(i); //lineArr에 오선 높이들을 push해둠
				for (int j = 0; j < col; j++)
				{
					image.at<cv::Vec3b>(i, j)[1] = 200; //오선 찾으면 초록색으로 바꿈
				}
			}
		}
	}
}

void Pretreatment::RemoveDup(vector<int>& lineArr) //오선이 두꺼워서 여러 줄일 경우, 가운데 값만 남기고 제거하는 함수. (현재 잘 작동하지 않음)
//ex 1 2 3 6 7 10 11 이 들어있는 경우, 2 7 11 만 남기고 지운다.
{
	int size = lineArr.size();
	for (int i = 0; i < size - 1; i++)
	{
		int j = i;
		for (; j < size - 1;)
		{
			if (lineArr[j + 1] - lineArr[j] == 1) //옆칸과 1 차이나면 j이동
				j++;
			else break; //멈춘 상태에서 j는 겹치는부분의 가장 오른쪽 값
		}
		//i는 겹치는 구간의 가장 왼쪽인덱스, j는 가장 오른쪽 인덱스 즉 1 2 3 5 6 일 경우 최초 i=0, j=2인 상황임
		int count = 0;
		if (count < j - i)
		{
			vector<int>::iterator it = lineArr.begin();
			while (count < j - i && it < lineArr.end() - i)
			{
				it = lineArr.begin();
				it += i;
				lineArr.erase(it); //lineArr[i]를 지우고 뒤에있는 것들을 한칸씩 당김
				size--;
				count++;

				if (count >= j - i || it >= lineArr.end() - i)
					break;
				it = lineArr.begin();
				it += j - 1;
				lineArr.erase(it);//lineArr[j-1]을 지우고 뒤에있는 것들을 한칸씩 당김
				size--;
				count++;
			}
		}
	}
}

void Pretreatment::DeterScale(cv::Mat image) //장단조 구분 및 LUP 만들기
{

}