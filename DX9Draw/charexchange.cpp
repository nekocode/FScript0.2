#include "stdafx.h"

char* wctoc(wchar_t* pwszUnicode)
{
	int iSize = WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, NULL, 0, NULL, NULL);
	char *pszMultiByte = (char*)malloc((iSize+1)/**sizeof(char)*/);
	WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, pszMultiByte, iSize, NULL, NULL);

	return pszMultiByte;
}
