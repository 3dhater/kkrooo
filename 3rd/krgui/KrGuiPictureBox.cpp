#include "KrGui.h"

using namespace Kr;

void Gui::GuiSystem::addPictureBox(const Vec2f& _size, unsigned long long textureAddress, float Alpha, const Vec4f& color )
{
	Vec2f size = _size;
	_checkSize(&size);

	auto oldDrawPointer = m_drawPointer;

	_newId();

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

	Vec4f buildRect = m_currentClipRect;
	_checkParentClipRect();
	
	if( !m_nextItemIgnoreInput && !m_blockInputGlobal )
		_updateMouseInput(mouseButton::LMB);
	else m_nextItemIgnoreInput = false;

	m_firstColor    = color;
	m_secondColor   = color;
	m_firstColor.w  = Alpha;
	m_secondColor.w = Alpha;

	if( textureAddress == 0 )
		textureAddress = (unsigned long long)(&m_defaultFont->m_defaultTexture->texture_address);

	_addPictureBox(m_currentClipRect, buildRect, textureAddress );
	m_drawPointer.x += m_currentClipRect.z - m_currentClipRect.x;
}

void Gui::GuiSystem::_addPictureBox(const Vec4f& clipRect, const Vec4f& buildRect, unsigned long long textureAddress)
{
	Gui::DrawCommands * command = _getDrawCommand();
	command->texture.texture_address = textureAddress;
	command->clipRect = clipRect;
	Gui::Vertex vertex1;
	Gui::Vertex vertex2;
	Gui::Vertex vertex3;
	Gui::Vertex vertex4;
	vertex1.position.set(buildRect.x, buildRect.y);
	vertex2.position.set(buildRect.x, buildRect.w);
	vertex3.position.set(buildRect.z, buildRect.w);
	vertex4.position.set(buildRect.z, buildRect.y);
	vertex1.textCoords.set( 0, 0 );
	vertex2.textCoords.set( 0, 1 );
	vertex3.textCoords.set( 1, 1 );
	vertex4.textCoords.set( 1, 0 );
	vertex1.color = m_firstColor;
	vertex2.color = m_secondColor;
	vertex3.color = m_secondColor;
	vertex4.color = m_firstColor;
	command->inds.clear();
	command->inds.push_back(0);
	command->inds.push_back(1);
	command->inds.push_back(2);
	command->inds.push_back(0);
	command->inds.push_back(2);
	command->inds.push_back(3);
	command->verts.clear();
	command->verts.push_back(vertex1);
	command->verts.push_back(vertex2);
	command->verts.push_back(vertex3);
	command->verts.push_back(vertex4);
}