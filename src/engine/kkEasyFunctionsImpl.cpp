// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "Classes/Patterns/kkSingleton.h"

#include <mutex>

#include "Memory/kkMemorySystemImpl.h"
#include "MainSystem/kkMainSystemImpl.h"

extern "C"
{
	KK_API int KK_C_DECL kkMessageBox(const char16_t* msg, const char16_t* title, int flags)
	{
		return kkSingleton<kkMainSystemImpl>::s_instance->messageBox(msg,title,flags);
	}

}

