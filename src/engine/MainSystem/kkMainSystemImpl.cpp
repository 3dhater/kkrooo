// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"
#include "MainSystem/kkMainSystem.h"
#include "Events/kkEvent.h"
#include "kkMainSystemImpl.h"
#include "../InputSystem/kkInputSystemImpl.h"
#include "../SceneSystem/kkSceneSystemImpl.h"
#include "../GraphicsSystem/kkGraphicsSystemImpl.h"
#include "../EventSystem/kkEventSystemImpl.h"
#include "../Window/kkWindowWin32.h"


kkMainSystemImpl::kkMainSystemImpl(kkEventConsumer* ec)
{
	//m_input_system = kkCreate(kkInputSystemImpl());
	m_input_system = kkCreate<kkInputSystemImpl>();

	//m_event_system = kkCreate(kkEventSystemImpl(m_input_system,ec));
	m_event_system = kkCreate<kkEventSystemImpl>(m_input_system,ec);

	m_scene_system = kkCreate<kkSceneSystemImpl>();

	m_time_then = std::chrono::high_resolution_clock::now();

	m_cctx = ZSTD_createCCtx();
}


kkMainSystemImpl::~kkMainSystemImpl()
{
	ZSTD_freeCCtx(m_cctx);

	kkDestroy(m_event_system);
	kkDestroy(m_input_system);
}


kkGraphicsSystem * kkMainSystemImpl::createGraphicsSystem( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth )
{
	//kkGraphicsSystemImpl * gs = kkObjectCreator<kkGraphicsSystemImpl>::create( kkGraphicsSystemImpl() );
	//kkGraphicsSystemImpl * gs = kkCreate( kkGraphicsSystemImpl() );
	kkGraphicsSystemImpl * gs = kkCreate<kkGraphicsSystemImpl>();
	if( gs )
	{
		if( !gs->initGS( output_window, back_buffer_size, color_depth ) )
		{
			KK_PRINT_FAILED;
			kkDestroy( gs );
		}
	}
	return gs;
}


kkWindow* kkMainSystemImpl::createWindow( u32 style, const v4i& rect, u32 state, kkWindow* parent, s32 id )
{
	//kkWindowWin32 * w = kkCreate( kkWindowWin32() );
	kkWindowWin32 * w = kkCreate<kkWindowWin32>();
	if( w )
	{
		static u32 window_count = 0;
		w->setID(id);
		if( !w->init(window_count++, style, rect, parent, state ) )
		{
			KK_PRINT_FAILED;
			kkDestroy( w );
		}
	}
	return w;
}

void kkMainSystemImpl::addEvent( const kkEvent& event )
{
	m_event_system->addEvent( event );
}

void kkMainSystemImpl::quit()
{
	m_is_run = false;
}

bool kkMainSystemImpl::isRun()
{
	return m_is_run;
}

bool kkMainSystemImpl::update()
{
	_osDependUpdates();

	m_event_system->getKeyboardAndMouseStates();
	//m_GUISystem->update();
	if( this->m_event_system->getEventConsumer() )
	{
		this->m_event_system->runEventLoop();
		this->m_event_system->clearEvents();
	}
	else
	{
	}
	updateTimer();

	return m_is_run;
}

void kkMainSystemImpl::updateTimer()
{
	static u64 t1 = 0u;
	u64 t2 = this->getTime();
	f32 m_tick = f32(t2 - t1);
	t1 = t2;

	m_delta_time = m_tick / 1000.f;

	if( m_use_timer )
	{
		m_time += m_tick;

		if( m_time > m_timer )
		{ // end
			m_use_timer = false;

			kkEvent e;
			e.type = kkEventType::System;
			e.systemEvent.action = kkEventSystemAction::Timer;
			addEvent( e );

			m_time = 0u;
		}
	}
}

void kkMainSystemImpl::setTimer( u32 milliseconds )
{
	if( !m_use_timer )
	{
		m_use_timer = true;
		m_timer = milliseconds;
	}
}


u64 kkMainSystemImpl::getTime()
{
	static bool isInit = false;
	static u64 baseTime;
	if( !isInit )
	{
		auto now = std::chrono::high_resolution_clock::now();
		baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		isInit = true;
	}
	auto now = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	return ms - baseTime;
}

f32* kkMainSystemImpl::getDeltaTime()
{
	return &m_delta_time;
}

void kkMainSystemImpl::_osDependUpdates()
{
#ifdef KK_PLATFORM_WINDOWS
	_updateWin32();
#endif
}

s32 kkMainSystemImpl::messageBox(const char16_t* message, const char16_t* title, s32 flags)
{
	s32 res = 0;
#ifdef KK_PLATFORM_WINDOWS
	res = MessageBoxW(NULL,(wchar_t*)message,(wchar_t*)title,flags);
#endif

	return 0;
}

kkInputSystem* kkMainSystemImpl::getInputSystem()
{
	return m_input_system;
}

kkEventSystem* kkMainSystemImpl::getEventSystem()
{
	return m_event_system;
}

kkSceneSystem* kkMainSystemImpl::getSceneSystem()
{
	return m_scene_system;
}

void kkMainSystemImpl::setPrintFunction(kkMainSystem_printFunction_t f)
{
	f_print_function = f;
}

void kkMainSystemImpl::printError(const char16_t* s)
{
	if( f_print_function )
		f_print_function(s);
	else
	{
		fprintf(stderr,"%s\n",kkStringA(s).data());
	}
}


u8* kkMainSystemImpl::compressData( u8* in_data, u32 in_data_size, u32& out_data_size )
{

	u8* out_data = (u8*)kkMemory::allocate(in_data_size);
	if( !out_data )
	{
		KK_PRINT_FAILED;
		return out_data;
	}

	auto compressBound = ZSTD_compressBound(in_data_size);

	size_t const cSize = ZSTD_compressCCtx( m_cctx, out_data, compressBound, in_data, in_data_size, 1);
    if( ZSTD_isError(cSize) )
	{
		kkMemory::free(out_data);
		return out_data;
	}

	kkMemory::reallocate(out_data,cSize);

	out_data_size = (u32)cSize;

	return out_data;
}

u8* kkMainSystemImpl::decompressData( u8* in_data, u32 in_data_size, u32& out_data_size )
{
	unsigned long long const rSize = ZSTD_getFrameContentSize(in_data, in_data_size);
	u8* out_data = (u8*)kkMemory::allocate(rSize);
	if( !out_data )
	{
		KK_PRINT_FAILED;
		return out_data;
	}

	size_t const dSize = ZSTD_decompress(out_data, rSize, in_data, in_data_size);
	out_data_size = (u32)dSize;
	return out_data;
}