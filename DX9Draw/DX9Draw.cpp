#include "stdafx.h"
#include "DX9Draw.h"
#include "fImage.h"
#include "fText.h"
#include "resource.h"

// 函数前置声明
void setupViewAndProjMat();
void listRender();

// 全局变量
HWND dhwnd;								//主窗口句柄
RECT drect;								//分辨率
LPDIRECT3D9 g_pD3D = NULL;				//Direct3D对象
IDirect3DDevice9* g_pd3dD;				//Direct3D设备对象
item *render_head,*render_now;

extern bool change_text;
extern bool change_img;

//==================================
//  函数:Render()
//  目的:渲染图形并显示
//
void _Render()
{
	if(!g_pd3dD)
		return;

	//清空后台缓存
	g_pd3dD->Clear(0,NULL,D3DCLEAR_TARGET && D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	if(SUCCEEDED(g_pd3dD->BeginScene()))
	{
		//在后台缓存绘制图形
		item *pr = render_head;
		while(pr)
		{
			pr->draw();
			pr = pr->next;
		}

		//绘制鼠标
		if(system_cur)
		{
			system_cur->draw();
		}
		
		//结束在后台缓存渲染图形
		g_pd3dD->EndScene();
	}
	//将在后台缓存绘制的图形提交到前台缓存显示
	g_pd3dD->Present(NULL,NULL,NULL,NULL);
}


//==================================
//  函数:InitD3D( HWND )
//  目的:初始化Direct3D设备对象
//
bool _InitD3D(HWND hwnd,RECT* Rect,const char* font_lib)
{
	dhwnd = hwnd;
	if(Rect)
	{
		drect.bottom = Rect->bottom-Rect->top;
		drect.right  = Rect->right-Rect->left;
	}
	else
	{
		GetClientRect(dhwnd, &drect);
	}

	imgbase* **&map = full_map.full_map;
	//初始化窗口map(用于检测btn)
	map = new imgbase* *[drect.right];
	for(int i=0;i<drect.right;i++)
		map[i] = new imgbase*[drect.bottom];

	for(int x=0;x<drect.right;x++)
	{
		for(int y=0;y<drect.bottom;y++)
			map[x][y] = 0;
	}
	full_map.width = drect.right; full_map.height = drect.bottom;

	//创建Direct3D对象    [用于获取硬件信息]
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D==NULL) return false;

	//检查是否支持硬件顶点处理
	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;  //硬件处理
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;  //软件处理


	//设置D3DPRESENT_PARAMETERS结构
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//创建Direct3D设备对象     [用于渲染图形]
	if( FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,dhwnd,vp,&d3dpp,&g_pd3dD)) )
		return false;


	setupViewAndProjMat();		//设置观察和投影矩阵
	for(int i=0; i<=2; i++)
	{
		//设置线性过滤
		g_pd3dD->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pd3dD->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

		//设置纹理寻址模式
		g_pd3dD->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0x00000000);
		g_pd3dD->SetSamplerState(i, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
		g_pd3dD->SetSamplerState(i, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	}

	g_pd3dD->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	//正反面都绘制
	g_pd3dD->SetRenderState( D3DRS_LIGHTING, FALSE );			//关闭光照

	//激活alpha混合
	g_pd3dD->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	if(!InitImg())		//初始化图片模块
		return false;
	if(!initText(font_lib))		//初始化字体模块
		return false;
	return true;
}


//==================================
//  函数:D3DRelease()
//  目的:释放Direct3D及Direct3D设备对象
//
VOID _ReleaseD3D()
{
	//卸载系统鼠标指针
	if(system_cur)
		delete system_cur;

	ReleaseText();
	ReleaseImg();

	if( g_pd3dD != NULL )
		g_pd3dD->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}


//==================================
//  函数:setupViewAndProjMat()
//  目的:初始化View和ProjMat矩阵
//
void setupViewAndProjMat()
{
	// 设置观察矩阵
	D3DXMATRIXA16 matView;
	D3DXMatrixIdentity( &matView );
	g_pd3dD->SetTransform( D3DTS_VIEW, &matView );

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0.0f, FLOAT(drect.right-drect.left), FLOAT(drect.bottom-drect.top), 0.0f, 0.0f, 1.0f );
	g_pd3dD->SetTransform( D3DTS_PROJECTION, &matProj );
}