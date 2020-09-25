#define KK_EXPORTS

#include "kkrooo.engine.h"

#ifdef KK_PLATFORM_WINDOWS

#include "MainSystem/kkMainSystem.h"
#include "kkWindowWin32.h"
#include "kkWindowWin32_loc.h"

#include "Events/kkEvent.h"

#include "GraphicsSystem/kkGraphicsSystem.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

kkWindowWin32::kkWindowWin32()
{
	m_main_system = kkGetMainSystem();
}

kkWindowWin32::~kkWindowWin32()
{
	if( m_is_init )
	{

		if( m_hWnd )
		{
			ReleaseDC(m_hWnd,m_dc);
			DestroyWindow( m_hWnd );
		}
		UnregisterClass( (wchar_t*)m_class_name.c_str(), GetModuleHandle( 0 ) );
	}
}

void kkWindowWin32::moveWindow(const v2i& position)
{
	auto size = m_window_rect.getWidthAndHeight();
	MoveWindow(m_hWnd, position.x, position.y, size.x, size.y, TRUE);
}

void kkWindowWin32::resizeWindow(const v2i& size)
{
	MoveWindow(m_hWnd, m_window_rect.x, m_window_rect.y, size.x, size.y, TRUE);
	m_window_rect.z = m_window_rect.x + size.x;
	m_window_rect.w = m_window_rect.y + size.y;
	m_creation_size = size;
}

void kkWindowWin32::hide()
{
	if( m_is_init )
	{
		ShowWindow(m_hWnd,SW_HIDE);
		m_is_visible = false;
	}
}

void kkWindowWin32::show()
{
	if( m_is_init )
	{
		ShowWindow(m_hWnd,SW_SHOWNORMAL);
		m_is_visible = true;
	}
}

bool kkWindowWin32::init( u32 i, u32 _style, const v4i& rect, kkWindow* parent, u32 state )
{
	if( m_is_init )
	{
		return false;
	}

	m_state = state;
	m_style = _style;
	m_window_rect = rect;

	m_creation_size = rect.getWidthAndHeight();

	DWORD style = WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX; // DWORD - ýòî unsigned long

	if( m_style & kk::window::style::popup )
	{
		style = WS_POPUP;
	}
	else
	{
		if( m_style & kk::window::style::maximize )
		{
			style |= WS_MAXIMIZEBOX;
		}
		if( m_style & kk::window::style::resize )
		{
			style |= WS_SIZEBOX;
		}
	}

	s32 windowLeft = m_window_rect.x;
	s32 windowTop  = m_window_rect.y;
	s32 realWidth  = m_window_rect.z;
	s32 realHeight = m_window_rect.w;
	
	if( m_style & kk::window::style::center )
	{

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = m_window_rect.z - m_window_rect.x;
		clientSize.bottom = m_window_rect.w - m_window_rect.y;

		AdjustWindowRect(&clientSize, style, FALSE);

		realWidth = clientSize.right - clientSize.left;
		realHeight = clientSize.bottom - clientSize.top;

		windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if( windowLeft < 0 )
		{
			windowLeft = 0;
		}

		if( windowTop < 0 )
		{
			windowTop = 0;
		}

		if( m_style & kk::window::style::popup )
		{
			windowLeft = 0;
			windowTop = 0;
		}
	}

	

	m_class_name = u"kkroooWINDOW_";
	m_class_name += u'a' + static_cast<char16_t>(i);

	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) ); // memset Winows style
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= GetModuleHandle( 0 );
	wc.hIcon			= nullptr;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_MENU+1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= (wchar_t*)m_class_name.c_str();
	wc.hIconSm			= nullptr;

	if( !RegisterClassEx(&wc) )
	{
		KK_PRINT_FAILED;
		return false;
	}

	DWORD exFlags = 0;

	if( m_style & kk::window::style::on_top )
	{
		exFlags = WS_EX_TOPMOST;
	}

	if( m_style & kk::window::style::modal && parent )
	{
		//style |= WS_CHILDWINDOW;
		exFlags |= WS_EX_WINDOWEDGE;
	}


	HWND hParent = nullptr;

	if( parent )
		hParent = (HWND)parent->getHandle(); 

	m_hWnd = CreateWindowEx( exFlags, (wchar_t*)m_class_name.c_str(),
		L"Window",
		style,
		windowLeft,
		windowTop,
		realWidth,
		realHeight,
		hParent,
		nullptr,
		wc.hInstance,
		this );

	if( !m_hWnd )
	{
		KK_PRINT_FAILED;
		return false;
	}

	m_style_old = m_style;
	
	if( (m_state & kk::window::state::hide) == 0 )
		ShowWindow( m_hWnd, SW_SHOWNORMAL  );
	else
		m_is_visible = false;

	if( m_style & m_style & kk::window::style::maximize || m_style & m_style & kk::window::style::resize ){
		if( m_state & kk::window::state::maximized )
		{
			ShowWindow( m_hWnd, SW_SHOWMAXIMIZED );
		}
		else if( m_state & kk::window::state::minimized )
		{
			ShowWindow( m_hWnd, SW_SHOWMINIMIZED );
		}
	}

	SetForegroundWindow( m_hWnd );
	SetFocus( m_hWnd );
	UpdateWindow( m_hWnd );

	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);

	int borderWidth = ((windowRect.right - windowRect.left) - (clientRect.right -clientRect.left))/2;

	POINT pt;
	pt.x = clientRect.left;
	pt.y = clientRect.top;
	ClientToScreen( m_hWnd, &pt );


	m_dc = GetDC(m_hWnd);

	m_is_init = true;

	return true;
}

void kkWindowWin32::setWindowText( const char16_t * text )
{
	if( SetWindowText( m_hWnd, (wchar_t*)text ) == FALSE )
	{
		KK_PRINT_FAILED;
		return;
	}
}

void kkWindowWin32::toFullscreen()
{
	if( !m_is_fullscreen )
	{
		/*auto desktop = GetDesktopWindow();
		RECT drc;
		GetClientRect( desktop, &drc );*/

		auto gs = getAssociatedGS();

		DEVMODE fullscreenSettings;
		EnumDisplaySettings( NULL, 0, &fullscreenSettings );
		fullscreenSettings.dmPelsWidth        = gs->getBackBufferSize().x;
		fullscreenSettings.dmPelsHeight       = gs->getBackBufferSize().y;
		fullscreenSettings.dmBitsPerPel       = gs->getColorDepth();
		fullscreenSettings.dmDisplayFrequency = GetDeviceCaps( m_dc, VREFRESH);
		fullscreenSettings.dmFields           = DM_PELSWIDTH |
												DM_PELSHEIGHT |
												DM_BITSPERPEL |
												DM_DISPLAYFREQUENCY;
		SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
	    SetWindowLongPtr( m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos( m_hWnd, HWND_TOPMOST, 0, 0, GetDeviceCaps(m_dc, HORZRES), GetDeviceCaps(m_dc, VERTRES), SWP_SHOWWINDOW);

		bool isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;

		if( isChangeSuccessful )
		{
			ShowWindow( m_hWnd, SW_MAXIMIZE );
		}
		else
		{
			KK_PRINT_FAILED;
			return;
		}

		GetWindowRect( m_hWnd, &m_oldWindowPosition );

		m_is_fullscreen = true;
	}
}

void kkWindowWin32::toWindow()
{
	if( m_is_fullscreen )
	{
		SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, WS_EX_LEFT);
		SetWindowLongPtr( m_hWnd, GWL_STYLE, m_style_old );
		ChangeDisplaySettings( NULL, CDS_RESET );
		SetWindowPos( m_hWnd, HWND_NOTOPMOST, m_oldWindowPosition.left, m_oldWindowPosition.top, m_oldWindowPosition.right, m_oldWindowPosition.bottom, SWP_SHOWWINDOW );
		ShowWindow( m_hWnd, SW_RESTORE );

		m_is_fullscreen = false;
	}
}

void* kkWindowWin32::getHandle()
{
	return m_hWnd;
}

void kkWindowWin32::setFocus(bool toTop)
{
	SetFocus(m_hWnd);
	if( toTop )
	{
		SetForegroundWindow(m_hWnd);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//printf("%u\n",message);
	RECT rc;

	kkWindowWin32* pD = nullptr;

	s32 wmId    = LOWORD(wParam);

	static HKL KEYBOARD_INPUT_HKL = 0;
	static u32 KEYBOARD_INPUT_CODEPAGE = 1252;

	kkEvent ev;
	ev.type = kkEventType::System;

	if( message == WM_NCCREATE )
	{
		pD = static_cast<kkWindowWin32*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if( !SetWindowLongPtr(hWnd, -21, reinterpret_cast<LONG_PTR>(pD)) )
		{
			if( GetLastError() != 0 )
			{
				return FALSE;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	else
	{
		pD = reinterpret_cast<kkWindowWin32*>(GetWindowLongPtr(hWnd, -21));
	}

	if( pD )
	{
		if( pD->f_Win32_WndProcHandler )
		{
			if( pD->f_Win32_WndProcHandler(hWnd, message, wParam, lParam) )
				return true;
		}
	}

	switch( message )
	{
	case WM_GETMINMAXINFO:
	{
		if(pD)
		{
			if( pD->m_style & kk::window::style::size_limit )
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize.x = pD->m_creation_size.x;
				lpMMI->ptMinTrackSize.y = pD->m_creation_size.y;
			}
		}
	}break;
	case WM_ERASEBKGND:
		if( pD )
		{
			if( pD->f_onMove && pD->m_main_system->isRun() )
			{
				pD->f_onMove( pD );
			}
		}
		return 1;
	case WM_SHOWWINDOW:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::Show;
		ev.windowEvent.window  = pD;
	break;
	case WM_MOVE:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::Move;
		ev.windowEvent.window  = pD;

		GetWindowRect( hWnd, &rc );

		if( pD )
		{
			pD->m_window_rect.set( rc.left, rc.top, rc.right, rc.bottom );
			if( pD->f_onMove && pD->m_main_system->isRun() )
			{
				pD->f_onMove( pD );
			}
		}
	break;
	case WM_KILLFOCUS:
	{
		if(pD)
		{
			pD->m_focus = false;
		}
	}break;
	case WM_SETFOCUS:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::FocusSet;
		ev.windowEvent.window  = pD;
		if(pD)
		{
			pD->m_focus = true;
		}
		break;
	case WM_ACTIVATEAPP:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::Activate;
		ev.windowEvent.window  = pD;
		break;
	case WM_PAINT:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::Paint;
		ev.windowEvent.window  = pD;
		if( pD )
		{
			if( pD->f_onPaint && pD->m_main_system->isRun() )
			{
				pD->f_onPaint( pD );
			}
		}
		break;

	case WM_SIZE:
	{
		ev.type   = kkEventType::Window;
		ev.windowEvent.window  = pD;
		ev.windowEvent.eventID = kkEventWindowAction::Size;
		kkGetMainSystem()->addEvent( ev );

		switch( wmId )
		{
		case SIZE_MAXIMIZED:
			ev.windowEvent.eventID = kkEventWindowAction::Maximize;
			break;
		case SIZE_MINIMIZED:
			ev.windowEvent.eventID = kkEventWindowAction::Minimize;
			break;
		case SIZE_RESTORED:
			ev.windowEvent.eventID = kkEventWindowAction::Restore;
			break;
		}


		if( pD )
		{
			GetWindowRect( hWnd, &rc );
			pD->m_window_rect.set( rc.left, rc.top, rc.right, rc.bottom );

			GetClientRect( hWnd, &rc );
			pD->m_client_rect.set( rc.left, rc.top, rc.right, rc.bottom );

			if( pD->f_onSize && pD->m_main_system->isRun() )
			{
				pD->f_onSize(pD);
				kkGetMainSystem()->addEvent( ev );
				return 1;
			}
		}

		return 0;
	}break;

	case WM_SIZING:
		ev.type   = kkEventType::Window;
		ev.windowEvent.eventID = kkEventWindowAction::Sizing;
		ev.windowEvent.window  = pD;

	break;
	
	//case WM_NCHITTEST:
	//{
	//	auto result = DefWindowProc( hWnd, message, wParam, lParam );
	//	switch (result)
	//	{
	//	case HTCLOSE:
	//	case HTMAXBUTTON:
	//	case HTMINBUTTON:
	//		return 0;
	//	default:
	//		break;
	//	}
	//}
	//break;
	//case WM_NCPAINT:
	//	if( pD )
	//	{
	//		GetClientRect( hWnd, &rc );

	//		HDC hdc;
	//		hdc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
	//		
	//		SelectObject(hdc,(HBRUSH)(COLOR_MENU+1));
	//		Rectangle(hdc, 0, 0, 5, 5 );

	//		ReleaseDC(hWnd, hdc);
	//		return 0;
	//	}
	//	break;
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		///kkGetMainSystem()->getSceneSystem( nullptr )->clearScene();
		//PostQuitMessage( 0 );
		if( pD->f_onClose )
		{
			if( kkGetMainSystem()->isRun() )
				pD->f_onClose(pD);
		}
		else
		{
			kkGetMainSystem()->quit();
		}
	return 0;

	case WM_NCMOUSEMOVE:
		ev.type = kkEventType::Mouse;
		ev.mouseEvent.state = 0u;
		ev.mouseEvent.x = -1;
		ev.mouseEvent.y = -1;
		kkGetMainSystem()->addEvent( ev );
		return 0;
	case WM_MOUSEWHEEL:
		if(pD)
		{
			if( pD->m_focus )
			{
				ev.type = kkEventType::MouseWheel;
				ev.mouseEvent.state = 0u;
				ev.mouseEvent.wheel = int( (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA );
				kkGetMainSystem()->addEvent( ev );
			}
		}
		return 0;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	{
		ev.type = kkEventType::Mouse;
		ev.mouseEvent.state = 0u;

		static f32 crdX = 0.f;
		static f32 crdY = 0.f;
		if(pD)
		{
			if( pD->m_focus )
			{
				POINT point;
				GetCursorPos(&point);
				ScreenToClient(hWnd,&point);
				crdX = (float)point.x;
				crdY = (float)point.y;
			}
		}
		/*ev.mouseEvent.x = LOWORD(lParam);
		ev.mouseEvent.y = HIWORD(lParam);*/
		ev.mouseEvent.x = crdX;
		ev.mouseEvent.y = crdY;

		if( wParam & MK_LBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_LMB_DOWN;
		if( wParam & MK_RBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_RMB_DOWN;
		if( wParam & MK_MBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_MMB_DOWN;
		if( wParam & MK_XBUTTON1 )
		{
			ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_DOWN;
			switch( message )
			{
				case WM_XBUTTONUP: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_UP; break;
				case WM_XBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_DOUBLE; break;
			}
		}
		if( wParam & MK_XBUTTON2 )
		{
			ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_DOWN;
			switch( message )
			{
				case WM_XBUTTONUP: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_UP; break;
				case WM_XBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_DOUBLE; break;
			}
		}

		switch( message )
		{
		case WM_LBUTTONUP:{	
			ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_LMB_UP; 
		}break;
			case WM_RBUTTONUP:	ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_RMB_UP; break;
			case WM_MBUTTONUP:	ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_MMB_UP; break;
			case WM_LBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_LMB_DOUBLE; break;
			case WM_RBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_RMB_DOUBLE; break;
			case WM_MBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_MMB_DOUBLE; break;
		}


		kkGetMainSystem()->addEvent( ev );

		return 0;
	}break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		ev.type = kkEventType::Keyboard;
		ev.keyboardEvent.key = static_cast<kkKey>( wParam );

		if(message == WM_SYSKEYDOWN){ ev.keyboardEvent.state_is_pressed = 1; }
		if(message == WM_KEYDOWN){ev.keyboardEvent.state_is_pressed = 1; }
		if(message == WM_SYSKEYUP){  ev.keyboardEvent.state_is_relesed = 1; }
		if(message == WM_KEYUP){ ev.keyboardEvent.state_is_relesed = 1; }

		const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

		if( ev.keyboardEvent.key == kkKey::K_SHIFT )
		{ // shift -> lshift rshift
			ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255u ), MY_MAPVK_VSC_TO_VK_EX ) );
			ev.keyboardEvent.state_is_shift = TRUE;
		}
		if( ev.keyboardEvent.key == kkKey::K_CTRL )
		{ // ctrl -> lctrl rctrl
			ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
			ev.keyboardEvent.state_is_ctrl = TRUE;
			if( lParam & 0x1000000 )
			{
				ev.keyboardEvent.key = static_cast<kkKey>( 163 );
			}
		}

		

		if( ev.keyboardEvent.key == kkKey::K_ALT )
		{ // alt -> lalt ralt
			ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
			if( lParam & 0x1000000 )
			{
				ev.keyboardEvent.key = static_cast<kkKey>(165);
			}
			//printf("alt = %i\n",(int)ev.keyboardEvent.key);
			ev.keyboardEvent.state_is_alt = TRUE;
		}

		

		u8 keys[ 256u ];
		GetKeyboardState( keys );
		WORD chars[ 2 ];

		if( ToAsciiEx( (UINT)wParam, HIWORD(lParam), keys, chars, 0, KEYBOARD_INPUT_HKL ) == 1 )
		{
			WORD uChar;
			MultiByteToWideChar( KEYBOARD_INPUT_CODEPAGE, MB_PRECOMPOSED, (LPCSTR)chars,
				sizeof(chars), (WCHAR*)&uChar, 1 );
			ev.keyboardEvent.character = uChar;
		}
		else 
		{
			ev.keyboardEvent.character = 0;
		}

		kkGetMainSystem()->addEvent( ev );

		if( message == WM_SYSKEYDOWN || message == WM_SYSKEYUP )
		{
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
		else
		{
			return 0;
		}
	}break;

	case WM_INPUTLANGCHANGE:
		KEYBOARD_INPUT_HKL = GetKeyboardLayout( 0 );
		KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );
	return 0;

	case WM_SYSCOMMAND:
		if( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
			( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
			( wParam & 0xFFF0 ) == SC_KEYMENU
		)
		{
			return 0;
		}
	}


	kkGetMainSystem()->addEvent( ev );

	return DefWindowProc( hWnd, message, wParam, lParam );
}



#endif

