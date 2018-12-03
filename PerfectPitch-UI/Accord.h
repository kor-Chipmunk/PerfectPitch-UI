#pragma once
#include<vector>
using namespace std;


class TempNotes
{
public:
	int x;
	int y;
	int beat;

	TempNotes(int x, int y, int beat)
	{
		this->x = x;
		this->y = y;
		this->beat = beat;
	}
};

class linAccord
{
public:
	vector<TempNotes> tempN;
	int XofAccord;
	int beat=0;
	
	linAccord(int XofAccord)
	{
		this->XofAccord = XofAccord;
	}
	void pushTempN(TempNotes n)
	{
		tempN.push_back(n);
		if (tempN.size() == 1)
			this->beat = tempN[0].beat;
		else if (n.beat < this->beat)
			this->beat = n.beat;
	}
};

class Accords
{
public:
	vector<linAccord> Accord;
	void pushlinAccord(TempNotes n, int x);
};

