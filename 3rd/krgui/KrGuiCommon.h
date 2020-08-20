#ifndef __KK_KRGUI_COMMON_H__
#define __KK_KRGUI_COMMON_H__

#if defined(_WIN32) || defined(WIN32)
#define KRGUI_PLATFORM_WINDOWS
#define KRGUI_FILE __FILE__
#define KRGUI_FUNCTION __FUNCTION__
#define KRGUI_LINE __LINE__
#else
#error Implement me. please.
#endif

#include <cstdio>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <map>
#include <fstream>
#include <filesystem>

#endif