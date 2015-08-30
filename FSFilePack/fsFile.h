#include <iostream>
#include <stdio.h> 
#include "zlib.h"
#pragma comment(lib,"zlib.lib")
using namespace std;

#define fwrite0() fwrite(&nul,sizeof(char),1,f)

const char nul = 0;

struct findex
{
friend class fspack;
public:
	char *path;
	char *name;
	char clevel;

private:
	char namelen;
	unsigned long fsize;
	unsigned long ftruesize;
	unsigned long nowp;
	unsigned long nextp;
};

struct ffile
{
friend class fspack;
public:
	unsigned long filenum;
	char *path;
	findex *fi;

private:
	FILE *f;
};

class fspack
{
public:
	int pack(ffile *ff);
	int unpack2file(ffile *ff,findex *fi);
	char* unpack2mem(ffile *ff,findex *fi);

	void getfilelist(ffile *ff);
	
private:
	unsigned long getfilesize(FILE *pf);
};