#include "kkrooo.engine.h"
#include "../Common.h"

#include "Scene3D/kkScene3DObject.h"
#include "Geometry/kkPolygonalModel.h"


#include "GeometryCreator.h"
#include "PolygonalModel.h"

#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"

GeometryCreator::GeometryCreator()
{
}

GeometryCreator::~GeometryCreator()
{
}

void GeometryCreator::BeginModel( const v4f& position )
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

void GeometryCreator::SetName( const char16_t* name_for_new_object )
{
	m_newModel_name = name_for_new_object;
}

void GeometryCreator::BeginPolygon()
{
	if( m_isModelBegin )
	{
		if(!m_isPolygonBegin)
		{
			m_isPolygonBegin = true;
			m_geomInfo.clear();
		}
		else
		{
			fprintf(stderr, "`beginPolygon` must be called only once on each polygon.\n");
		}
	}
}

void GeometryCreator::AddPosition( f32 x, f32 y, f32 z )
{
	m_geomInfo.m_position.push_back(v3f(x,y,z));
}

void GeometryCreator::AddColor( f32 r, f32 g, f32 b )
{
	m_geomInfo.m_color.push_back(v3f(r,g,b));
}

void GeometryCreator::AddUV( f32 u, f32 v )
{
	m_geomInfo.m_uv.push_back(v2f(u,v));
}

void GeometryCreator::AddNormal( f32 x, f32 y, f32 z )
{
	m_geomInfo.m_normal.push_back(v3f(x,y,z));
}

void GeometryCreator::AddBinormal( f32 x, f32 y, f32 z )
{
	m_geomInfo.m_binormal.push_back(v3f(x,y,z));
}

void GeometryCreator::AddTangent( f32 x, f32 y, f32 z )
{
	m_geomInfo.m_tangent.push_back(v3f(x,y,z));
}

void GeometryCreator::EndPolygon( bool weld_verts, bool triangulate, bool flip )
{
	if( m_isPolygonBegin )
	{
		// Передача полигона в модель здесь
		if( m_geomInfo.m_position.size() < 3)
		{
			fprintf(stderr, "`endPolygon` - polygon must have minimum 3 verts.\n");
		}
		else
		{
			m_newModel->AddPolygon(&m_geomInfo, weld_verts, triangulate, flip);
		}
		m_isPolygonBegin = false;
	}
}

void GeometryCreator::GenerateNormals(bool flat)
{
	m_generateNormals     = true;
	m_generateFlatNormals = flat;
}

void GeometryCreator::GenerateBT()
{
	m_generateBT = true;
}


kkScene3DObject* GeometryCreator::EndModel(bool forceWeld, f32 len)
{
	kkScene3DObject* ptr = nullptr;
	if( m_isModelBegin )
	{

		// передаём указатель в Scene3D и обнуляем m_newModel
		if( !m_newModel->m_polygonsCount )
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

			if( m_generateNormals )
				m_newModel->generateNormals(m_generateFlatNormals);

			if( m_generateBT )
				m_newModel->generateBT();

			m_newModel->onEndCreation();
			ptr = m_scene3D->createNewPolygonalObject(m_newModel_name.data(), m_newModel, m_newModel_position);
		}

		m_newModel            = nullptr;
		m_isModelBegin        = false;
		m_generateNormals     = false;
		m_generateBT = false;
		m_generateFlatNormals = false;
		m_triangulate = false;
		m_flipNormals = false;
	}

	return ptr;
}

void GeometryCreator::Reset()
{
	if( m_isModelBegin )
	{
		// этого не должно было быть
		// удаляем модель
		kkDestroy( m_newModel );
		m_newModel     = nullptr;
		m_isModelBegin = false;
		m_generateNormals     = false;
		m_generateBT = false;
		m_generateFlatNormals = false;
	}

	if( m_isPolygonBegin )
	{
		// этого не должно было быть
		m_isPolygonBegin = false;
	}

	m_triangulate = false;
	m_flipNormals = false;
}

void GeometryCreator::Triangulate()
{
	m_triangulate = true;
}

void GeometryCreator::FlipNormals()
{
	m_flipNormals = true;
}