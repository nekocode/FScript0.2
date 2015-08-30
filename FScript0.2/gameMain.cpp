#include "StdAfx.h"
#include "gameMain.h"

bool draw_slow = false;
boost::mutex draw_mutex;
boost::mutex script_mian_mutex;

void cur_process();

//��Ϸ������Ⱦ����
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
		GetCursorPos(&pos);   //��windows������ȡ�������
		ScreenToClient(hwnd,&pos);  //ת��Ϊ�ͻ�����

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
		MessageBox(NULL,L"�޷���ȡ�ļ�ͼ����Ⱦ��!",L"����",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		SendMessage(hwnd,WM_CLOSE,NULL,NULL);
	}

	// ��ʼ��D3D
	if(!DX_InitD3D(hwnd,&RenderRect,font_lib.c_str()))
	{
		MessageBox(NULL,L"��ʼ��D3Dʧ��!",L"����",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
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