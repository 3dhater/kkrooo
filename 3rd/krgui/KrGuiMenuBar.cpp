#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::menuBarBegin(  bool enabled, Style* style, bool useNewLine )
{
	_checkStyle(&style);
	Vec4f rectangle;
	m_drawPointer.set(0.f, 0.f);
	rectangle.z = m_OSWindowClientRect.z;
	rectangle.w = rectangle.y + style->menuBarHeight;

	/*m_firstColor  = style->menuBarColor1;
	m_secondColor = style->menuBarColor2;
	m_firstColor.w  = style->menuBarBGAlpha;
	m_secondColor.w = style->menuBarBGAlpha;
	_addRectangle(rectangle, rectangle);*/

	m_currentMenuBar.m_current = true;
	m_currentMenuBar.m_rect  = rectangle;
	m_currentMenuBar.m_style = style;
	m_currentMenuBar.m_useNewLine = useNewLine;
	m_currentMenuBar.m_enabled = enabled;
	m_currentMenuBar.m_itemCounter = 0;
	m_currentMenuBar.m_insidePopupFind = false;

	_setNewDrawGroup(false);

	return true;
}


bool Gui::GuiSystem::menuBarEnd(float * out_menuHeight)
{
	m_currentMenuBar.m_insidePopup = m_currentMenuBar.m_insidePopupFind;

	_setPrevDrawGroup();
	
	m_firstColor  = m_currentMenuBar.m_style->menuBarColor1;
	m_secondColor = m_currentMenuBar.m_style->menuBarColor2;
	m_firstColor.w  = m_currentMenuBar.m_style->menuBarBGAlpha;
	m_secondColor.w = m_currentMenuBar.m_style->menuBarBGAlpha;
	_addRectangle(m_currentMenuBar.m_rect, m_currentMenuBar.m_rect);

	m_currentMenuBar.m_current = false;

	/*auto oldDrawPoint = m_drawPointer;
	m_drawPointer.y -= ;*/

	newLine(m_currentMenuBar.m_style->menuBarHeight);

	if(out_menuHeight)
	{
		*out_menuHeight = m_currentMenuBar.m_rect.w - m_currentMenuBar.m_rect.y;
	}
	return _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentMenuBar.m_rect );
}

bool Gui::GuiSystem::menuBarMenu(const char16_t* text)
{
	assert(text);

	// надо добавить пункт меню
	Vec2f textsize;
	auto textstrlen = this->getTextLen(text, &textsize, m_currentMenuBar.m_style->menuBarTextSpacing, m_currentMenuBar.m_style->menuItemTextSpaceAddSize);
	if(!textstrlen)
		return false;

	Vec4f rect;
	rect.x = m_drawPointer.x;
	rect.y = m_drawPointer.y;
	rect.z = rect.x + textsize.x + m_currentMenuBar.m_style->menuBarTextBorderOffset + m_currentMenuBar.m_style->menuBarTextBorderOffset;
	rect.w = rect.y + m_currentMenuBar.m_style->menuBarHeight;

	if(rect.z > m_OSWindowClientRect.z && m_currentMenuBar.m_useNewLine)
	{
		m_drawPointer.x = 0;
		m_drawPointer.y += m_currentMenuBar.m_style->menuBarHeight;

		rect.x = m_drawPointer.x;
		rect.y = m_drawPointer.y;
		rect.z = rect.x + textsize.x + m_currentMenuBar.m_style->menuBarTextBorderOffset + m_currentMenuBar.m_style->menuBarTextBorderOffset;
		rect.w = rect.y + m_currentMenuBar.m_style->menuBarHeight;

		m_currentMenuBar.m_rect.w += m_currentMenuBar.m_style->menuBarHeight;
	}

	++m_currentMenuBar.m_itemCounter; // 1 2 3 ...
	
	bool result = false;
	m_currentMenuBar.m_inRect = false;
	bool inRect = _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, rect );
	if(inRect && m_currentMenuBar.m_enabled && !m_currentMenuBar.m_insidePopup)
	{
		m_currentMenuBar.m_inRect = true;
		m_firstColor  = m_currentMenuBar.m_style->menuBarHoverBGColor1;
		m_secondColor = m_currentMenuBar.m_style->menuBarHoverBGColor2;
		m_firstColor.w  = m_currentMenuBar.m_style->menuBarHoverBGAlpha;
		m_secondColor.w = m_currentMenuBar.m_style->menuBarHoverBGAlpha;
		m_textColor = m_currentMenuBar.m_style->menuBarTextHoverColor;
		
		if( m_mouseIsLMB_firstClick )
		{
			m_mouseIsLMB_firstClick = false;

			_resetAllPopupMenu();

			if(m_currentMenuBar.m_activated)
			{
				m_currentMenuBar.m_activated = false;
				m_currentMenuBar.m_activeItem = -1;
				m_blockInputGlobal = false;
			}
			else
			{
				m_currentMenuBar.m_activated = true;
				m_currentMenuBar.m_activeItem = m_currentMenuBar.m_itemCounter-1;
			}
		}
		else
		{
			// если меню активировано, то при перемещение на другие пункты нужны скрыть активное окно и показать другое
			if(m_currentMenuBar.m_activated)
			{
				if(m_currentMenuBar.m_activeItem != m_currentMenuBar.m_itemCounter-1)
				{
					_resetAllPopupMenu();
					m_currentMenuBar.m_activeItem = m_currentMenuBar.m_itemCounter-1;
				}
			}
		}
	}
	else
	{
		m_firstColor.w  = 0.f;
		m_secondColor.w = 0.f;
		if(!m_currentMenuBar.m_enabled)
			m_textColor = m_currentMenuBar.m_style->menuBarDisabledTextColor;
		else
			m_textColor = m_currentMenuBar.m_style->menuBarTextColor;
	}

	_addRectangle(rect, rect);

	auto textRect = rect;
	textRect.x += m_currentMenuBar.m_style->menuBarTextBorderOffset;
	textRect.y += m_currentMenuBar.m_style->menuBarTextYOffset;
	_addText(textRect, textRect, text, textstrlen, m_currentMenuBar.m_style->menuBarTextSpacing, m_currentMenuBar.m_style->menuItemTextSpaceAddSize, false);

	m_drawPointer.x += rect.z - rect.x;
	
	if( m_currentMenuBar.m_activated && (m_currentMenuBar.m_activeItem == m_currentMenuBar.m_itemCounter-1) )
	{
		result = true;
		setNexPopupPosition(rect.x, rect.w);
	}

	return result;
}



//void Gui::GuiSystem::menuBarEndMenu()
//{
//	//popupMenuEnd();
//}