#include "msg.h"

class MainUI_Class
{
public:
	MainUI_Class();

	bool UI_Init();
	bool UI_Show();

private:
	fimg	*img_BG;
	fimg	*btn[8];
	fimg	*btn_select[8];
	faimg	*aimg_loading;

	bool loaduires(const char *uiresfile);
	void refresh_btnpos(int firstbtn_x);
};

extern MainUI_Class MainUI;