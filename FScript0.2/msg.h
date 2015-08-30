#pragma once
#include "script.h"
LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
int __stdcall btn_Callback(void *outitem,int type,int msg,void *data);

extern bool inecho;
extern bool inecho_isclicked;

extern bool inpause;
extern bool inpause_rego;

extern bool instop;
extern bool instop_isclicked;