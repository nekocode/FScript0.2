#include "stdafx.h"
#include "fImage.h"
#include "resource.h"
#include <stdio.h>

LPDIRECT3DVERTEXBUFFER9 lpVB;                  //顶点缓冲
IDirect3DPixelShader9* MultiTexPS = 0;
ID3DXConstantTable* MultiTexCT    = 0;
// 着色器句柄
D3DXHANDLE sample0Handle = 0;
D3DXHANDLE sample1Handle = 0;
D3DXHANDLE sample2Handle = 0;
D3DXHANDLE alphaHandle = 0;
D3DXHANDLE alpha_oldHandle = 0;
D3DXHANDLE alpha_newHandle = 0;
D3DXHANDLE typeHandle = 0;
D3DXCONSTANT_DESC sample0Desc;
D3DXCONSTANT_DESC sample1Desc;
D3DXCONSTANT_DESC sample2Desc;

texlist *texlist_headp[256*256],*texlist_nowp[256*256];
framelist *framelist_headp[256*256],*framelist_nowp[256*256];


//系统固有对象

fimg *system_cur = 0;



//初始化像素着色器
bool InitEffect()
{
	HRESULT rlt;
	ID3DXBuffer* shader      = NULL;
	ID3DXBuffer* errorBuffer = NULL;

/*	
	rlt = D3DXCompileShaderFromFileW(
		L"effect.txt",0,0,
		"effect","ps_2_0",0,&shader,&errorBuffer,&MultiTexCT);//*/


	rlt = D3DXCompileShaderFromResource(hmod,MAKEINTRESOURCEW(IDR_RCDATA1),0,0,"effect","ps_2_0",0,&shader,&errorBuffer,&MultiTexCT);
	

	if( errorBuffer )
	{
		MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
		return false;
	}
	if(FAILED(rlt))
	{
		MessageBoxA(0,"执行D3DXCompileShader失败!", "错误", 0);
		return false;
	}


	if(FAILED(g_pd3dD->CreatePixelShader((DWORD*)shader->GetBufferPointer(),&MultiTexPS)))
	{
		MessageBoxA(0,"执行CreatePixelShader失败!", "错误", 0);
		return false;
	}
	shader->Release();


	MultiTexCT->SetDefaults(g_pd3dD);

	alphaHandle		= MultiTexCT->GetConstantByName(0, "alpha");
	alpha_oldHandle	= MultiTexCT->GetConstantByName(0, "alpha_old");
	alpha_newHandle	= MultiTexCT->GetConstantByName(0, "alpha_new");
	typeHandle		= MultiTexCT->GetConstantByName(0, "type");
	sample0Handle	= MultiTexCT->GetConstantByName(0, "Samp0");
	sample1Handle	= MultiTexCT->GetConstantByName(0, "Samp1");
	sample2Handle	= MultiTexCT->GetConstantByName(0, "Samp2");

	UINT count;
	MultiTexCT->GetConstantDesc(sample0Handle, &sample0Desc, &count);
	MultiTexCT->GetConstantDesc(sample1Handle, &sample1Desc, &count);
	MultiTexCT->GetConstantDesc(sample2Handle, &sample2Desc, &count);

	return true;
}

void ReleaseEffect()
{
	if( MultiTexPS != NULL )
		MultiTexPS->Release();
	if( MultiTexCT != NULL )
		MultiTexCT->Release();
}

bool InitImg()
{
	// 创建顶点缓存
	if( FAILED(g_pd3dD->CreateVertexBuffer(sizeof(Vertex)*4, 0, VertexFVF, D3DPOOL_MANAGED, &lpVB, NULL)) )
		return false;

	Vertex* pVertices;
	if( FAILED(lpVB->Lock( 0, 0, (void**)&pVertices, 0 )) )
		return false;

	pVertices[0] = Vertex(0.0f, 0.0f		,		0.0f, 0.0f);
	pVertices[1] = Vertex(0.0f, 1.0f		,		0.0f, 1.0f);
	pVertices[2] = Vertex(1.0f, 0.0f		,		1.0f, 0.0f);
	pVertices[3] = Vertex(1.0f, 1.0f		,		1.0f, 1.0f);
	lpVB->Unlock();

	//初始化texlist
	for(int i=0;i<65535;i++)
	{
		texlist_headp[i] = 0;
		texlist_nowp[i] = 0;
	}

	// 初始化着色器
	if(!InitEffect())
		return false;

	return true;
}

void ReleaseImg()
{
	// 释放贴图列表
	for(int i=0;i<65535;i++)
	{
		texlist *sch = texlist_headp[i];
		while(sch)
		{
			sch->tex->tex->Release();
			texlist *tmp = sch;
			sch = sch->next;
			delete tmp;
		}
	}

	// 释放顶点缓存
	if( lpVB != NULL )
		lpVB->Release();

	// 释放化着色器资源
	ReleaseEffect();
}

// BKDR Hash Function
inline unsigned int BKDRHash(wchar_t *name)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;
 
	while (*name)
	{
		hash = hash * seed + (*name++);
	}
 
	return (hash%65536);
}

/*===========================================================
					  img的读取相关函数
===========================================================*/
//从内存中读取纹理到纹理列表
bool LoadImgFromMem(char *mem,unsigned long size,bool mask,wchar_t *name)
{
	texlist *temp = new texlist;
	temp->tex = new mytex;
	int namelen = wcslen(name);
	if(namelen<1)
		return false;
	temp->name = new wchar_t[namelen];
	wcscpy(temp->name,name);

	temp->next = 0;

	//获取图片信息
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFileInMemory(mem, size, &imageInfo);

	temp->tex->height = imageInfo.Height;
	if(mask)
		temp->tex->width = imageInfo.Width/2;
	else
		temp->tex->width = imageInfo.Width;


	if(!mask)
	{
		//从内存中创建贴图
		if(FAILED(D3DXCreateTextureFromFileInMemoryEx(g_pd3dD,mem,size,imageInfo.Width,imageInfo.Height,imageInfo.MipLevels,0,imageInfo.Format,
			D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0,NULL,NULL,&temp->tex->tex)))
		{
			MessageBox(0,name,TEXT("读取图片错误"),MB_OK);
			return 0;
		}
		/*
		UINT SizeX=0,SizeY=0;
		if(FAILED(D3DXCreateTextureFromFileInMemoryEx(g_pd3dD,mem,size,SizeX,SizeY,D3DX_FROM_FILE,0,D3DFMT_FROM_FILE,
			D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0,&imageInfo,NULL,&temp->tex->tex)))
		{
			MessageBox(0,name,TEXT("读取图片错误"),MB_OK);
			return 0;
		}
		if(SizeX==(UINT)-2)
			imageInfo.Width = SizeX;
		if(SizeY==(UINT)-2)
			imageInfo.Height = SizeY;


			temp->tex->height = imageInfo.Height;
			if(mask)
				temp->tex->width = imageInfo.Width/2;
			else
				temp->tex->width = imageInfo.Width;*/
	}
	else
	{
		unsigned int width = temp->tex->width;
		unsigned int height = temp->tex->height;
		long bytenum = width*height*4;
		byte* masked_pixel_buf = new byte[bytenum];

		if(FAILED(D3DXCreateTextureFromFileInMemoryEx(g_pd3dD,mem,size,imageInfo.Width,imageInfo.Height,imageInfo.MipLevels,0,imageInfo.Format,
			D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0,NULL,NULL,&temp->tex->tex)))
		{
			MessageBox(0,name,TEXT("读取图片错误"),MB_OK);
			return 0;
		}
		else
		{
			//对tex进行处理
			D3DLOCKED_RECT locked_rect;
			temp->tex->tex->LockRect(0, &locked_rect, NULL, 0);

			BYTE *pByte = (BYTE *) locked_rect.pBits;
			BYTE *pmaskByte = (BYTE *) locked_rect.pBits + width*4;

			unsigned int iOffset = locked_rect.Pitch/2;    //每行的字节数

			unsigned int i=0;
			for(unsigned int iRow=0; iRow<height; iRow++ )
			{
				for(unsigned int iCol=0; iCol<width; iCol++ )
				{
					masked_pixel_buf[i] = pByte[0];
					masked_pixel_buf[i+1] = pByte[1];
					masked_pixel_buf[i+2] = pByte[2];
					masked_pixel_buf[i+3] = 0xff - pmaskByte[0];

					i += 4;
					pByte += 4;
					pmaskByte += 4;
				}
				pByte += iOffset;
				pmaskByte += iOffset;
			}

			temp->tex->tex->UnlockRect(0);
			temp->tex->tex->Release();

			if (g_pd3dD->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &temp->tex->tex, NULL) == D3D_OK)
			{
				//对tex进行处理
				D3DLOCKED_RECT locked_rect;
				temp->tex->tex->LockRect(0, &locked_rect, NULL, 0);

				//直接复制内存块
				CopyMemory(locked_rect.pBits,masked_pixel_buf,bytenum);

				temp->tex->tex->UnlockRect(0);
			}
			else
				return 0;
		}
	}

	unsigned int i = BKDRHash(name);

	if(!texlist_headp[i])
	{
		texlist_headp[i] = temp;
		texlist_nowp[i] = texlist_headp[i];
	}
	else
	{
		texlist_nowp[i]->next = temp;
		texlist_nowp[i] = temp;
	}
	return true;
}

//从文件中读取纹理到纹理列表
bool LoadImgFromFile(wchar_t *path,bool mask,wchar_t *name)
{
	FILE *f;
	if(f=_wfopen(path,L"rb"))
	{
		//获得大小
		fseek(f,0,2);
		unsigned long size = ftell(f);
		fseek(f,0,0);

		char *fb = new char[size];
		fread(fb,size,1,f);

		return LoadImgFromMem(fb,size,mask,name);
	}
	wchar_t err_out[255]; 
	wsprintf(err_out,L"无法读取文件: %s\n请确定文件存在并可读",path);
	MessageBox(0,err_out,L"错误",0);
	return false;
}

mytex* gettex(wchar_t *name)
{
	unsigned int i = BKDRHash(name);

	texlist *sch=texlist_headp[i];
	while(sch)
	{
		if(!wcscmp(sch->name,name))
		{
			return sch->tex;
		}
		sch = sch->next;
	}
	return 0;
}


/*===========================================================
					 aimg的读取相关函数
===========================================================*/
bool LoadaImgFromImgs(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name)
{
	if(!imgsname)
		return false;

	frames *tempf = new frames;
	tempf->frame_number = img_number;
	frame *f = new frame[img_number];
	for(unsigned int i=0;i<img_number;i++)
	{
		if(delay)
			f[i].delay = delay[i];

		f[i].tex = gettex(imgsname[i]);
	}
	tempf->f = f;

	framelist *temp = new framelist;
	int namelen = wcslen(name);
	temp->name = new wchar_t[namelen];
	wcscpy(temp->name,name);

	temp->fs = tempf;
	temp->next = 0;

	unsigned int i = BKDRHash(name);
	
	if(!framelist_headp[i])
	{
		framelist_headp[i] = temp;
		framelist_nowp[i] = framelist_headp[i];
	}
	else
	{
		framelist_nowp[i]->next = temp;
		framelist_nowp[i] = temp;
	}
	
	return true;
}

bool LoadaImgFromMem(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name)
{
	LPDIRECT3DTEXTURE9 tex_all;
	//获取图片信息
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFileInMemory(mem, size, &imageInfo);

	unsigned int width = imageInfo.Width;
	unsigned int height = imageInfo.Height;
	unsigned int h_width = width/hn;
	unsigned int v_height = height/vn;

	long bytenum = h_width*v_height*4;
	unsigned int all = hn*vn;
	byte **pixel_buf = new byte*[all];
	for(unsigned int i=0;i<all;i++)
	{
		pixel_buf[i] = new byte[bytenum];
	}

	if(FAILED(D3DXCreateTextureFromFileInMemoryEx(g_pd3dD,mem,size,imageInfo.Width,imageInfo.Height,imageInfo.MipLevels,0,D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,D3DX_FILTER_NONE,D3DX_DEFAULT,0,NULL,NULL,&tex_all)))
	{
		MessageBox(0,name,TEXT("读取图片错误"),MB_OK);
		return 0;
	}
	else
	{
		//对tex进行处理
		D3DLOCKED_RECT locked_rect;
		tex_all->LockRect(0, &locked_rect, NULL, 0);


		BYTE *pByte = (BYTE *) locked_rect.pBits;
		int p1 = 0,p2 = 0,p2_base = 0;

		for(unsigned int n=0;n<all;n++)
		{
			p2_base = (n/hn)*locked_rect.Pitch*v_height+(n%hn)*h_width*4;
			for(unsigned int h=0;h<v_height;h++)
			{
				for(unsigned int w=0;w<h_width;w++)
				{
					pixel_buf[n][p1] = pByte[p2_base+p2];
					pixel_buf[n][p1+1] = pByte[p2_base+p2+1];
					pixel_buf[n][p1+2] = pByte[p2_base+p2+2];
					pixel_buf[n][p1+3] = pByte[p2_base+p2+3];
					p1+=4;
					p2+=4;
				}
				p2 = p2 + locked_rect.Pitch - h_width*4;
			}
			p1 = 0; p2 = 0;
		}

		tex_all->UnlockRect(0);
		tex_all->Release();

		///////////////////////////////////////////////////////
		frames *tempf = new frames;
		tempf->frame_number = all;
		frame *f = new frame[all];
		for(unsigned int i=0;i<all;i++)
		{
			if(delay)
				f[i].delay = delay[i];
			else
				f[i].delay = 0;

			f[i].tex = new mytex;
			if (g_pd3dD->CreateTexture(h_width, v_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &f[i].tex->tex, NULL) == D3D_OK)
			{
				//对tex进行处理
				D3DLOCKED_RECT locked_rect;
				f[i].tex->tex->LockRect(0, &locked_rect, NULL, 0);

				//直接复制内存块
				CopyMemory(locked_rect.pBits,pixel_buf[i],bytenum);

				f[i].tex->tex->UnlockRect(0);
				f[i].tex->width = h_width;
				f[i].tex->height = v_height;
			}
			else
				return 0;
		}
		tempf->f = f;

		framelist *temp = new framelist;
		int namelen = wcslen(name);
		temp->name = new wchar_t[namelen];
		wcscpy(temp->name,name);

		temp->fs = tempf;
		temp->next = 0;

		unsigned int i = BKDRHash(name);

		if(!framelist_headp[i])
		{
			framelist_headp[i] = temp;
			framelist_nowp[i] = framelist_headp[i];
		}
		else
		{
			framelist_nowp[i]->next = temp;
			framelist_nowp[i] = temp;
		}

		return true;
	}
}

bool LoadaImgFromFile(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name)
{
	FILE *f;
	if(f=_wfopen(path,L"rb"))
	{
		//获得大小
		fseek(f,0,2);
		unsigned long size = ftell(f);
		fseek(f,0,0);

		char *fb = new char[size];
		fread(fb,size,1,f);

		return LoadaImgFromMem(fb,size,hn,vn,delay,name);
	}
	wchar_t err_out[255]; 
	wsprintf(err_out,L"无法读取文件: %s\n请确定文件存在并可读",path);
	MessageBox(0,err_out,L"错误",0);
	return false;
}

frames* getframes(wchar_t *name)
{
	unsigned int i = BKDRHash(name);

	framelist *sch=framelist_headp[i];
	while(sch)
	{
		if(!wcscmp(sch->name,name))
		{
			return sch->fs;
		}
		sch = sch->next;
	}
	return 0;
}



/*===========================================================
					  Render To Texture
===========================================================*/
bool RendrToTexture(mytex* &tex,D3DPOOL pool,int &x_delta,int &y_delta,int nowx,int nowy,imgbase *img)
{
	bool rlt = true;
	LPDIRECT3DTEXTURE9 pRenderTexture = NULL; // 目标纹理
	LPDIRECT3DSURFACE9 pRenderSurface = NULL,pBackBuffer = NULL;
	// pRenderSurface是pRenderTexture 对应的Surface		// pBackBuffer用于保存原来的Render Target

	//注意这里的第三个参数必须为D3DUSAGE_RENDERTARGET      //第四个参数决定纹理的格式，不同的场景会要求不同的格式
	g_pd3dD->CreateTexture(drect.right,drect.bottom,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&pRenderTexture,NULL);

	//获得pRenderTexture对应的Surface
	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);

	//这里保存下原来的Render target，在做完RTT后再恢复
	g_pd3dD->GetRenderTarget(0,&pBackBuffer);
	if( SUCCEEDED( g_pd3dD->BeginScene() ) )
	{
		//设置我们的纹理为render target
		g_pd3dD->SetRenderTarget(0, pRenderSurface);
		g_pd3dD->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.00f,0.00f,1.00f), 1.0f, 0);

		//绘制tex
		img->rtt_draw_func();

		//重新将render target设置为帧缓存
		g_pd3dD->SetRenderTarget(0, pBackBuffer);
		g_pd3dD->EndScene();
		pBackBuffer->Release();
	}
	else
	{
		SAFE_RELEASE(pRenderSurface);
		SAFE_RELEASE(pRenderTexture);
		return false;
	}


	//获取rendertarget内的值
	LPDIRECT3DTEXTURE9 RenderTarget_Tex;
	LPDIRECT3DSURFACE9 RenderTarget_Surface;
	g_pd3dD->CreateTexture(drect.right,drect.bottom,1,0,D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &RenderTarget_Tex, NULL);
	RenderTarget_Tex->GetSurfaceLevel(0,&RenderTarget_Surface);
	if (g_pd3dD->GetRenderTargetData(pRenderSurface, RenderTarget_Surface) == D3D_OK)
	{
		D3DLOCKED_RECT lockbits;
		if (RenderTarget_Tex->LockRect(0,&lockbits, NULL, 0) == D3D_OK)
		{
			BYTE* bits=(BYTE*)(lockbits.pBits);
			int line_Offset = lockbits.Pitch;	//每行字节数
			BYTE* bits_head = bits;

			int left = 0x7fffffff , right = -1, top = 0x7fffffff, bottom = -1;
			for( int y=0; y<drect.bottom; y++ )
			{
				for( int x=0; x<drect.right; x++ )
				{
					if(bits[3]!=0)	//如果alpha不为0
					{
						if(x<left) left=x;
						if(x>right) right=x;
						if(y<top) top=y;
						if(y>bottom) bottom=y;
					}
					bits += 4;
				}
			}
			bits = bits_head;

			int dwidth = right-left ,dheight = bottom-top;
			int bytenum = dwidth*dheight*4;
			BYTE* dest_bits = new BYTE[bytenum];
			BYTE* dest_head = dest_bits;
			//定义初始pos 
			bits += top*line_Offset + left*4;
			//复制目标块
			for( int y=0; y<dheight; y++ )
			{
				for( int x=0; x<dwidth; x++ )
				{
					dest_bits[0] = bits[0];
					dest_bits[1] = bits[1];
					dest_bits[2] = bits[2];
					dest_bits[3] = bits[3];

					bits += 4;
					dest_bits +=4;
				}
				bits += line_Offset - dwidth*4;
			}

			RenderTarget_Tex->UnlockRect(0);

			D3DLOCKED_RECT lockmapbits;
			if (g_pd3dD->CreateTexture(dwidth, dheight, 1, 0, D3DFMT_A8R8G8B8, pool, &(tex->tex), NULL) == D3D_OK)
			{
				tex->tex->LockRect(0, &lockmapbits, NULL, 0);
				//直接复制内存块
				CopyMemory(lockmapbits.pBits,dest_head,bytenum);
				tex->tex->UnlockRect(0);
			}
			else {rlt = false;}

			//rendertarget的宽高以及相对于draw之前的偏移量
			tex->width = dwidth;
			tex->height = dheight;
			x_delta = left - nowx;
			y_delta = top - nowy;
		}
		else {rlt = false;}
	}
	else {rlt = false;}

	/*/保存纹理到图片
	if(FAILED(D3DXSaveTextureToFileW(L"rt.png",D3DXIFF_PNG,tex->tex,0)))
		RenderTarget_Tex = 0;//*/

	SAFE_RELEASE(RenderTarget_Tex);
	SAFE_RELEASE(RenderTarget_Surface);
	SAFE_RELEASE(pRenderSurface);
	SAFE_RELEASE(pRenderTexture);
	
	return rlt;
}

/*===========================================================
					 按钮绑定相关函数
===========================================================*/
void getbtnmap(mytex *btnmap_tex,btnmap* &btnmap_object,int min_alpha)
{
	byte ** &map = btnmap_object->btn_map;
	unsigned int width = btnmap_tex->width, height = btnmap_tex->height;
	btnmap_object->width = width; btnmap_object->height = height;

	//创建btn_map的map数组
	map = new byte *[width];
	for(unsigned int i=0;i<width;i++)
		map[i] = new byte[height];
	for(unsigned int x=0;x<width;x++)
	{
		for(unsigned int y=0;y<height;y++)
			map[x][y] = 0;
	}

	//对tex进行处理
	D3DLOCKED_RECT locked_rect;
	btnmap_tex->tex->LockRect(0, &locked_rect, NULL, 0);

	BYTE *pByte = (BYTE *) locked_rect.pBits;

	for(unsigned int iRow=0; iRow<height; iRow++ )
	{
		for(unsigned int iCol=0; iCol<width; iCol++ )
		{
			if(pByte[3]>=min_alpha)
				map[iCol][iRow] = 1;

			pByte += 4;
		}
	}

	btnmap_tex->tex->UnlockRect(0);
}

//render的map
fullmap full_map;


/*==================================================================================================
										imgbase类
==================================================================================================*/
imgbase::imgbase()
{
	nowx = nowy = 0;
	rota = rota_x = rota_y = 0;
	alpha = 0;

	btn_map = 0; btn_enable = false;
}

imgbase::~imgbase()
{
	if(btn_map)
	{
		if(btn_map->min_alpha)
		{
			delete [] *(btn_map->btn_map);
			delete [] btn_map->btn_map;
		}
		delete btn_map;
	}
}

//设置图片坐标
void imgbase::setpos(float x,float y)
{
	*nowx = x; *nowy = y;
	D3DXMatrixTranslation(&trans, float((int)(*nowx))-0.5f, float((int)(*nowy))-0.5f, 0);
}

//设置图片长宽
void imgbase::setsize(float _width,float _height)
{
	if(_width&&_height)
	{
		*width = _width;
		*height = _height;
	}
	D3DXMatrixScaling(&scale,*width,*height,0);
}

//顺时针旋转图片
void imgbase::setrota(float rotation,float x,float y)
{
	D3DXMATRIXA16 tmp,tmp2,tmp3;
	D3DXMatrixTranslation(&tmp, -x, -y, 0);
	D3DXMatrixRotationZ(&tmp2, rotation *D3DX_PI/180);
	D3DXMatrixTranslation(&tmp3, +x, +y, 0);
	rot = tmp*tmp2*tmp3;
	*rota = rotation;
	*rota_x = x; *rota_y = y;
}

//绑定img为btn
void imgbase::btn_bind(void *outitem,int min_alpha,void *data)
{
	btn_map = new btnmap;

	//创建不规则按钮
	if(min_alpha!=0)
	{
		mytex *btn_map_tex = new mytex;
		btn_map_tex->tex = 0;
		if(rendr_to_texture(btn_map_tex,btn_map->x_delta,btn_map->y_delta))
		{
			btn_map->width = btn_map_tex->width; btn_map->height = btn_map_tex->height;
			getbtnmap(btn_map_tex,btn_map,min_alpha);
		}
		//释放btnmap纹理
		if(btn_map_tex)
			SAFE_RELEASE(btn_map_tex->tex);
		delete btn_map_tex;
		btn_map_tex = 0;
	}

	btn_map->min_alpha = min_alpha;
	btn_map->outitem = outitem;		//主程序中储存img数据的结构指针
	btn_map->data = data;			//绑定的数据,回调时返回

	btn_enable = true;
}

//当img发生变化(放缩,角度变化)时重新刷新btnmap
void imgbase::refresh_map()
{
	if(btn_map->min_alpha==0)
		return;

	mytex *btn_map_tex = new mytex;
	if(rendr_to_texture(btn_map_tex,btn_map->x_delta,btn_map->y_delta))
	{
		//释放原有的map
		delete[] *(btn_map->btn_map);
		delete[] btn_map->btn_map;

		btn_map->width = btn_map_tex->width; btn_map->height = btn_map_tex->height;
		getbtnmap(btn_map_tex,btn_map,btn_map->min_alpha);
	}
	//释放btnmap纹理
	SAFE_RELEASE(btn_map_tex->tex);
	delete btn_map_tex;
	btn_map_tex = 0;
}

void imgbase::rtt_draw_func()
{
	if(lpt)
	{
		setpos(*nowx, *nowy);
		setsize(*width, *height);
		setrota(*rota, *rota_x, *rota_y);
		g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));
		g_pd3dD->SetTexture( 0, lpt->tex );
		g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
		g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pd3dD->SetStreamSource( 0, lpVB, 0, sizeof( Vertex ) );
		g_pd3dD->SetFVF( VertexFVF );
		g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );
	}
}

bool imgbase::rendr_to_texture(mytex* &tex,int &x_delta,int &y_delta)
{
	return RendrToTexture(tex,D3DPOOL_SYSTEMMEM,x_delta,y_delta,*nowx,*nowy,this);
}


/*==================================================================================================
										  fimg类
==================================================================================================*/
fimg::fimg()
{
	item_type = 1;
	nt = 0;
}

void fimg::release()
{
	if(nt)
		delete nt;
}

void fimg::setfademap(wchar_t *name)
{
	if(!nt)
	{
		nt = new NextTex;
		g_pd3dD->CreateVertexBuffer(sizeof(VertexThree)*4, 0, VertexThreeFVF, D3DPOOL_MANAGED, &nt->lpVB_change, NULL);
	}

	nt->lpt_map = gettex(name);
}

void fimg::fade(wchar_t *name,float fadestep,float nowx2,float nowy2,unsigned int width2,unsigned int height2,float alpha2)
{
	if(!nt)
	{
		nt = new NextTex;
		nt->lpt_map = 0;
		g_pd3dD->CreateVertexBuffer(sizeof(VertexThree)*4, 0, VertexThreeFVF, D3DPOOL_MANAGED, &nt->lpVB_change, NULL);
	}

	if(!(nt->lpt_next=gettex(name)))
		return;
	nt->fade_alpha = 0.0f;
	nt->fadestep = fadestep;
	nt->next_x = nowx2;
	nt->next_y = nowy2;
	nt->next_alpha = alpha2;
	if((width2)||(height2))
	{
		nt->next_width = width2;
		nt->next_height = height2;
	}
	else
	{
		nt->next_width = nt->lpt_next->width;
		nt->next_height = nt->lpt_next->height;
		width2 = nt->next_width;
		height2 = nt->next_height;
	}

	// 储存新图的长宽
	float w,h;
	float x0,y0;
	float x1[4],x2[4],y1[4],y2[4];	// 储存两张图各点坐标

	if((!width2)||(!height2))
	{width2=nt->next_width; height2=nt->next_height;}

	x1[0]=*nowx;y1[0]=*nowy;
	x1[1]=*nowx;y1[1]=*nowy+*height;
	x1[2]=*nowx+*width;y1[2]=*nowy;
	//x1[3]=nowx+height;y1[3]=nowy+width;

	x2[0]=nowx2;y2[0]=nowy2;
	x2[1]=nowx2;y2[1]=nowy2+height2;
	x2[2]=nowx2+width2;y2[2]=nowy2;
	//x2[3]=nowx2+height2;y2[3]=nowy2+width2;

	x0= (x1[0]<x2[0]?x1[0]:x2[0]);
	y0= (y1[0]<y2[0]?y1[0]:y2[0]);
	w = (x1[2]>x2[2]?x1[2]:x2[2])-x0;
	h = (y1[1]>y2[1]?y1[1]:y2[1])-y0;

	x1[0] = (x0  -*nowx)/ *width; y1[0] = (y0  -*nowy)/ *height;
	x1[1] = (x0  -*nowx)/ *width; y1[1] = (y0+h-*nowy)/ *height;
	x1[2] = (x0+w-*nowx)/ *width; y1[2] = (y0  -*nowy)/ *height;
	x1[3] = (x0+w-*nowx)/ *width; y1[3] = (y0+h-*nowy)/ *height;

	x2[0] = (x0  -nowx2)/ width2; y2[0] = (y0  -nowy2)/ height2;
	x2[1] = (x0  -nowx2)/ width2; y2[1] = (y0+h-nowy2)/ height2;
	x2[2] = (x0+w-nowx2)/ width2; y2[2] = (y0  -nowy2)/ height2;
	x2[3] = (x0+w-nowx2)/ width2; y2[3] = (y0+h-nowy2)/ height2;

	//修改顶点缓存,重写转换纹理坐标,过渡图采用原图的大小
	VertexThree* pVtemp;
	nt->lpVB_change->Lock( 0, 0, (void**)&pVtemp, 0 );
	pVtemp[0] = VertexThree(0.0f, 0.0f		,       x1[0], y1[0],x2[0], y2[0],x1[0], y1[0]);
	pVtemp[1] = VertexThree(0.0f, 1.0f		,       x1[1], y1[1],x2[1], y2[1],x1[1], y1[1]);
	pVtemp[2] = VertexThree(1.0f, 0.0f		,       x1[2], y1[2],x2[2], y2[2],x1[2], y1[2]);
	pVtemp[3] = VertexThree(1.0f, 1.0f		,       x1[3], y1[3],x2[3], y2[3],x1[3], y1[3]);
	nt->lpVB_change->Unlock();

	setsize(w,h);
	setpos(x0,y0);
}

void fimg::draw()
{
	if(!lpt)
		return;

	if(!(*alpha))
		return;
	/***********************
	绘制普通图片
	***********************/
	if((!nt)||(!nt->lpt_next))
	{
		//刷新数值
		setpos(*nowx, *nowy);
		setsize(*width, *height);
		setrota(*rota, *rota_x, *rota_y);

		g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));								//设置世界矩阵(rot)
		g_pd3dD->SetTexture( 0, lpt->tex );

		if(*alpha!=255)
		{
			g_pd3dD->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0,0,0,int(255.0-*alpha)) );
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);				//相减
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		}
		else
		{
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		}

		g_pd3dD->SetStreamSource( 0, lpVB, 0, sizeof( Vertex ) );
		g_pd3dD->SetFVF( VertexFVF );
		g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );

	}
	/***********************
	绘制转换图片
	***********************/
	else
	{
		g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));								//设置世界矩阵(rot)

		MultiTexCT->SetFloat( g_pd3dD, alphaHandle, nt->fade_alpha/255.0f );			//设置fade中的alpha值
		MultiTexCT->SetFloat( g_pd3dD, alpha_oldHandle, (*alpha/255.0f) );
		MultiTexCT->SetFloat( g_pd3dD, alpha_newHandle, (nt->next_alpha/255.0f) );
		if(nt->lpt_map)
		{
			MultiTexCT->SetFloat( g_pd3dD, typeHandle, 1.0f );
			g_pd3dD->SetTexture( sample2Desc.RegisterIndex, nt->lpt_map->tex );
		}
		else
		{
			MultiTexCT->SetFloat( g_pd3dD, typeHandle, 0.0f );
		}
		g_pd3dD->SetPixelShader( MultiTexPS );

		g_pd3dD->SetTexture( sample0Desc.RegisterIndex, lpt->tex);
		g_pd3dD->SetTexture( sample1Desc.RegisterIndex, nt->lpt_next->tex );
		g_pd3dD->SetTextureStageState(sample0Desc.RegisterIndex,D3DTSS_TEXCOORDINDEX,0);
		g_pd3dD->SetTextureStageState(sample1Desc.RegisterIndex,D3DTSS_TEXCOORDINDEX,1);
		g_pd3dD->SetTextureStageState(sample2Desc.RegisterIndex,D3DTSS_TEXCOORDINDEX,2);

		g_pd3dD->SetStreamSource( 0, nt->lpVB_change, 0, sizeof(VertexThree) );
		g_pd3dD->SetFVF( VertexThreeFVF );
		g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );
		g_pd3dD->SetPixelShader( NULL );

		nt->fade_alpha += nt->fadestep;
		if(nt->fade_alpha>=255.0f)
		{
			*nowx = nt->next_x;
			*nowy = nt->next_y;
			*width = nt->next_width;
			*height = nt->next_height;
			*alpha = nt->next_alpha;
			lpt = nt->lpt_next;
			nt->lpt_next = 0;

			setsize(*width,*height);
			setpos(*nowx,*nowy);
		}
	}
}

bool fimg::load(wchar_t *name)
{
	lpt = gettex(name);

	if(lpt)
	{
		D3DXMatrixIdentity(&scale);
		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&rot);

		setsize(lpt->width,lpt->height);
	}

	return bool(lpt);
}


/*==================================================================================================
										  faimg类
==================================================================================================*/
faimg::faimg()
{
	item_type = 2;
	autoplay = 0;
	loop = 0;
}

void faimg::release()
{
}

void faimg::draw()
{
	if(!f)
		return;

	if(!(*alpha))
		return;

	if(*frame_now<=*frame_end)
	{
		if(*autoplay)
		{
			if(*delay)
			{
				if(*delay_count<*delay)
				{
					(*delay_count)++;
				}
				else
				{
					if(*frame_now<*frame_end)
						(*frame_now)++;
					else if(*loop)
						*frame_now = *frame_start;
					else
						(*frame_now)++;		//loop为false,停止播放

					*delay_count = 0;
				}
				lpt = f[*frame_now].tex;
			}
			else							//为0则使用每一帧不同的延迟参数
			{
				if(*delay_count<delay_perframe)
				{
					(*delay_count)++;
				}
				else
				{
					if(*frame_now<*frame_end)
						(*frame_now)++;
					else if(*loop)
						*frame_now = *frame_start;
					else
						(*frame_now)++;		//loop为false,停止播放

					delay_perframe = f[*frame_now].delay;
					*delay_count = 0;
				}
				lpt = f[*frame_now].tex;
			}
		}

		//刷新数值
		setpos(*nowx, *nowy);
		setsize(*width, *height);
		setrota(*rota, *rota_x, *rota_y);

		g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));								//设置世界矩阵(rot)
		g_pd3dD->SetTexture( 0, lpt->tex );

		if((*alpha)!=255)
		{
			if(!(*alpha))
				return;
			g_pd3dD->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0,0,0,int(255.0-*alpha)) );
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);				//相减
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		}
		else
		{
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
			g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		}

		g_pd3dD->SetStreamSource( 0, lpVB, 0, sizeof( Vertex ) );
		g_pd3dD->SetFVF( VertexFVF );
		g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );

	}
}

bool faimg::load(wchar_t *name)
{
	frames *fs = getframes(name);
	if(fs)
	{
		f = fs->f;
		*frame_all = fs->frame_number;
		*frame_start = 0;
		*frame_end = *frame_all-1;
		*frame_now = 0;
		*delay_count = 0;
		delay_perframe = f[0].delay;

		lpt = f[0].tex;

		D3DXMatrixIdentity(&scale);
		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&rot);
		setsize(f[0].tex->width,f[0].tex->height);
	}
	else
	{
		f = 0;
	}
	return bool(fs);
}