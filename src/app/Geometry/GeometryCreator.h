// SPDX-License-Identifier: GPL-3.0-only
#ifndef __GEOMETRY_CREATOR_H__
#define __GEOMETRY_CREATOR_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Math/kkVector4.h"
#include "Geometry/kkGeometryCreator.h"

class Scene3D;

class PolygonalModel;
class Polygon3D;
class Vertex;

class GeometryCreator : public kkGeometryCreator
{

	// вызов beginModel устанавливает значение true
	// вызов endModel устанавливает значение false
	bool m_isModelBegin = false;
	
	// вызов beginPolygon устанавливает значение true
	// вызов endPolygon устанавливает значение false
	// вызывать нужно между beginModel и endModel
	bool m_isPolygonBegin = false;
	
	// вызов beginVertex устанавливает значение true
	// вызов endVertex устанавливает значение false
	// вызывать нужно между beginPolygon и endPolygon
	bool m_isVertexBegin = false;

	// При импорте модели идёт построение этой модели
	// Если всё успешно, указатель должен передаться в Scene3D
	PolygonalModel * m_newModel = nullptr;

	// Модель состоит из полигонов. Минимум из 1го.
	// Полигон состоит из минимум 3х вершин.
	Polygon3D * m_newPolygon = nullptr;

	// Для настройки позиций и прочего нужно вызывать set методы
	// Вызывать set методы нужно между beginVertex и endVertex
	// endVertex добавит текущую вершину в активный полигон
	Vertex * m_newVertex = nullptr;

	// следующие переменные установятся в модель при вызове endModel
	kkString   m_newModel_name;
	v4f        m_newModel_position;


	Scene3D *  m_scene3D = nullptr;

	bool m_generateNormals     = false;
	bool m_generateFlatNormals = false;
	
	bool m_triangulate = false;
	bool m_flipNormals = false;

	friend class Application;

public:
	GeometryCreator();
	virtual ~GeometryCreator();

	void beginModel( const v4f& position = v4f() );
	void setName( const char16_t* name_for_new_object );
	void beginPolygon();
	void beginVertex( u32 index );
	void setPosition( f32 x, f32 y, f32 z );
	void setColor( f32 r, f32 g, f32 b );
	void setUV( f32 u, f32 v );
	void setNormal( f32 x, f32 y, f32 z );
	void setBinormal( f32 x, f32 y, f32 z );
	void setTangent( f32 x, f32 y, f32 z );
	void endVertex();
	void endPolygon( bool weld_verts, bool triangulate, bool flip );
	void generateNormals(bool flat);
	void generateBT();
	void triangulate();
	void flipNormals();
	kkScene3DObject* endModel(bool weldByLen = false, f32 len = Epsilon);
	
	// если модель не доделалась или ещё что-то, то этот метод должен 
	// позаботиться об удалении ненужных объектов.
	// Он вызывается:
	//    -  после импорта модели.
	void reset();
	
};

#endif