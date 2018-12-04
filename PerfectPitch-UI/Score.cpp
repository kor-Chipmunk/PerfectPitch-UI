#include"stdafx.h"
#include"Score.h"

Score::Score(HMIDIOUT devh, int tempo)
{
	m_DevHandle = devh;
	for (int i = 0; i < 5; i++)
	{
		setVolume_L(100);
		setVolume_R(100);
		Threads_R[i].SetThread(m_DevHandle, tempo); //500은 사이즈
		Threads_L[i].SetThread(m_DevHandle, tempo);
	}
}

void Score::PlayMusic()
{
	thread thr1 = Threads_R[0].PlayThread();
	thread thr2 = Threads_R[1].PlayThread();
	thread thr3 = Threads_R[2].PlayThread();
	thread thr4 = Threads_R[3].PlayThread();
	thread thr5 = Threads_R[4].PlayThread();
	thread thr6 = Threads_L[0].PlayThread();
	thread thr7 = Threads_L[0].PlayThread();
	thread thr8 = Threads_L[2].PlayThread();
	thread thr9 = Threads_L[3].PlayThread();
	thread thr10 = Threads_L[4].PlayThread();

	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();
	thr6.join();
	thr7.join();
	thr8.join();
	thr9.join();
	thr10.join();

}

void Score::NewNote(int RL, int beat, int n1, int n2, int n3, int n4, int n5)
{
	if (beat != 0)
	{
		if (RL == 0) //오른손
		{
			Threads_R[0].pushNote(n1);
			Threads_R[1].pushNote(n2);
			Threads_R[2].pushNote(n3);
			Threads_R[3].pushNote(n4);
			Threads_R[4].pushNote(n5);
			EmptyNote(0, beat - 1);
		}
		else if (RL == 1)//왼손
		{
			Threads_L[0].pushNote(n1);
			Threads_L[1].pushNote(n2);
			Threads_L[2].pushNote(n3);
			Threads_L[3].pushNote(n4);
			Threads_L[4].pushNote(n5);
			EmptyNote(1, beat - 1);
		}
		else//한손악보_test용
		{
			Threads_R[0].pushNote(n1);
			Threads_R[1].pushNote(n2);
			Threads_R[2].pushNote(n3);
			Threads_R[3].pushNote(n4);
			Threads_R[4].pushNote(n5);
			EmptyNote(0, beat - 1);
			EmptyNote(1, beat);
		}
	}
}
void Score::NewNote(int RL, linAccord ns)
{
	if (RL == 0) //오른손
	{
		for (int i = 0; i < ns.tempN.size(); i++)
		{
			Threads_R[i].pushNote(ns.tempN[i].pitch);
		}
		for (int i = ns.tempN.size(); i < 5; i++)
		{
			Threads_R[i].pushNote(100);
		}
		EmptyNote(RL, ns.beat - 1);
	}
	else if (RL == 1)//왼손
	{
		for (int i = 0; i < ns.tempN.size(); i++)
		{
			Threads_L[i].pushNote(ns.tempN[i].pitch);
		}
		for (int i = ns.tempN.size(); i < 5; i++)
		{
			Threads_L[i].pushNote(100);
		}
		EmptyNote(RL, ns.beat - 1);
	}
	else //테스트용 한손코드
	{
		for (int i = 0; i < ns.tempN.size(); i++)
		{
			Threads_R[i].pushNote(ns.tempN[i].pitch);
		}
		for (int i = ns.tempN.size(); i < 5; i++)
		{
			Threads_R[i].pushNote(100);
		}
		EmptyNote(0, ns.beat - 1);
		EmptyNote(1, ns.beat);
	}
}

void Score::EmptyNote(int RL, int beat)
{
	if (beat != 0)
	{
		if (RL == 1) //왼손
		{
			for (int i = 0; i < beat; i++)
			{
				Threads_L[0].pushNote(100);
				Threads_L[1].pushNote(100);
				Threads_L[2].pushNote(100);
				Threads_L[3].pushNote(100);
				Threads_L[4].pushNote(100);
			}
		}
		else if (RL == 0) //오른손
		{
			for (int i = 0; i < beat; i++)
			{
				Threads_R[0].pushNote(100);
				Threads_R[1].pushNote(100);
				Threads_R[2].pushNote(100);
				Threads_R[3].pushNote(100);
				Threads_R[4].pushNote(100);
			}
		}
	}
}