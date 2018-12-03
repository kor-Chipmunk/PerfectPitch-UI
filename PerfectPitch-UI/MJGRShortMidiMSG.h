#pragma once
#include<iostream>
using namespace std;
#pragma pack(push,1)

typedef struct
{
	BYTE byteState;
	BYTE byteNote;
	BYTE byteVelocity;
	BYTE byteNULL;

}MJGRShortMidiMSG_ST;

typedef union
{
	DWORD                dwMidi_Data;
	MJGRShortMidiMSG_ST  stMidi_Data;

}MJGRShortMidiMSG;

#pragma pack(pop)