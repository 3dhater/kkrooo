#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::addCheckBox( 
	bool * v,
	Style* style, 
	const char16_t* text, 
	bool placeTextOnRightSide,
	bool enabled
)
{
	assert(v);
	auto oldDrawPointer = m_drawPointer;
	bool result = false;

	_checkStyle(&style);
	_newId();
	
	if(!enabled) m_lastDisabledItemId = m_uniqueIdCounter; // id disabled, remember ID

	char16_t buf[2] = {style->checkboxCheckSymbol, 0 };
	if(!*v)buf[0] = style->checkboxUncheckSymbol;
	auto oldFont = this->getCurrentFont();
	this->setCurrentFont(style->iconFont);
	Vec2f boxSize;
	getTextLen(buf,&boxSize,style->buttonTextSpacing, style->buttonTextSpaceAddSize);
	this->setCurrentFont(oldFont);

	Vec2f itemSize = boxSize;

	Vec2f textSize;
	int textLen = 0;
	if(text)
	{
		textLen = getTextLen(text,&textSize,style->buttonTextSpacing, style->buttonTextSpaceAddSize);

		itemSize.x += style->checkboxBoxTextOffset;
		itemSize.x += textSize.x;

		if( textSize.y > itemSize.y ) itemSize.y = textSize.y;
	}


	auto end_x_position = m_drawPointer.x + itemSize.x;
	_checkNewLine(end_x_position);

	auto position = m_drawPointer;
	_checkZoomPosition(&position);
	/*if( m_currentNodePosition )
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
	}*/

	_setCurrentClipRect(position, itemSize);

	auto buildRect = m_currentClipRect;
	_checkParentClipRect();

	// update input
	if( !m_nextItemIgnoreInput && !m_blockInputGlobal )
		_updateMouseInput(mouseButton::LMB);
	else m_nextItemIgnoreInput = false;

	if( placeTextOnRightSide || !text )
	{
		// если текст справа, то сначала рисование чекбокса. он может изменять цвет при наведении мышки
		if( m_pressedItemIdLMB == m_uniqueIdCounter && !m_blockInputGlobal )// if pressed
		{
			if( m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter  )
			{
				m_textColor = style->buttonTextPushColor;
			}
			else
			{
				m_textColor = style->buttonTextHoverColor;
			}
		}else 
		{
			if( m_lastDisabledItemId == m_uniqueIdCounter )
			{
				m_textColor = style->buttonTextDisabledColor;
			}else if( (m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter) && m_pressedItemIdLMB == 0 )
			{
				m_textColor = style->buttonTextHoverColor;
			}else 
			{
				m_textColor = style->buttonTextIdleColor;
			}
		}
		m_textColor.w = style->buttonTextAlpha;

		Vec4f textRect;
		textRect.x = buildRect.x;
		textRect.y = buildRect.y;
		textRect.z = textRect.x + boxSize.x;
		textRect.w = textRect.y + boxSize.y;
		oldFont = this->getCurrentFont();
		this->setCurrentFont(style->iconFont);
		_addText(m_currentClipRect, textRect,buf,1,style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);
		this->setCurrentFont(oldFont);

		if( text )
		{
			m_drawPointer.x += boxSize.x + style->checkboxBoxTextOffset;
			if( m_lastDisabledItemId == m_uniqueIdCounter )
			{
				m_textColor = style->buttonTextDisabledColor;
			}
			else
			{
				m_textColor = style->buttonTextIdleColor;
			}

			/*
			* auto position = m_drawPointer;
			_checkZoomPosition(&position);
			*/
			position = m_drawPointer;
			m_currentNodeContentOffset.x = boxSize.x+style->checkboxBoxTextOffset;
			m_currentNodeContentOffset.x *= 0.5f;
			m_currentNodeContentOffset.x *= (1.f/m_guiZoom);
			_checkZoomPosition(&position);
			_setCurrentClipRect(position, itemSize);
			_checkParentClipRect();

			textRect.x = position.x;
			textRect.y = position.y;
			textRect.z = textRect.x + textSize.x;
			textRect.w = textRect.y + textSize.y;

			textRect.x += style->buttonTextPositionAdd.x;
			textRect.y += style->buttonTextPositionAdd.y;
			textRect.z += style->buttonTextPositionAdd.x;
			textRect.w += style->buttonTextPositionAdd.y;
			_addText(m_currentClipRect, textRect,text,textLen,style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);
		}
	}
	else
	{
		// сначало рисование текста
		if( m_lastDisabledItemId == m_uniqueIdCounter )
		{
			m_textColor = style->buttonTextDisabledColor;
		}
		else
		{
			m_textColor = style->buttonTextIdleColor;
		}
		Vec4f textRect;
		textRect.x = buildRect.x;
		textRect.y = buildRect.y;
		textRect.z = textRect.x + textSize.x;
		textRect.w = textRect.y + textSize.y;
		textRect.x += style->buttonTextPositionAdd.x;
		textRect.y += style->buttonTextPositionAdd.y;
		textRect.z += style->buttonTextPositionAdd.x;
		textRect.w += style->buttonTextPositionAdd.y;
		_addText(m_currentClipRect, textRect,text,textLen,style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);
		

		oldFont = this->getCurrentFont();
		this->setCurrentFont(style->iconFont);

		m_drawPointer.x += textSize.x + style->checkboxBoxTextOffset;
		position = m_drawPointer;
		m_currentNodeContentOffset.x = textSize.x+style->checkboxBoxTextOffset;
		m_currentNodeContentOffset.x *= 0.5f;
		m_currentNodeContentOffset.x *= (1.f/m_guiZoom);
		_checkZoomPosition(&position);
		_setCurrentClipRect(position, itemSize);
		_checkParentClipRect();

		textRect.x = position.x;
		textRect.y = position.y;
		textRect.z = textRect.x + boxSize.x;
		textRect.w = textRect.y + boxSize.y;

		if( m_pressedItemIdLMB == m_uniqueIdCounter && !m_blockInputGlobal )// if pressed
		{
			if( m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter  )
			{
				m_textColor = style->buttonTextPushColor;
			}
			else
			{
				m_textColor = style->buttonTextHoverColor;
			}
		}else 
		{
			if( m_lastDisabledItemId == m_uniqueIdCounter )
			{
				m_textColor = style->buttonTextDisabledColor;
			}else if( (m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter) && m_pressedItemIdLMB == 0 )
			{
				m_textColor = style->buttonTextHoverColor;
			}else 
			{
				m_textColor = style->buttonTextIdleColor;
			}
		}
		m_textColor.w = style->buttonTextAlpha;

		_addText(m_currentClipRect, textRect,buf,1,style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);

		this->setCurrentFont(oldFont);
	}

	_checkItemHeight(itemSize.y);
	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);

	if( m_mouseIsLMB_up && m_pressedItemIdLMB == m_uniqueIdCounter && m_lastCursorHoverItemId == m_uniqueIdCounter && !m_blockInputGlobal )
	{
		result = true;
		*v = *v ? false : true;
	}

	return result;
}