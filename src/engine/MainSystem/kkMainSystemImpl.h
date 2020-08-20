#ifndef __KK_MAIN_SYSTEM_IMPL_H__
#define __KK_MAIN_SYSTEM_IMPL_H__

#include <chrono>

#include <zstd.h>

#include "MainSystem/kkMainSystem.h"
#include "Events/kkEventSystem.h"
#include "SceneSystem/kkSceneSystem.h"

class kkInputSystemImpl;
class kkEventSystemImpl;
class kkSceneSystemImpl;

class kkMainSystemImpl : public kkMainSystem
{
	kkInputSystemImpl * m_input_system = nullptr;
	kkEventSystemImpl * m_event_system = nullptr;
	kkSceneSystemImpl * m_scene_system = nullptr;

	bool                m_is_run = true;

	bool				m_use_timer = false;
	f64   		m_timer = 0.;
	f64			m_time  = 0.;
	f32			m_delta_time  = 0.f;
	std::chrono::high_resolution_clock::time_point m_time_then;
	std::chrono::high_resolution_clock::time_point m_time_now;
	void updateTimer();
	
	void _osDependUpdates();
#ifdef KK_PLATFORM_WINDOWS
	void _updateWin32();
#endif

	kkMainSystem_printFunction_t f_print_function = nullptr;


	ZSTD_CCtx* m_cctx = nullptr;

public:

	kkMainSystemImpl(kkEventConsumer* ec);
	virtual ~kkMainSystemImpl();

	kkWindow* createWindow( u32 style, const v4i& rect, u32 state = 0, kkWindow* parent = nullptr, s32 id = -1 );
	kkGraphicsSystem * createGraphicsSystem( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth );
	
	void addEvent( const kkEvent& event );
	bool update();
	bool isRun();
	void quit();

	u64 getTime();
	void  setTimer( u32 milliseconds );
	f32* getDeltaTime();

	s32 messageBox(const char16_t* message, const char16_t* title, s32 flags);

	kkInputSystem* getInputSystem();
	kkEventSystem* getEventSystem();
	kkSceneSystem* getSceneSystem();

	void setPrintFunction(kkMainSystem_printFunction_t);
	void printError(const char16_t*);

	u8* compressData( u8* in_data, u32 in_data_size, u32& out_data_size );
	u8* decompressData( u8* in_data, u32 in_data_size, u32& out_data_size );
};

#endif