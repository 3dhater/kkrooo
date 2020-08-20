// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "kkMainSystemImpl.h"


#include <Windows.h>

void kkMainSystemImpl::_updateWin32()
{
	MSG msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		GetMessage( &msg, NULL, 0, 0 );
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
}