#include "stdafx.h"
#include "out_control.h"
#include "fImage.h"
#include "fText.h"
//=============================================================
//                  img列表控制实现(导出)
//=============================================================

//系统鼠标指针の外壳
imgout out_cur;

/****************************
          增加图片
****************************/
DX9DRAW_API bool _addimg(imgout **_img,wchar_t *_name,void *_foward)
{
	if(!*_img)
	{
		fimg *imgnew = new fimg;
		imgnew->front = 0;
		imgnew->next = 0;

		imgout *_out = new imgout;
		_out->alpha = 0;
		_out->nowx = _out->nowy = _out->rota = 0;
		imgnew->width		= &(_out->width);
		imgnew->height		= &(_out->height);
		imgnew->nowx		= &(_out->nowx);
		imgnew->nowy		= &(_out->nowy);
		imgnew->rota		= &(_out->rota);
		imgnew->rota_x		= &(_out->rota_x);
		imgnew->rota_y		= &(_out->rota_y);
		imgnew->alpha		= &(_out->alpha);
		_out->p_nexttex		= &(imgnew->nt);

		_out->img			= imgnew;
		*_img = _out;

		imgnew->load(_name);

		// 如果参数frontimg不为0,则将新图片插入到frontimg之后
		if(_foward)
		{
			item *rfoward = *((item**)_foward);

			if(rfoward==render_head)
			{
				imgnew->next = render_head;
				render_head->front = imgnew;
				render_head = imgnew;
			}
			else
			{
				imgnew->next = rfoward;
				imgnew->front = rfoward->front;
				rfoward->front->next = imgnew;
				rfoward->front = imgnew;
			}
		}
		// 否则插入到图片队列的最尾部
		else
		{
			if(render_head)
			{
				imgnew->front = render_now;
				render_now->next = imgnew;
				render_now = imgnew;
			}
			else
			{
				render_head = imgnew;
				render_now = render_head;
			}
		}
	}
	else
	{
		if(!((fimg*)((*_img)->img))->load(_name))
		{
			return 0;
		}
	}
	return true;
}

/****************************
          增加图片
****************************/
DX9DRAW_API bool _addaimg(aimgout **_aimg,wchar_t *_name,unsigned int _delay,void *_foward)
{
	if(!*_aimg)
	{
		faimg *aimgnew = new faimg;
		aimgnew->front = 0;
		aimgnew->next = 0;

		aimgout *_out = new aimgout;
		_out->alpha = 0;
		_out->nowx = _out->nowy = _out->rota = 0;
		aimgnew->width			= &(_out->width);
		aimgnew->height			= &(_out->height);
		aimgnew->nowx			= &(_out->nowx);
		aimgnew->nowy			= &(_out->nowy);
		aimgnew->rota			= &(_out->rota);
		aimgnew->rota_x			= &(_out->rota_x);
		aimgnew->rota_y			= &(_out->rota_y);
		aimgnew->alpha			= &(_out->alpha);

		_out->delay = _delay;
		_out->autoplay = true;
		_out->loop = true;
		aimgnew->frame_start	= &(_out->frame_start);
		aimgnew->frame_end		= &(_out->frame_end);
		aimgnew->frame_now		= &(_out->frame_now);
		aimgnew->frame_all		= &(_out->frame_all);
		aimgnew->delay			= &(_out->delay);
		aimgnew->delay_count	= &(_out->delay_count);
		aimgnew->autoplay		= &(_out->autoplay);
		aimgnew->loop			= &(_out->loop);

		_out->aimg	= aimgnew;
		*_aimg = _out;

		aimgnew->load(_name);

		// 如果参数frontimg不为0,则将新图片插入到frontimg之后
		if(_foward)
		{
			item *rfoward = *((item**)_foward);

			if(rfoward==render_head)
			{
				aimgnew->next = render_head;
				render_head->front = aimgnew;
				render_head = aimgnew;
			}
			else
			{
				aimgnew->next = rfoward;
				aimgnew->front = rfoward->front;
				rfoward->front->next = aimgnew;
				rfoward->front = aimgnew;
			}
		}
		// 否则插入到图片队列的最尾部
		else
		{
			if(render_head)
			{
				aimgnew->front = render_now;
				render_now->next = aimgnew;
				render_now = aimgnew;
			}
			else
			{
				render_head = aimgnew;
				render_now = render_head;
			}
		}

		
	}
	else
	{
		if(!((faimg*)((*_aimg)->aimg))->load(_name))
		{
			return 0;
		}
		*(((faimg*)((*_aimg)->aimg))->delay) = _delay;
	}
	return true;
}

/****************************
          增加文字
****************************/
DX9DRAW_API bool _addtext(textout **_text,wchar_t *_strText,void *_foward)
{
	if(!*_text)
	{
		ftext *textnew = new ftext;
		textnew->front = 0;
		textnew->next = 0;

		int len = wcslen(_strText)+1;
		wchar_t *tempstr = new wchar_t[len]; 
		wcscpy(tempstr,_strText);
		textnew->_strText = tempstr;

		textout *_out = new textout;
		_out->point.x = _out->point.y = 0;
		_out->font_width = _out->font_height = 25;
		_out->line_width = 800;
		_out->center = false;
		_out->color = 0xffffffff;
		_out->_strText			= &(textnew->_strText);
		textnew->point			= &(_out->point);
		textnew->font_width		= &(_out->font_width);
		textnew->font_height	= &(_out->font_height);
		textnew->line_width		= &(_out->line_width);
		textnew->center			= &(_out->center);
		textnew->color			= &(_out->color);

		_out->text			= textnew;
		*_text = _out;

		
		// 如果参数frontimg不为0,则将新图片插入到frontimg之后
		if(_foward)
		{
			item *rfoward = *((item**)_foward);

			if(rfoward==render_head)
			{
				textnew->next = render_head;
				render_head->front = textnew;
				render_head = textnew;
			}
			else
			{
				textnew->next = rfoward;
				textnew->front = rfoward->front;
				rfoward->front->next = textnew;
				rfoward->front = textnew;
			}
		}
		// 否则插入到图片队列的最尾部
		else
		{
			if(render_head)
			{
				textnew->front = render_now;
				render_now->next = textnew;
				render_now = textnew;
			}
			else
			{
				render_head = textnew;
				render_now = render_head;
			}
		}
	}
	else
	{
		if(*((*_text)->_strText))
			delete[] *((*_text)->_strText);

		int len = wcslen(_strText)+1;
		wchar_t *tempstr = new wchar_t[len]; 
		wcscpy(tempstr,_strText);
		*((*_text)->_strText) = tempstr;
	}

	return true;
}


/****************************
          删除图片
****************************/
DX9DRAW_API void _delete(void *_r)
{
	item *r = *((item**)_r);

	if(r==render_head)
	{
		if(r->next)
		{
			r->next->front = 0;
			render_head = r->next;
		}
		else
		{
			render_head = 0;
			render_now = 0;
		}
	}
	else if(r==render_now)
	{
		r->front->next = 0;
		render_now = r->front;
	}
	else
	{
		r->front->next = r->next;
		r->next->front = r->front;
	}
	
	r->release();
	delete r;
	_r = 0;
}


/****************************
       插入目标item前
****************************/
DX9DRAW_API void _fowardinsert(void *_dest,void *_source)
{
	item *rsource = *((item**)_source);

	if(rsource==render_head)
	{
		rsource->next->front = 0;
		render_head = rsource->next;
	}
	else if(rsource==render_now)
	{
		rsource->front->next = 0;
		render_now = rsource->front;
	}
	else
	{
		rsource->next->front = rsource->front;
		rsource->front->next = rsource->next;
	}
	rsource->next = 0;
	rsource->front = 0;

	if(_dest)
	{
		item *rfoward = *((item**)_dest);

		if(rfoward==render_head)
		{
			rsource->next = render_head;
			render_head->front = rsource;
			render_head = rsource;
		}
		else
		{
			rsource->next = rfoward;
			rsource->front = rfoward->front;
			rfoward->front->next = rsource;
			rfoward->front = rsource;
		}
	}
	// 否则插入到图片队列的最尾部
	else
	{
		if(render_head)
		{
			rsource->front = render_now;
			render_now->next = rsource;
			render_now = rsource;
		}
		else
		{
			render_head = rsource;
			render_now = render_head;
		}
	}
}

/****************************
       交换两个item次序
****************************/
DX9DRAW_API void _exchange(void *_r1,void *_r2)
{
	item *r1		= *((item**)_r1);
	item *r2		= *((item**)_r2);
	item** tmp = &(r1->next);
	_fowardinsert(_r2,_r1);
	_fowardinsert(tmp,_r2);
}

/****************************
       获得item的次序
****************************/  //  ft* _r_array[i]
DX9DRAW_API void _getsequence(void **_r_array,int num)
{
	item **tmp = new item*[num];
	int *seq = new int[num];

	for(int i=0;i<num;i++)
	{
		item *r = (*((item***)_r_array))[i];
		tmp[i] = r;
	}

	item *sch = render_head;
	int num_seq = 0,nownum = 0;
	while(sch)
	{
		if(nownum==num)
			break;
		for(int i=0;i<num;i++)
		{
			if(sch == tmp[i])
			{
				seq[nownum] = i;
				nownum++;
				break;
			}
		}
		sch = sch->next;
	}
}



callbackfunc cb_func;
int last_x=0, last_y=0;
imgbase* last_mousedown_img = (imgbase*)0;
imgbase* last_img = (imgbase*)1;

DX9DRAW_API void _bindbtn(void *_r,int min_alpha,void *data)
{
	imgbase *img = *((imgbase**)_r);
	img->btn_bind(_r,min_alpha,data);
}

DX9DRAW_API void _setbtn_enable(void *_r,bool enable)
{
	imgbase *img = *((imgbase**)_r);
	if(img->btn_map)
		img->btn_enable = enable;
}

DX9DRAW_API void _setbtn_callbackfunc(callbackfunc func)
{
	cb_func = func;
}

DX9DRAW_API void _refresh_btnmap(void *_r)
{
	imgbase *img = *((imgbase**)_r);
	img->refresh_map();
}

DX9DRAW_API void _refresh_allmap()
{
	last_mousedown_img = (imgbase*)0;
	last_img = (imgbase*)1;

	item *sch = render_head;

	imgbase* **&fmap = full_map.full_map;
	for(int x=0;x<full_map.width;x++)
	{
		for(int y=0;y<full_map.height;y++)
			fmap[x][y] = 0;
	}

	while(sch)
	{
		//如果为img或faimg的话
		if(sch->item_type>0)
		{
			imgbase *img = (imgbase*)sch;
			if(img->btn_enable&&img->btn_map)
			{
				//!----------不规则按钮----------
				if(img->btn_map->min_alpha)
				{
					int base_x = *(img->nowx)+img->btn_map->x_delta, base_y = *(img->nowy)+img->btn_map->y_delta;
					int width = img->btn_map->width, height = img->btn_map->height;
					byte **&map = img->btn_map->btn_map;
					imgbase* **&fmap = full_map.full_map;

					if((base_x+width<0)||(base_x>full_map.width)||(base_y+height<0)||(base_y>full_map.height))
					{
						sch = sch->next;
						continue;
					}

					//遍历btn_map,并写入full_map
					for(int x=0;x<width;x++)
					{
						for(int y=0;y<height;y++)
						{
							if(map[x][y])
							{
								int f_x = base_x+x, f_y = base_y+y;
								//如果在屏幕范围内的话
								if((f_x<full_map.width)&&(f_x>0)&&(f_y<full_map.height)&&(f_y>0))
									fmap[f_x][f_y] = img;
							}
						}
					}
				}
				//!----------规则按钮----------
				else
				{
					int base_x = *(img->nowx), base_y = *(img->nowy);
					int width = *(img->width), height = *(img->height);
					imgbase* **&fmap = full_map.full_map;

					if((base_x+width<0)||(base_x>full_map.width)||(base_y+height<0)||(base_y>full_map.height))
					{
						sch = sch->next;
						continue;
					}

					//遍历btn_map,并写入full_map
					for(int x=0;x<width;x++)
					{
						for(int y=0;y<height;y++)
						{
							int f_x = base_x+x, f_y = base_y+y;
							//如果在屏幕范围内的话 -----------!!!!!!!!!!!!!!!!!!!!可提高效率
							if((f_x<full_map.width)&&(f_x>0)&&(f_y<full_map.height)&&(f_y>0))
								fmap[f_x][f_y] = img;
						}
					}
				}
			}
		}

		sch = sch->next;
	}
}

DX9DRAW_API void _setmousepos(int x,int y)
{
	imgbase* **fmap = full_map.full_map;
	out_cur.nowx = x-10; out_cur.nowy = y-10;

	//如果没有改变则直接返回
	if(last_img==fmap[x][y])
		return;

	if(last_img!=(imgbase*)0 && last_img!=fmap[x][y] && last_img!=(imgbase*)1)
		//mouse_over
		cb_func(last_img->btn_map->outitem,last_img->item_type,0,last_img->btn_map->data);

	if(fmap[x][y])
	{
		btnmap *tmp = fmap[x][y]->btn_map;
		//mouse_in
		cb_func(tmp->outitem,fmap[x][y]->item_type,1,tmp->data);

		last_img = fmap[x][y];
	}
	else
	{
		last_img = (imgbase*)0;
	}

	last_x=x, last_y=y;
}

DX9DRAW_API void _setmouseclick(int msg)
{
	imgbase* **fmap = full_map.full_map;

	if(fmap[last_x][last_y])
	{
		btnmap *tmp = fmap[last_x][last_y]->btn_map;
		cb_func(tmp->outitem,fmap[last_x][last_y]->item_type,msg,tmp->data);

		//mouse_up
		if(msg==3)
		{
			if(last_mousedown_img == fmap[last_x][last_y])
			{
				//mouse_click
				cb_func(tmp->outitem,fmap[last_x][last_y]->item_type,4,tmp->data);
			}
		}
	}

	last_mousedown_img = fmap[last_x][last_y];
}

DX9DRAW_API bool  _setcur(wchar_t *name)
{
	if(system_cur)
	{
		return system_cur->load(name);
	}
	else
	{
		system_cur = new fimg;

		system_cur->width		= &(out_cur.width);
		system_cur->height		= &(out_cur.height);
		system_cur->nowx		= &(out_cur.nowx);
		system_cur->nowy		= &(out_cur.nowy);
		system_cur->rota		= &(out_cur.rota);
		system_cur->rota_x		= &(out_cur.rota_x);
		system_cur->rota_y		= &(out_cur.rota_y);
		system_cur->alpha		= &(out_cur.alpha);
		
		out_cur.alpha = 255;

		return system_cur->load(name);
	}
}



DX9DRAW_API void _fadeimg(imgout *_img,wchar_t *name,float fadestep,float x,float y,unsigned int width,unsigned int height,float alpha)
{
	fimg *img = (fimg *)(_img->img);
	img->fade(name,fadestep,x,y,width,height,alpha);
}

DX9DRAW_API void _setfademap(imgout *_img,wchar_t *name)
{
	fimg *img = (fimg *)(_img->img);
	img->setfademap(name);
}

DX9DRAW_API bool _loadimgfromfile(wchar_t *path,bool mask,wchar_t *name)
{
	return LoadImgFromFile(path,mask,name);
}

DX9DRAW_API bool _loadimgfrommem(char *mem,unsigned long size,bool mask,wchar_t *name)
{
	return LoadImgFromMem(mem,size,mask,name);
}


//========================aimg=========================

DX9DRAW_API bool _loadaimgfromfile(wchar_t *path,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name)
{
	return LoadaImgFromFile(path,hn,vn,delay,name);
}

DX9DRAW_API bool _loadaimgfrommem(char *mem,unsigned long size,unsigned int hn,unsigned int vn,unsigned int *delay,wchar_t *name)
{
	return LoadaImgFromMem(mem,size,hn,vn,delay,name);
}

DX9DRAW_API bool _loadaimgfromimgs(unsigned int img_number,wchar_t **imgsname,unsigned int *delay,wchar_t *name)
{
	return LoadaImgFromImgs(img_number,imgsname,delay,name);
}

DX9DRAW_API void _test()
{
	fimg *t = new fimg;
	
	//记得为其加上文件exist处理
	LoadImgFromFile(L"pic\\1.png",false,L"穹");
//	t->load(L"穹");
	render_head = (item*)t;
}