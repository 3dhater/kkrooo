#include "KrGui.h"

using namespace Kr;

void Gui::GuiSystem::_addText(const Vec4f& clipRect, const Vec4f& textRect, const char16_t* text, int numOfChars, 
	float spacing, float spaceAdd,
	bool centerOnZoom
	)
{
	Gui::DrawCommands * command = _getDrawCommand();
	command->inds.clear();
	command->verts.clear();
	command->clipRect = clipRect;
	auto char_last = numOfChars - 1;
	int last_texture = -1;
	int index_index = 0;

	Vec2f text_pointer;
	text_pointer.x = textRect.x;
	text_pointer.y = textRect.y;

	for( int i = 0; i < numOfChars; ++i )
	{
		auto g = m_currentFont->m_glyphs[ text[i] ];

		if(g.symbol != text[i])
			continue;
		
		if( last_texture != g.textureID && last_texture != -1 )
		{
			command = _getDrawCommand();
			command->inds.clear();
			command->verts.clear();
			command->clipRect = clipRect;
			index_index = 0;
		}

		command->texture.texture_address = m_currentFont->m_textures[g.textureID]->texture_address;

		Gui::Vertex vertex1, vertex2, vertex3, vertex4;
		
		auto TXN = text_pointer.x;
		auto TXP = TXN + g.width;
		auto TYN = text_pointer.y;
		auto TYP = TYN + g.height;

		auto centerX = TXN;
		auto centerY = TYN;
		
		TXP -= centerX; TXP = ( TXP * m_guiZoom ); TXP += centerX;
		TYP -= centerY; TYP = ( TYP * m_guiZoom ); TYP += centerY;

		vertex1.position.set(TXN, TYN); // LT
		vertex2.position.set(TXN, TYP); // LB

		vertex3.position.set(TXP, TYP); // RB
		vertex4.position.set(TXP, TYN); // RT

		vertex1.color = m_textColor;
		vertex2.color = m_textColor;
		vertex3.color = m_textColor;
		vertex4.color = m_textColor;

		vertex1.textCoords = g.lt;
		vertex2.textCoords = g.lb;
		vertex3.textCoords = g.rb;
		vertex4.textCoords = g.rt;

		command->inds.push_back(index_index);
		command->inds.push_back(index_index+1);
		command->inds.push_back(index_index+2);
		command->inds.push_back(index_index);
		command->inds.push_back(index_index+2);
		command->inds.push_back(index_index+3);

		index_index += 4;

		last_texture = g.textureID;
		text_pointer.x += (g.width)*m_guiZoom;
		text_pointer.x += spacing*m_guiZoom;
		
		if( g.symbol == u' ' )
		{
			text_pointer.x += spaceAdd*m_guiZoom;		
		}

		command->verts.push_back(vertex1);
		command->verts.push_back(vertex2);
		command->verts.push_back(vertex3);
		command->verts.push_back(vertex4);
	}
}

void Gui::GuiSystem::addText(const char* text, Gui::Style* style)
{
	char16_t buffer[256];
	auto src = text; 
	auto dst = &buffer[0]; 
	while((int)*src)
	{
		*dst = (char16_t)*src;
		++dst; ++src;
	}
	*dst = (char16_t)*src; // 0
	addText(buffer, style);
}

void Gui::GuiSystem::addText(Style* style, const char16_t* fmt, ...)
{
	std::u16string text;
	va_list args;
    va_start(args, fmt);
	_internal::deformat(fmt, args, text);
    va_end(args);
	addText(text.data(), style);
}

void Gui::GuiSystem::addText(const char16_t* text, Gui::Style* style)
{
	if(!text) return;
	//auto oldDrawPointer = m_drawPointer;
	_checkStyle(&style);
	_newId();

	// get size of the text in pixels
	Vec2f size;
	int text_strLen = getTextLen(text,&size,style->commonTextSpacing, style->commonTextSpaceAddSize);
	
	auto end_x_position = m_drawPointer.x + size.x;
	_checkNewLine(end_x_position);

	auto position = m_drawPointer;
	if( m_currentNodePosition )
	{
		position.x = m_currentNodePosition->x + m_currentNodeContentOffset.x;
		position.y = m_currentNodePosition->y + m_currentNodeContentOffset.y;

		position.x += m_currentNodeEditor->m_eyePosition.x;
		position.y += m_currentNodeEditor->m_eyePosition.y;

		position.x = ( position.x * m_guiZoom );
		position.y = ( position.y * m_guiZoom );

		auto centerOffset = m_currentNodeEditor->m_center;

		position.x += centerOffset.x;
		position.y += centerOffset.y;
	}

	_setCurrentClipRect(position, size);

	m_currentClipRect.y += style->commonTextYOffset;
	m_currentClipRect.w += style->commonTextYOffset;

	auto buildRect = m_currentClipRect;
	
	//Vec2f size( m_currentClipRect.z - m_currentClipRect.x, m_currentClipRect.w - m_currentClipRect.y );

	// нужно обновить
	/*if( m_currentNodePosition )
	{
		m_currentClipRect.x = position.x;
		m_currentClipRect.y = position.y;
		m_currentClipRect.z = position.x + size.x;
		m_currentClipRect.w = position.y + size.y;

		if( m_currentClipRect.x < m_currentNodeClipRect.x ) m_currentClipRect.x = m_currentNodeClipRect.x;
		if( m_currentClipRect.y < m_currentNodeClipRect.y ) m_currentClipRect.y = m_currentNodeClipRect.y;
		if( m_currentClipRect.z > m_currentNodeClipRect.z ) m_currentClipRect.z = m_currentNodeClipRect.z;
		if( m_currentClipRect.w > m_currentNodeClipRect.w ) m_currentClipRect.w = m_currentNodeClipRect.w;
	}*/

	_checkParentClipRect();


	/*if( !m_nextItemIgnoreInput && !m_blockInputGlobal)
		_updateMouseInput(mouseButton::LMB);
	else m_nextItemIgnoreInput = false;*/
	if(m_nextItemIgnoreInput || m_blockInputGlobal)
		m_nextItemIgnoreInput = false;
	else
		_checkMouseHover();

	if((m_lastCursorMoveItemId == m_uniqueIdCounter) 
		|| (m_lastCursorHoverItemId == m_uniqueIdCounter))
	{
		if(m_mouseIsLMB_firstClick)
		{
			m_pressedItemIdLMB = m_uniqueIdCounter;
		}
	}

	m_textColor = style->commonTextColor;
	m_textColor.w = style->commonTextAlpha;

	_addText( m_currentClipRect, buildRect, text, text_strLen,  style->commonTextSpacing, style->commonTextSpaceAddSize, /*m_currentNodePosition ? true :*/ false );
	
	_checkItemHeight(size.y);
	m_drawPointer.x += size.x;
}