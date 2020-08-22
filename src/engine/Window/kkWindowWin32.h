// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_WINDOW_WIN32_H__
#define __KKROO_WINDOW_WIN32_H__

#include "Window/kkWindow.h"
#include "Classes/Strings/kkString.h"

#include <Windows.h>

class kkWindowWin32 : public kkWindow
{
	kkMainSystem * m_main_system = nullptr;
	HWND m_hWnd = nullptr;
	bool m_is_init = false;

	kkString m_class_name;

	u32 m_style = 0;
	u32 m_style_old = 0;
	u32 m_state = 0;

	RECT  m_oldWindowPosition;

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:

	kkWindowWin32();
	virtual ~kkWindowWin32();
	bool  init( u32 i, u32 style, const v4i& rect, kkWindow* parent = nullptr, u32 state = 0 );

	void  setWindowText( const char16_t * text );
	void  toFullscreen();
	void  toWindow();
	void* getHandle();
	void hide();
	void show();
	void setFocus(bool toTop);
	void moveWindow(const v2i& position);
	void resizeWindow(const v2i& size);

	HDC  m_dc;
};


#endif