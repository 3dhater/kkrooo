#ifndef __GIZMO_H__
#define __GIZMO_H__

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "Scene3D/kkScene3DObject.h"

enum class GizmoPart : u32
{
	Default,
	X,
	Y,
	Z,
	XZ_plane,
	XY_plane,
	ZY_plane,
	Screen_plane
};

class Scene3DObject;
class Gizmo
{
	kkGraphicsSystem * m_gs  = nullptr;
	Application      * m_app = nullptr;
	
	Scene3DObject* m_gizmoMoveObjects[6];
	Scene3DObject* m_gizmoMoveObjects_col[6];

	Scene3DObject* m_gizmoScaleObjects[6];

	Scene3DObject* m_gizmoRotationObjects[3];
	Scene3DObject* m_gizmoRotationObjects_col[3];

	// m_matrixNoTranslation нужна для получение объекта лучём
	// в ней не должно быть перемещения, так как функция для луча использует m_pivot
	// сама-же матрица для рисовани m_matrix
	kkMatrix4   m_matrix, m_matrixNoTranslation;
	kkVector4   m_scaleSize; // при изменении масштаба это значение изменяется

	kkRayTriangleIntersectionResultSimple m_intersectionResult;
	
	GizmoPart m_gizmoPart = GizmoPart::Default;

	friend class Scene3D;

public:

	Gizmo();
	virtual ~Gizmo();

	void setGraphicsSystem( kkGraphicsSystem * );
	void init();
	GizmoPart updateInput(CursorRay*);
	void drawMove(const kkVector4&,f32, const kkRay& );
	void drawMove2D( v2i * cp, const v2i& point2d );
	void drawScale(const kkVector4&,f32, const kkRay& );
	void drawScale2D(v2i * cp, const v2i& point2d);
	void drawRotation(const kkVector4&,f32, const kkRay& );
	void drawRotation2D(v2i * cp, const v2f& vp_sz, const v4f& rect );
};

#endif