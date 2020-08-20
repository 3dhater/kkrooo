// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_INFO_DLL_H__
#define __KKROO_INFO_DLL_H__

#if defined(KK_PLATFORM_WINDOWS)
#	define KK_LIBRARY_HANDLE HMODULE
#	define KK_LOAD_LIBRARY(x) LoadLibrary(x)
#	define KK_LOAD_FUNCTION GetProcAddress
#	define KK_FREE_LIBRARY FreeLibrary
#include <windows.h>
#elif defined(KK_PLATFORM_LINUX) || defined(KK_PLATFORM_ANDROID)
#	define KK_LIBRARY_HANDLE void*
#	define KK_LOAD_LIBRARY(x) dlopen((const char*)x,(RTLD_NOW|RTLD_GLOBAL))
#	define KK_LOAD_FUNCTION dlsym
#	define KK_FREE_LIBRARY dlclose
#include <dlfcn.h>
#endif

#endif