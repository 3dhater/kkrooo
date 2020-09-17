#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::addRangeSlider( float minimum, float maximum, float * value, const Vec2f& _size,
			bool isHorizontal, float speed, Gui::Style* style,
			const Vec4f& rounding )
{
	assert(value);

	auto old_value = *value;

	if( minimum > maximum )
		minimum = maximum;

	if( *value > maximum ) *value = maximum;
	if( *value < minimum ) *value = minimum;

	if( m_IsShift ) speed *= 10.f;
	if( m_IsAlt ) speed *= 0.01f;

	_checkStyle(&style);
	_newId();
	
	Vec2f size = _size;
	_checkSize(&size);

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


	// we need to know...minimum value per pixel
	float range = maximum - minimum;
	float rangeStepX = range / size.x; // 1(range) / 100(size) = 0.01 per pixel ... depends on `speed`
	float rangeStepY = range / size.y;

	if( !m_blockInputGlobal )
	{
		_updateMouseInput(mouseButton::LMB);
	}
	if( m_pressedItemIdLMB == m_uniqueIdCounter )// if pressed
	{
		if( isHorizontal )
		{
			*value += m_mouseDelta.x * rangeStepX * speed;
		}
		else
		{
			*value -= m_mouseDelta.y * rangeStepY * speed;
		}
		if( *value > maximum ) *value = maximum;
		if( *value < minimum ) *value = minimum;
	}

	auto rangeClipRect = m_currentClipRect;
	auto v = *value;
	if( v == 0.f ) v += 0.001f;
	if( isHorizontal )
	{
		float clipSize = m_currentClipRect.z - m_currentClipRect.x;
		rangeClipRect.z = m_currentClipRect.x + (clipSize * (v/range));
	}
	else
	{
		float clipSize = m_currentClipRect.w - m_currentClipRect.y;
		rangeClipRect.y = m_currentClipRect.w - (clipSize * (v/range));
	}

	m_firstColor  = style->rangeSliderFgColor;
	m_secondColor = style->rangeSliderFgColor;
	_addRectangle(rangeClipRect, buildRect, rounding);

	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);

	return old_value != *value;
}

bool Gui::GuiSystem::addRangeSliderInt( int minimum, int maximum, int * value, const Vec2f& _size,
			bool isHorizontal, float speed, Gui::Style* style, const Vec4f& rounding )
{
	assert(value);
	auto old_value = *value;

	if( minimum > maximum )
		minimum = maximum;

	if( *value > maximum ) *value = maximum;
	if( *value < minimum ) *value = minimum;

	if( m_IsShift ) speed *= 10.f;
	if( m_IsAlt ) speed *= 0.01f;

	_checkStyle(&style);
	_newId();
	
	Vec2f size = _size;
	_checkSize(&size);

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


	// we need to know...minimum value per pixel
	float range = (float)maximum - (float)minimum;
	float rangeStepX = range / size.x; // 1(range) / 100(size) = 0.01 per pixel ... depends on `speed`
	float rangeStepY = range / size.y;

	if( !m_blockInputGlobal )
	{
		_updateMouseInput(mouseButton::LMB);
	}
	if( m_pressedItemIdLMB == m_uniqueIdCounter )// if pressed
	{
		if( isHorizontal )
		{
			*value += (int)ceil(m_mouseDelta.x * rangeStepX * speed);
		}
		else
		{
			*value -= (int)ceil(m_mouseDelta.y * rangeStepY * speed);
		}
		if( *value > maximum ) *value = maximum;
		if( *value < minimum ) *value = minimum;
	}

	auto rangeClipRect = m_currentClipRect;
	auto v = *value;
	if( v == 0 ) ++v;
	if( isHorizontal )
	{
		float clipSize = m_currentClipRect.z - m_currentClipRect.x;
		rangeClipRect.z = m_currentClipRect.x + (clipSize * (v/range));
	}
	else
	{
		float clipSize = m_currentClipRect.w - m_currentClipRect.y;
		rangeClipRect.y = m_currentClipRect.w - (clipSize * (v/range));
	}

	m_firstColor  = style->rangeSliderFgColor;
	m_secondColor = style->rangeSliderFgColor;
	_addRectangle(rangeClipRect, buildRect, rounding);

	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);

	return old_value != *value;
}