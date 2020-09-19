#include "KrGui.h"

using namespace Kr;

Gui::Vec2f g_oldDrawPoint;

bool Gui::GuiSystem::_popupMenuBegin( bool * active, Style* style, const Vec2f& pos)
{
	g_oldDrawPoint = m_drawPointer;
	_checkStyle(&style);

	if(m_mouseIsRMB_firstClick)
	{
		if( m_currentMenuBar.m_activated ) 
		{
			m_currentMenuBar.m_activated = false;
			m_currentMenuBar.m_activeItem = -1;
			return false;
		}
	}


	// получаю новый PopupMenuInformation
	PopupMenuInformation * activePopup = &m_popupMenuInfo[m_popupMenuInfoCount];
	activePopup->m_lastItemPosition.set(0.f,0.f);
	activePopup->m_isActive_ptr = active; // сохраняю указатели
	activePopup->m_style_ptr = style;
	activePopup->m_size.set(style->popupBorderIndent.x + style->popupBorderIndent.z, style->popupBorderIndent.y + style->popupBorderIndent.w );
	activePopup->m_shortcutWidthMax = 0.f;
	++m_popupMenuInfoCount;  // прибавляю 1 для следующих меню. значение обнулится в newFrame

	if(*active == true)
	{

		m_blockInputGlobal = true;

		m_popupMenuActive[m_popupMenuActiveCount] = activePopup;
		++m_popupMenuActiveCount; // прибавить 1 если активно. надо убавить в popupMenuEnd
		

		// при клике, нужно запомнить позицию и сделать ещё некторые вещи
		if( !activePopup->m_isActive || m_mouseIsRMB_firstClick )
		{
			activePopup->m_isActive = true;
			activePopup->m_position = pos;
			activePopup->m_scrollValue = 0.f;
		}
		
		activePopup->m_addMenuItemCounter = 0;
		
		_setCurrentClipRect(activePopup->m_position, activePopup->m_sizeEnd);
		m_currentClipRect.x -= activePopup->m_style_ptr->popupBorderIndent.x;
		m_currentClipRect.y -= activePopup->m_style_ptr->popupBorderIndent.y;
		m_currentClipRect.z += activePopup->m_style_ptr->popupBorderIndent.z;
		m_currentClipRect.w += activePopup->m_style_ptr->popupBorderIndent.w;

		m_parentClipRects[++m_parentClipRectCount] = m_currentClipRect;

		// если активно, то будет рисоваться фон и содержимое. 
		// фон и содержимое должны рисоваться поверх всего остального
		// для этого надо создать новую группу для рисования.
		// 2 раза, так как по сути сначала добавляются items потом в popupMenuEnd рисуется фон
		_setNewDrawGroup(false); // popup window must be on top
	//	pinfo.m_drawGroup = m_currentDrawGroup; // сохраню на всякий случай
		_setNewDrawGroup(false); // for menu items 

	}
	/*else
	{
		_resetAllPopupMenu();
	}*/

	return *active;
}

bool Gui::GuiSystem::popupMenuBegin(bool * active, Style* style)
{
	assert(active);
	return _popupMenuBegin(active, style, m_popupMenuPosition);
}

bool Gui::GuiSystem::popupMenuEnd(const Vec4f& rounding)
{
	--m_popupMenuActiveCount;
	PopupMenuInformation * activePopup = m_popupMenuActive[m_popupMenuActiveCount];

	activePopup->m_sizeEnd = activePopup->m_size;

	auto oldDrawPoint = this->getDrawPosition();
	// активна группа для items
	_setPrevDrawGroup(); // переход на предидущую группу
	this->setDrawPosition(activePopup->m_position.x - activePopup->m_style_ptr->popupBorderIndent.x, 
		activePopup->m_position.y - activePopup->m_style_ptr->popupBorderIndent.y, false); // установка на нужную позицию
	addRectangle(activePopup->m_style_ptr, Vec2f(activePopup->m_size.x + activePopup->m_style_ptr->popupBorderIndent.z, 
		activePopup->m_size.y + activePopup->m_style_ptr->popupBorderIndent.w), 
		activePopup->m_style_ptr->popupAlpha, rounding ); // можно рисовать фон
	_setPrevDrawGroup(); // переход на предидущую группу
	
	this->setDrawPosition(oldDrawPoint.x, oldDrawPoint.y, false);
	
	--m_parentClipRectCount;

	activePopup->m_inRect = _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect );
	if(activePopup->m_inRect)
	{
		if(m_currentMenuBar.m_activated )
		{
			m_currentMenuBar.m_insidePopupFind = true;
		}
		// try scroll here
		auto scrollLimit = (activePopup->m_position.y + activePopup->m_sizeEnd.y) - m_OSWindowClientRect.w;
		if( scrollLimit > 0 ) // can scroll
		{
			if( m_wheel_delta > 0 )
			{
				activePopup->m_scrollValue += activePopup->m_scrollSpeed;
				if( activePopup->m_scrollValue > 0.f )
					activePopup->m_scrollValue = 0.f;
			}

			if( m_wheel_delta < 0 )
			{
			//	printf("[%f] [%f]\n", activePopup->m_scrollValue, scrollLimit);
				activePopup->m_scrollValue -= activePopup->m_scrollSpeed;
				if( std::fabs( activePopup->m_scrollValue ) > scrollLimit )
					activePopup->m_scrollValue = -scrollLimit;
			}
		}
	}
	else
	{
		if( m_mouseIsLMB_up && !m_currentMenuBar.m_inRect ) // клик вне области окна
		{
			m_currentMenuBar.m_activated = false;
			m_currentMenuBar.m_activeItem = -1;

			activePopup->m_isActive = false;
			*activePopup->m_isActive_ptr = false;
			m_blockInputGlobal = false;
			_resetAllPopupMenu();
		}
	}

	this->setDrawPosition(g_oldDrawPoint.x, g_oldDrawPoint.y, true);
	return activePopup->m_inRect;
}

bool Gui::GuiSystem::_addMenuItem(const char16_t* text, const char16_t* shortcutText, char16_t iconFontSymbolLeft, char16_t iconFontSymbolRight1, char16_t iconFontSymbolRight2, bool * check, bool isSubMenu )
{
	bool result = false;
	auto zoom_old = m_guiZoom; // зум может быть изменён, по этому надо его сохранить и установит 1 пока добавляется элемент
	m_guiZoom = 1.f;

	// текущее popup меню
	auto activePopup = m_popupMenuActive[m_popupMenuActiveCount-1];

	// получение размера элемента
	Vec2f itemSize;
	itemSize.y = activePopup->m_style_ptr->menuItemHeight;
	itemSize.x = activePopup->m_style_ptr->menuItemIconSize.x;
	itemSize.x += activePopup->m_style_ptr->menuItemIconTextOffset;

	Vec2f textsize;
	auto textstrlen = this->getTextLen(text, &textsize, activePopup->m_style_ptr->menuItemTextSpacing, activePopup->m_style_ptr->menuItemTextSpaceAddSize);

	itemSize.x += textsize.x;

	Vec2f shortcut_textsize;
	int shortcut_len = 0;
	if( shortcutText )
	{
		shortcut_len = this->getTextLen(shortcutText, &shortcut_textsize,
			activePopup->m_style_ptr->menuItemTextSpacing, activePopup->m_style_ptr->menuItemTextSpaceAddSize);

		itemSize.x += activePopup->m_style_ptr->menuItemItemTextShortcutOffset;
		itemSize.x += shortcut_textsize.x;

		if(shortcut_textsize.x > activePopup->m_shortcutWidthMax)
			activePopup->m_shortcutWidthMax = shortcut_textsize.x;
	}
	itemSize.x += activePopup->m_style_ptr->menuItemIconSubmenuSize.x;

	// если длина элемента выше чем длина фона то надо увеличить фон
	if(itemSize.x + activePopup->m_style_ptr->popupBorderIndent.z + activePopup->m_shortcutWidthMax > activePopup->m_size.x)
		activePopup->m_size.x = itemSize.x  + activePopup->m_style_ptr->popupBorderIndent.z + activePopup->m_shortcutWidthMax;

	// увеличение фона по y
	activePopup->m_size.y += activePopup->m_style_ptr->menuItemHeight;

	// фон окна нарисуется потом
	// сейчас надо нарисовать фон элемента
	auto position = activePopup->m_position; // начальная позиция рисования это позиция окна
	//position.y += (itemSize.y * (++activePopup->m_addMenuItemCounter))-itemSize.y; // перемещение на нужную строчку
	position.y += activePopup->m_lastItemPosition.y;
	position.y += activePopup->m_scrollValue;
	activePopup->m_lastItemPosition.y += itemSize.y;

	++activePopup->m_addMenuItemCounter;

	// элементы могут иметь различную длинну. нужно сделать одну длинну.
	float popup_x_without_indent = activePopup->m_sizeEnd.x - activePopup->m_style_ptr->popupBorderIndent.z;
	if( itemSize.x < popup_x_without_indent )
		itemSize.x = popup_x_without_indent;

	// рисование фона
	Vec4f itemRect = Vec4f(position.x, position.y, position.x + itemSize.x, position.y + itemSize.y);
	auto itemBuildRect = itemRect;
	_checkParentClipRect(itemRect);
	
//	activePopup->m_currentItemId = activePopup->m_addMenuItemCounter - 1;

	bool inRect = _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, itemRect );
	if(inRect)
	{
		m_firstColor  = activePopup->m_style_ptr->menuItemHoverBGColor1;
		m_secondColor = activePopup->m_style_ptr->menuItemHoverBGColor2;
		m_firstColor.w  = activePopup->m_style_ptr->menuItemHoverBGAlpha;
		m_secondColor.w = activePopup->m_style_ptr->menuItemHoverBGAlpha;
		_addRectangle(itemRect, itemBuildRect, Vec4f(3.f,3.f,3.f,3.f) );

		activePopup->m_hoverItemId = activePopup->m_addMenuItemCounter - 1;
		//activePopup->m_hoverItemId = activePopup->m_currentItemId;
		//printf("%i\n", activePopup->m_activeItemId);
	}
	else
	{
		m_firstColor  = activePopup->m_style_ptr->menuItemBGColor1;
		m_secondColor = activePopup->m_style_ptr->menuItemBGColor2;
		m_firstColor.w  = activePopup->m_style_ptr->popupAlpha;
		m_secondColor.w = activePopup->m_style_ptr->popupAlpha;
		_addRectangle(itemRect, itemBuildRect );
	}

	// рисование иконки
	if( activePopup->m_style_ptr->iconFont && iconFontSymbolLeft && activePopup->m_style_ptr->menuItemIconSize.x > 0 )
	{
		auto r = itemBuildRect;
		r.z = r.x + activePopup->m_style_ptr->menuItemIconSize.x;
		r.w = r.y + activePopup->m_style_ptr->menuItemIconSize.y;
		char16_t buf[2] = { iconFontSymbolLeft, 0 };
		auto old_font = this->getCurrentFont();
		this->setCurrentFont(activePopup->m_style_ptr->iconFont);
		m_textColor  = ColorWhite;
		auto rb = r;
		_checkParentClipRect(r);
		_addText(r, rb, buf, 1, 0, 0, false);
		this->setCurrentFont(old_font);
	}
	
	// основной текст
	m_textColor  = inRect ? activePopup->m_style_ptr->menuItemTextHoverColor : activePopup->m_style_ptr->menuItemTextColor;
	m_textColor.w  = activePopup->m_style_ptr->menuItemHoverBGAlpha;
	Vec4f textRect = itemBuildRect;
	textRect.x += activePopup->m_style_ptr->menuItemIconSize.x + activePopup->m_style_ptr->menuItemIconTextOffset;
	textRect.y += activePopup->m_style_ptr->menuItemTextYOffset;
	textRect.z = textRect.x + textsize.x;
	textRect.w = textRect.y + activePopup->m_style_ptr->menuItemHeight;
	auto textBuildRect = textRect;
	_checkParentClipRect(textRect);
	_addText(textRect, textBuildRect, text, textstrlen, activePopup->m_style_ptr->menuItemTextSpacing, 
		activePopup->m_style_ptr->menuItemTextSpaceAddSize, false);

	// текст для горячих клавиш
	if( shortcutText )
	{
		textRect.x = activePopup->m_position.x + activePopup->m_sizeEnd.x - shortcut_textsize.x; // нужно отступать справа
		textRect.x -= activePopup->m_style_ptr->menuItemIconSubmenuSize.x;
		textRect.y = itemBuildRect.y +  activePopup->m_style_ptr->menuItemTextYOffset;
		textRect.z = textRect.x + shortcut_textsize.x;
		textRect.w = textRect.y + activePopup->m_style_ptr->menuItemHeight;
		textBuildRect = textRect;
		_checkParentClipRect(textRect);
		_addText(textRect, textBuildRect, shortcutText, shortcut_len, activePopup->m_style_ptr->menuItemTextSpacing, 
			activePopup->m_style_ptr->menuItemTextSpaceAddSize, false);
	}

	char16_t symbol = iconFontSymbolRight1;

	if(check)
	{
		if( *check == false )
			symbol = iconFontSymbolRight2;
	}

	// иконки справа
	if( activePopup->m_style_ptr->iconFont && symbol && activePopup->m_style_ptr->menuItemIconSubmenuSize.x > 0 )
	{
		Vec4f r;
		r.x = activePopup->m_position.x + activePopup->m_sizeEnd.x - activePopup->m_style_ptr->menuItemIconSubmenuSize.x;
		r.y = itemBuildRect.y;
		r.z = r.x + activePopup->m_style_ptr->menuItemIconSubmenuSize.x;
		r.w = r.y + activePopup->m_style_ptr->menuItemIconSubmenuSize.y;
		char16_t buf[2] = { symbol, 0 };
		auto old_font = this->getCurrentFont();
		this->setCurrentFont(activePopup->m_style_ptr->iconFont);
		m_textColor  = ColorWhite;
		auto rb = r;
		_checkParentClipRect(r);
		_addText(r, rb, buf, 1, 0, 0, false);
		this->setCurrentFont(old_font);
	}
	
	// если клик по пункту с субменю, то реакция не нужна
	if(m_mouseIsLMB_up && isSubMenu && inRect)
	{
		m_mouseIsLMB_up = false;
	}

	if(!m_nextItemIgnoreInput && !isSubMenu)
	{
		if( m_mouseIsLMB_up && inRect )
		{
			activePopup->m_isActive = false;
			*activePopup->m_isActive_ptr = false;
		
			if(check)
			{
				if(*check)*check = false;
				else *check = true;
			}

			if( m_currentMenuBar.m_activated ) 
			{
				m_currentMenuBar.m_activated = false;
				m_currentMenuBar.m_activeItem = -1;
			}
			m_blockInputGlobal = false;
			result = true;
		}
	}else m_nextItemIgnoreInput = false;

	m_popupMenuLastItemRect = itemBuildRect;

	m_guiZoom = zoom_old;
	
	return result;
}

bool Gui::GuiSystem::addMenuItemCheck(const char16_t* text, const char16_t* shortcutText, bool* check, char16_t iconFontSymbol, char16_t iconFontCheckSymbol, char16_t iconFontUncheckSymbol )
{
	assert(text);
	return _addMenuItem(text, shortcutText, iconFontSymbol, iconFontCheckSymbol, iconFontUncheckSymbol, check, false );
}

bool Gui::GuiSystem::addMenuItem(const char16_t* text, const char16_t* shortcutText, char16_t iconFontSymbol, char16_t iconFontSubmenuSymbol)
{
	assert(text);
	return _addMenuItem(text, shortcutText, iconFontSymbol, iconFontSubmenuSymbol, 0, nullptr, false );
}

void Gui::GuiSystem::addSeparator(Style* style)
{
	_checkStyle(&style);
	auto zoom_old = m_guiZoom;
	m_guiZoom = 1.f;
	
	auto activePopup = m_popupMenuActive[m_popupMenuActiveCount-1];
	Vec2f itemSize;
	itemSize.x = activePopup->m_sizeEnd.x - style->popupBorderIndent.z;
	itemSize.y = style->separatorHeight;

	activePopup->m_size.y += style->separatorHeight;

	auto position = activePopup->m_position;
	position.y += activePopup->m_lastItemPosition.y;
	position.y += activePopup->m_scrollValue;
	activePopup->m_lastItemPosition.y += itemSize.y;


	Vec4f itemRect = Vec4f(position.x, position.y, position.x + itemSize.x, position.y + itemSize.y);
	auto itemBuildRect = itemRect;
	_checkParentClipRect(itemRect);

	m_firstColor  = activePopup->m_style_ptr->menuItemBGColor1;
	m_secondColor = activePopup->m_style_ptr->menuItemBGColor2;
	m_firstColor.w  = activePopup->m_style_ptr->popupAlpha;
	m_secondColor.w = activePopup->m_style_ptr->popupAlpha;
	_addRectangle(itemRect, itemBuildRect );

	float halfSize_y = itemSize.y * 0.5f;

	itemRect.x += style->separatorLineBorderOffset;
	itemRect.z -= style->separatorLineBorderOffset;
	itemRect.y += halfSize_y;
	itemRect.w -= halfSize_y;

	float halfLine_y = style->separatorLineWidth * 0.5f;
	itemRect.y -= halfLine_y;
	itemRect.w += halfLine_y;

	m_firstColor  = style->separatorLineColor;
	m_secondColor = style->separatorLineColor;
	_addRectangle(itemRect, itemBuildRect );

	/*
	drawLine(
		Vec2f(itemBuildRect.x + style->separatorLineBorderOffset, itemBuildRect.y + half_y),
		Vec2f(itemBuildRect.z - style->separatorLineBorderOffset, itemBuildRect.y + half_y), 
		style->separatorLineWidth, 
		style->separatorLineColor1, 
		style->separatorLineColor2
	);*/

	m_guiZoom = zoom_old;
}

bool Gui::GuiSystem::beginMenu(const char16_t* text, char16_t iconFontSymbol, char16_t iconFontSubmenuSymbol)
{
	assert(text);

	/*if( m_currentMenuBar.m_current )
	{
		if( !m_currentMenuBar.m_activated )
			return _beginMenuBarItem(text);
	}*/

	setNextItemIgnoreInput();
	_addMenuItem(text, 0, iconFontSymbol, iconFontSubmenuSymbol, 0, nullptr, true);
	auto activePopup = m_popupMenuActive[m_popupMenuActiveCount-1];
	
	// активным субменю может быть только 1 пункт, по этому нужно игнорировать все неверные beginMenu

	// курсор в области пункта "SubMenu"
	// если истина то нужно рисовать след. окно
	bool result = _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_popupMenuLastItemRect+Vec4f(0.f,0.f,5.f,0.f));
	auto nextPopup = m_popupMenuActive[m_popupMenuActiveCount];
	
	if( activePopup->m_subMenuActiveId == activePopup->m_addMenuItemCounter - 1) result = true;

	if(!result)
	{
		if(nextPopup)
		{
			m_popupMenuActive[m_popupMenuActiveCount] = nullptr;
			nextPopup->m_isActive = false;
			*nextPopup->m_isActive_ptr = false;
		}
		return false;
	}

	activePopup->m_subMenuActiveId = activePopup->m_hoverItemId;
	activePopup->m_subMenuActive = true;
	//_popupMenuBegin(&activePopup->m_subMenuActive, activePopup->m_style_ptr, Vec2f(m_popupMenuLastItemRect.z, m_popupMenuLastItemRect.y));
	//m_popupMenuPosition
	setNexPopupPosition(m_popupMenuLastItemRect.z, m_popupMenuLastItemRect.y);
	if(nextPopup)
	{
		// возможно даст проблему с попапом при RMB
		nextPopup->m_isActive = false;
	}
	_popupMenuBegin(&activePopup->m_subMenuActive, activePopup->m_style_ptr, m_popupMenuPosition);
	return true;
}

bool Gui::GuiSystem::endMenu()
{
	return popupMenuEnd();
}

void Gui::GuiSystem::_resetAllPopupMenu()
{
	/*if(m_popupMenuActiveCount > 0)
	{
		auto activePopup = m_popupMenuActive[m_popupMenuActiveCount-1];
		activePopup->m_isActive = false;
		*activePopup->m_isActive_ptr = false;
	}*/

	for(int i = 0; i < KRGUI_NUM_OF_POPUPS; ++i)
	{
		//m_popupMenuInfo[ i ] = m_popupMenuZero;
		if(m_popupMenuInfo[ i ].m_isActive_ptr)
		{
			m_popupMenuInfo[ i ].m_isActive = false;
			*m_popupMenuInfo[ i ].m_isActive_ptr = false;
		}
		m_popupMenuInfo[ i ].reset();
		
	}

	for(int i = 0; i < KRGUI_POPUP_DEPTH; ++i)
	{
	//	m_popupMenuActive[i] = nullptr;
	}
}

void Gui::GuiSystem::setNexPopupPosition(float x, float y)
{
	m_popupMenuPosition.x = x;
	m_popupMenuPosition.y = y;
}
