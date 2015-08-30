#include "StdAfx.h"
#include "UI_Interface.h"
#include "music.h"
#include "resmanager.h"
using namespace std;

#define Btn_addx	0		//btn间间隙
#define Btn_x		0		//btn初始x指

int __stdcall UIbtn_Callback(void *outitem,int type,int msg,void *data);

MainUI_Class MainUI;

MainUI_Class::MainUI_Class()
{
	img_BG=0;
	for(int i=0;i<8;i++) btn[i]=0;
	aimg_loading=0;
}

bool MainUI_Class::UI_Init()
{
	loaduires("res.txt");

	DX_SetCur(L"UI_cur");

	DX_AddImg(&img_BG,L"UIBackground",0);
	img_BG->alpha = 255;


	DX_AddImg(&btn[0],L"UIbtn_home",0);
	DX_AddImg(&btn[1],L"UIbtn_internet",0);
	DX_AddImg(&btn[2],L"UIbtn_movie",0);
	DX_AddImg(&btn[3],L"UIbtn_pic",0);
	DX_AddImg(&btn[4],L"UIbtn_music",0);
	DX_AddImg(&btn[5],L"UIbtn_game",0);
	DX_AddImg(&btn[6],L"UIbtn_chat",0);
	DX_AddImg(&btn[7],L"UIbtn_setting",0);
	
	DX_SetBtnCallBackFunc(UIbtn_Callback);
	for(int i=0;i<8;i++)
	{
		btn[i]->width *= 0.8; 
		btn[i]->height *= 0.8;
		btn[i]->nowy = RenderRect.bottom - btn[i]->height/2;		//上升时的高度
		int *btn_num = new int;
		*btn_num = i;
		DX_BindBtn(btn[i],0,btn_num);
	}
	btn[0]->nowx = Btn_x;
	refresh_btnpos(btn[0]->nowx);
	//*/

	for(int i=0;i<8;i++)
		DX_AddImg(&btn_select[i],L"UIbtn_select",0);

	/*
	DX_AddaImg(&aimg_loading,L"UI_loading",0,0);
	aimg_loading->nowy = 578;
	aimg_loading->alpha = 255;
	*/
	return true;
}

bool MainUI_Class::UI_Show()
{
	//渐入btn
	{
		int oldy = btn[0]->nowy;
		int up = btn[0]->height/2;
		for(double per=0;per<=1;per+=0.0000002)
		{
			for(int i=0;i<8;i++)
			{
				btn[i]->alpha = per*255;
				btn[i]->nowy = oldy-(per*up);
			}
		}
		DX_RefreshAllmap();
	}

	return true;
}

void MainUI_Class::refresh_btnpos(int firstbtn_x)
{
	for(int i=1;i<8;i++)
	{
		btn[i]->nowx = btn[i-1]->nowx + btn[i-1]->width + Btn_addx;
	}
}

bool MainUI_Class::loaduires(const char *uiresfile)
{
	if(!UI_ResManager.LoadResArray(uiresfile))
		return false;

	if(list<img_member*>::iterator *iter = new list<img_member*>::iterator)
	{
		for(*iter = UI_ResManager.imglist.begin();*iter != UI_ResManager.imglist.end();(*iter)++)
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			DX_LoadImgFromFile((**iter)->path,(**iter)->mask,(**iter)->name);
		}
	}

	if(list<aimg_member*>::iterator *iter = new list<aimg_member*>::iterator)
	{
		for(*iter = UI_ResManager.aimglist.begin();*iter != UI_ResManager.aimglist.end();(*iter)++)
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			DX_LoadaImgFromFile((**iter)->path,(**iter)->hnum,(**iter)->vnum,0,(**iter)->name);
		}
	}

	for(int i=1;i<3;i++)
		module_music.openmusic(i,UI_ResManager.musicmap.find(string("UIwav_min"))->second.c_str());

	return true;
}

int __stdcall UIbtn_Callback(void *outitem,int type,int msg,void *data)
{
	//type 0:none 1:fimg 2:faimg
	//msg  0:mouse_over(btn) 1:mouse_in(btn) 2:mouse_down(btn) 3:mouse_up(btn) 4:mouse_click(btn)

	int btn_num = *(int *)data;

	fimg *UI_btn = (fimg*)outitem;

	switch(msg)
	{
	case 0:
		UI_btn->width *= 0.8;
		UI_btn->height *= 0.8;
		UI_btn->nowx += ((UI_btn->width/0.8)-UI_btn->width)/2;
		UI_btn->nowy += ((UI_btn->height/0.8)-UI_btn->height);
		return 0;

	case 1:
		UI_btn->nowx -= ((UI_btn->width/0.8)-UI_btn->width)/2;
		UI_btn->nowy -= ((UI_btn->height/0.8)-UI_btn->height);
		UI_btn->width /= 0.8;
		UI_btn->height /= 0.8;
		module_music.playmusic(btn_num%1+1);
		return 0;

	case 2:
		return 0;
	case 3:
		return 0;
	case 4:
		return 0;
	}
	return 0;
}