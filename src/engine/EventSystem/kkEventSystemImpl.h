// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_EVENT_SYSTEM_IMPL_H__
#define __KK_EVENT_SYSTEM_IMPL_H__

#include "Events/kkEventSystem.h"

constexpr u32 kk_event_max = 128u;

class kkInputSystemImpl;

class kkEventSystemImpl : public kkEventSystem
{
	kkEventConsumer* m_user_consumer = nullptr;
	u32		     m_num_of_events  = 0;
	u32			 m_current_event = 0;

	kkEvent			m_events[ kk_event_max ];

	kkInputSystemImpl * m_input = nullptr;
public:

	kkEventSystemImpl( kkInputSystemImpl * is, kkEventConsumer * ec );
	virtual ~kkEventSystemImpl();
	void getKeyboardAndMouseStates();
	void runEventLoop();
	kkEventConsumer* getEventConsumer();

	void clearEvents();
	void addEvent( const kkEvent& event );
	bool pollEvent( kkEvent& event );
	void setEventConsumer( kkEventConsumer * e );
};


#endif