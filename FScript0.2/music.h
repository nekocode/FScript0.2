#pragma once
#include "libzplay\\libzplay.h"
#pragma comment(lib,"libzplay\\libzplay.lib")

enum player_state{isplaying,isstop};

class player
{
public:
	player_state state;
	bool loop;

	player();
	~player();
	bool openmusic(const char* path);
	bool playmusic();
	bool stopmusic();
	bool getFFTData(int *Amplitude,int num);		//Ĭ�϶�ȡtrack0�Ķ�̬Ƶ��

private:
	libZPlay::ZPlay *zplayer;
	int volume;

	friend int __stdcall Module_Music_CallbackFunc(void*, void*, libZPlay::TCallbackMessage, unsigned int, unsigned int);
};

class music
{
public:
	player p[5];

	~music();
	bool openmusic(int track,const char* path);
	bool open_and_playmusic(int track,const char* path);
	bool playmusic(int track);
	bool stopmusic(int track);
	bool getFFTData(int *Amplitude,int num=257);		//Ĭ�϶�ȡtrack0�Ķ�̬Ƶ��
};

extern music module_music;