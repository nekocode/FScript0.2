#include "StdAfx.h"
#include "gameMain.h"

#include "msg.h"

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				MyShowWindow(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				ReadConf();
//全局变量
HINSTANCE hinst;					//当前实例
HWND hwnd;							//当前窗口句柄
RECT ClientRect;					//客户区当前矩形
RECT RenderRect;					//渲染矩形
int mouse_x,mouse_y;

std::string data_path;
std::string games_path;
std::string font_lib;
std::string fs_entry;

LPCWSTR szTitle;		//标题栏文本
LPCWSTR szWindowClass=L"fscript";				//主窗口类名


INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT )
{
	hinst = hInstance;			//将实例句柄存储在全局变量中
	
	MyRegisterClass(hInstance);	//注册窗口类
	ReadConf();					//读取配置文件

	//创建并显示主窗口
	if (MyShowWindow(hInstance, SW_SHOWDEFAULT))
	{
		Init();

		ShowCursor(false);
#ifndef _DEBUG
		//创建脚本处理线程
		boost::thread Thread_Script_Process(Script_Process);
#endif

		//主消息循环:
		MSG msg;
		ZeroMemory(&msg,sizeof(msg));
		while(msg.message != WM_QUIT)
		{
			if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				gameRender();
		}

		//传递线程退出消息给ScriptThread
		ScriptThreadExit = true;
		ReleaseMusicModule();

#ifndef _DEBUG
		//等待脚本处理线程结束
		Thread_Script_Process.join();
#endif

		//释放dx渲染库
		ReleaseDxDrawDLL();
	}

	UnregisterClass(szWindowClass,hinst);
	return 0;
}



//
//  函数:	MyRegisterClass()
//
//  目的:	注册窗口类。
//
//  注释:	仅当希望此代码与添加到 Windows 95 中的“RegisterClassEx”
//			函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//			这样应用程序就可以获得关联的“格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= MsgProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(NULL, IDI_APPLICATION);或者LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXDEMO1))
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //背景颜色:黑色
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}



//
//   函数:	MyShowWindow(HINSTANCE, int)
//
//   目的:	创建主窗口
//
BOOL MyShowWindow(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	//依据所需客户矩形的大小，计算需要的窗口矩形的大小(false=没有菜单)
	RECT rect={0,0,800,600};
	AdjustWindowRect(&ClientRect, WS_OVERLAPPEDWINDOW, false);

	hWnd = CreateWindowEx(NULL,
		szWindowClass,						//窗口类名
		szTitle,							//窗口标题
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,						//初始x位置
		CW_USEDEFAULT,						//初始y位置
		ClientRect.right-ClientRect.left,	//初始宽度
		ClientRect.bottom-ClientRect.top,	//初始高度
		NULL,
		NULL,								//窗口菜单句柄
		hInstance,							//实例句柄
		NULL);

	if (!hWnd)
		return FALSE;

	hwnd=hWnd;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void MySetCurrentDirectory(std::string Path)
{
	if(Path.length())
	{
		SetCurrentDirectoryA(Path.c_str());
	}
	else
	{
		static WCHAR CurrentPath[MAX_PATH] = {0};

		if(CurrentPath[0]==0)
		{
			//获得exe所在目录
			WCHAR szbufPath[MAX_PATH];
			GetModuleFileName(NULL,CurrentPath,MAX_PATH);
			GetLongPathName(szbufPath,CurrentPath,MAX_PATH);
			for(int i=wcslen(CurrentPath);i>=0;i--)
			{
				if(CurrentPath[i]=='\\')
				{
					CurrentPath[i+1]='\0';
					break;
				}
			}
		}
		//设置exe所在目录为当前目录
		SetCurrentDirectory(CurrentPath);
	}
}


void ReadConf()
{
	//设置工作目录为exe所在目录
	MySetCurrentDirectory(std::string(""));

	Lua::LuaContext conf;
	try
	{
		conf.executeCode(std::ifstream("init.conf"));

		//初始化窗体大小
		ClientRect.right = conf.readVariable<int>("窗口设置.宽");
		ClientRect.bottom = conf.readVariable<int>("窗口设置.高");
		std::string ctile = conf.readVariable<std::string>("窗口设置.标题");

		szTitle = ctow(ctile.c_str());

		//初始化渲染表面大小
		RenderRect.right = conf.readVariable<int>("渲染表面.宽");
		RenderRect.bottom = conf.readVariable<int>("渲染表面.高");

		//读取各种目录属性
		data_path = conf.readVariable<std::string>("公用数据目录");
		games_path = conf.readVariable<std::string>("游戏目录");

		//设置字库路径
		font_lib = conf.readVariable<std::string>("字库");
		if(!font_lib.length())
			font_lib = "default.ttf";

		//设置脚本入口
		fs_entry = conf.readVariable<std::string>("脚本入口");
		if(!fs_entry.length())
			fs_entry = "0.txt";
	}
	catch(Lua::LuaContext::ExecutionErrorException &err)		{MessageBoxA(0,err.what(),"读取配置出错",0);}
	catch(Lua::LuaContext::VariableDoesntExistException &err)	{MessageBoxA(0,err.what(),"读取配置出错",0);}
	catch(Lua::LuaContext::SyntaxErrorException &err)			{MessageBoxA(0,err.what(),"读取配置出错",0);}
	catch(Lua::LuaContext::WrongTypeException &err)				{MessageBoxA(0,err.what(),"读取配置出错",0);}


}



