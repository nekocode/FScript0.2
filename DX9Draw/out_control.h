//imgout->imglist->dximg

//=======================================
//         ����<ͼ��>�ṹ
//=======================================
struct imgout
{
	item *img;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	void *p_nexttex;		//�����ж��Ƿ��Ѿ�ת����
};//*/

//=======================================
//         ����<����>�ṹ
//=======================================
struct aimgout
{
	item *aimg;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	unsigned int frame_start;		//��ʼ֡
	unsigned int frame_end;		//����֡
	unsigned int frame_now;		//��ǰ֡(������֡����ֹͣ����)
	unsigned int frame_all;		//��֡��
	unsigned int delay;			//Ĭ��ÿ֡�ӳ� (���Ϊ0��ʹ��ÿһ֡��ͬ���ӳٲ���,��Ϊ0��ÿ֡���ӳٲ���ʧЧ)
	unsigned int delay_count;		//�ӳٵļ�����
	bool autoplay;
	bool loop;
};//*/


//=======================================
//         ����<����>�ṹ
//=======================================
struct textout
{
	item *text;

	wchar_t **_strText;
	POINT point;
	unsigned int font_width;
	unsigned int font_height;
	unsigned int line_width;
	bool center;
	D3DCOLOR color;
};//*/


//=======================================
//             ��������
/*/=======================================
DX9DRAW_API bool _addimg(imgout** _img,wchar_t *_name,void* _foward);
DX9DRAW_API bool _addaimg(aimgout** _aimg,wchar_t *_name,unsigned int _delay,void* _foward);
DX9DRAW_API bool _addtext(textout** _text,wchar_t *_strText,void* _foward);

DX9DRAW_API void _delete(void* _r);
DX9DRAW_API void _exchange(void* _r1,void* _r2);
DX9DRAW_API void _fowardinsert(void *_dest,void *_source);

DX9DRAW_API void _fadeimg(imgout *_img,wchar_t *name,float fadestep,float x,float y,unsigned int width,unsigned int height,float alpha);
DX9DRAW_API void _setfademap(imgout *_img,wchar_t *name);


DX9DRAW_API bool _loadimgfromfile(wchar_t *path,bool mask,wchar_t *name);
DX9DRAW_API bool _loadimgfrommem(char *mem,unsigned long size,bool mask,wchar_t *name);

DX9DRAW_API bool _loadaimgfromfile(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
DX9DRAW_API bool _loadimgfrommem(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
DX9DRAW_API bool _loadaimgfromimgs(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name);

DX9DRAW_API void _test();*/