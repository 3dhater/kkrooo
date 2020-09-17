#ifndef __KKROO__GEOMETRY_CREATOR_H__
#define __KKROO__GEOMETRY_CREATOR_H__

#include "Classes/Math/kkVector4.h"
#include "Common/kkForward.h"

struct kkGeometryInformation
{
	kkArray<v3f> m_position = kkArray<v3f>(4);
	kkArray<v3f> m_color    = kkArray<v3f>(4);
	kkArray<v3f> m_normal   = kkArray<v3f>(4);
	kkArray<v3f> m_binormal   = kkArray<v3f>(4);
	kkArray<v3f> m_tangent   = kkArray<v3f>(4);
	kkArray<v2f> m_uv       = kkArray<v2f>(4);

	void clear()
	{
		m_position.clear();
		m_color.clear();
		m_normal.clear();
		m_binormal.clear();
		m_tangent.clear();
		m_uv.clear();
	}
};

class kkGeometryCreator
{
protected:

public:

	kkGeometryCreator(){}
	virtual ~kkGeometryCreator(){}

	// On each model(mesh buffer) - call 2 methods: begin_model and end_model.
	// Allocate memory for new model
	virtual void BeginModel( const v4f& position = v4f() ) = 0;

	virtual void SetName( const char16_t* name_for_new_object ) = 0;

	// On each polygon - call 2 methods: begin_polygon and end_polygon.
	// Allocate memory for new polygon
	virtual void BeginPolygon() = 0;
		
	// On each vertex - call 2 methods: begin_vertex and end_vertex.
	// Allocate memory for new vertex
	//virtual void beginVertex( u32 index ) = 0;
		
	// call before add_vertex
	// call between methods begin_polygon and end_polygon
	virtual void AddPosition( f32 x, f32 y, f32 z ) = 0;
	virtual void AddColor( f32 r, f32 g, f32 b ) = 0;
	virtual void AddUV( f32 u, f32 v ) = 0;
	virtual void AddNormal( f32 x, f32 y, f32 z ) = 0;
	virtual void AddBinormal( f32 x, f32 y, f32 z ) = 0;
	virtual void AddTangent( f32 x, f32 y, f32 z ) = 0;

	// add vertex to polygon
	// call between methods begin_polygon and end_polygon
	//virtual void EndVertex() = 0;

	virtual void EndPolygon( bool weld_verts, bool triangulate, bool flip ) = 0;

	virtual void GenerateNormals(bool flat) = 0;
	virtual void GenerateBT() = 0; /// binormals and tangents
	
	// forceWeld - only if vertex with welding (endPolygon(true,......))
	virtual kkScene3DObject* EndModel( bool forceWeld = false, f32 len = Epsilon ) = 0;
};


#endif