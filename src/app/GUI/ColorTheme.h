#ifndef __COLOR_THEME_H__
#define __COLOR_THEME_H__

#include "Classes/kkColor.h"

struct ColorTheme
{
	kkColor clear_color                = 0xff505050;
	kkColor viewport_backgroung_color1 = 0xff767676;	// верх 
	kkColor viewport_backgroung_color2 = 0xff565656;	// низ
	kkColor viewport_grid_color1 = 0xffDD4E6E;			// ось Х
	kkColor viewport_grid_color2 = 0xff6EEE66;			// ось Z
	kkColor viewport_grid_color3 = 0xff808080;			// остальная сетка
	kkColor viewport_grid_color4 = 0xff505050;			// камера ниже 0, полосы другого цвета
	kkColor viewport_grid_color5 = 0xff6E66EE;			// ось Y
	kkColor viewport_border_color = kkColorBlack;
	kkColor viewport_active_color = kkColorYellow;
	kkColor viewport_mousehover_color = 0xFF596CA0;
	
	// основные кнопки
	kkColor buttons_common_normal_color = 0xFF444444;
	kkColor buttons_common_hover_color  = 0xFF666666;
	kkColor buttons_common_active_color = 0xFF222222;
};

#endif