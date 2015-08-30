#include "stdafx.h"
#include "FreeTypeLib.h"

bool change_text = false;

// �������ݸ�ʽ
struct FontVertex
{
	float _x, _y, _z;
	float u,v;
	DWORD color;

	FontVertex(float x, float y, float tx, float ty)
	{
		_x = x; _y = y;	_z = 0.0f;
		u = tx; v = ty;
	}
};
DWORD vertex_fvf = D3DFVF_XYZ |D3DFVF_TEX1;

IDirect3DVertexBuffer9* Triangle = 0;	//���㻺��
D3DXMATRIXA16 pos;						//�������(��������)
D3DXMATRIXA16 wh;						//�������(�����ַ���С)

FT_Library			m_FT2Lib;
FT_Face				m_FT_Face;

bool initFreeType(const char* font_lib)
{
	//========================
	// ��ʼ��FT��
	//========================
	FT_Error error = FT_Init_FreeType( &m_FT2Lib);
	if (error)
	{
		::MessageBox(0,L"��ʼ���ֿ�ʧ��",0,0);
		exit(0);
	}
	//����һ������,ȡĬ�ϵ�Face,һ��ΪRegualer
	error = FT_New_Face( m_FT2Lib, font_lib, 0, &m_FT_Face );
	if (error)
	{
		::MessageBox(0,L"�����ֿ��ļ�ʧ��",0,0);
		exit(0);
	}
	error = FT_Select_Charmap(m_FT_Face, FT_ENCODING_UNICODE);
	if (error)
	{
		::MessageBox(0,L"ѡ��Unicode����ʧ��",0,0);
		exit(0);
	}
	error = FT_Set_Pixel_Sizes(m_FT_Face,30, 30);
	if (error)
	{
		::MessageBox(0,L"����Ĭ�������Сʧ��",0,0);
		exit(0);
	}

	//========================
	//��ʼ�����干�ö��㻺��
	//========================
	FontVertex *v;     //���干�ö���

	if( FAILED(g_pd3dD->CreateVertexBuffer(4*sizeof(FontVertex),0,vertex_fvf,D3DPOOL_MANAGED,&Triangle,NULL)))
		return false;

	if( FAILED(Triangle->Lock(0,0,(void **)&v,0)))
		return false;

	v[0] = FontVertex(0.0f, 0.0f, 0.0f, 0.0f );
	v[1] = FontVertex(0.0f, 1.0f, 0.0f, 1.0f );
	v[2] = FontVertex(1.0f, 0.0f, 1.0f, 0.0f );
	v[3] = FontVertex(1.0f, 1.0f, 1.0f, 1.0f );
	Triangle->Unlock();

	return true;
}

void ReleaseFreeType()
{
	//�ͷŶ��㻺��
	if( Triangle != NULL )
		Triangle->Release();
}

//===============================================================================
CFreeTypeLib::CFreeTypeLib(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	memset(m_TexID,0,sizeof(SCharTexture*)*65535);
	used = 0;
}

CFreeTypeLib::~CFreeTypeLib(void)
{
	for (int i = 0; i < 65535; i++)
	{
		if(m_TexID[i])
			m_TexID[i]->m_Texture->Release();
	}
	delete[] m_TexID;
	FT_Done_FreeType(m_FT2Lib);
}

bool CFreeTypeLib::SetSize(int _w , int _h)
{
	m_w = _w ; m_h = _h;

	FT_Error error = FT_Set_Pixel_Sizes(m_FT_Face,m_w, m_h);
	if (error)
	{
		::MessageBox(0,L"���ô�Сʧ��",0,0);
		return false;
	}
	return true;
}

void CFreeTypeLib::ClearCahe(int min_used)
{
	for (int i = 0; i < 65535; i++)
	{
		if(m_TexID[i])
		{
			if(m_TexID[i]->m_used<min_used)
			{
				m_TexID[i]->m_Texture->Release();
				delete m_TexID[i];
			}
		}
	}
}


//================================================================================
SCharTexture* CFreeTypeLib::getTextChar(wchar_t ch)
{
	if(m_TexID[ch])			//����Ѿ�������ֱ�ӷ���
	{
		if(change_text)
			m_TexID[ch]->m_used++;
		return m_TexID[ch];
	}

	// װ��TTF����glyph
	SetSize(m_w, m_h);
	FT_Error error = FT_Load_Char(m_FT_Face, ch,FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT );
	if(error)
	{
		return NULL;
	}

	FT_GlyphSlot slot = m_FT_Face->glyph;
	FT_Bitmap bitmap = slot->bitmap;

	int width  =  bitmap.width;
	int height =  bitmap.rows;

	SCharTexture *charTex = new SCharTexture;
	m_TexID[ch] = charTex;

	charTex->m_Width = width;
	charTex->m_Height = height;
	charTex->m_delta_x = (slot->metrics.horiBearingX >> 6);
	charTex->m_delta_y = slot->bitmap_top - height;
	charTex->m_used = 0;

	//��λͼ���ݿ����Լ��������������.�����Ϳ��Ի�����Ҫ�Ķ��������ˡ�
	LPDIRECT3DTEXTURE9 d3d9_texture = NULL;
	if (m_pDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d3d9_texture, NULL) == D3D_OK)
	{
		D3DLOCKED_RECT locked_rect;
		d3d9_texture->LockRect(0, &locked_rect, NULL, 0);
		switch (m_FT_Face->glyph->bitmap.pixel_mode)
		{
		case FT_PIXEL_MODE_GRAY:
			{
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						unsigned char _vl =  (x>=bitmap.width || y>=bitmap.rows) ? 0 : bitmap.buffer[x + bitmap.width*y];
						byte* destination_pixel = ((byte*) locked_rect.pBits) + locked_rect.Pitch * y + x * 4;

						destination_pixel[0] = 0x00; // b
						destination_pixel[1] = 0x00; // g
						destination_pixel[2] = 0x00; // r
						destination_pixel[3] = _vl; // a 
					}
				}
			}
			break;
		case FT_PIXEL_MODE_MONO:
			{	
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						unsigned char _vl = 0;
						if(bitmap.buffer[y*bitmap.pitch + x/8] & (0x80 >> (x & 7)))
							_vl = 0xFF;
						else
							_vl = 0x00;

						byte* destination_pixel = ((byte*) locked_rect.pBits) + locked_rect.Pitch * y + x * 4;
						destination_pixel[0] = 0x00;
						destination_pixel[1] = 0x00;
						destination_pixel[2] = 0x00;
						destination_pixel[3] = _vl;

					}
				}
			}
			break;
		}

		d3d9_texture->UnlockRect(0);
	}
	charTex->m_Texture = d3d9_texture;
	return charTex;
}

void CFreeTypeLib::DrawText(wchar_t* _strText, POINT point, unsigned int font_width, unsigned int font_height, unsigned int line_width, bool center, D3DCOLOR color)
{
	SCharTexture* pCharTex;
	int char_w,char_h;							//ÿ���ַ�Ƭ�Ŀ��
	float ch_x,ch_y;							//ÿ���ַ����xy����
	int drawX = point.x;						//��ǰ����x����
	int drawY = point.y;						//��ǰ����y����
	int offsetX;								//����xƫ����
	float fontHegiht = font_height;				//Ŀ������߶�
	float fontWidth  = font_width;				//Ŀ�������ȶ�
	bool checkCenter = true;					//�Ƿ���Ҫ���¼������x����
	int clen = wcslen(_strText);
	int cpos = 0;

	color = (color & 0x00ffffff) + ((255 - (color >> 24)) << 24);	//��FF��ȥcolor��alphaλ

	for(unsigned int i = 0 ; i < clen ; i ++)
	{
		if(_strText[i]=='\n')
		{
			drawY += fontHegiht ; checkCenter = true;
			continue;
		}
		else if((drawX + fontWidth) > (point.x + line_width))
		{
			drawY += fontHegiht ; checkCenter = true;
		}

		if(checkCenter)
		{
			if(center)
			{
				offsetX = 0;
				for(cpos = i ; cpos < clen ; cpos ++)
				{
					pCharTex = getTextChar(_strText[cpos]);
					//����������з��򳬳��������
					if((_strText[cpos] == L'\n') || ((offsetX+pCharTex->m_Width) > line_width))
						break;
					offsetX += fontWidth;
				}
				offsetX = (line_width-offsetX)/2;
				drawX = offsetX;
				checkCenter = false;
			}
			else
			{
				drawX = point.x;
				checkCenter = false;
			}
		}


		//=======================================
		pCharTex = getTextChar(_strText[i]);
		//=======================================

		char_w = pCharTex->m_Width*(fontWidth/m_w);
		char_h = pCharTex->m_Height*(fontHegiht/m_h);
		ch_x = drawX + pCharTex->m_delta_x*(fontWidth/m_w) - 0.5f;
		ch_y = drawY - char_h - pCharTex->m_delta_y*(fontHegiht/m_h) + fontHegiht -0.5f;

		D3DXMatrixScaling(&wh,char_w,char_h,0.0f);
		D3DXMatrixTranslation(&pos, ch_x, ch_y, 0.0f);
		m_pDevice->SetTransform(D3DTS_WORLD,&(wh*pos));
		m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, color);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_ADD);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		m_pDevice->SetStreamSource(0,Triangle,0,sizeof(FontVertex));
		m_pDevice->SetFVF(vertex_fvf);
		m_pDevice->SetTexture(0,pCharTex->m_Texture);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		drawX += fontWidth;
	}

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_SELECTARG1);
}