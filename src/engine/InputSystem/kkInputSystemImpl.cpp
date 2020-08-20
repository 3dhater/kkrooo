// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "kkInputSystemImpl.h"

kkInputSystemImpl::kkInputSystemImpl()
{
}

kkInputSystemImpl::~kkInputSystemImpl()
{
}

bool kkInputSystemImpl::isKeyDown( kkKey key )
{
	return m_keys_down[ (u32)key ];
}

const v2_s16& kkInputSystemImpl::getCursorPosition()
{
	return m_cursor_position;
}

bool kkInputSystemImpl::isLMBDown()
{
	return m_mouse_state.isLeftButtonDown();
}

bool kkInputSystemImpl::isRMBDown()
{
	return m_mouse_state.isRightButtonDown();
}

bool kkInputSystemImpl::isMMBDown()
{
	return m_mouse_state.isMiddleButtonDown();
}

void kkInputSystemImpl::reset()
{
	m_mouse_state.state = 0u;
	
	/// resete all other things here
}

void kkInputSystemImpl::setKeyboardState( kkEventKeyboard * e )
{
	if( !e )
		return;

	u32 i = (u32)e->key;
	
	if( i > 256 )
		return;

	m_keys_down[ i ] = e->state_is_pressed;
}

void kkInputSystemImpl::setMouseState( kkEventMouse * e )
{
	m_mouse_state = *e;
	m_cursor_position.x = e->x;
	m_cursor_position.y = e->y;
}