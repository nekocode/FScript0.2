#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <fstream>
#include <list>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

extern "C" {
	#include "lua5.21\\lua-5.2.1\\lua.h"
	#include "lua5.21\\lua-5.2.1\\lualib.h"
	#include "lua5.21\\lua-5.2.1\\lauxlib.h"
}
#include "luawrapper\\LuaContext.h"
#include "charexchange.h"

#include "dllfun.h"

#pragma comment(lib,"lua5.21\\Debug\\lua5.21.lib")

//全局变量
extern HINSTANCE hinst;					//当前实例
extern HWND hwnd;						//当前窗口句柄
extern RECT ClientRect;					//客户区当前矩形
extern RECT RenderRect;					//渲染矩形
extern int mouse_x,mouse_y;
extern std::string font_lib;
extern std::string fs_entry;

extern boost::mutex draw_mutex;
extern boost::mutex script_mian_mutex;
extern bool draw_slow;
#define LOCK(m) boost::mutex::scoped_lock lock(m)
//辅助对象.自动设置布尔值
class AOTU_BOOL
{
public:
	AOTU_BOOL(bool &in) {_in = &in;in = true;}
	~AOTU_BOOL() {*_in = false;}
private:
	bool *_in;
};


//函数声明
void MySetCurrentDirectory(std::string Path);