#pragma once
//=======================================
//         ����<ͼ��>�ṹ
//=======================================
struct fimg
{
	void *img;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	void **p_nexttex;		//�����ж��Ƿ��Ѿ�ת����
};

//=======================================
//         ����<����>�ṹ
//=======================================
struct faimg
{
	void *aimg;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	unsigned int frame_start;		//��ʼ֡
	unsigned int frame_end;			//����֡
	unsigned int frame_now;			//��ǰ֡(������֡����ֹͣ����)
	unsigned int frame_all;			//��֡��
	unsigned int delay;				//Ĭ��ÿ֡�ӳ� (���Ϊ0��ʹ��ÿһ֡��ͬ���ӳٲ���,��Ϊ0��ÿ֡���ӳٲ���ʧЧ)
	unsigned int delay_count;		//�ӳٵļ�����
	bool autoplay;
	bool loop;
};//*/

//=======================================
//         ����<ͼ��>�ṹ
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

//�ص�����
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
