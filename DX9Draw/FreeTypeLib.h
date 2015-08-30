#pragma once
// Freetype ͷ�ļ�
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"


extern bool change_text;


// �洢һ���ַ�ͼƬ��صĽṹ��
struct SCharTexture
{
	LPDIRECT3DTEXTURE9 m_Texture;	// ����
	int		m_Width;				// ��
	int		m_Height;				// ��
	int		m_delta_x;
	int		m_delta_y;

	int		m_used;
};

class CFreeTypeLib
{
public:
	int used;

	CFreeTypeLib(IDirect3DDevice9* pDevice);
	~CFreeTypeLib(void);
	void ClearCahe(int min_used);
	// ���������С
	bool SetSize(int _w , int _h);
	// ��FreeType��Ⱦ�ַ���
	void DrawText(wchar_t* _strText, POINT point, unsigned int font_width, unsigned int font_height, unsigned int line_width, bool center,D3DCOLOR color);

private:
	// ��ȡ�ַ�
	SCharTexture*		getTextChar(wchar_t ch);

	IDirect3DDevice9*	m_pDevice;

	SCharTexture		*m_TexID[256*256];
	
	int   m_w;	// �ֿ�
	int   m_h;	// �ֳ�
};


bool initFreeType(const char* font_lib);
void ReleaseFreeType();