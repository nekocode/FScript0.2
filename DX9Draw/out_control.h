//imgout->imglist->dximg

//=======================================
//         导出<图像>结构
//=======================================
struct imgout
{
	item *img;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	void *p_nexttex;		//用于判断是否已经转换完
};//*/

//=======================================
//         导出<动画>结构
//=======================================
struct aimgout
{
	item *aimg;

	unsigned int width,height;
	float nowx,nowy;
	float rota,rota_x,rota_y;
	float alpha;

	unsigned int frame_start;		//开始帧
	unsigned int frame_end;		//结束帧
	unsigned int frame_now;		//当前帧(大于总帧数则停止播放)
	unsigned int frame_all;		//总帧数
	unsigned int delay;			//默认每帧延迟 (如果为0则使用每一帧不同的延迟参数,不为0则每帧的延迟参数失效)
	unsigned int delay_count;		//延迟的计数器
	bool autoplay;
	bool loop;
};//*/


//=======================================
//         导出<文字>结构
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
//             导出函数
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