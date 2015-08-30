#include "StdAfx.h"
#include "script.h"
#include "music.h"
#include "msg.h"

Lua::LuaContext *FSLua;
int echospeed = 50;


/*==================================================
				辅助对象or函数
==================================================*/
//用来获取fadeimg内的nexttex指针
struct getischanging
{
	void *p;
};

//fs类的泛型定义(用于获取指针或者isbtn属性)
class fspoint
{
public:
	void *p;
	bool isbtn;
};

//检测对象是否为btn,如果为真则刷新btnmap
inline void btnneedrefresh(void *fsobject)
{
	fspoint *btn = (fspoint*)fsobject;
	if(btn->isbtn)
		DX_RefreshAllmap();
}



/*==================================================
				lua 中可控制对象
==================================================*/
class fsimg
{
public:
	fimg *fi;
	bool isbtn;

	fsimg(){fi=0;isbtn=false;DX_AddImg(&fi,L"",0);}
	~fsimg(){DX_Delete(fi);if(isbtn) DX_RefreshAllmap();}
	void setimg(std::string name)
	{
		wchar_t *wtmp = ctow(name.c_str());
		DX_AddImg(&fi,wtmp,0);
		delete[] wtmp;

		btnneedrefresh(this);
	}

	void setsize(unsigned int width,unsigned int height){fi->width = width;fi->height = height;}
	void setpos(float nowx,float nowy){fi->nowx = nowx;fi->nowy = nowy;  btnneedrefresh(this);}
	void setrota(float rota,float rota_x,float rota_y){fi->rota = rota;fi->rota_x = rota_x;fi->rota_y = rota_y;}
	void setalpha(float alpha){fi->alpha = alpha;}
	
	void bindbtn(int min_alpha,std::string cbfunc)
	{
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			char *name = new char[cbfunc.length()+1];
			strcpy_s(name,cbfunc.length()+1,cbfunc.c_str());		//-----!
			DX_BindBtn(fi,min_alpha,name);
		}
		DX_RefreshAllmap();
		isbtn = true;
	}
	void setbtnenable(bool enable){DX_SetBtnEnable(fi,enable); DX_RefreshAllmap();}
	void refreshbtn()
	{
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			DX_RefreshBtnmap(fi);
		}
		DX_RefreshAllmap();
	}

	int width(){return fi->width;}
	int height(){return fi->height;}
	double nowx(){return fi->nowx;}
	double nowy(){return fi->nowy;}
	double rota(){return fi->rota;}
	double rota_x(){return fi->rota_x;}
	double rota_y(){return fi->rota_y;}
	double alpha(){return fi->alpha;}
	bool ischanging(){
		if(*(fi->p_nexttex))
			return (((getischanging *)(*(fi->p_nexttex)))->p)>0;
		return false;
	}


	void fadeimg(std::string name) {
		wchar_t *wtmp = ctow(name.c_str());

		float fadestep = FSLua->readVariable<float>("nextimg.fadestep");
		float x = FSLua->readVariable<float>("nextimg.x");
		float y = FSLua->readVariable<float>("nextimg.y");
		unsigned int width = FSLua->readVariable<int>("nextimg.width");
		unsigned int height = FSLua->readVariable<int>("nextimg.height");
		float alpha = FSLua->readVariable<float>("nextimg.alpha");

		DX_FadeImg(fi,wtmp,fadestep,x,y,width,height,alpha);
		delete[] wtmp;
	}

	void setfademap(std::string name) {
		wchar_t *wtmp = ctow(name.c_str());
		DX_SetFadeMap(fi,wtmp);
		delete[] wtmp;
	}
};

class fstext
{
public:
	ftext *ft;

	fstext(){ft=0;DX_AddText(&ft,L"",0);}
	~fstext(){DX_Delete(ft);DX_RefreshAllmap();}
	void settext(std::string str)
	{
		wchar_t *wtmp = ctow(str.c_str());

		DX_AddText(&ft,wtmp,0);
		delete[] wtmp;
	}
	void settextW(wchar_t *str)
	{
		DX_AddText(&ft,str,0);
	}

	void setpos(int nowx,int nowy){ft->point.x = nowx;ft->point.y = nowy;}
	void setfontsize(unsigned int width,unsigned int height){ft->font_width = width;ft->font_height = height;}
	void setlinewidth(unsigned int width){ft->line_width = width;}
	void setiscenter(bool center){ft->center = center;}
	void setcolor(int a,int r,int g,int b){ft->color = ((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff));}

	std::string text(){return std::string(wtoc(*(ft->_strText)));}
	int nowx(){return ft->point.x;}
	int nowy(){return ft->point.y;}
	int font_width(){return ft->font_width;}
	int font_height(){return ft->font_height;}
	int line_width(){return ft->line_width;}
	bool center(){return ft->center;}
};

class fsaimg
{
public:
	faimg *fai;
	bool isbtn;

	fsaimg(){fai=0;isbtn=false;DX_AddaImg(&fai,L"",0,0);}
	~fsaimg(){DX_Delete(fai);if(isbtn) DX_RefreshAllmap();}
	void setimg(std::string name,int delay)
	{
		wchar_t *wtmp = ctow(name.c_str());

		DX_AddaImg(&fai,wtmp,delay,0);
		delete[] wtmp;
	}

	void setsize(unsigned int width,unsigned int height){fai->width = width;fai->height = height;}
	void setpos(float nowx,float nowy){fai->nowx = nowx;fai->nowy = nowy;  btnneedrefresh(this);}
	void setrota(float rota,float rota_x,float rota_y){fai->rota = rota;fai->rota_x = rota_x;fai->rota_y = rota_y;}
	void setalpha(float alpha){fai->alpha = alpha;}

	void setframe(unsigned int frame_now){fai->frame_now = frame_now-1;fai->delay_count = 0;}
	void setframelen(unsigned int frame_start,unsigned int frame_end){fai->frame_start = frame_start-1;fai->frame_end = frame_end-1;fai->delay_count = 0;}
	void setdelay(unsigned int delay){fai->delay = delay;fai->delay_count = 0;}
	void setautoplay(bool autoplay){fai->autoplay = autoplay;}
	void setloop(bool loop){fai->loop = loop;}

	void bindbtn(int min_alpha,std::string cbfunc)
	{
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			char *name = new char[cbfunc.length()+1]; 
			strcpy_s(name,cbfunc.length()+1,cbfunc.c_str()); 
			DX_BindBtn(fai,min_alpha,name); 
		}
		DX_RefreshAllmap();
		isbtn = true;
	}
	void setbtnenable(bool enable){DX_SetBtnEnable(fai,enable); DX_RefreshAllmap();}
	void refreshbtn()
	{
		{
			AOTU_BOOL auto_slow(draw_slow);
			LOCK(draw_mutex);
			DX_RefreshBtnmap(fai);
		}
		DX_RefreshAllmap();
	}

	//读取数据
	int width(){return fai->width;}
	int height(){return fai->height;}
	double nowx(){return fai->nowx;}
	double nowy(){return fai->nowy;}
	double rota(){return fai->rota;}
	double rota_x(){return fai->rota_x;}
	double rota_y(){return fai->rota_y;}
	double alpha(){return fai->alpha;}

	int frame_now(){return fai->frame_now;}
	int frame_start(){return fai->frame_start;}
	int frame_end(){return fai->frame_end;}
	int frame_all(){return fai->frame_all;}
	int delay(){return fai->delay;}
	bool autoplay(){return fai->autoplay;}
	bool loop(){return fai->loop;}
};

fstext *echo=0,*rolename=0;



/*==================================================
				lua command
==================================================*/
//fs自带函数
void fssleep(int time)
{
	//接受到线程退出消息
	if(ScriptThreadExit)
		return;

	Sleep(time);
}

void fsecho(std::string str,std::string name)
{
	bool ptr = false;
	if(name.length())
		ptr = true;
	FSLua->callLuaFunction<void,bool>("_echo_start",ptr);

	wchar_t *oldstr = ctow(str.c_str());
	int len = wcslen(oldstr);
	wchar_t *ecstr = new wchar_t[len+1];
	for(int i=0;i<len+1;i++) ecstr[i] = 0;
	memset(ecstr,0,sizeof(wchar_t)*(len+1));
	int i=0,i2=0;

	rolename->settext(name);

	bool echopass = false;

	{
		AOTU_BOOL auto_setinecho(inecho);
		for(;i<len;i++)
		{
			if(oldstr[i]==1)
				continue;
			else if(oldstr[i]==3)
			{
				if(!echopass){echo->settextW(ecstr); fssleep(echospeed);}

				ecstr[i2] = '\n';
				i2++;
				continue;
			}
			else if(oldstr[i]==2)
			{
				/*while(!inecho_isclicked){Sleep(5);};
				inecho_isclicked=false;*/
				FSLua->callLuaFunction<void>("_echo_stop");
				inecho_isclicked=false;
				continue;
			}

			if(!echopass){echo->settextW(ecstr); fssleep(echospeed);}

			ecstr[i2] = oldstr[i];
			i2++;

			if(inecho_isclicked){inecho_isclicked=false;echopass=true;}
		}

		echo->settextW(ecstr);
		fssleep(echospeed);
		inecho_isclicked=false;
		delete[] oldstr;
		delete[] ecstr;
	}
	
	FSLua->callLuaFunction<void>("_echo_pause");
}

void fspause()
{
	AOTU_BOOL auto_setinpause(inpause);

	while(!inpause_rego){if(ScriptThreadExit) return;Sleep(5);};

	inpause_rego = false;
}

void fsstop()
{
	AOTU_BOOL auto_setinstop(instop);

	while(!instop_isclicked){if(ScriptThreadExit) return; Sleep(5);}

	instop_isclicked = false;
}

void fsplaymusic(int track,std::string path,bool loop)
{
	module_music.open_and_playmusic(track,path.c_str());
	module_music.p[track].loop = loop;
}

void fsstopmusic(int track)
{
	module_music.stopmusic(track);
}

//读取文件函数
void fsloadimg(std::string p,bool mask,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	wchar_t *wtmp = ctow(p.c_str());
	wchar_t *wtmp2 = ctow(p2.c_str());

	DX_LoadImgFromFile(wtmp,mask,wtmp2);
	delete[] wtmp;
	delete[] wtmp2;
}

void fsloadaimg(std::string p,int hn,int vn,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	wchar_t *wtmp = ctow(p.c_str());
	wchar_t *wtmp2 = ctow(p2.c_str());

	DX_LoadaImgFromFile(wtmp,hn,vn,0,wtmp2);
	delete[] wtmp;
	delete[] wtmp2;
}

//创建对象函数
void fsaddimg(std::string p,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fsimg *newitem;
	FSLua->writeVariable(p.c_str(), std::unique_ptr<fsimg>(newitem = new fsimg()));

	if(strlen(p2.c_str()))
		newitem->setimg(p2);
}

void fsaddtext(std::string p,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fstext *newitem;
	FSLua->writeVariable(p.c_str(), std::unique_ptr<fstext>(newitem = new fstext()));

	if(strlen(p2.c_str()))
		newitem->settext(p2);
}

void fsaddaimg(std::string p,std::string p2,int delay)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fsaimg *newitem;
	FSLua->writeVariable(p.c_str(), std::unique_ptr<fsaimg>(newitem = new fsaimg()));

	if(strlen(p2.c_str()))
		newitem->setimg(p2,delay);
}

//改变顺序函数
void fsexchange(std::string p,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fspoint *f_item1,*f_item2;
	if(strlen(p.c_str()))
	{
		f_item1 = (fspoint*)FSLua->fs_readObjP(p.c_str());
	}
	if(strlen(p2.c_str()))
	{
		f_item2 = (fspoint*)FSLua->fs_readObjP(p2.c_str());
	}

	DX_Exchange(f_item1->p,f_item2->p);
}

void fsfowardinsert(std::string p,std::string p2)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fspoint *f_item1=0,*f_item2=0;

	if(strlen(p2.c_str()))
		f_item2 = (fspoint*)FSLua->fs_readObjP(p2.c_str());
	else
		return;

	if(strlen(p.c_str()))
		f_item1 = (fspoint*)FSLua->fs_readObjP(p.c_str());
	else
	{
		DX_FowardInsert(0,f_item2->p);
		return;
	}

	DX_FowardInsert(f_item1->p,f_item2->p);
}

void fsdelete(std::string p)
{
	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);
	fspoint *f_item;
	if(strlen(p.c_str()))
	{
		f_item = (fspoint*)FSLua->fs_readObjP(p.c_str());
	}

	DX_Delete(f_item->p);
	delete f_item;
	f_item = 0;
}



/*=============================================================
                          myLua类
=============================================================*/
//初始化Lua的基本函数
void InitLuaBaseFunc(Lua::LuaContext *L)
{
	//注册c++tolua函数
	L->writeVariable("doecho",  [](std::string str,std::string name) { fsecho(str,name); });
	L->writeVariable("playm",  [](int track,std::string path,bool loop) { fsplaymusic(track,path,loop); });
	L->writeVariable("stopm",  [](int track) { fsstopmusic(track); });
	L->writeVariable("setechospeed",  [](int p) { echospeed = p; });
	L->writeVariable("sleep",  [](int p) { fssleep(p); });
	L->writeVariable("pause",  []() { fspause(); });
	L->writeVariable("stop",  []() { fsstop(); });
	L->writeVariable("loadimg",  [](std::string p,bool m,std::string p2) { fsloadimg(p,m,p2); });
	L->writeVariable("loadaimg",  [](std::string p,int hn,int vn,std::string p2) { fsloadaimg(p,hn,vn,p2); });
	L->writeVariable("addimg",  [](std::string p,std::string p2) { fsaddimg(p,p2); });
	L->writeVariable("addtext",  [](std::string p,std::string p2) { fsaddtext(p,p2); });
	L->writeVariable("addaimg",  [](std::string p,std::string p2,int delay) { fsaddaimg(p,p2,delay); });
	L->writeVariable("exchange",  [](std::string p,std::string p2) { fsexchange(p,p2); });
	L->writeVariable("fowardinsert",  [](std::string p,std::string p2) { fsfowardinsert(p,p2); });
	L->writeVariable("delete",  [](std::string p) { fsdelete(p); });

	//注册img,text,aimg对象的方法
	//==========================================================
	L->registerFunction("setimg", &fsimg::setimg);
	L->registerFunction("setsize", &fsimg::setsize);
	L->registerFunction("setpos", &fsimg::setpos);
	L->registerFunction("setrota", &fsimg::setrota);
	L->registerFunction("setalpha", &fsimg::setalpha);
	L->registerFunction("bindbtn", &fsimg::bindbtn);
	L->registerFunction("setbtnenable", &fsimg::setbtnenable);
	L->registerFunction("refreshbtn", &fsimg::refreshbtn);
	L->registerFunction("width", &fsimg::width);
	L->registerFunction("height", &fsimg::height);
	L->registerFunction("nowx", &fsimg::nowx);
	L->registerFunction("nowy", &fsimg::nowy);
	L->registerFunction("rota", &fsimg::rota);
	L->registerFunction("rota_x", &fsimg::rota_x);
	L->registerFunction("rota_y", &fsimg::rota_y);
	L->registerFunction("alpha", &fsimg::alpha);
	L->registerFunction("ischanging", &fsimg::ischanging);
	L->registerFunction("fadeimg", &fsimg::fadeimg);
	L->registerFunction("setfademap", &fsimg::setfademap);
	//==========================================================
	L->registerFunction("settext", &fstext::settext);
	L->registerFunction("setpos", &fstext::setpos);
	L->registerFunction("setfontsize", &fstext::setfontsize);
	L->registerFunction("setlinewidth", &fstext::setlinewidth);
	L->registerFunction("setiscenter", &fstext::setiscenter);
	L->registerFunction("setcolor", &fstext::setcolor);
	L->registerFunction("text", &fstext::text);
	L->registerFunction("nowx", &fstext::nowx);
	L->registerFunction("nowy", &fstext::nowy);
	L->registerFunction("font_width", &fstext::font_width);
	L->registerFunction("font_height", &fstext::font_height);
	L->registerFunction("line_width", &fstext::line_width);
	L->registerFunction("center", &fstext::center);
	//==========================================================
	L->registerFunction("setimg", &fsaimg::setimg);
	L->registerFunction("setsize", &fsaimg::setsize);
	L->registerFunction("setpos", &fsaimg::setpos);
	L->registerFunction("setrota", &fsaimg::setrota);
	L->registerFunction("setalpha", &fsaimg::setalpha);
	L->registerFunction("setframe", &fsaimg::setframe);
	L->registerFunction("setframelen", &fsaimg::setframelen);
	L->registerFunction("setdelay", &fsaimg::setdelay);
	L->registerFunction("setautoplay", &fsaimg::setautoplay);
	L->registerFunction("setloop", &fsaimg::setloop);
	L->registerFunction("bindbtn", &fsaimg::bindbtn);
	L->registerFunction("setbtnenable", &fsaimg::setbtnenable);
	L->registerFunction("refreshbtn", &fsaimg::refreshbtn);
	L->registerFunction("width", &fsaimg::width);
	L->registerFunction("height", &fsaimg::height);
	L->registerFunction("nowx", &fsaimg::nowx);
	L->registerFunction("nowy", &fsaimg::nowy);
	L->registerFunction("rota", &fsaimg::rota);
	L->registerFunction("rota_x", &fsaimg::rota_x);
	L->registerFunction("rota_y", &fsaimg::rota_y);
	L->registerFunction("alpha", &fsaimg::alpha);
	L->registerFunction("frame_now", &fsaimg::frame_now);
	L->registerFunction("frame_start", &fsaimg::frame_start);
	L->registerFunction("frame_end", &fsaimg::frame_end);
	L->registerFunction("frame_all", &fsaimg::frame_all);
	L->registerFunction("delay", &fsaimg::delay);
	L->registerFunction("autoplay", &fsaimg::autoplay);
	L->registerFunction("loop", &fsaimg::loop);
	//==========================================================

}

myLua::myLua()
{
	L = new Lua::LuaContext;

	AOTU_BOOL auto_slow(draw_slow);
	LOCK(draw_mutex);

	//初始化lua的基本函数
	InitLuaBaseFunc(L);

	//FS固有对象
	L->writeVariable("background", std::unique_ptr<fsimg>(new fsimg()));
	L->writeVariable("role1", std::unique_ptr<fsimg>(new fsimg()));
	L->writeVariable("role2", std::unique_ptr<fsimg>(new fsimg()));
	L->writeVariable("echo", std::unique_ptr<fstext>(echo = new fstext()));
	L->writeVariable("rolename", std::unique_ptr<fstext>(rolename = new fstext()));

	//传递指针给全局变量FSLua
	FSLua = L;
}

myLua::~myLua()
{
	FSLua = 0;
	delete L;
}

void myLua::executeCode(const char *code)
{
	try
	{
		L->executeCode(code);
	}
	catch(Lua::LuaContext::ExecutionErrorException &err)		{MessageBoxA(0,err.what(),"脚本错误",0);}
	catch(Lua::LuaContext::VariableDoesntExistException &err)	{MessageBoxA(0,err.what(),"脚本错误",0);}
	catch(Lua::LuaContext::SyntaxErrorException &err)			{MessageBoxA(0,err.what(),"脚本错误",0);}
	catch(Lua::LuaContext::WrongTypeException &err)				{MessageBoxA(0,err.what(),"脚本错误",0);}
}