#include "StdAfx.h"
#include "script.h"
#include "UI_Interface.h"

fscript *module_fscript = 0;
bool ScriptThreadExit = false;

void OpenGame(Game game);

//脚本处理线程,主UI线程
void Script_Process()
{
	MainUI.UI_Init();
	MainUI.UI_Show();

	while(1)
	{
		Sleep(1);
		//接受到线程退出消息
		if(ScriptThreadExit)
			break;
	}
}

//Game_Entry为游戏目录  游戏启动脚本: title.txt->脚本入口
void OpenGame(Game game)
{
	module_fscript = new fscript;

	//设置工作目录
	MySetCurrentDirectory(game.game_path);

	if(module_fscript->open(game.game_entry))
	{
		module_fscript->dofs();
	}

	delete module_fscript;
	module_fscript = 0;
}




/**********************************************
                fscript类
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
		buf[filelen] = 0;	//结束符

		return true;
	}

	char err_out[100];
	sprintf_s(err_out,"无法打开脚本:%s",path);
	MessageBoxA(0,err_out,"错误",0);
	return false;
}

void fscript::dofs()
{
	lua.executeCode(buf);
}