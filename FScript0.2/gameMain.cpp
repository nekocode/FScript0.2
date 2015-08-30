#include "StdAfx.h"
#include "gameMain.h"

bool draw_slow = false;
boost::mutex draw_mutex;
boost::mutex script_mian_mutex;

void cur_process();

//游戏画面渲染函数
void gameRender()
{
	cur_process();

	while(draw_slow)
		Sleep(1);
	LOCK(draw_mutex);

	DX_Render();
}

void cur_process()
{
	static bool cur_isshowed = false;
	POINT pos;

	if(!IsIconic(hwnd))
	{
		GetCursorPos(&pos);   //用windows函数获取鼠标坐标
		ScreenToClient(hwnd,&pos);  //转换为客户坐标

		int x=pos.x*RenderRect.right/ClientRect.right;
		int y=pos.y*RenderRect.bottom/ClientRect.bottom;

		if(x>=0&&x<=ClientRect.right&&y>=0&&y<=ClientRect.bottom)
		{
			if(cur_isshowed) {ShowCursor(false);cur_isshowed=false;};
		}
		else
		{
			if(!cur_isshowed) {ShowCursor(true);cur_isshowed=true;};
		}
	}

}


void Init()
{
//	SetWindowPos(hwnd,HWND_TOPMOST,1000,0,0,0,SWP_NOSIZE);

	if(!initdll())
	{
		MessageBox(NULL,L"无法读取文件图形渲染库!",L"错误",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		SendMessage(hwnd,WM_CLOSE,NULL,NULL);
	}

	// 初始化D3D
	if(!DX_InitD3D(hwnd,&RenderRect,font_lib.c_str()))
	{
		MessageBox(NULL,L"初始化D3D失败!",L"错误",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		SendMessage(hwnd,WM_CLOSE,NULL,NULL);
		return;
	}
}

void ReleaseDxDrawDLL()
{
	if(DX_ReleaseD3D)
		DX_ReleaseD3D();
	FreeLibrary(hDLL);
}

void ReleaseMusicModule()
{
	module_music.~music();
}