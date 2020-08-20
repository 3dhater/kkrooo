// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Scene3D/kkScene3DObject.h"

#include "GeometryCreator.h"
#include "PolygonalModel.h"
#include "Polygon3D.h"
#include "Vertex.h"

#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"

GeometryCreator::GeometryCreator()
{
}

GeometryCreator::~GeometryCreator()
{
}

void GeometryCreator::beginModel( const v4f& position )
{
	if( !m_isModelBegin )
	{
		m_isModelBegin = true;

		m_newModel = kkCreate<PolygonalModel>();
		m_newModel_position = position;
	}
	else
	{
		fprintf(stderr, "`beginModel` must be called only once on each model.\n");
	}
}

void GeometryCreator::setName( const char16_t* name_for_new_object )
{
	m_newModel_name = name_for_new_object;
}

void GeometryCreator::beginPolygon()
{
	if( m_isModelBegin )
	{
		if(!m_isPolygonBegin)
		{
			m_isPolygonBegin = true;
			m_newPolygon = kkCreate<Polygon3D>();
		}
		else
		{
			fprintf(stderr, "`beginPolygon` must be called only once on each polygon.\n");
		}
	}
}

void GeometryCreator::beginVertex( u32 index )
{
	if( m_isPolygonBegin )
	{
		if( !m_isVertexBegin )
		{
			m_isVertexBegin = true;
			m_newVertex = kkCreate<Vertex>();
		}
		else
		{
			fprintf(stderr, "`beginVertex` must be called only once on each vertex.\n");
		}
	}
}

void GeometryCreator::setPosition( f32 x, f32 y, f32 z )
{
	if( m_isVertexBegin )
	{
		m_newVertex->m_Position.set(x,y,z);
		m_newVertex->m_Position_fix.set(x,y,z);
	}
}

void GeometryCreator::setColor( f32 r, f32 g, f32 b )
{
	if( m_isVertexBegin )
	{
		m_newVertex->m_Color.set(r,g,b);
	}
}

void GeometryCreator::setUV( f32 u, f32 v )
{
	if( m_isVertexBegin )
	{
		m_newVertex->m_UV.set(u,v,0.f,0.f);
	}
}

void GeometryCreator::setNormal( f32 x, f32 y, f32 z )
{
	if( m_isVertexBegin )
	{
		m_newVertex->m_Normal.set(x,y,z);
		m_newVertex->m_Normal_fix.set(x,y,z);
	}
}

void GeometryCreator::setBinormal( f32 x, f32 y, f32 z )
{
	if( m_isVertexBegin )
	{
		//m_newVertex->m_Binormal.set(x,y,z);
	}
}

void GeometryCreator::setTangent( f32 x, f32 y, f32 z )
{
	if( m_isVertexBegin )
	{
		//m_newVertex->m_Tangent.set(x,y,z);
	}
}

void GeometryCreator::endVertex()
{
	if( m_isVertexBegin )
	{
		// передача вершины в текущий полигон
		m_newPolygon->addVertex(m_newVertex);

		m_newVertex     = nullptr;
		m_isVertexBegin = false;
	}
}

void GeometryCreator::endPolygon( bool weld_verts, bool triangulate, bool flip )
{
	if( m_isPolygonBegin )
	{
		// Передача полигона в модель здесь
		if( m_newPolygon->m_verts.size() < 3)
		{
			kkDestroy(m_newPolygon);
			fprintf(stderr, "`endPolygon` - polygon must have minimum 3 verts.\n");
		}
		else
		{
			m_newModel->addPolygon(m_newPolygon, weld_verts, triangulate, flip);
		}
		m_newPolygon     = nullptr;
		m_isPolygonBegin = false;
	}
}

void GeometryCreator::generateNormals(bool flat)
{
	m_generateNormals     = true;
	m_generateFlatNormals = flat;
}

void GeometryCreator::generateBT()
{
	if( m_isModelBegin )
	{
	}
}


kkScene3DObject* GeometryCreator::endModel(bool forceWeld, f32 len)
{
	kkScene3DObject* ptr = nullptr;
	if( m_isModelBegin )
	{

		// передаём указатель в Scene3D и обнуляем m_newModel
		if( m_newModel->getPolygonCount() == 0 )
		{
			kkDestroy(m_newModel);
			fprintf(stderr, "`endModel` - model must have minimum 1 polygon.\n");
		}
		else
		{
			if( forceWeld )
			{
				m_newModel->weldByLen(len);
			}

			m_newModel->createControlPoints();

			if( m_generateNormals )
				m_newModel->generateNormals(m_generateFlatNormals);


			ptr = m_scene3D->createNewPolygonalObject(m_newModel_name.data(), m_newModel, m_newModel_position);


			m_newModel->onEndCreation();

		}

		m_newModel            = nullptr;
		m_isModelBegin        = false;
		m_generateNormals     = false;
		m_generateFlatNormals = false;
		m_triangulate = false;
		m_flipNormals = false;
	}

	return ptr;
}


void GeometryCreator::reset()
{
	if( m_isModelBegin )
	{
		// этого не должно было быть
		// удаляем модель
		kkDestroy( m_newModel );
		m_newModel     = nullptr;
		m_isModelBegin = false;
		m_generateNormals     = false;
		m_generateFlatNormals = false;
	}

	if( m_isPolygonBegin )
	{
		// этого не должно было быть
		kkDestroy( m_newPolygon );
		m_newPolygon = nullptr;
		m_isPolygonBegin = false;
	}

	if( m_isVertexBegin )
	{
		kkDestroy(m_newVertex);
		m_newVertex     = nullptr;
		m_isVertexBegin = false;
	}

	m_triangulate = false;
	m_flipNormals = false;
}

void GeometryCreator::triangulate()
{
	m_triangulate = true;
}

void GeometryCreator::flipNormals()
{
	m_flipNormals = true;
}