#pragma once
//#define islegchar(c)	ctype_[(c)+1] & (3)

struct img_member
{
	wchar_t *name;
	wchar_t *path;
	bool mask;
};

struct aimg_member
{
	wchar_t *name;
	wchar_t *path;
	int hnum;
	int vnum;
};

class ResManagerClass
{
public:
	std::list<img_member*> imglist;
	std::list<aimg_member*> aimglist;
	std::map<std::string,std::string> musicmap;

	bool LoadResArray(const char *resfile);

private:
	FILE *f;

	char *buf;
	unsigned long filelen;
};

extern ResManagerClass UI_ResManager;