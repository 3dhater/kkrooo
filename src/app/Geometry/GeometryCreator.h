// SPDX-License-Identifier: GPL-3.0-only
#ifndef __GEOMETRY_CREATOR_H__
#define __GEOMETRY_CREATOR_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Math/kkVector4.h"
#include "Geometry/kkGeometryCreator.h"

class Scene3D;
class PolygonalModel;

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
	//bool m_isVertexBegin = false;

	// При импорте модели идёт построение этой модели
	// Если всё успешно, указатель должен передаться в Scene3D
	PolygonalModel * m_newModel = nullptr;

	// Модель состоит из полигонов. Минимум из 1го.
	// Полигон состоит из минимум 3х вершин.
	//kkPolygon * m_newPolygon = nullptr;

	// Для настройки позиций и прочего нужно вызывать set методы
	// Вызывать set методы нужно между beginVertex и endVertex
	// endVertex добавит текущую вершину в активный полигон
	//kkVertex * m_newVertex = nullptr;
	kkGeometryInformation m_geomInfo;

	// следующие переменные установятся в модель при вызове endModel
	kkString   m_newModel_name;
	v4f        m_newModel_position;


	Scene3D *  m_scene3D = nullptr;

	bool m_generateNormals     = false;
	bool m_generateFlatNormals = false;
	bool m_generateBT = false;
	
	bool m_triangulate = false;
	bool m_flipNormals = false;

	friend class Application;

public:
	GeometryCreator();
	virtual ~GeometryCreator();

	void BeginModel( const v4f& position = v4f() );
	void SetName( const char16_t* name_for_new_object );
	void BeginPolygon();
	//void beginVertex( u32 index );
	void AddPosition( f32 x, f32 y, f32 z );
	void AddColor( f32 r, f32 g, f32 b );
	void AddUV( f32 u, f32 v );
	void AddNormal( f32 x, f32 y, f32 z );
	void AddBinormal( f32 x, f32 y, f32 z );
	void AddTangent( f32 x, f32 y, f32 z );
	//void endVertex();
	void EndPolygon( bool weld_verts, bool flip );
	void GenerateNormals(bool flat);
	void GenerateBT();
	void Triangulate();
	void FlipNormals();
	kkScene3DObject* EndModel(bool weldByLen = false, f32 len = Epsilon);
	
	// если модель не доделалась или ещё что-то, то этот метод должен 
	// позаботиться об удалении ненужных объектов.
	// Он вызывается:
	//    -  после импорта модели.
	void Reset();
	
};

#endif