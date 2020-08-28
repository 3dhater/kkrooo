#include "KrGui.h"

using namespace Kr;

char16_t g_textBufForTextinput[32];
bool valueSelector_inputCallback(char16_t ch)
{
	switch (ch)
	{
	case u'0':
	case u'1':
	case u'2':
	case u'3':
	case u'4':
	case u'5':
	case u'6':
	case u'7':
	case u'8':
	case u'9':
	case u'-':
	case u'.':
	return true;
	default:
		break;
	}
	return false;
};

bool Gui::GuiSystem::addValueSelectorLimit( float minim, float maxim, float * value, const Vec2f& _size,
	bool isHorizontal,
	float speed, Style* style,
	const Vec4f& rounding )
{
	if(*value < minim) *value = minim;
	if(*value > maxim) *value = maxim;

	auto old_value = *value;

	if( m_IsShift ) speed *= 10.f;
	if( m_IsAlt ) speed *= 0.01f;

	_checkStyle(&style);
	_newId();
	
	static bool input_mode = false;
	static int input_mode_elementID = -1;
	
	Vec2f size = _size;
	_checkSize(&size);

	auto front_position = m_drawPointer;

	auto end_x_position = m_drawPointer.x + size.x;
	_checkNewLine(end_x_position);
	_checkItemHeight(size.y);

	auto position = m_drawPointer;
	if( m_currentNodePosition )
	{
		position.x = m_currentNodePosition->x + m_currentNodeContentOffset.x;
		position.y = m_currentNodePosition->y + m_currentNodeContentOffset.y;
		position.x += m_currentNodeEditor->m_eyePosition.x;
		position.y += m_currentNodeEditor->m_eyePosition.y;
		position.x *=  m_guiZoom;
		position.y *=  m_guiZoom;
		auto centerOffset = m_currentNodeEditor->m_center;
		position.x += centerOffset.x;
		position.y += centerOffset.y;
	}
	_setCurrentClipRect(position, size);

	/*if( !m_currentNodePosition )
	{
		m_currentClipRect *= m_guiZoom;
	}*/

	auto buildRect = m_currentClipRect;

	_checkParentClipRect();

	m_firstColor  = style->rangeSliderBgColor;
	m_secondColor = style->rangeSliderBgColor;
	_addRectangle(m_currentClipRect, buildRect, rounding);

	char textBuf[32];
	sprintf(textBuf, "%f", *value);

	if( !m_blockInputGlobal )
	{
		_updateMouseInput(mouseButton::LMB);

		auto inRect = _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect );
		if( (inRect && m_mouseIsRMB_up) || (inRect && m_IsLMBDouble) )
		{
			input_mode = true;
			input_mode_elementID = m_uniqueIdCounter;

			for( int i = 0; i < 32; ++i )
			{
				g_textBufForTextinput[i] = (char16_t)textBuf[i];
			}
		}
	}

	bool inputmode = false;
	if( input_mode && m_uniqueIdCounter == input_mode_elementID )
	{
		inputmode = true;
	}

	if( m_pressedItemIdLMB == m_uniqueIdCounter && !inputmode )// if pressed
	{
		if( isHorizontal )
		{
			*value += m_mouseDelta.x * speed;
		}
		else
		{
			*value -= m_mouseDelta.y * speed;
		}
	}

	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);
	
	

	auto back_position = m_drawPointer;
	m_drawPointer = front_position;
	if(!inputmode)
	{
		setNextItemIgnoreInput();
		addText(textBuf);
	}
	else
	{
		if( addTextInputPopup(size, g_textBufForTextinput, 32, 18, valueSelector_inputCallback, 0) )
		{
			input_mode = false;
			m_lastKeyboardInputItemIdExit = m_uniqueIdCounter;
			//printf("F=%f\n", _internal::string_to_float(g_textBufForTextinput, _internal::stl_len(g_textBufForTextinput)) );
		}
		else
		{
			m_lastKeyboardInputItemId = m_uniqueIdCounter;
		}
		if(m_character || m_IsEnter || m_IsDelete || m_IsBackspace)
			*value = _internal::string_to_float(g_textBufForTextinput, _internal::stl_len(g_textBufForTextinput));
	}
	m_drawPointer = back_position;

	if(*value < minim) *value = minim;
	if(*value > maxim) *value = maxim;

	return old_value != *value;
}

bool Gui::GuiSystem::addValueSelector( float * value, const Vec2f& _size,
			bool isHorizontal, float speed, Gui::Style* style,
			const Vec4f& rounding )
{
	assert(value);
	return addValueSelectorLimit(-FLT_MAX, FLT_MAX, value, _size,
			isHorizontal, speed, style,
			rounding);
}

