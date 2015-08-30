#pragma once
//=======================================
//         导出<图像>结构
//=======================================
struct fimg
{
	void *img;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	void **p_nexttex;		//用于判断是否已经转换完
};

//=======================================
//         导出<动画>结构
//=======================================
struct faimg
{
	void *aimg;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	unsigned int frame_start;		//开始帧
	unsigned int frame_end;			//结束帧
	unsigned int frame_now;			//当前帧(大于总帧数则停止播放)
	unsigned int frame_all;			//总帧数
	unsigned int delay;				//默认每帧延迟 (如果为0则使用每一帧不同的延迟参数,不为0则每帧的延迟参数失效)
	unsigned int delay_count;		//延迟的计数器
	bool autoplay;
	bool loop;
};//*/

//=======================================
//         导出<图像>结构
//=======================================
struct ftext
{
	void *text;

	wchar_t **_strText;
	POINT point;
	unsigned int font_width;
	unsigned int font_height;
	unsigned int line_width;
	bool center;
	DWORD color;
};//*/

//回调函数
typedef int (__stdcall *btn_callbackfunc)(void *outitem,int type,int msg,void *data);

extern HMODULE hDLL;
extern bool(*DX_InitD3D)(HWND _hwnd, RECT* _rect,const char* font_lib); 
extern void(*DX_Render)(void);
extern void(*DX_ReleaseD3D)(void);

extern bool(*DX_AddImg)(fimg** _img,wchar_t *_name,void* _foward);
extern bool(*DX_AddaImg)(faimg** _aimg,wchar_t *_name,unsigned int _delay,void* _foward);
extern bool(*DX_AddText)(ftext** _img,wchar_t *_strText, void* _foward);

extern void(*DX_Delete)(void* _r);
extern void(*DX_Exchange)(void* _r1, void* _r2);
extern void(*DX_FowardInsert)(void* _r1, void* _r2);

extern bool(*DX_FadeImg)(fimg* _r,wchar_t *name, float _fadestep, float _x, float _y, unsigned int _width, unsigned int _height, float _alpha);
extern bool(*DX_SetFadeMap)(fimg* _r, wchar_t *name);

extern bool(*DX_LoadImgFromFile)(wchar_t *path, bool mask, wchar_t *name);
extern bool(*DX_LoadImgFromMem)(char *mem, unsigned long size, bool mask, wchar_t *name);
extern bool(*DX_LoadaImgFromFile)(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
extern bool(*DX_LoadaImgFromMem)(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
extern bool(*DX_LoadaImgFromImgs)(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name);

extern void(*DX_BindBtn)(void *_r,int min_alpha,void *data);
extern void(*DX_SetBtnEnable)(void *_r,bool enable);
extern void(*DX_SetBtnCallBackFunc)(btn_callbackfunc func);
extern void(*DX_RefreshBtnmap)(void *_r);
extern void(*DX_RefreshAllmap)();
extern void(*DX_SetMousePos)(int x,int y);
extern void(*DX_SetMouseClick)(int msg);

extern bool(*DX_SetCur)(wchar_t *name);
bool initdll();
