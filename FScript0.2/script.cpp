#include "StdAfx.h"
#include "script.h"
#include "UI_Interface.h"

fscript *module_fscript = 0;
bool ScriptThreadExit = false;

void OpenGame(Game game);

//�ű������߳�,��UI�߳�
void Script_Process()
{
	MainUI.UI_Init();
	MainUI.UI_Show();

	while(1)
	{
		Sleep(1);
		//���ܵ��߳��˳���Ϣ
		if(ScriptThreadExit)
			break;
	}
}

//Game_EntryΪ��ϷĿ¼  ��Ϸ�����ű�: title.txt->�ű����
void OpenGame(Game game)
{
	module_fscript = new fscript;

	//���ù���Ŀ¼
	MySetCurrentDirectory(game.game_path);

	if(module_fscript->open(game.game_entry))
	{
		module_fscript->dofs();
	}

	delete module_fscript;
	module_fscript = 0;
}




/**********************************************
                fscript��
**********************************************/
fscript::fscript()
{
	f = 0;buf = 0;filelen = 0;
	DX_SetBtnCallBackFunc(btn_Callback);
}

bool fscript::open(const char *path)
{
	if(!fopen_s(&f,path,"rb"))
	{
		fseek(f,0,2);
		filelen = ftell(f);
		fseek(f,0,0);

		buf = new char[filelen+1];
		fread(buf,filelen,1,f);
		buf[filelen] = 0;	//������

		return true;
	}

	char err_out[100];
	sprintf_s(err_out,"�޷��򿪽ű�:%s",path);
	MessageBoxA(0,err_out,"����",0);
	return false;
}

void fscript::dofs()
{
	lua.executeCode(buf);
}