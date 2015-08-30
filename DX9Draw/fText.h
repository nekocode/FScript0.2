bool initText(const char *font_lib);
void ReleaseText();

class ftext : public item
{
public:
	wchar_t *_strText;
	POINT *point;
	unsigned int *font_width;
	unsigned int *font_height;
	unsigned int *line_width;
	bool *center;
	D3DCOLOR *color;

	ftext() {item_type=0;}
	void release();
	void draw();
};