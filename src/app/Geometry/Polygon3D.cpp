// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Polygon3D.h"
#include "Vertex.h"



Polygon3D::Polygon3D()
{
}

Polygon3D::~Polygon3D()
{
}

kkArray<kkVertex*>& Polygon3D::GetVerts()
{
	return m_verts;
}

//kkArray<u32>& Polygon3D::GetVertInds()
//{
//	return m_vertsInds;
//}

kkArray<u32>& Polygon3D::GetControlVertInds()
{
	return m_controlVertsInds;
}

void      Polygon3D::addVertex(Vertex*v)
{
//	v->m_UV._f32[1] = 1.f - v->m_UV._f32[1];
	m_verts.push_back(v);
}

void Polygon3D::MarkToDelete()
{
	m_toDelete = true;
}

void Polygon3D::RemoveMarkToDelete()
{
	m_toDelete = false;
}

bool Polygon3D::IsToDelete()
{
	return m_toDelete;
}

bool Polygon3D::IsSelected()
{
	return m_isSelected;
}

void Polygon3D::Select()
{
	m_isSelected = true;
}

void Polygon3D::Deselect()
{
	m_isSelected = false;
}

