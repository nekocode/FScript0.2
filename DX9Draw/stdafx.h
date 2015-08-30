// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <tchar.h>

// DirectX ͷ�ļ�
#include <d3d9.h>
#include <d3dx9.h>

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DX9DRAW_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DX9DRAW_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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

// ȫ�ֱ���
extern HMODULE hmod;
extern HWND dhwnd;
extern RECT drect;
extern LPDIRECT3D9 g_pD3D;						//Direct3D����
extern IDirect3DDevice9* g_pd3dD;				//Direct3D�豸����
extern item *render_head,*render_now;			//��Ⱦ����
