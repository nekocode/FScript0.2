//用于单张图片显示
struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y,
		float _tu, float _tv)
	{
		x  = _x;  y  = _y;  z  = 0.0f;
		tu = _tu;  tv  = _tv;
	}
	FLOAT x, y, z; // The position
	FLOAT tu, tv;   // The texture coordinates
};
#define VertexFVF (D3DFVF_XYZ |D3DFVF_TEX1)
//用于两张纹理之间的切换
struct VertexThree
{
	VertexThree(){}
	VertexThree(float _x, float _y,
		float _tu, float _tv, float _tu2, float _tv2, float _tu3, float _tv3)
	{
		x  = _x;  y  = _y;  z  = 0.0f;
		tu = _tu;  tv  = _tv;
		tu2 = _tu2;  tv2  = _tv2;
		tu3 = _tu3;  tv3  = _tv3;
	}
	FLOAT x, y, z; // The position
	FLOAT tu, tv, tu2, tv2, tu3, tv3;   // The texture coordinates
};
#define VertexThreeFVF (D3DFVF_XYZ |D3DFVF_TEX1 |D3DFVF_TEX2 |D3DFVF_TEX3)

bool InitImg();
void ReleaseImg();
bool LoadImgFromFile(wchar_t *path,bool mask,wchar_t *name);
bool LoadImgFromMem(char *mem,unsigned long size,bool mask,wchar_t *name);

bool LoadaImgFromImgs(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name);
bool LoadaImgFromFile(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);
bool LoadaImgFromMem(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name);


//=======================================
//            静态纹理
//=======================================
struct mytex
{
	LPDIRECT3DTEXTURE9 tex;
	unsigned int width,height;
};

struct texlist
{
	wchar_t *name;
	mytex *tex;
	texlist *next;
};

extern texlist *texlist_headp[256*256],*texlist_nowp[256*256];


//=======================================
//               帧
//=======================================
struct frame
{
	unsigned int delay;
	mytex *tex;
};

struct frames
{
	unsigned int frame_number;
	frame *f;
};

struct framelist
{
	wchar_t *name;
	frames *fs;
	framelist *next;
};

extern framelist *framelist_headp[256*256],*framelist_nowp[256*256];



//=======================================
//              转换纹理
//=======================================
struct NextTex
{
	// 过渡纹理以及目标纹理
	mytex					*lpt_next;
	mytex					*lpt_map;
	unsigned int			next_width,next_height;
	float					next_x,next_y;
	float					next_alpha;
	float					fade_alpha;
	float					fadestep;
	LPDIRECT3DVERTEXBUFFER9	lpVB_change;
};



//=======================================
//              btnmap
//=======================================
typedef int (__stdcall *callbackfunc)(void *outitem,int type,int msg,void *data);

//规则按钮通常没有属性,依靠nowx,nowy和width,height(自身所带)属性来refresh fullmap
//不规则按钮全部属性都有
//通过min_alpha来判断规则(min_alph==0)与不规则按钮(min_alph>0)
struct btnmap
{
	byte **btn_map;
	int min_alpha;

	int x_delta,y_delta;	//用于不规则图形的偏移
	int width,height;		//用于不规则图形的宽高

	void *outitem;
	void *data;
};

class imgbase : public item
{
public:
	unsigned int *width,*height;
	float *nowx,*nowy;
	float *rota,*rota_x,*rota_y;
	float *alpha;

	//button
	btnmap *btn_map;
	bool btn_enable;

	imgbase();
	~imgbase();
	void setpos(float x,float y);
	void setsize(float _width,float _height);
	void setrota(float rotation,float x,float y);

	void rtt_draw_func();
	bool rendr_to_texture(mytex* &tex,int &x_delta,int &y_delta);

	void btn_bind(void *outitem,int min_alpha,void *data);
	void refresh_map();
protected:
	D3DXMATRIXA16			trans, rot, scale;	//变换矩阵
	mytex					*lpt;
};

class fimg : public imgbase
{
public:
	NextTex		*nt;

	fimg();
	void release();
	void draw();
	void setfademap(wchar_t *name);
	void fade(wchar_t *name,float fadestep,float nowx2,float nowy2,unsigned int width2,unsigned int height2,float alpha2);

	bool load(wchar_t *name);
	
};

class faimg : public imgbase
{
public:
	unsigned int *frame_start;	//开始帧
	unsigned int *frame_end;		//结束帧
	unsigned int *frame_now;		//当前帧(大于总帧数则停止播放)
	unsigned int *frame_all;		//总帧数
	unsigned int *delay;			//默认每帧延迟 (如果为0则使用每一帧不同的延迟参数,不为0则每帧的延迟参数失效)
	unsigned int *delay_count;	//延迟的计数器
	bool *autoplay;
	bool *loop;

	faimg();
	void release();
	void draw();
	bool load(wchar_t *name);

private:
	frame *f;
	unsigned int delay_perframe;//每一帧的延迟
};


struct fullmap
{
	imgbase* **full_map;
	int width,height;
};
extern fullmap full_map;


extern fimg *system_cur;					//系统鼠标