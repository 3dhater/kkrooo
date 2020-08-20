#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::addButtonSymbol( 
	char16_t iconSymbol, 
	Style* style, 
	const Vec2f& size, 
	bool enabled , 
	bool textToCenter ,
	const Vec4f& rounding ){
	char16_t text[2];
	text[0] = iconSymbol;
	text[1] = 0;
	return addButton(text, style, size, enabled, textToCenter, rounding);
}

bool Gui::GuiSystem::addButton( 
	const char16_t* text, 
	Gui::Style* style, 
	const Vec2f& _size, 
	bool enabled, 
	bool textToCenter, 
	const Vec4f& rounding 
)
{
	auto oldDrawPointer = m_drawPointer;
	bool result = false;

	_checkStyle(&style);
	_newId();
	
	if(!enabled) m_lastDisabledItemId = m_uniqueIdCounter; // id disabled, remember ID

	Vec2f size = _size;
	_checkSize(&size);

	auto end_x_position = m_drawPointer.x + size.x;
	_checkNewLine(end_x_position);

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

	/*if( m_currentNodePosition )
	{
	}
	else
	{
		m_currentClipRect *= m_guiZoom;
	}*/

	auto buildRect = m_currentClipRect;
	_checkParentClipRect();


	// update input
	if( !m_nextItemIgnoreInput && !m_blockInputGlobal )
		_updateMouseInput(mouseButton::LMB);
	else m_nextItemIgnoreInput = false;

	// change colors
	if( m_pressedItemIdLMB == m_uniqueIdCounter && !m_blockInputGlobal )// if pressed
	{
		if( m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter  )
		{
			m_firstColor  = style->buttonPushColor1;
			m_secondColor = style->buttonPushColor2;
			if( text ) m_textColor = style->buttonTextPushColor;
		}
		else
		{
			m_firstColor  = style->buttonHoverColor1;
			m_secondColor = style->buttonHoverColor2;
			if( text ) m_textColor = style->buttonTextHoverColor;
		}
	}else 
	{
		if( m_lastDisabledItemId == m_uniqueIdCounter )
		{
			m_firstColor  = style->buttonDisabledColor1;
			m_secondColor = style->buttonDisabledColor2;
			if( text ) m_textColor = style->buttonTextDisabledColor;
		}else if( (m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter) && m_pressedItemIdLMB == 0 )
		{
			m_firstColor  = style->buttonHoverColor1;
			m_secondColor = style->buttonHoverColor2;
			if( text ) m_textColor = style->buttonTextHoverColor;
		}else 
		{
			m_firstColor  = style->buttonIdleColor1;
			m_secondColor = style->buttonIdleColor2;
			if( text ) m_textColor = style->buttonTextIdleColor;
		}
	}


	m_firstColor.w  = style->buttonBackgroundAlpha;
	m_secondColor.w = style->buttonBackgroundAlpha;	

	// the size of rectangle equal of m_currentClipRect
	_addRectangle(m_currentClipRect, buildRect, rounding);

	if( text )
	{
		Vec2f textSize;
		auto textstrlen = getTextLen(text,&textSize,style->buttonTextSpacing, style->buttonTextSpaceAddSize);

		Vec4f textRect;
		textRect.x = buildRect.x;
		textRect.y = buildRect.y;
		textRect.z = textRect.x + textSize.x;
		textRect.w = textRect.y + textSize.y;

		if( textToCenter )
		{
			auto half_bg_x = float(buildRect.z - buildRect.x) * 0.5f;
			auto half_bg_y = float(buildRect.w - buildRect.y) * 0.5f;
			auto half_text_x = float(textRect.z - textRect.x) * 0.5f;
			auto half_text_y = float(textRect.w - textRect.y) * 0.5f;
			auto d_w = half_bg_x - half_text_x;
			auto d_h = half_bg_y - half_text_y;

			textRect.x += d_w;
			textRect.z += d_w;
			textRect.y += d_h;
			textRect.w += d_h;
		}
		else
		{
			textRect.x += style->buttonTextPositionAdd.x;
			textRect.y += style->buttonTextPositionAdd.y;
			textRect.z += style->buttonTextPositionAdd.x;
			textRect.w += style->buttonTextPositionAdd.y;
		}

		m_textColor.w = style->buttonTextAlpha;
		_addText(m_currentClipRect, textRect,text,textstrlen,style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);
	}

	_checkItemHeight(size.y);
	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);

	if( m_mouseIsLMB_up && m_pressedItemIdLMB == m_uniqueIdCounter && m_lastCursorHoverItemId == m_uniqueIdCounter && !m_blockInputGlobal )
		result = true;

	return result;
}