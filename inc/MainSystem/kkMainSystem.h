// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_MAINSYSTEM_H__
#define __KKROO_MAINSYSTEM_H__

class kkInputSystem;
class kkEventSystem;
class kkGraphicsSystem;
class kkWindow;
struct kkEvent;

namespace kk
{
	namespace window
	{
		namespace style
		{
			constexpr u32 popup    = 1;
			constexpr u32 maximize = 2;
			constexpr u32 resize   = 4;
			constexpr u32 center   = 8;
			constexpr u32 on_top   = 16;
			constexpr u32 modal    = 32;
		}
		namespace state
		{
			constexpr u32 maximized   = 1;
			constexpr u32 minimized   = 2;
			constexpr u32 hide        = 4;
		}
	}
	
	namespace threads
	{
		constexpr u32 run          = 1;
		constexpr u32 finish       = 2;
		constexpr u32 need_to_stop = 4;
	}
}

#include <Classes/Math/kkVector4.h>

using kkMainSystem_printFunction_t = void(*)(const char16_t*);
class kkMainSystem
{
public:

	kkMainSystem(){};
	virtual ~kkMainSystem(){};


	virtual kkWindow* createWindow( u32 style, const v4i& rect, u32 state = 0, kkWindow* parent = nullptr, s32 id = -1 ) = 0;
	virtual kkGraphicsSystem * createGraphicsSystem( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth ) = 0;

	virtual void addEvent( const kkEvent& event ) = 0;

	virtual bool update() = 0;
	virtual bool isRun() = 0;
	virtual void quit() = 0;

	virtual u64 getTime() = 0;
	virtual void  setTimer( u32 milliseconds ) = 0;
	virtual f32* getDeltaTime() = 0;

	virtual s32 messageBox(const char16_t* message, const char16_t* title, s32 flags) = 0;
	
	virtual kkInputSystem* getInputSystem() = 0;
	virtual kkEventSystem* getEventSystem() = 0;

	virtual void setPrintFunction(kkMainSystem_printFunction_t) = 0;
	virtual void printError(const char16_t*) = 0;


	virtual u8* compressData( u8* in_data, u32 in_data_size, u32& out_data_size ) = 0;
	virtual u8* decompressData( u8* in_data, u32 in_data_size, u32& out_data_size ) = 0;
};



#endif