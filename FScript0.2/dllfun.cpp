#include "StdAfx.h"
#include "dllfun.h"

#define AUTOFUNC *(FARPROC *)&

HMODULE hDLL;
bool(*DX_InitD3D)(HWND _hwnd, RECT* _rect,const char* font_lib); 
void(*DX_Render)(void);
void(*DX_ReleaseD3D)(void);

bool(*DX_AddImg)(fimg** _img,wchar_t *_name,void* _foward);
bool(*DX_AddaImg)(faimg** _aimg,wchar_t *_name,unsigned int _delay,void* _foward);
bool(*DX_AddText)(ftext** _img,wchar_t *_strText, void* _foward);

void(*DX_Delete)(void* _r);
void(*DX_Exchange)(void* _r1, void* _r2);
void(*DX_FowardInsert)(void* _r1, void* _r2);

bool(*DX_FadeImg)(fimg* _r,wchar_t *name, float _fadestep, float _x, float _y, unsigned int _width, unsigned int _height, float _alpha);
bool(*DX_SetFadeMap)(fimg* _r, wchar_t *name);

bool(*DX_LoadImgFromFile)(wchar_t *path, bool mask, wchar_t *name);
bool(*DX_LoadImgFromMem)(char *mem, unsigned long size, bool mask, wchar_t *name);
bool(*DX_LoadaImgFromFile)(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
bool(*DX_LoadaImgFromMem)(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
bool(*DX_LoadaImgFromImgs)(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name);

void(*DX_BindBtn)(void *_r,int min_alpha,void *data);
void(*DX_SetBtnEnable)(void *_r,bool enable);
void(*DX_SetBtnCallBackFunc)(btn_callbackfunc func);
void(*DX_RefreshBtnmap)(void *_r);
void(*DX_RefreshAllmap)();
void(*DX_SetMousePos)(int x,int y);
void(*DX_SetMouseClick)(int msg);

bool(*DX_SetCur)(wchar_t *name);

bool initdll()
{
	// 读取动态链接库
	hDLL = LoadLibraryA("DX9Draw.dll");

	if(hDLL)
	{
		// 获取函数指针
		AUTOFUNC DX_InitD3D				= GetProcAddress(hDLL,"_InitD3D");
		AUTOFUNC DX_Render				= GetProcAddress(hDLL,"_Render");
		AUTOFUNC DX_ReleaseD3D			= GetProcAddress(hDLL,"_ReleaseD3D");

		AUTOFUNC DX_AddImg				= GetProcAddress(hDLL,"_addimg");
		AUTOFUNC DX_AddaImg				= GetProcAddress(hDLL,"_addaimg");
		AUTOFUNC DX_AddText				= GetProcAddress(hDLL,"_addtext");

		AUTOFUNC DX_Delete				= GetProcAddress(hDLL,"_delete");
		AUTOFUNC DX_Exchange			= GetProcAddress(hDLL,"_exchange");
		AUTOFUNC DX_FowardInsert		= GetProcAddress(hDLL,"_fowardinsert");

		AUTOFUNC DX_FadeImg				= GetProcAddress(hDLL,"_fadeimg");
		AUTOFUNC DX_SetFadeMap			= GetProcAddress(hDLL,"_setfademap");

		AUTOFUNC DX_LoadImgFromFile		= GetProcAddress(hDLL,"_loadimgfromfile");
		AUTOFUNC DX_LoadImgFromMem		= GetProcAddress(hDLL,"_loadimgfrommem");

		AUTOFUNC DX_LoadaImgFromFile	= GetProcAddress(hDLL,"_loadaimgfromfile");
		AUTOFUNC DX_LoadaImgFromMem		= GetProcAddress(hDLL,"_loadaimgfrommem");
		AUTOFUNC DX_LoadaImgFromImgs	= GetProcAddress(hDLL,"_loadaimgfromimgs");

		AUTOFUNC DX_BindBtn				= GetProcAddress(hDLL,"_bindbtn");
		AUTOFUNC DX_SetBtnEnable		= GetProcAddress(hDLL,"_setbtn_enable");
		AUTOFUNC DX_SetBtnCallBackFunc	= GetProcAddress(hDLL,"_setbtn_callbackfunc");
		AUTOFUNC DX_RefreshBtnmap		= GetProcAddress(hDLL,"_refresh_btnmap");
		AUTOFUNC DX_RefreshAllmap		= GetProcAddress(hDLL,"_refresh_allmap");
		AUTOFUNC DX_SetMousePos			= GetProcAddress(hDLL,"_setmousepos");
		AUTOFUNC DX_SetMouseClick		= GetProcAddress(hDLL,"_setmouseclick");

		AUTOFUNC DX_SetCur				= GetProcAddress(hDLL,"_setcur");
	}
	else
	{
		return false;
	}

	return true;
}