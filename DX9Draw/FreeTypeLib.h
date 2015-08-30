#pragma once
// Freetype 头文件
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"


extern bool change_text;


// 存储一个字符图片相关的结构体
struct SCharTexture
{
	LPDIRECT3DTEXTURE9 m_Texture;	// 纹理
	int		m_Width;				// 宽
	int		m_Height;				// 高
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
	// 设置字体大小
	bool SetSize(int _w , int _h);
	// 用FreeType渲染字符串
	void DrawText(wchar_t* _strText, POINT point, unsigned int font_width, unsigned int font_height, unsigned int line_width, bool center,D3DCOLOR color);

private:
	// 获取字符
	SCharTexture*		getTextChar(wchar_t ch);

	IDirect3DDevice9*	m_pDevice;

	SCharTexture		*m_TexID[256*256];
	
	int   m_w;	// 字宽
	int   m_h;	// 字长
};


bool initFreeType(const char* font_lib);
void ReleaseFreeType();