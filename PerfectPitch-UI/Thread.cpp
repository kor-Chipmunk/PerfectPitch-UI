#include "stdafx.h"
#include"Thread.h"
#include"Midi.h"
#include<thread>


void Thread::PlayNotes()
{
	for (unsigned int i = 0; i < notes.size(); i++)
	{
		Midi::SendShortMsg(m_DevHandle, 0x90, (BYTE)(0x30 + notes[i] - 1), Vol);
		//0x90:�Ҹ�Ŵ
		//�״����� ������
		//�״����� ����(30�������� �ִ� 127����)
		//���� 0x90�ڸ��� #define CHANGE_INST =0xc0 ������
		//�� �����ڸ��� �Ǳ������� �ǰ�(INST 10�� �ø� �غ���), �������� 0 ����
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