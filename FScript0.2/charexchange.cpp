#include "stdafx.h"

char* wtoc(const wchar_t *str)
{
	if(str)
	{
		int iSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
//		char *pszMultiByte = (char*)malloc((iSize+1)/**sizeof(char)*/);
		char *pszMultiByte = new char[iSize+1];
		WideCharToMultiByte(CP_ACP, 0, str, -1, pszMultiByte, iSize, NULL, NULL);
		return pszMultiByte;
	}

	return 0;
}

wchar_t* ctow(const char *str)
{
	if(str)
	{
		size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,-1,NULL,0);
		wchar_t *buffer = new wchar_t[n+1];
		MultiByteToWideChar(CP_ACP,0,(const char *)str,-1,buffer,int(n));
		return buffer;
	}

	return 0;
}
