#include"stdafx.h"
#include"Accord.h"

void Accords::pushlinAccord(TempNotes n, int x)
{
	int capa = Accord.size();
	if (capa != 0 && abs(x - Accord[capa - 1].XofAccord) < 4)
	{
		Accord[capa - 1].pushTempN(n);
	}
	else
	{
		linAccord temp(x);
		temp.pushTempN(n);
		Accord.push_back(temp);
	}
}