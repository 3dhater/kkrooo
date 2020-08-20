// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_INPUT_SYSTEM_IMPL_H__
#define __KK_INPUT_SYSTEM_IMPL_H__

#include "Input/kkInputSystem.h"

#include "Classes/Math/kkVector4.h"
#include "Events/kkEvent.h"

class kkInputSystemImpl : public kkInputSystem
{
	s8           m_keys_down[ 256 ];
	v2_s16        m_cursor_position;
	kkEventMouse  m_mouse_state;
public:

	kkInputSystemImpl();
	~kkInputSystemImpl();

	//gtPtr<gtInputController> createInputContoller( const GT_GUID& plugin );
	
	bool isKeyDown( kkKey key );
	const v2_s16& getCursorPosition();
	bool isLMBDown();
	bool isRMBDown();
	bool isMMBDown();

	void reset();
	void setKeyboardState( kkEventKeyboard * e );
	void setMouseState( kkEventMouse * e );
};

#endif