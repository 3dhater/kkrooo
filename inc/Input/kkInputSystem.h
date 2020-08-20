// SPDX-License-Identifier: GPL-3.0-only
#ifndef __GT_INPUT_SYSTEM_H__
#define __GT_INPUT_SYSTEM_H__

#include "kkInputKey.h"
#include "Classes/Math/kkVector4.h"


class kkInputSystem
{
public:
	//virtual kkInputController create_input_contoller( const KK_GUID& plugin ) = 0;
	virtual bool isKeyDown( kkKey key ) = 0;
	virtual bool isLMBDown() = 0;
	virtual bool isRMBDown() = 0;
	virtual bool isMMBDown() = 0;
	virtual const v2_s16& getCursorPosition() = 0;
};

#endif
