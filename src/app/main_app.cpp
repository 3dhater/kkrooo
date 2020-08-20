// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "Application.h"
#include <cstdio>


void move_console(int x, int y)
{
#ifdef KK_PLATFORM_WINDOWS
	auto hwnd = GetConsoleWindow();
	MoveWindow( hwnd, x, y, 800, 600, 1 );
//	ShowWindow(hwnd,SW_HIDE);
#endif
}


int main()
{
	move_console(0,0);

	try
	{
		Application app;

		app.init();
		app.run();
	}
	catch( std::runtime_error& e )
	{
		printf("%s\n",e.what());
#ifdef KK_PLATFORM_WINDOWS
		MessageBoxA(0,e.what(),"Exception!",MB_OK | MB_ICONERROR);
#endif
	}
	catch( std::exception& e )
	{
		printf("%s\n",e.what());
#ifdef KK_PLATFORM_WINDOWS
		MessageBoxA(0,e.what(),"Exception!",MB_OK | MB_ICONERROR);
#endif
	}
	catch(...)
	{
#ifdef KK_PLATFORM_WINDOWS
		MessageBoxA(0,"Unknown exception","Exception!",MB_OK | MB_ICONERROR);
#endif
	}
	

	return 0;
}