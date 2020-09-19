#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::beginGroup(const Vec2f& _size, bool * expandCollapse, Style* style)
{
	_checkStyle(&style);

	auto size = _size;
	_checkSize(&size);

	// пока группа гарантированно expanded
	// проще сделать отдельную кнопку для скрытия\показа
	bool expanded = true;

	auto * currentGroup = &m_groupInfo[++m_groupInfoCount];
	currentGroup->m_inRect = false;

	auto old_m_drawPointer = m_drawPointer;
	if(expandCollapse)
	{
		auto button_height = 10.f;
		//m_currentClipRect.y += 10; // button height
		if( addButton(*expandCollapse ? u"Hide" : u"Show", 0, Vec2f(size.x, button_height) ) )
		{
			*expandCollapse = *expandCollapse ? false : true;
		}
		m_drawPointer = old_m_drawPointer;
		m_drawPointer.y += button_height;
		old_m_drawPointer = m_drawPointer;
	
		expanded = *expandCollapse;
	}

	if(expanded)
	{
		m_groupList[m_groupListCurrent] = currentGroup;
		++m_groupListCurrent;

		// когда группа начинается, нужно заново определять высочайший элемент в строке
		m_maxItemHeightOnCurrentRow[++m_maxItemHeightOnCurrentRowCount] = 0; // в endGroup значение становится = высоте группы
		currentGroup->m_size = size;


		_newId();
		_setCurrentClipRect(m_drawPointer, size);

		auto buildRect = m_currentClipRect;

		// проверить текущий clipRect с предидущим родительским clipRect
		_checkParentClipRect();

		// и установить новый для следующих объектов
		m_parentClipRects[++m_parentClipRectCount] = m_currentClipRect;

		bool inRect = false;
		if( !m_blockInputGlobal )
		{
			if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) )
			{
				inRect = true;
				_updateMouseInputInRectMoveHover();
			}
		}
		
		currentGroup->m_inRect = inRect;

		m_firstColor    = style->groupColor1;
		m_secondColor   = style->groupColor2;
		m_firstColor.w  = style->groupBackgroundAlpha;
		m_secondColor.w = style->groupBackgroundAlpha;

		if( inRect )
		{
			m_firstColor  = style->groupHoverColor1;
			m_secondColor = style->groupHoverColor2;
			m_currentGroupInRectId = m_uniqueIdCounter;
		}

		// нужно нарисовать фон
		_addRectangle(m_currentClipRect, buildRect);
		// и установить курсор на позицию до рисования фона
		m_drawPointer = old_m_drawPointer;

		/*if(expandCollapse)
		{
			old_m_drawPointer = m_drawPointer;
		}*/

		currentGroup->m_clipRect = m_currentClipRect;
		
		currentGroup->m_contentHeight = 0; // сброс высоты

		m_drawPointer.y += (int)(currentGroup->m_scrollValue * m_scrollMultipler) * (1.f/m_guiZoom);
	}
	return expanded;
}

void Gui::GuiSystem::setCurrentGroupContentHeight(float v)
{
	if( m_groupInfoCount )
	{
		m_groupInfo[m_groupInfoCount].m_contentHeight = v;
	}
}

void Gui::GuiSystem::endGroup()
{
	--m_groupListCurrent;
	auto * currentGroup = m_groupList[m_groupListCurrent];
	

	// размер последней строчки не учитывается внутри newLine
	currentGroup->m_contentHeight += m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] * m_guiZoom;
	
	// подготовка к выходу из группы
	--m_maxItemHeightOnCurrentRowCount;
	//--m_groupInfoCount;
	--m_parentClipRectCount;
	
	/*if(currentGroup->m_inRect)
	{
		printf("currentGroup->m_contentHeight %f\n",currentGroup->m_contentHeight);
	}*/

	bool scroll = currentGroup->m_inRect && ( currentGroup->m_contentHeight > currentGroup->m_size.y * m_guiZoom );
	if(!scroll) //если размер маленький но прокрутка уже была
	{
		if( m_wheel_delta > 0 && currentGroup->m_scrollValue < 0.f  )
		{
			scroll = true;
		}
	}
	if( scroll && currentGroup->m_inRect )
	{
		if( m_wheel_delta > 0 )
		{
			currentGroup->m_scrollValue += m_wheel_delta;
			if( currentGroup->m_scrollValue > 0.f )
			{
				currentGroup->m_scrollValue = 0.f;
			}
		}
		if( m_wheel_delta < 0 )
		{
			currentGroup->m_scrollValue += m_wheel_delta;
			auto scrollValue = std::abs(currentGroup->m_scrollValue * m_scrollMultipler);
			auto scrollLimitY = currentGroup->m_contentHeight - currentGroup->m_size.y * m_guiZoom;
			if( scrollValue > scrollLimitY )
			{
				currentGroup->m_scrollValue += (scrollValue - scrollLimitY) / m_scrollMultipler;
			}
		}
	}
	setDrawPosition(currentGroup->m_clipRect.z, currentGroup->m_clipRect.y);

	//auto size_y = currentGroup->m_clipRect.w - currentGroup->m_clipRect.y;
	auto size_y = currentGroup->m_contentHeight;
	if( size_y > m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] )
		m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] = size_y;
}

Gui::Group* Gui::GuiSystem::getCurrentGroup()
{
	return m_groupList[m_groupListCurrent - 1];
}