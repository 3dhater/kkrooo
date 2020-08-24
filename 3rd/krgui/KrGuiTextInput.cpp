#include "KrGui.h"

using namespace Kr;

// данная вещь предполагается использовать только один раз за кадр.
// он работает как popup окно (не технически а визуально). он включается например при нажатии ПКМ на valueSelector
// возможно нужно написать чуть по другому. чтобы было множество полей с вводом текста
bool Gui::GuiSystem::addTextInputPopup(const Vec2f& _size, char16_t* buf, size_t buf_size, size_t char_limit, bool(*filter)(char16_t), Style* style )
{
	assert(value);

	static size_t cursor_position = 0;
	static bool is_selected = false;
	static size_t select_begin = 0;
	static size_t select_end = 0;
	static size_t select_num = 0;
	//printf("%zu\n",select_num);
	auto str_len = _internal::strLen(buf);
	if( m_IsDelete )
	{
		bool ok = false;
		for( size_t i = cursor_position; i < str_len; ++i )
		{
			ok = true;
			if( i+1 == str_len )
				break;
			buf[i] = buf[i+1];
		}
		if(ok)
			buf[str_len-1] = 0;
	}
	else if( m_IsBackspace )
	{
		bool ok = false;
		for( size_t i = cursor_position; i < str_len; ++i )
		{
			if(i == 0)
				break;
			
			ok = true;

			buf[i-1] = buf[i];
		}
		if(cursor_position == str_len && !ok)
		{
			ok = true;
		}

		if(ok)
		{
			if(str_len-1 >= 0)
			{
				--cursor_position;
				buf[str_len-1] = 0;
			}
		}
	}
	else if( m_IsHome )
	{
		
		cursor_position = 0;
	}
	else if( m_IsEnd )
	{
		cursor_position = str_len;
	}
	else if( m_IsLeft )
	{
		if( cursor_position > 0 )
		{
			--cursor_position;
		}
	}
	else if( m_IsRight )
	{
		++cursor_position;
		if( cursor_position > str_len )
			cursor_position = str_len;
		if(cursor_position == char_limit) --cursor_position;
	}
	else if( m_character && filter )
	{
		if( filter(m_character) )
		{
			if(str_len > char_limit) 
				str_len = char_limit;
			
			size_t i = str_len;

			while(i >= cursor_position)
			{
				auto next = i + 1;
				if(next < char_limit)
					buf[next] = buf[i];

				if(i == 0)
					break;
				--i;
			}
			buf[cursor_position] = m_character;
			++cursor_position;
			if(cursor_position == char_limit) --cursor_position;
		}
	}
	str_len = _internal::strLen(buf);

	if(is_selected)
	{
		//printf("%zu %zu\n", select_begin, select_end);
	}

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

	auto buildRect = m_currentClipRect;

	_checkParentClipRect();

	auto oldPosition = m_drawPointer;

	m_firstColor  = style->textInputBgColor;
	m_secondColor = style->textInputBgColor;
	_addRectangle(m_currentClipRect, buildRect);
	
	m_drawPointer.x += (m_currentClipRect.z - m_currentClipRect.x) * (1.f/m_guiZoom);
	auto endPosition = m_drawPointer;
//	
	// draw text
	
	m_drawPointer = oldPosition;
	size_t sel_1 = select_begin;
	size_t sel_2 = select_end;
	if(is_selected)
	{
		if( sel_1 > sel_2 )
		{
			sel_1 = sel_2;
			sel_2 = select_begin;
		}
		this->_setNewDrawGroup(false);
	}
	Gui::DrawCommands * command = _getDrawCommand();
	command->inds.clear();
	command->verts.clear();
	command->clipRect = m_currentClipRect;
	int last_texture = -1;
	int index_index = 0;
	Vec2f text_pointer;
	text_pointer.x = m_currentClipRect.x;
	text_pointer.y = m_currentClipRect.y;
	Vec4f selectRect;
	Vec4f cursorBuildRect;
	for( size_t i = 0; i < buf_size; ++i )
	{
		Vec2f text_pointer_begin = text_pointer;

		char16_t ch = buf[i];
		if(ch == 0)
			break;

		if(ch == u'\n' || ch == u'\r')
			continue;

		auto & glyph = m_currentFont->m_glyphs[ch];

		if(glyph.symbol != ch)
			continue;

		if( last_texture != glyph.textureID && last_texture != -1 )
		{
			command = _getDrawCommand();
			command->inds.clear();
			command->verts.clear();
			command->clipRect = m_currentClipRect;
			index_index = 0;
		}
		command->texture.texture_address = m_currentFont->m_textures[glyph.textureID]->texture_address;
		Gui::Vertex vertex1, vertex2, vertex3, vertex4;
		auto TXN = text_pointer.x;
		auto TXP = TXN + glyph.width;
		auto TYN = text_pointer.y;
		auto TYP = TYN + glyph.height;
		if(is_selected)
		{
			if( i == sel_1 )
			{
				selectRect.x = TXN;
				selectRect.y = TYN;
			}
			if( i == sel_2 )
			{
				selectRect.z = TXP;
				selectRect.w = TYP;
			}
		}
		auto centerX = TXN;
		auto centerY = TYN;
		TXP -= centerX; TXP = ( TXP * m_guiZoom ); TXP += centerX;
		TYP -= centerY; TYP = ( TYP * m_guiZoom ); TYP += centerY;
		vertex1.position.set(TXN, TYN); // LT
		vertex2.position.set(TXN, TYP); // LB
		vertex3.position.set(TXP, TYP); // RB
		vertex4.position.set(TXP, TYN); // RT
		vertex1.color = style->textInputTextColor;
		vertex2.color = style->textInputTextColor;
		vertex3.color = style->textInputTextColor;
		vertex4.color = style->textInputTextColor;
		vertex1.textCoords = glyph.lt;
		vertex2.textCoords = glyph.lb;
		vertex3.textCoords = glyph.rb;
		vertex4.textCoords = glyph.rt;
		command->inds.push_back(index_index);
		command->inds.push_back(index_index+1);
		command->inds.push_back(index_index+2);
		command->inds.push_back(index_index);
		command->inds.push_back(index_index+2);
		command->inds.push_back(index_index+3);
		index_index += 4;
		last_texture = glyph.textureID;
		text_pointer.x += (glyph.width)*m_guiZoom;
		text_pointer.x += style->buttonTextSpacing *m_guiZoom;
		if( glyph.symbol == u' ' )
			text_pointer.x += style->buttonTextSpaceAddSize*m_guiZoom;		
		command->verts.push_back(vertex1);
		command->verts.push_back(vertex2);
		command->verts.push_back(vertex3);
		command->verts.push_back(vertex4);

		if(i == cursor_position )
		{
			cursorBuildRect.x = text_pointer_begin.x;
			cursorBuildRect.y = m_currentClipRect.y + 2.f;
			cursorBuildRect.z = cursorBuildRect.x + 1.f;
			cursorBuildRect.w = cursorBuildRect.y + (m_currentClipRect.w - m_currentClipRect.y) - 4.f;
		}
	}
	if(cursor_position==str_len)
	{
		cursorBuildRect.x = text_pointer.x;
		cursorBuildRect.y = m_currentClipRect.y + 2.f;
		cursorBuildRect.z = cursorBuildRect.x + 1.f;
		cursorBuildRect.w = cursorBuildRect.y + (m_currentClipRect.w - m_currentClipRect.y) - 4.f;
	}
	m_firstColor = style->textInputTextColor;
	m_secondColor = style->textInputTextColor;
	_addRectangle(m_currentClipRect, cursorBuildRect );
	if(is_selected)
	{
		this->_setPrevDrawGroup();
		m_firstColor = style->textInputSelectedBgColor;
		m_secondColor = style->textInputSelectedBgColor;
		_addRectangle(m_currentClipRect, selectRect );
	}
	m_drawPointer = endPosition;

	bool result = m_IsEnter;
	if(!result)
	{
		if( !_internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) && m_mouseIsLMB_firstClick )
		{
			result = true;
			cursor_position = 0;
			is_selected = false;
			select_begin = 0;
			select_end = 0;
		}
	}
	return result;
}

