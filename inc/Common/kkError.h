#ifndef __KKROO_ERROR_H__
#define __KKROO_ERROR_H__

#include "Common/kkEasyFunctions.h"

#ifdef KK_PLATFORM_WINDOWS
#ifdef KK_DEBUG
#define KK_DEBUGBREAK __debugbreak()
#else
#define KK_DEBUGBREAK
#endif
#else
#define KK_MSGBOX 
#endif

#include <cstdio>
#define KK_PRINT_FAILED	kkLogWriteError( "Failed [%s][%s][%i]:(\n", KK_FILE, KK_FUNCTION, KK_LINE )


#endif