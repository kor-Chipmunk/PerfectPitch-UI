#pragma once
#include<iostream>
#include<conio.h>
#include<windows.h>
#include<mmsystem.h>
#include"MJGRShortMidiMSG.h"


class Midi
{
public:
	static void CALLBACK OutProc(HMIDIOUT MidiOut, UINT Msg, DWORD instance, DWORD Param1, DWORD Para2) {};
	//static void OutputError(MMRESULT mmrErrCode);
	static long GetDevID(HMIDIOUT m_DevOutHAndle);
	static void GetDevCaps(HMIDIOUT m_DevOutHandle, MIDIOUTCAPS *Caps);
	static HMIDIOUT Open(WORD wDevsNum);
	static LRESULT Close(HMIDIOUT m_DevOutHandle);
	static void SendShortMsg(HMIDIOUT m_DevOutHandle, BYTE byteState, BYTE byteNote, BYTE byteValo);
	static void SendLongMsg(HMIDIOUT m_DevOutHandle, BYTE *lpMsg, DWORD dwMsgLength);
	static void AllChannelSoundOff(HMIDIOUT m_DevOutHandle);
};
