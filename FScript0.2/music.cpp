#include "StdAfx.h"
#include "music.h"
using namespace libZPlay;

music module_music;
bool MusicModuleExit = false;

//回调函数
int __stdcall Module_Music_CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
//	ZPlay *p = (ZPlay*)instance;
	player *_player = (player*)user_data;

	switch(message)
	{
	case MsgPlay:
		{  
		}
		return 0;

	case MsgExitVolumeSlide:
		{
			if(_player->volume==0)
				_player->zplayer->Stop();
		}
		return 0;

	case MsgStop:
		{
			if(MusicModuleExit)
				return 0;
			if(_player->loop&&(_player->state!=isstop))
				_player->playmusic();
		}
		return 0;
	}
	return 0;
}

//===========================================
//              music类 
//===========================================
music::~music()
{
	for(int i=0;i<5;i++)
		p[i].~player();
}

bool music::openmusic(int track,const char* path)
{
	return p[track].openmusic(path);
}

bool music::open_and_playmusic(int track,const char* path)
{
	if(!p[track].openmusic(path))
		return false;
	if(!p[track].playmusic())
		return false;

	return true;
}

bool music::playmusic(int track)
{
	return p[track].playmusic();
}

bool music::stopmusic(int track)
{
	return p[track].stopmusic();
}

bool music::getFFTData(int *Amplitude,int num)
{
	return p[0].getFFTData(Amplitude,num);
}


//===========================================
//              player类 
//===========================================
player::player()
{
	zplayer = CreateZPlay();
	zplayer->SetCallbackFunc(Module_Music_CallbackFunc, (TCallbackMessage) (MsgPlay + MsgStop), this);
	state = isstop;
	volume = 0;
}

player::~player()
{
	if(zplayer)
	{
		MusicModuleExit = true;
		zplayer->Release();
		zplayer = 0;
	}
}

bool player::openmusic(const char* path)
{
	if(state==isplaying)
		stopmusic();

	state = isstop;
	if(zplayer->OpenFile(path, sfAutodetect/*自动解析*/))
		return true;
	return false;
}

bool player::playmusic()
{
	state = isplaying;
	if(zplayer->Play())
	{
		/*
		//声音渐入
		TStreamTime start;
		TStreamTime end;
		zplayer->GetPosition(&start);
		end.sec = start.sec + 1;
		zplayer->SlideVolume(tfSecond, &start, volume,volume, tfSecond, &end, 100,100);
		volume = 100;//*/

		return true;
	}
	return false;
}

bool player::stopmusic()
{
	//声音渐出
	/*
	TStreamTime start;
	TStreamTime end;
	zplayer->GetPosition(&start);
	end.sec = start.sec + 1;
	zplayer->SlideVolume(tfSecond, &start, volume,volume, tfSecond, &end, 0,0);
	volume = 0;//*/


	state = isstop;
	if(zplayer->Stop())
		return true;//*/

	return true;
}

bool player::getFFTData(int *Amplitude,int num)
{
	//必须是8的倍数+1
//	int nLeftAmplitude[257]; // we are using 512 FFT points: number of harmonics is 512/2 + 1 = 257
	if(zplayer->GetFFTData((num-1)*2, libZPlay::fwTriangular, 0, 0,Amplitude, 0, 0, 0))
		return true;

	return false;
}