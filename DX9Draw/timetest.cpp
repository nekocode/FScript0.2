#include "stdafx.h"
#include "timetest.h"

double timetest()
{
	static int i = 0;
	static LONGLONG QPart1,QPart2;
	static LARGE_INTEGER litmp;

	if(!i)
	{
		i++;
		QueryPerformanceFrequency(&litmp);
		QueryPerformanceCounter(&litmp);
		QPart1	=	litmp.QuadPart;
	}
	else
	{
		QueryPerformanceCounter(&litmp);
		QPart2   =   litmp.QuadPart;
		double rt = (double)(QPart2-QPart1);

		i		=	0;
		QPart1	=	0;
		QPart2	=	0;
		litmp.QuadPart = 0;
		return rt;
	}
	return 0;
}