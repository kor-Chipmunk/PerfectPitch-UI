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
public:
	vector<int> notes;
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
	int lenThread() { return notes.size(); }
};