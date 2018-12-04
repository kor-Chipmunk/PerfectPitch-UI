#pragma once
#include<thread>
#include"Thread.h"
#include"Midi.h"
#include"Accord.h"

class Score
{
	HMIDIOUT m_DevHandle;
public:
	int tempo;
	Thread Threads_R[5];
	Thread Threads_L[5];

	//Score() {};
	Score(HMIDIOUT devh, int tempo);
	void PlayMusic();
	void NewNote(int RL, int beat, int n1 = 100, int n2 = 100, int n3 = 100, int n4 = 100, int n5 = 100);
	void NewNote(int RL, linAccord ns);
	void EmptyNote(int RL, int beat);
	void setTempo(int tempo)
	{
		this->tempo = tempo;
		for (int i = 0; i < 5; i++)
		{
			Threads_R[i].SetThreadTempo(tempo);
			Threads_L[i].SetThreadTempo(tempo);
		}
	}
	void setVolume_R(int Vol)
	{
		for (int i = 0; i < 5; i++)
			Threads_R[i].setThreadVol(Vol);
	}
	void setVolume_L(int Vol)
	{
		for (int i = 0; i < 5; i++)
			Threads_L[i].setThreadVol(Vol);
	}
};