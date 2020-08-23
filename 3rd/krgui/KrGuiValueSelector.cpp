#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::addValueSelector( float * value, const Vec2f& _size,
			bool isHorizontal, float speed, Gui::Style* style,
			const Vec4f& rounding )
{
	assert(value);

	auto old_value = *value;

	if( m_IsShift ) speed *= 10.f;
	if( m_IsAlt ) speed *= 0.01f;

	_checkStyle(&style);
	_newId();
	
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


	if( !m_blockInputGlobal )
	{
		_updateMouseInput(mouseButton::LMB);
	}
	if( m_pressedItemIdLMB == m_uniqueIdCounter )// if pressed
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

	char textBuf[32];
	sprintf(textBuf, "%f", *value);
	setNextItemIgnoreInput();
	
	m_drawPointer = front_position;
	
	addText(textBuf);

	m_drawPointer = back_position;

	return old_value != *value;
}

