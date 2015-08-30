#include "StdAfx.h"
#include "gameMain.h"

#include "msg.h"

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				MyShowWindow(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				ReadConf();
//ȫ�ֱ���
HINSTANCE hinst;					//��ǰʵ��
HWND hwnd;							//��ǰ���ھ��
RECT ClientRect;					//�ͻ�����ǰ����
RECT RenderRect;					//��Ⱦ����
int mouse_x,mouse_y;

std::string data_path;
std::string games_path;
std::string font_lib;
std::string fs_entry;

LPCWSTR szTitle;		//�������ı�
LPCWSTR szWindowClass=L"fscript";				//����������


INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT )
{
	hinst = hInstance;			//��ʵ������洢��ȫ�ֱ�����
	
	MyRegisterClass(hInstance);	//ע�ᴰ����
	ReadConf();					//��ȡ�����ļ�

	//��������ʾ������
	if (MyShowWindow(hInstance, SW_SHOWDEFAULT))
	{
		Init();

		ShowCursor(false);
#ifndef _DEBUG
		//�����ű������߳�
		boost::thread Thread_Script_Process(Script_Process);
#endif

		//����Ϣѭ��:
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

		//�����߳��˳���Ϣ��ScriptThread
		ScriptThreadExit = true;
		ReleaseMusicModule();

#ifndef _DEBUG
		//�ȴ��ű������߳̽���
		Thread_Script_Process.join();
#endif

		//�ͷ�dx��Ⱦ��
		ReleaseDxDrawDLL();
	}

	UnregisterClass(szWindowClass,hinst);
	return 0;
}



//
//  ����:	MyRegisterClass()
//
//  Ŀ��:	ע�ᴰ���ࡣ
//
//  ע��:	����ϣ���˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//			����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//			����Ӧ�ó���Ϳ��Ի�ù����ġ���ʽ��ȷ�ġ�Сͼ�ꡣ
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
	wcex.hIcon			= NULL;//LoadIcon(NULL, IDI_APPLICATION);����LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXDEMO1))
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //������ɫ:��ɫ
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}



//
//   ����:	MyShowWindow(HINSTANCE, int)
//
//   Ŀ��:	����������
//
BOOL MyShowWindow(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	//��������ͻ����εĴ�С��������Ҫ�Ĵ��ھ��εĴ�С(false=û�в˵�)
	RECT rect={0,0,800,600};
	AdjustWindowRect(&ClientRect, WS_OVERLAPPEDWINDOW, false);

	hWnd = CreateWindowEx(NULL,
		szWindowClass,						//��������
		szTitle,							//���ڱ���
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,						//��ʼxλ��
		CW_USEDEFAULT,						//��ʼyλ��
		ClientRect.right-ClientRect.left,	//��ʼ���
		ClientRect.bottom-ClientRect.top,	//��ʼ�߶�
		NULL,
		NULL,								//���ڲ˵����
		hInstance,							//ʵ�����
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
			//���exe����Ŀ¼
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
		//����exe����Ŀ¼Ϊ��ǰĿ¼
		SetCurrentDirectory(CurrentPath);
	}
}


void ReadConf()
{
	//���ù���Ŀ¼Ϊexe����Ŀ¼
	MySetCurrentDirectory(std::string(""));

	Lua::LuaContext conf;
	try
	{
		conf.executeCode(std::ifstream("init.conf"));

		//��ʼ�������С
		ClientRect.right = conf.readVariable<int>("��������.��");
		ClientRect.bottom = conf.readVariable<int>("��������.��");
		std::string ctile = conf.readVariable<std::string>("��������.����");

		szTitle = ctow(ctile.c_str());

		//��ʼ����Ⱦ�����С
		RenderRect.right = conf.readVariable<int>("��Ⱦ����.��");
		RenderRect.bottom = conf.readVariable<int>("��Ⱦ����.��");

		//��ȡ����Ŀ¼����
		data_path = conf.readVariable<std::string>("��������Ŀ¼");
		games_path = conf.readVariable<std::string>("��ϷĿ¼");

		//�����ֿ�·��
		font_lib = conf.readVariable<std::string>("�ֿ�");
		if(!font_lib.length())
			font_lib = "default.ttf";

		//���ýű����
		fs_entry = conf.readVariable<std::string>("�ű����");
		if(!fs_entry.length())
			fs_entry = "0.txt";
	}
	catch(Lua::LuaContext::ExecutionErrorException &err)		{MessageBoxA(0,err.what(),"��ȡ���ó���",0);}
	catch(Lua::LuaContext::VariableDoesntExistException &err)	{MessageBoxA(0,err.what(),"��ȡ���ó���",0);}
	catch(Lua::LuaContext::SyntaxErrorException &err)			{MessageBoxA(0,err.what(),"��ȡ���ó���",0);}
	catch(Lua::LuaContext::WrongTypeException &err)				{MessageBoxA(0,err.what(),"��ȡ���ó���",0);}


}



