#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::addTextInput( 
	const char16_t* text,
	const Vec2f& _size, 
	bool(*filter)(char16_t),
	Style* style, 
	const Vec4f& rounding )
{
	bool result = false;

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
	auto buildRect = m_currentClipRect;
	_checkParentClipRect();
	m_firstColor  = style->buttonIdleColor1;
	m_secondColor = style->buttonIdleColor1;
	_addRectangle(m_currentClipRect, buildRect, rounding);
	
	if( !m_blockInputGlobal )
	{
		_updateMouseInput(mouseButton::LMB);

		if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) && m_mouseIsLMB_up)
		{
			input_mode = true;
			input_mode_elementID = m_uniqueIdCounter;
			m_inputTextBuffer.clear();
			m_inputTextBuffer.reserve(1024);
			for( int i = 0; i < 32; ++i )
			{
				m_inputTextBuffer += (char16_t)text[i];
			}
		}
	}

	bool inputmode = false;
	if( input_mode && m_uniqueIdCounter == input_mode_elementID )
	{
		inputmode = true;
	}

	auto back_position = m_drawPointer;
	m_drawPointer = front_position;
	if(!inputmode)
	{
		setNextItemIgnoreInput();
		m_textColor  = style->buttonTextIdleColor;
		_addText(m_currentClipRect, buildRect, text, _internal::strLen(text), style->buttonTextSpacing, style->buttonTextSpaceAddSize, false);
	}
	else
	{
		if( addTextInputPopup(size, m_inputTextBuffer.data(), 1024, 32, filter, 0) )
		{
			input_mode = false;
			m_lastKeyboardInputItemIdExit = m_uniqueIdCounter;
			m_inputTextBuffer_out = m_inputTextBuffer;
			result = true;
		}
		else
		{
			m_lastKeyboardInputItemId = m_uniqueIdCounter;
		}
	}
	m_drawPointer = back_position;
	return result;
}

// данная вещь предполагается использовать только один раз за кадр.
// он работает как popup окно (не технически а визуально). он включается например при нажатии ПКМ на valueSelector
// возможно нужно написать чуть по другому. чтобы было множество полей с вводом текста
bool Gui::GuiSystem::addTextInputPopup(const Vec2f& _size, char16_t* buf, size_t buf_size, size_t char_limit, bool(*filter)(char16_t), Style* style )
{
	static size_t cursor_position = 0; // text cursor
	static bool is_selected = false;
	static size_t select_begin = 0; // if(select_begin==select_end)then no selection
	static size_t select_end = 0;
	auto str_len = _internal::strLen(buf);
	auto _delete_selection = [&]()
	{

		size_t sel_1 = select_begin;
		size_t sel_2 = select_end;
		if( sel_1 > sel_2 )
		{
			sel_1 = sel_2;
			sel_2 = select_begin;
		}
		//printf("count %zu\n", sel_2 - sel_1);
		auto num_to_delete = sel_2 - sel_1;

		for(size_t i = sel_1; i < str_len; ++i)
		{
			buf[i] = buf[i+num_to_delete];
		}
		buf[str_len-num_to_delete] = 0;
		
		cursor_position = sel_1;

		is_selected = false;
	};
	if( m_IsEsc )
	{
		if(is_selected)
		{
			is_selected = false;
		}
	}
	else if( (m_IsCtrl && m_IsA) || m_IsLMBDouble)
	{
		is_selected = true;
		select_begin = 0;
		select_end = str_len;
		cursor_position = select_end;
	}
	else if( m_IsCtrl && m_IsX )
	{
		if(is_selected)
		{
			std::u16string strToClipboard;
			size_t sel_1 = select_begin;
			size_t sel_2 = select_end;
			if( sel_1 > sel_2 )
			{
				sel_1 = sel_2;
				sel_2 = select_begin;
			}
			auto num_to_select = sel_2 - sel_1;
			for(size_t i = sel_1; i < sel_2; ++i)
			{
				strToClipboard += buf[i];
			}
			CopyToClipboard(strToClipboard.c_str());
		}
		_delete_selection();
	}
	else if( m_IsCtrl && m_IsC )
	{
		if(is_selected)
		{
			std::u16string strToClipboard;
			size_t sel_1 = select_begin;
			size_t sel_2 = select_end;
			if( sel_1 > sel_2 )
			{
				sel_1 = sel_2;
				sel_2 = select_begin;
			}
			auto num_to_select = sel_2 - sel_1;
			for(size_t i = sel_1; i < sel_2; ++i)
			{
				strToClipboard += buf[i];
			}
			CopyToClipboard(strToClipboard.c_str());
		}
	}
	else if( m_IsCtrl && m_IsV )
	{
		if(is_selected)
		{
			_delete_selection();
			str_len = _internal::strLen(buf);
		}
		auto text = GetTextFromClipboard();
		auto text_size = text.size();
		if(text_size)
		{
			size_t i = str_len;
			while(i >= cursor_position)
			{
				auto next = i + text_size;
				if(next < char_limit)
				{
					buf[next] = buf[i];
					buf[i]=u' ';
				}
				if(i == 0)
					break;
				--i;
			}

			for( i = cursor_position; i < text_size; ++i )
			{
				buf[i] = text[i - cursor_position];
			}
		}
	}
	else if( m_IsDelete )
	{
		bool ok = false;
		if(is_selected)
		{
			_delete_selection();
		}
		else
		{
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
	}
	else if( m_IsBackspace )
	{
		bool ok = false;
		if(is_selected)
		{
			_delete_selection();
		}
		else
		{
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
	}
	else if( m_IsHome )
	{
		if( cursor_position > 0 )
		{
			if( m_IsShift )
			{
				if(!is_selected)
				{
					is_selected = true;
					select_begin = cursor_position;
				}
				select_end = 0;
			}
		}
		if( !m_IsShift )
		{
			is_selected = false;
		}
		cursor_position = 0;
	}
	else if( m_IsEnd )
	{
		if(cursor_position != str_len )
		{
			if( m_IsShift )
			{
				if(!is_selected)
				{
					is_selected = true;
					select_begin = cursor_position;
				}
				select_end = str_len;
			}
		}
		if( !m_IsShift )
		{
			is_selected = false;
		}
		cursor_position = str_len;
	}
	else if( m_IsLeft )
	{
		if( !m_IsShift )
		{
			if(is_selected)
			{
				is_selected = false;
				if(select_begin > select_end)
				{
					++cursor_position;
				}
				else
				{
					cursor_position = select_begin+1;
				}
			}
		}

		if( cursor_position > 0 )
		{
			if( m_IsShift )
			{
				if(!is_selected)
				{
					is_selected = true;
					select_begin = cursor_position;
				}
				select_end = cursor_position - 1;
			}
			--cursor_position;
		}
	}
	else if( m_IsRight )
	{
		if( m_IsShift )
		{
			if(!is_selected)
			{
				is_selected = true;
				select_begin = cursor_position;
			}
		}
		else
		{
			if(is_selected)
			{
				is_selected = false;
				if(select_begin > select_end)
				{
					cursor_position = select_begin-1;
				}
				else
				{
					--cursor_position;
				}
			}
		}

		++cursor_position;
		if( cursor_position > str_len )
			cursor_position = str_len;
		if(cursor_position == char_limit) --cursor_position;
	
		if( m_IsShift )
		{
			select_end = cursor_position;
			if(!str_len)
				is_selected = false;
		}
	}
	else if( m_character && filter && !m_IsCtrl )
	{
		if( filter(m_character) )
		{
			if(is_selected)
			{
				_delete_selection();
			}

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

	m_isEnterTextMode = true;
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
		--sel_2;
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
	m_isEnterTextMode = result ? false : true;
	return result;
}

