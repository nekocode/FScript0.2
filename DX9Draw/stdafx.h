// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <tchar.h>

// DirectX 头文件
#include <d3d9.h>
#include <d3dx9.h>

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DX9DRAW_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DX9DRAW_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DX9DRAW_EXPORTS
#define DX9DRAW_API __declspec(dllexport)
#else
#define DX9DRAW_API __declspec(dllimport)
#endif

#define SAFE_RELEASE(p) { if(p){ (p)->Release(); (p)=0; }}

struct mytex;

#ifndef defitem
#define defitem
class item
{
public:
	item *front,*next;
	int item_type;		//0:ftext    1:fimg    2:faimg

	virtual void release() = 0;
	virtual void draw() = 0;
};
#endif

// 全局变量
extern HMODULE hmod;
extern HWND dhwnd;
extern RECT drect;
extern LPDIRECT3D9 g_pD3D;						//Direct3D对象
extern IDirect3DDevice9* g_pd3dD;				//Direct3D设备对象
extern item *render_head,*render_now;			//渲染链表
