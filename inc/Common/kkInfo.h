// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_INFO_H__
#define __KKROO_INFO_H__

#define KK_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
            ((u32)(u8)(ch0)|((u32)(u8)(ch1)<<8)|\
            ((u32)(u8)(ch2)<<16)|((u32)(u8)(ch3)<<24))

#if defined _WIN32 || defined __CYGWIN__
#define KK_PLATFORM_WINDOWS
#else
#error Only for Windows
#endif

#ifdef KK_PLATFORM_WINDOWS
#if defined _WIN64 || defined __x86_64__
#define KK_BIT_64
#endif
#else
#endif


#ifdef KK_PLATFORM_WINDOWS

#ifdef _MSC_VER
#define KK_C_DECL _cdecl
#ifdef KK_EXPORTS
#define KK_API _declspec(dllexport)
#else
#define KK_API _declspec(dllimport)
#endif
#else
#define KK_C_DECL
#define KK_API
#endif

#else
#define KK_C_DECL
#define KK_API
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define KK_DEBUG
#endif


#ifdef KK_PLATFORM_WINDOWS
#define KK_FILE __FILE__
#define KK_FUNCTION __FUNCTION__
#define KK_LINE __LINE__
#ifdef _MSC_VER
#define KK_FORCE_INLINE __forceinline
#else
#define KK_FORCE_INLINE inline
#endif
#endif


#define BIT(x)0x1<<(x)

#endif