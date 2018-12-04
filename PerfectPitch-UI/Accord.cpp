#include"stdafx.h"
#include"Accord.h"

void Accords::pushlinAccord(cv::Mat& image, TempNotes n, int x, int Wofhead, int Hofhead)
{
	int capa = Accord.size();
	if (capa != 0 && abs(x - Accord[capa - 1].XofAccord) < 6) //x좌표 차이가 6미만인 경우
	{
		Accord[capa - 1].pushTempN(n);
	}
	else //새로 화음을 만들어서 넣는 경우
	{
		linAccord temp(x);
		temp.pushTempN(n);
		Accord.push_back(temp);
	}

	int IndexofSamePitch = -1;
	if (capa > 1 && n.pitch != 100)
	{
		vector<linAccord>::iterator it;
		it = Accord.end();
		it -= 2;
		int LastX = (*it).XofAccord;
		for (int i = 0; i < (*it).tempN.size(); i++)
		{
			if ((*it).tempN[i].pitch == n.pitch)
			{
				IndexofSamePitch = i;
				break;
			}
		}
		if (IndexofSamePitch != -1)
		{
			int Hcount = 0;
			for (int i = 0; i < 5; i++)
			{
				int count = 0;
				for (int j = -2.5 * Hofhead; j < 2.5 * Hofhead; j++)
				{
					if (n.y + j < 0 || n.y + j >= image.rows)
						return;
					if ((n.x + LastX + Wofhead) / 2 + i < 0 || (x + LastX + Wofhead) / 2 + i >= image.cols)
						return;
					if (image.at<cv::Vec3b>(n.y + j, (x + LastX + Wofhead) / 2 + i)[1] == 0)
					{
						image.at<cv::Vec3b>(n.y + j, (x + LastX + Wofhead) / 2 + i)[2] = 100;
						image.at<cv::Vec3b>(n.y + j, (x + LastX + Wofhead) / 2 + i)[1] = 100;
						image.at<cv::Vec3b>(n.y + j, (x + LastX + Wofhead) / 2 + i)[0] = 255;
						count++;
					}
				}
				if (count != 0)
					Hcount++;
			}
			if (Hcount == 5)
			{
				Accord.back().tempN.back().pitch = 100;
				cout << "붙임줄 찾음" << endl;
			}
		}
	}
}