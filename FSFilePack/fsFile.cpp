#include "fsFile.h"

int fspack::pack(ffile *ff)
{
	unsigned long filenum = ff->filenum;
	char *path = ff->path;
	findex *fi = ff->fi;
	FILE *f = 0;

	if(!(f = fopen(path,"wb")))
		return -1;
	fwrite("FSFILE",6,1,f);
	fwrite0();
	//写入文件数量
	fwrite(&filenum,sizeof(unsigned long),1,f);
	fwrite0();

	for(int i=0;i<filenum;i++)
	{
		FILE *pf = 0;
		if(!(pf = fopen(fi[i].path,"rb")))
			return -1;
		//获得文件大小
		fi[i].ftruesize = getfilesize(pf);

		//读取文件到字符串
		char *fb = new char[fi[i].ftruesize];
		fread(fb,fi[i].ftruesize,1,pf);

		//压缩
		if(int(fi[i].clevel)!=0)
		{
			char *fbc = new char[fi[i].ftruesize*2];
			fi[i].fsize = fi[i].ftruesize*2;
			compress2((byte *)fbc,&(fi[i].fsize),(byte *)fb,fi[i].ftruesize,fi[i].clevel);
			delete fb;
			fb = fbc;
			fbc = 0;
		}
		else
		{
			fi[i].fsize = fi[i].ftruesize;
		}

		fi[i].namelen = strlen(fi[i].name);
		//获得文件大小,并计算出偏移
		fi[i].nextp = ftell(f) + 10 + fi[i].namelen + fi[i].fsize;
		fclose(pf);

		fwrite(&fi[i].namelen,sizeof(char),1,f);			//名称长度
		fwrite(fi[i].name,(int)fi[i].namelen,1,f);			//名称
		fwrite(&fi[i].clevel,sizeof(char),1,f);				//压缩度(0-9)
		fwrite(&fi[i].ftruesize,sizeof(unsigned long),1,f);	//文件解压后大小
		fwrite(&fi[i].nextp,sizeof(unsigned long),1,f);		//下一个索引的位置
		fi[i].nowp = ftell(f);
		fwrite(fb,fi[i].fsize,1,f);							//写入文件
	}

	fclose(f);
	return 0;
}


int fspack::unpack2file(ffile *ff,findex *fi)
{
	FILE *f = 0;
	
	if(!(f = fopen(ff->path,"rb")))
		return -1;

	fseek(f,fi->nowp,0);

	char *fb = new char[fi->fsize];
	fread(fb,fi->fsize,1,f);
	//解压缩
	if(int(fi->clevel))
	{
		char *fbc = new char[fi->ftruesize*2];
		unsigned long destlen = fi->ftruesize*2;
		uncompress((byte *)fbc,&destlen,(byte *)fb,fi->fsize);
		delete fb;
		fb = fbc;
		fbc = 0;
	}

	FILE *pf;
	if(!(pf = fopen(fi->name,"wb")))
		return -1;
	fwrite(fb,fi->ftruesize,1,pf);
	fclose(pf);

	return 0;
}


char* fspack::unpack2mem(ffile *ff,findex *fi)
{
	FILE *f = 0;

	if(!(f = fopen(ff->path,"rb")))
		return 0;

	fseek(f,fi->nowp,0);

	char *fb = new char[fi->fsize];
	fread(fb,fi->fsize,1,f);
	//解压缩
	if(int(fi->clevel))
	{
		char *fbc = new char[fi->ftruesize*2];
		unsigned long destlen = fi->ftruesize*2;
		uncompress((byte *)fbc,&destlen,(byte *)fb,fi->fsize);
		delete fb;
		fb = fbc;
		fbc = 0;
	}
	return fb;
}


void fspack::getfilelist(ffile *ff)
{
	unsigned long *filenum = &ff->filenum;
	char *path = ff->path;

	ff->f = fopen(path,"rb");
	FILE *f = 0;

	fseek(f,7,0);

	//读取文件数量
	fread(filenum,sizeof(unsigned long),1,f);
	ff->fi = new findex[*filenum];
	findex *fi = ff->fi;

	fseek(f,1,1);

	for(int i=0;i<*filenum;i++)
	{
		fread(&fi[i].namelen,sizeof(char),1,f);				//名称长度
		fi[i].name = new char[(int)fi[i].namelen];
		fread(fi[i].name,(int)fi[i].namelen,1,f);			//名称
		fi[i].name[(int)fi[i].namelen] = '\0';
		fread(&fi[i].clevel,sizeof(char),1,f);				//压缩度(0-9)
		fread(&fi[i].ftruesize,sizeof(unsigned long),1,f);	//文件解压后大小
		fread(&fi[i].nextp,sizeof(unsigned long),1,f);		//下一个索引的位置

		fi[i].nowp = ftell(f);
		fi[i].fsize = fi[i].nextp - fi[i].nowp;				//文件大小
		fseek(f,fi[i].fsize,1);
	}
}


unsigned long fspack::getfilesize(FILE *pf)
{
	long nowp = ftell(pf);
	fseek(pf,0,2);
	long len = ftell(pf);
	fseek(pf,nowp,0);

	return len;
};