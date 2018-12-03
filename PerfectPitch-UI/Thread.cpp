#include "stdafx.h"
#include"Thread.h"
#include"Midi.h"
#include<thread>


void Thread::PlayNotes()
{
	for (unsigned int i = 0; i < notes.size(); i++)
	{
		Midi::SendShortMsg(m_DevHandle, 0x90, (BYTE)(0x30 + notes[i] - 1), Vol);
		//0x90:소리킴
		//그다음은 음높이
		//그다음은 볼륨(30정도부터 최대 127까지)
		//만약 0x90자리에 #define CHANGE_INST =0xc0 들어오면
		//그 다음자리는 악기종류가 되고(INST 10씩 플마 해보자), 마지막은 0 고정
		Sleep(tempo);
	}
}

void Thread::pushNote(int N)
{
	notes.push_back(N);
}

std::thread Thread::PlayThread()
{
	thread thr(&Thread::PlayNotes, this);
	return thr;
}