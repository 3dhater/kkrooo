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

void Polygon3D::Flip()
{
	for( u64 i = 0, sz = m_verts.size(); i < sz/2; ++i )
	{
		auto v = m_verts[ i ];
		auto index = sz - (1+i);
		m_verts[ i ] = m_verts[ index ];
		m_verts[ index ] = v;
	}
}

kkVector4& Polygon3D::GetNormal()
{
	return m_facenormal;
}

void Polygon3D::CalculateNormals()
{
	kkVector4 e1, e2, no;
	auto num_of_verts = (u64)m_verts.size();
	for( u64 i2 = 0, sz2 = num_of_verts; i2 < sz2; ++i2 )
	{
		((Vertex*)m_verts[ i2 ])->m_Normal.set( 0.f, 0.f, 0.f );
	}
	u64 index2, index3;
	for( u64 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
	{
		index2  = i2+1;
		index3  = index2 + 1;
		if( index3 == num_of_verts )
			index3 = 0;

		auto vertex2 = (Vertex*)m_verts[ 0 ];
		auto vertex1 = (Vertex*)m_verts[ index2 ];
		auto vertex3 = (Vertex*)m_verts[ index3 ];

		e1 = vertex2->m_Position - vertex1->m_Position;
		e2 = vertex3->m_Position - vertex1->m_Position;
		//no;
		e1.cross(e2, no);

		vertex1->m_Normal -= no;
		vertex2->m_Normal -= no;
		vertex3->m_Normal -= no;
	}

	for( u64 i2 = 0, sz2 = num_of_verts; i2 < sz2; ++i2 )
	{
		((Vertex*)m_verts[ i2 ])->m_Normal.normalize2();
		((Vertex*)m_verts[ i2 ])->m_Normal_fix = ((Vertex*)m_verts[ i2 ])->m_Normal;
	}
	m_facenormal = ((Vertex*)m_verts[ 0 ])->m_Normal;
}
