#include "stdafx.h"
#include "fText.h"
#include "FreeTypeLib.h";

#define textLevel 10		//10¼¶Ëõ·Å
CFreeTypeLib* g_pFreeTypeLib[textLevel];

bool initText(const char *font_lib)
{
	if(!initFreeType(font_lib))
		return false;
	return true;
}

void ReleaseText()
{
	ReleaseFreeType();
}

void ftext::release()
{
	if(_strText)
		delete[] _strText;
}

void ftext::draw()
{
	int max = ((*font_width>*font_height ? *font_width:*font_height)-1)/10+1;
	if(max > textLevel)
		max = textLevel;

	if(!g_pFreeTypeLib[max])
	{
		g_pFreeTypeLib[max] = new CFreeTypeLib(g_pd3dD);
		g_pFreeTypeLib[max]->SetSize(max*10,max*10);
	}

	g_pFreeTypeLib[max]->DrawTextW(_strText, *point, *font_width, *font_height, *line_width, *center, *color);
}