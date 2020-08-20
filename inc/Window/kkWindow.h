// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_WINDOW_H__
#define __KKROO_WINDOW_H__

#include "Classes/Math/kkVector4.h"

#ifdef KK_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

class kkGraphicsSystem;

class kkWindow
{
	/// Call only inside kkGraphicsSystem
	friend class kkGraphicsSystem;
	void setAssociatedGS( kkGraphicsSystem * new_gs )
	{
		m_gs = new_gs;
	}

protected:
	v4i				m_client_rect;
	v4i				m_window_rect;
	void (*f_onMove)(kkWindow*) = nullptr;
	void (*f_onSize)(kkWindow*) = nullptr;
	void (*f_onPaint)(kkWindow*) = nullptr;
	void (*f_onClose)(kkWindow*) = nullptr;



	bool			m_is_fullscreen = false;
	bool			m_is_visible = true;
	kkGraphicsSystem * m_gs  = nullptr;

	s32 m_id = -1;

public:

	kkWindow(){};
	virtual ~kkWindow(){};

	virtual void setWindowText( const char16_t * text ) = 0;
	virtual void toFullscreen() = 0;
	virtual void toWindow() = 0;
	virtual void hide() = 0;
	virtual void show() = 0;
	virtual void setFocus(bool toTop) = 0;


	/// HWND
	virtual void* getHandle() = 0;

	//----------------------------------------------------------------
	//----------------------------------------------------------------
	//----------------------------------------------------------------

	void set_onMove( void(*f)(kkWindow*) )
	{
		f_onMove = f;
	}

	void set_onSize( void(*f)(kkWindow*) )
	{
		f_onSize = f;
	}

	void set_onPaint( void(*f)(kkWindow*) )
	{
		f_onPaint = f;
	}

	void set_onClose( void(*f)(kkWindow*) )
	{
		f_onClose = f;
	}

#ifdef KK_PLATFORM_WINDOWS
	bool (*f_Win32_WndProcHandler)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = nullptr;
	void set_onWin32_WndProcHandler( bool(*f)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) )
	{
		f_Win32_WndProcHandler = f;
	}
#endif

	const v4i& getClientRect()
	{
		return m_client_rect;
	}

	const v4i& getWindowRect()
	{
		return m_window_rect;
	}

	bool    isFullscreen()
	{
		return m_is_fullscreen;
	}

	bool    isVisible()
	{
		return m_is_visible;
	}

	kkGraphicsSystem * getAssociatedGS()
	{
		return m_gs;
	}

	s32 getID() const
	{
		return m_id;
	}

	void setID(s32 id)
	{
		m_id = id;
	}

	
};


#endif