// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO__GEOMETRY_CREATOR_H__
#define __KKROO__GEOMETRY_CREATOR_H__

#include "Classes/Math/kkVector4.h"
#include "Common/kkForward.h"

class kkGeometryCreator
{
protected:

public:

	kkGeometryCreator(){}
	virtual ~kkGeometryCreator(){}

	// On each model(mesh buffer) - call 2 methods: begin_model and end_model.
	// Allocate memory for new model
	virtual void beginModel( const v4f& position = v4f() ) = 0;

	virtual void setName( const char16_t* name_for_new_object ) = 0;

	// On each polygon - call 2 methods: begin_polygon and end_polygon.
	// Allocate memory for new polygon
	virtual void beginPolygon() = 0;
		
	// On each vertex - call 2 methods: begin_vertex and end_vertex.
	// Allocate memory for new vertex
	virtual void beginVertex( u32 index ) = 0;
		
	// call before add_vertex
	// call between methods begin_polygon and end_polygon
	virtual void setPosition( f32 x, f32 y, f32 z ) = 0;
	virtual void setColor( f32 r, f32 g, f32 b ) = 0;
	virtual void setUV( f32 u, f32 v ) = 0;
	virtual void setNormal( f32 x, f32 y, f32 z ) = 0;
	virtual void setBinormal( f32 x, f32 y, f32 z ) = 0;
	virtual void setTangent( f32 x, f32 y, f32 z ) = 0;

	// add vertex to polygon
	// call between methods begin_polygon and end_polygon
	virtual void endVertex() = 0;

	virtual void endPolygon( bool weld_verts, bool triangulate, bool flip ) = 0;

	virtual void generateNormals(bool flat) = 0;
	virtual void generateBT() = 0; /// binormals and tangents
	
	// forceWeld - only if vertex with welding (endPolygon(true,......))
	virtual kkScene3DObject* endModel( bool forceWeld = false, f32 len = Epsilon ) = 0;
};


#endif