#include "StdAfx.h"
#include "msg.h"

bool inecho = false;
bool inecho_isclicked = false;

bool inpause = false;
bool inpause_rego = false;

bool instop = false;
bool instop_isclicked = false;
//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hwnd,&ClientRect);
		break;
	case WM_MOUSEMOVE:
		mouse_x = (int)LOWORD(lParam)*800/ClientRect.right;//ȡ��λmouse_x*800/ClientRect.right
        mouse_y = (int)HIWORD(lParam)*600/ClientRect.bottom;//ȡ��λ

		//����d3d�е��������
		DX_SetMousePos(mouse_x,mouse_y);
		break;
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
		if(inecho)
			inecho_isclicked = true;
		if(inpause)
			inpause_rego = true;

		//����d3d�еİ�����Ϣ
		DX_SetMouseClick(2);
		break;
	case WM_LBUTTONUP:

		//����d3d�еİ�����Ϣ
		DX_SetMouseClick(3);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//
//  ����: btn_Callback(void *outitem,int type,int msg,void *data)
//
//  Ŀ��: ����ť�ص���Ϣ��
//
int __stdcall btn_Callback(void *outitem,int type,int msg,void *data)
{
	//type 0:none 1:fimg 2:faimg
	//msg  0:mouse_over(btn) 1:mouse_in(btn) 2:mouse_down(btn) 3:mouse_up(btn) 4:mouse_click(btn)

	if(FSLua)
		FSLua->callLuaFunction<void,int>((char *)data,msg);

	if(instop&&msg==4)
		instop_isclicked = true;
	return 0;
}