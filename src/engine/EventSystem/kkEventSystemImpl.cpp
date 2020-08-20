// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "Input/kkInputSystem.h"
#include "Events/kkEvent.h"

#include "kkEventSystemImpl.h"

#include "../InputSystem/kkInputSystemImpl.h"

#include <cstring>

kkEventSystemImpl::kkEventSystemImpl(kkInputSystemImpl * is, kkEventConsumer * ec)
	:
	m_input(is),
	m_user_consumer(ec)
{
	memset( m_events, 0, kk_event_max * sizeof( kkEvent ) );
}

kkEventSystemImpl::~kkEventSystemImpl()
{
}

void kkEventSystemImpl::clearEvents()
{
	m_num_of_events = 0;
}

void kkEventSystemImpl::getKeyboardAndMouseStates()
{
	u32 cur = m_current_event;
	while( true )
	{
		if( m_events[ cur ].type == kkEventType::None )
		{
			break;
		}

		switch( m_events[ cur ].type )
		{
			case kkEventType::Keyboard:{
				//m_keysDown[ (u32)m_events[ cur ].keyboardEvent.key ] = m_events[ cur ].keyboardEvent.state.bits.b0;
				m_input->setKeyboardState( &m_events[ cur ].keyboardEvent );
			}break;
			case kkEventType::Mouse:{
				m_input->setMouseState( &m_events[ cur ].mouseEvent );
			}break;
			case kkEventType::None:
			case kkEventType::Joystick:
			case kkEventType::GUI:
			case kkEventType::Window:
			case kkEventType::System:
			break;
		}
		++cur;
	}
}

bool kkEventSystemImpl::pollEvent( kkEvent& event )
{
	if( m_events[ m_current_event ].type == kkEventType::None )
	{
		m_num_of_events = 0;
		return false;
	}

	event = m_events[ m_current_event ];
	m_events[ m_current_event ].type = kkEventType::None;
	++m_current_event;

	if( m_current_event == m_num_of_events )
	{
		m_current_event = 0;
	}

	return true;
}

void kkEventSystemImpl::addEvent( const kkEvent& event )
{
	if( m_num_of_events < kk_event_max )
	{
		m_events[ m_num_of_events ] = event;
		m_num_of_events++;
	}
}

void kkEventSystemImpl::runEventLoop()
{
	kkEvent system_event;
	system_event.type = kkEventType::System;
	system_event.systemEvent.action = kkEventSystemAction::BeginEventLoop;

	if( m_user_consumer )
	{
		m_user_consumer->processEvent( system_event );
	}

	while( true )
	{
		if( m_events[ m_current_event ].type == kkEventType::None )
		{
			break;
		}

		if( m_user_consumer )
		{
			m_user_consumer->processEvent( m_events[ m_current_event ] );
		}

		m_events[ m_current_event ].type = kkEventType::None;


		++m_current_event;

		if( m_current_event == m_num_of_events )
		{
			m_current_event = 0;
		}
	}

	system_event.systemEvent.action = kkEventSystemAction::EndEventLoop;
	if( m_user_consumer )
	{
		m_user_consumer->processEvent( system_event );
	}
}

kkEventConsumer* kkEventSystemImpl::getEventConsumer()
{
	return m_user_consumer;
}

void kkEventSystemImpl::setEventConsumer( kkEventConsumer * e )
{
	m_user_consumer = e;
}