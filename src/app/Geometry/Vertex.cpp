// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Vertex.h"


Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

void Vertex::set(Vertex* other)
{
	m_Boneinds = other->m_Boneinds;
	m_Color = other->m_Color;
	m_Normal = other->m_Normal;
	m_Normal_fix = other->m_Normal_fix;
	m_Position = other->m_Position;
	m_Position_fix = other->m_Position_fix;
	m_UV = other->m_UV;
	m_Weights = other->m_Weights;
	m_weld = other->m_weld;

	m_parentPolygon = other->m_parentPolygon;
	m_controlVertex = other->m_controlVertex;
}

kkVector4    Vertex::getPosition()
{
	return m_Position;
}

kkVector4    Vertex::getUV()
{
	return m_UV;
}

kkVector4    Vertex::getNormal()
{
	return m_Normal;
}

//kkVector4    Vertex::getBinormal()
//{
//	return kkVector4();
//}
//
//kkVector4    Vertex::getTangent()
//{
//	return kkVector4();
//}

kkVector4    Vertex::getBoneInds()
{
	return m_Boneinds;
}

kkVector4    Vertex::getWeights()
{
	return m_Weights;
}


// -----------------------------------------------------------
// -----------------------------------------------------------

ControlVertex::ControlVertex()
{
}

ControlVertex::~ControlVertex()
{
}

kkArray<kkVertex*>& ControlVertex::getVerts()
{
	return m_verts;
}

//kkArray<u32>& ControlVertex::getVertInds()
//{
//	return m_vertexIndex;
//}

bool ControlVertex::isSelected()
{
	return m_isSelected;
}

bool ControlVertex::isSelectedEdge()
{
	return m_isSelected_edge;
}

bool ControlVertex::isSelectedPoly()
{
	return m_isSelected_poly;
}

void ControlVertex::select()
{
	m_isSelected = true;
}

void ControlVertex::deselect()
{
	m_isSelected = false;
}

u32 ControlVertex::getIndex()
{
	return m_index;
}

kkVector4& ControlVertex::getAverageFaceNormal()
{
	return m_faceNormal;
}
