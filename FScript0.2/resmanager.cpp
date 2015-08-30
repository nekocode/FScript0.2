#include "StdAfx.h"
#include "resmanager.h"
using namespace std;

//每种格式的个数
#define MEMBER_NUM_IMG 3
#define MEMBER_NUM_AIMG 4
#define MEMBER_NUM_MUSIC 2
enum ResStyle {img,aimg,music,none};

ResManagerClass UI_ResManager;


bool ResManagerClass::LoadResArray(const char *resfile)
{
	if(fopen_s(&f,resfile,"rb"))
		return false;

	fseek(f,0,2); filelen = ftell(f); fseek(f,0,0);
	buf = new char[filelen];
	fread(buf,filelen,1,f);

	//初始化各种mark
	bool in_note = false, in_string = false;
	int string_strat_pos = -1, mn_img = 0, mn_aimg = 0, mn_music = 0;
	char *string = 0;
	ResStyle style_entry = none;
	char *imgmember_str[MEMBER_NUM_IMG], *aimgmember_str[MEMBER_NUM_AIMG], *musicmember_str[MEMBER_NUM_MUSIC];

	for(unsigned long p=0;p<filelen;p++)
	{
		switch(buf[p])
		{
		case ' ': case '\f': case '\t': case '\v':		//跳过空格制表符
		case '\n': case '\r':							//跳过换行
			if(in_note) break;
			if(in_string)
			{
				buf[p] = 0;

				int slen = p-string_strat_pos + 1;
				string = new char[slen];
				strcpy_s(string,slen,buf+string_strat_pos);

				string_strat_pos = -1;
				in_string = false;
			}
			break;

		//注释
		case '/':
			if(buf[p+1]=='*') {p++; in_note = true;}
			break;
		case '*':
			if(buf[p+1]=='/')
				if(in_note) {p++; in_note = false;};
			break;

		//类型入口
		case ':':
			if(buf[p+1]=='i'&&buf[p+2]=='m'&&buf[p+3]=='g')	{style_entry=img;p+=3;}
			else if(buf[p+1]=='a'&&buf[p+2]=='i'&&buf[p+3]=='m'&&buf[p+4]=='g') {
				style_entry=aimg;p+=4;}
			else if(buf[p+1]=='m'&&buf[p+2]=='u'&&buf[p+3]=='s'&&buf[p+4]=='i'&&buf[p+5]=='c') {
				style_entry=music;p+=5;}
			else style_entry=none;
			break;


		//String
		default:
			if(in_note) break;

			if(!in_string)
			{
				string_strat_pos = p;
				in_string = true;
			}
			else
				//到达文件尾
				if(p==filelen-1)
				{
					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					int slen = p-string_strat_pos + 2;
					string = new char[slen];
					for(int i=0;i<slen+1;i++)
						string[i] = buf[string_strat_pos++];
					string[slen-1]=0;
				}

			break;
		}

		//接收到string的话
		if(string)
		{
			//确定资源类型
			switch(style_entry)
			{
			case img:
				//将获得的string放入resarray_member
				if(mn_img < MEMBER_NUM_IMG)
				{
					imgmember_str[mn_img] = string;
					string = 0;
				}
				//string达到数量时,放入一个pic_member中,并push list
				if(++mn_img == MEMBER_NUM_IMG)
				{
					img_member *im = new img_member;
					im->name = ctow(imgmember_str[0]);
					im->path = ctow(imgmember_str[1]);
					im->mask = !strcmp("yes",imgmember_str[2]);

					imglist.push_back(im);

					delete[] imgmember_str[0];
					delete[] imgmember_str[1];
					delete[] imgmember_str[2];

					mn_img=0;
				}
				break;
			case aimg:
				if(mn_aimg < MEMBER_NUM_AIMG)
				{
					aimgmember_str[mn_aimg] = string;
					string = 0;
				}
				if(++mn_aimg == MEMBER_NUM_AIMG)
				{
					aimg_member *aim = new aimg_member;
					aim->name = ctow(aimgmember_str[0]);
					aim->path = ctow(aimgmember_str[1]);
					aim->hnum= atoi(aimgmember_str[2]);
					aim->vnum= atoi(aimgmember_str[3]);

					aimglist.push_back(aim);

					delete[] aimgmember_str[0];
					delete[] aimgmember_str[1];
					delete[] aimgmember_str[2];
					delete[] aimgmember_str[3];

					mn_aimg=0;
				}
				break;
			case music:
				if(mn_music < MEMBER_NUM_MUSIC)
				{
					musicmember_str[mn_music] = string;
					string = 0;
				}
				if(++mn_music == MEMBER_NUM_MUSIC)
				{
					musicmap.insert(pair<std::string,std::string>(musicmember_str[0],musicmember_str[1]));

					delete[] musicmember_str[0];
					delete[] musicmember_str[1];

					mn_music=0;
				}
				break;
			}
		}
	}

	return true;
}


/*
//from lctype.c
LUAI_DDEF const unsigned char ctype_[UCHAR_MAX + 2] = {
  0x00,  // EOZ
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// 0. 
  0x00,  0x08,  0x08,  0x08,  0x08,  0x08,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// 1. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x0c,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,	// 2. 
  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,
  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,	// 3. 
  0x16,  0x16,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,
  0x04,  0x15,  0x15,  0x15,  0x15,  0x15,  0x15,  0x05,	// 4. 
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,	// 5. 
  0x05,  0x05,  0x05,  0x04,  0x04,  0x04,  0x04,  0x05,
  0x04,  0x15,  0x15,  0x15,  0x15,  0x15,  0x15,  0x05,	// 6. 
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,	// 7. 
  0x05,  0x05,  0x05,  0x04,  0x04,  0x04,  0x04,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// 8. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// 9. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// a. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// b. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// c. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// d. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// e. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	// f. 
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
};*/