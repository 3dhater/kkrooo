// SPDX-License-Identifier: GPL-3.0-only
#ifndef __GT_INPUT_H__
#define __GT_INPUT_H__

enum class kkKey : u32 
{
	K_NONE			=	0,
	K_BACKSPACE		=	8,
	K_TAB			=	9,
	K_ENTER			=	13,
	K_SHIFT			=	16,
	K_CTRL			=	17,
	K_ALT			=	18,
	K_PAUSE			=	19,
	K_CAPS_LOCK		=	20,
	K_ESCAPE		=	27,
	K_SPACE			=	32,
	K_PGUP			=	33,
	K_PGDOWN		=	34,
	K_END			=	35,
	K_HOME			=	36,
	K_LEFT			=	37,
	K_UP			=	38,
	K_RIGHT			=	39,
	K_DOWN			=	40,
	K_PRTSCR		=	44,
	K_INSERT		=	45,
	K_DELETE		=	46,
	K_0				=	48,
	K_1				=	49,
	K_2				=	50,
	K_3				=	51,
	K_4				=	52,
	K_5				=	53,
	K_6				=	54,
	K_7				=	55,
	K_8				=	56,
	K_9				=	57,
	K_A				=	65,
	K_B				=	66,
	K_C				=	67,
	K_D				=	68,
	K_E				=	69,
	K_F				=	70,
	K_G				=	71,
	K_H				=	72,
	K_I				=	73,
	K_J				=	74,
	K_K				=	75,
	K_L				=	76,
	K_M				=	77,
	K_N				=	78,
	K_O				=	79,
	K_P				=	80,
	K_Q				=	81,
	K_R				=	82,
	K_S				=	83,
	K_T				=	84,
	K_U				=	85,
	K_V				=	86,
	K_W				=	87,
	K_X				=	88,
	K_Y				=	89,
	K_Z				=	90,
	K_WIN			=	91,
	K_CONTEXT		=	93,
	K_NUM_0			=	96,
	K_NUM_1			=	97,
	K_NUM_2			=	98,
	K_NUM_3			=	99,
	K_NUM_4			=	100,
	K_NUM_5			=	101,
	K_NUM_6			=	102,
	K_NUM_7			=	103,
	K_NUM_8			=	104,
	K_NUM_9			=	105,
	K_NUM_MUL		=	106, // *
	K_NUM_ADD		=	107, // +
	K_NUM_SUB		=	109, // -
	K_NUM_DOT		=	110, // .
	K_NUM_SLASH		=	111, // /
	K_F1			=	112,
	K_F2			=	113,
	K_F3			=	114,
	K_F4			=	115,
	K_F5			=	116,
	K_F6			=	117,
	K_F7			=	118,
	K_F8			=	119,
	K_F9			=	120,
	K_F10			=	121,
	K_F11			=	122,
	K_F12			=	123,
	K_NUM_LOCK		=	144,
	K_SCROLL_LOCK	=	145,
	K_LSHIFT		=	160,
	K_RSHIFT		=	161,
	K_LCTRL			=	162,
	K_RCTRL			=	163,
	K_LALT			=	164,
	K_RALT			=	165,
	K_FUNC_MY_COMP	=	182, // Function key. open My computer
	K_FUNC_CALC		=	183, // Function key. open Calculator
	K_COLON			=	186, //;:
	K_ADD			=	187, //=+
	K_COMMA			=	188, //,<
	K_SUB			=	189, //-_
	K_DOT			=	190, //.>
	K_SLASH			=	191, //?
	K_TILDE			=	192, //`~
	K_FIGURE_OPEN	=	219, //[{
	K_BACKSLASH		=	220, //\|
	K_FIGURE_CLOSE	=	221, //]}
	K_QUOTE			=	222, //'"

	END,
};


#endif
