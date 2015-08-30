#include "fsFile.h"

int main()
{
	fspack fp;

	ffile f;
	findex fi[3];
	f.path = "data.dat";
	f.filenum = 3;

	//Ñ¹ËõÊ¾Àý
	//*
	f.fi = fi;
	fi[0].path = "0.txt";
	fi[0].name = "0.txt";
	fi[0].clevel = 9;
	fi[1].path = "0.doc";
	fi[1].name = "0.doc";
	fi[1].clevel = 9;
	fi[2].path = "0.ttf";
	fi[2].name = "0.ttf";
	fi[2].clevel = 9;

	fp.pack(&f);//*/

	//½âÑ¹Ê¾Àý
	/*
	fp.getfilelist(&f);
	for(int i=0;i<3;i++)
		fp.unpack2file(&f,&f.fi[i]);
	return 0;//*/
}