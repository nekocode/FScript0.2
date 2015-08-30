#include "stdafx.h"
#include "DX9Draw.h"
#include "fImage.h"
#include "fText.h"
#include "resource.h"

// ����ǰ������
void setupViewAndProjMat();
void listRender();

// ȫ�ֱ���
HWND dhwnd;								//�����ھ��
RECT drect;								//�ֱ���
LPDIRECT3D9 g_pD3D = NULL;				//Direct3D����
IDirect3DDevice9* g_pd3dD;				//Direct3D�豸����
item *render_head,*render_now;

extern bool change_text;
extern bool change_img;

//==================================
//  ����:Render()
//  Ŀ��:��Ⱦͼ�β���ʾ
//
void _Render()
{
	if(!g_pd3dD)
		return;

	//��պ�̨����
	g_pd3dD->Clear(0,NULL,D3DCLEAR_TARGET && D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	if(SUCCEEDED(g_pd3dD->BeginScene()))
	{
		//�ں�̨�������ͼ��
		item *pr = render_head;
		while(pr)
		{
			pr->draw();
			pr = pr->next;
		}

		//�������
		if(system_cur)
		{
			system_cur->draw();
		}
		
		//�����ں�̨������Ⱦͼ��
		g_pd3dD->EndScene();
	}
	//���ں�̨������Ƶ�ͼ���ύ��ǰ̨������ʾ
	g_pd3dD->Present(NULL,NULL,NULL,NULL);
}


//==================================
//  ����:InitD3D( HWND )
//  Ŀ��:��ʼ��Direct3D�豸����
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
	//��ʼ������map(���ڼ��btn)
	map = new imgbase* *[drect.right];
	for(int i=0;i<drect.right;i++)
		map[i] = new imgbase*[drect.bottom];

	for(int x=0;x<drect.right;x++)
	{
		for(int y=0;y<drect.bottom;y++)
			map[x][y] = 0;
	}
	full_map.width = drect.right; full_map.height = drect.bottom;

	//����Direct3D����    [���ڻ�ȡӲ����Ϣ]
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D==NULL) return false;

	//����Ƿ�֧��Ӳ�����㴦��
	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;  //Ӳ������
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;  //�������


	//����D3DPRESENT_PARAMETERS�ṹ
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//����Direct3D�豸����     [������Ⱦͼ��]
	if( FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,dhwnd,vp,&d3dpp,&g_pd3dD)) )
		return false;


	setupViewAndProjMat();		//���ù۲��ͶӰ����
	for(int i=0; i<=2; i++)
	{
		//�������Թ���
		g_pd3dD->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pd3dD->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

		//��������Ѱַģʽ
		g_pd3dD->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0x00000000);
		g_pd3dD->SetSamplerState(i, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
		g_pd3dD->SetSamplerState(i, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	}

	g_pd3dD->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	//�����涼����
	g_pd3dD->SetRenderState( D3DRS_LIGHTING, FALSE );			//�رչ���

	//����alpha���
	g_pd3dD->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	if(!InitImg())		//��ʼ��ͼƬģ��
		return false;
	if(!initText(font_lib))		//��ʼ������ģ��
		return false;
	return true;
}


//==================================
//  ����:D3DRelease()
//  Ŀ��:�ͷ�Direct3D��Direct3D�豸����
//
VOID _ReleaseD3D()
{
	//ж��ϵͳ���ָ��
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
//  ����:setupViewAndProjMat()
//  Ŀ��:��ʼ��View��ProjMat����
//
void setupViewAndProjMat()
{
	// ���ù۲����
	D3DXMATRIXA16 matView;
	D3DXMatrixIdentity( &matView );
	g_pd3dD->SetTransform( D3DTS_VIEW, &matView );

	//����ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0.0f, FLOAT(drect.right-drect.left), FLOAT(drect.bottom-drect.top), 0.0f, 0.0f, 1.0f );
	g_pd3dD->SetTransform( D3DTS_PROJECTION, &matProj );
}