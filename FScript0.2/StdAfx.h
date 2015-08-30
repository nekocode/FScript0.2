#pragma once

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

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

//ȫ�ֱ���
extern HINSTANCE hinst;					//��ǰʵ��
extern HWND hwnd;						//��ǰ���ھ��
extern RECT ClientRect;					//�ͻ�����ǰ����
extern RECT RenderRect;					//��Ⱦ����
extern int mouse_x,mouse_y;
extern std::string font_lib;
extern std::string fs_entry;

extern boost::mutex draw_mutex;
extern boost::mutex script_mian_mutex;
extern bool draw_slow;
#define LOCK(m) boost::mutex::scoped_lock lock(m)
//��������.�Զ����ò���ֵ
class AOTU_BOOL
{
public:
	AOTU_BOOL(bool &in) {_in = &in;in = true;}
	~AOTU_BOOL() {*_in = false;}
private:
	bool *_in;
};


//��������
void MySetCurrentDirectory(std::string Path);