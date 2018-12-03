#pragma once
#include<process.h>
#include<vector>
#include"Midi.h"
#include<thread>
using namespace std;

class Thread
{
	int tempo;
	int Vol;
	HMIDIOUT m_DevHandle;
	vector<int> notes;
	
public:
	void SetThread(HMIDIOUT devh, int tempo)
	{
		m_DevHandle = devh;
		this->tempo = tempo;
	}
	void SetThreadTempo(int tempo) { this->tempo = tempo; }
	
	void PlayNotes();
	void pushNote(int N);
	std::thread PlayThread();
	void setThreadVol(int Vol) { this->Vol = Vol; }
};