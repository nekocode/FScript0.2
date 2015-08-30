#include "StdAfx.h"
#include "msg.h"

bool inecho = false;
bool inecho_isclicked = false;

bool inpause = false;
bool inpause_rego = false;

bool instop = false;
bool instop_isclicked = false;
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
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
		mouse_x = (int)LOWORD(lParam)*800/ClientRect.right;//取低位mouse_x*800/ClientRect.right
        mouse_y = (int)HIWORD(lParam)*600/ClientRect.bottom;//取高位

		//更新d3d中的鼠标坐标
		DX_SetMousePos(mouse_x,mouse_y);
		break;
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
		if(inecho)
			inecho_isclicked = true;
		if(inpause)
			inpause_rego = true;

		//更新d3d中的按键消息
		DX_SetMouseClick(2);
		break;
	case WM_LBUTTONUP:

		//更新d3d中的按键消息
		DX_SetMouseClick(3);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//
//  函数: btn_Callback(void *outitem,int type,int msg,void *data)
//
//  目的: 处理按钮回调消息。
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