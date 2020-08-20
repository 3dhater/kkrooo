// SPDX-License-Identifier: GPL-3.0-only
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
#define KK_PRINT_FAILED	printf( "Failed [%s][%s][%i]:(\n", KK_FILE, KK_FUNCTION, KK_LINE )


#ifdef KK_DEBUG
#define KK_ASSERT(expr) if(!(expr)){ \
						KK_PRINT_FAILED; \
						KK_MSGBOX;\
						KK_DEBUGBREAK;\
						}
#else
#define KK_ASSERT(expr)
#endif

#endif