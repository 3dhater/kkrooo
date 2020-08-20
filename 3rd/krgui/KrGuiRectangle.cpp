#include "KrGui.h"

using namespace Kr;

void Gui::GuiSystem::_addRectangle(const Vec4f& clipRect, const Vec4f& buildRect, const Vec4f& _rounding )
{
	Vec4f rounding = _rounding;
	rounding *= m_guiZoom;

	Gui::DrawCommands * command = _getDrawCommand();
	command->clipRect = clipRect;
	command->texture.texture_address = 0;
	command->verts.clear();
	command->inds.clear();

	int numOfPoints = 1 + int(rounding.x);
	if( numOfPoints > 1 )
	{
		Vec2f center(buildRect.x + rounding.x,buildRect.y + rounding.x);
		auto len = (center.y - buildRect.y);
		float segmentSize  = Gui::DegToRad(90.f / float(numOfPoints-1));
		float angle = Gui::DegToRad(180.f);
		Vec2f point = center;
		point.y -= len;
		for( int i = 0; i < numOfPoints; ++i )
		{
			Gui::Vertex vertex;
			vertex.position = point;
			vertex.color = m_firstColor;
			vertex.textCoords.set(0.f,0.f);
			command->verts.push_back(vertex);

			point.x = len * std::sin(angle);
			point.y = len * std::cos(angle);
			point += center;

			angle += segmentSize;
		}
	}
	else
	{
		Gui::Vertex vertex;
		vertex.position = Vec2f(buildRect.x, buildRect.y);
		//vertex.color = KrColorRed;   // LEFT TOP
		vertex.color = m_firstColor;
		vertex.textCoords.set(0.f,0.f);
		command->verts.push_back(vertex);
	}
	
	numOfPoints = 1 + int(rounding.y);
	if( numOfPoints > 1 )
	{
		Vec2f center(buildRect.x + rounding.y,buildRect.w - rounding.y);
		float   len = (buildRect.w - center.y);
		float segmentSize  = Gui::DegToRad(90.f / float(numOfPoints-1));
		float angle = Gui::DegToRad(270.f);
		Vec2f point = center;
		point.x -= len;
		for( int i = 0; i < numOfPoints; ++i )
		{
			Gui::Vertex vertex;
			vertex.position = point;
			vertex.color = m_firstColor;
			vertex.textCoords.set(0.f,0.f);
			command->verts.push_back(vertex);
				 
			point.x = len * std::sin(angle);
			point.y = len * std::cos(angle);
			point += center;

			angle += segmentSize;
		}
	}
	else
	{
		Gui::Vertex vertex;
		vertex.position = Vec2f(buildRect.x, buildRect.w);
		vertex.color = m_secondColor;    // LEFT BOTTOM
		vertex.textCoords.set(0.f,0.f);
		command->verts.push_back(vertex);
	}

	numOfPoints = 1 + int(rounding.z);
	if( numOfPoints > 1 )
	{
		Vec2f center(buildRect.z - rounding.z,buildRect.w - rounding.z);
		float   len = (buildRect.w - center.y);
		float segmentSize  = Gui::DegToRad(90.f / float(numOfPoints-1));
		float angle = Gui::DegToRad(360.f);
		Vec2f point = center;
		point.y += len;
		for( int i = 0; i < numOfPoints; ++i )
		{
			Gui::Vertex vertex;
			vertex.position = point;
			vertex.color = m_firstColor;
			vertex.textCoords.set(0.f,0.f);
			
			point.x = len * std::sin(angle);
			point.y = len * std::cos(angle);
			point += center;

			angle += segmentSize;
			command->verts.push_back(vertex);
		}
	}
	else
	{
		Gui::Vertex vertex;
		vertex.position = Vec2f(buildRect.z, buildRect.w);
		vertex.color = m_secondColor;
		vertex.textCoords.set(0.f,0.f);
		command->verts.push_back(vertex);
	}

	numOfPoints = 1 + int(rounding.w);
	if( numOfPoints > 1 )
	{
		Vec2f center(buildRect.z - rounding.w,buildRect.y + rounding.w);
		float   len = (center.y - buildRect.y);
		float segmentSize  = Gui::DegToRad(90.f / float(numOfPoints-1));
		float angle = Gui::DegToRad(90.f);
		Vec2f point = center;
		point.x += len;
		for( int i = 0; i < numOfPoints; ++i )
		{
			Gui::Vertex vertex;
			vertex.position = point;
			vertex.color = m_firstColor;
			vertex.textCoords.set(0.f,0.f);
			
			command->verts.push_back(vertex);

			point.x = len * std::sin(angle);
			point.y = len * std::cos(angle);
			point += center;

			angle += segmentSize;
		}
	}
	else
	{
		Gui::Vertex vertex;
		vertex.position = Vec2f(buildRect.z, buildRect.y);
		vertex.color = m_firstColor;
		vertex.textCoords.set(0.f,0.f);
		command->verts.push_back(vertex);
	}


	// create triangles
	int indexCounter = 0;
	for( size_t i = 0, sz = command->verts.size() - 2; i < sz; ++i )
	{
		command->inds.push_back(0);
		command->inds.push_back(1+indexCounter);
		command->inds.push_back(2+indexCounter);
		++indexCounter;
	}
}

void Gui::GuiSystem::addRectangle( 
	Gui::Style* style, 
	const Vec2f& _size, 
	float Alpha, 
	const Vec4f& rounding 
)
{
	//auto oldDrawPointer = m_drawPointer;

	_checkStyle(&style); 
	_newId();

	Vec2f size = _size;
	_checkSize(&size);

	// обязательно вычисление дальней точки текущего объекта
	// это можно использовать ниже для определения, нужно ли переходить
	// на след. строку
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

	m_firstColor  = style->rectangleIdleColor1;
	m_secondColor = style->rectangleIdleColor2;

	// buildRect нужен для правильного построения геометрии, так как 
	// m_currentClipRect ниже может сузиться, и это будет заметно с текстурой или цветом
	auto buildRect = m_currentClipRect;

	// обновить clipRect, чтобы объект рисовался в области родителя
	_checkParentClipRect();

	if(m_nextItemIgnoreInput || m_blockInputGlobal)
		m_nextItemIgnoreInput = false;
	else
		_checkMouseHover();

	if( m_lastCursorHoverItemId == m_uniqueIdCounter || m_lastCursorMoveItemId  == m_uniqueIdCounter )
	{
		m_firstColor  = style->rectangleHoverColor1;
		m_secondColor = style->rectangleHoverColor2;
	}

	m_firstColor.w  = Alpha;
	m_secondColor.w = Alpha;
	_addRectangle(m_currentClipRect, buildRect, rounding);

	_checkItemHeight(size.y);
	m_drawPointer.x += size.x;
}