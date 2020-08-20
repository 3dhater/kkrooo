// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_3D_SCENE_OBJECT_H__
#define __KK_3D_SCENE_OBJECT_H__

//#include "boost/container/vector.hpp"
//#include "boost/unordered_map.hpp"

#include "Common/kkForward.h"

#include "Classes/Containers/kkArray.h"
#include "Classes/Strings/kkString.h"
#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMatrix.h"
#include "Classes/kkAABB.h"
#include "Classes/kkOBB.h"

#include "Plugins/kkPluginCommonInterface.h"

struct kkTriangleRayTestResult;
class kkPolygon;
class kkRay;


KK_FORCE_INLINE u64 hash_value( const v2i& v )
{
	union
	{
		u64 _64;
		s32  _32[2];
	};

	_32[ 0 ] = v.x;
	_32[ 1 ] = v.y;

	return _64;
}

class kkScene3DObjectCommon
{
protected:
	kkScene3DObjectType m_type = kkScene3DObjectType::PolygonObject;
public:
	kkScene3DObjectCommon(){}
	virtual ~kkScene3DObjectCommon(){}
};

enum class kkRayTriangleIntersectionAlgorithm
{
	MollerTrumbore,
	Watertight
};

class kkScene3DObject;
struct kkRayTriangleIntersectionResultSimple
{
	kkVector4 m_intersectionPoint;
	f32 m_T = 0.f; // m_distance
	f32 m_U = 0.f;
	f32 m_V = 0.f;
	f32 m_W = 0.f;
	u64 m_polygonIndex = 0;

	kkScene3DObject * m_object = nullptr;
};

class kkScene3DObject
{
public:
	kkScene3DObject(){}
	virtual ~kkScene3DObject(){}

	virtual void  SetName( const char16_t * ) = 0;
	virtual const char16_t* GetName() = 0;
	virtual u64 GetPolygonCount() = 0;
	virtual kkPolygon* GetPolygon(u64) = 0;

	virtual kkArray<kkVertex*>&  GetVertexArray() = 0;
	virtual kkArray<kkControlVertex*>&  GetControlVertexArray() = 0;
	virtual kkArray<kkPolygon*>&  GetPolygonArray() = 0;

	// for 1 intersection
	virtual bool IsRayIntersect( const kkRay& ray, kkRayTriangleIntersectionResultSimple& result, kkRayTriangleIntersectionAlgorithm alg = kkRayTriangleIntersectionAlgorithm::MollerTrumbore ) = 0;

	// for many intersections
	virtual bool IsRayIntersectMany( const kkRay& ray, std::vector<kkRayTriangleIntersectionResultSimple>& result, kkRayTriangleIntersectionAlgorithm alg = kkRayTriangleIntersectionAlgorithm::MollerTrumbore ) = 0;

	// before using RayTest and other, call PrepareForRaytracing.
	virtual void PrepareForRaytracing(kkRenderInfo*) = 0; // build BVH and other
	virtual void RayTest( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation* renderObjectMaterial ) = 0;
	//virtual void RayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation* renderObjectMaterial ) = 0;
	virtual void FinishRaytracing() = 0; // remove BVH and other

	//virtual void UpdateScreenSpacePoints() = 0;
	
	virtual void ApplyPosition() = 0;
	virtual void RestorePosition() = 0;

	virtual void SetMatrix(const kkMatrix4& m) = 0;
	virtual kkAabb& Aabb() = 0;
	virtual kkObb&  Obb() = 0;
	virtual void UpdateAabb() = 0;
	virtual kkScene3DObjectType GetType() = 0;
	virtual kkVector4& GetPivot() = 0;
	virtual kkVector4& GetPivotFixed() = 0;
	virtual void RestorePivot() = 0;
	virtual void ApplyPivot() = 0;
	virtual kkMatrix4& GetMatrix()  = 0;
	virtual kkMatrix4& GetMatrixWorld()  = 0;
	virtual void UpdateWorldMatrix() = 0;

	virtual kkPluginGUIWindow* GetPluginGUIWindow() = 0;

	// при создании объекта в плагине, можно создать окно с параметрами для настройки объекта
	// у каждого объекта должны быть свои уникальные параметры
	// нужно выделять память в соответствии со структурой которая будет представлять данные
	/*
		struct S
		{
			float h, w;
		};
		object = scene->GetSelectedObject(0);
		object->AllocateParametersWindowData(sizeof(S));
	*/
	virtual void* AllocateParametersWindowData(u32 size) = 0;
	// передать указатель на выделенную ранее память
	// обязательно нужно будет указать это потом в setParametersWindowData
	virtual void* DropParametersWindowData() = 0;
	// установить указатель если уже есть чей-то dropParametersWindowData
	// и не было вызвано allocateParametersWindowData
	virtual void SetParametersWindowData(void*) = 0;

	// установить новому создаваемому объекту окно с параметрами
	// окно должно иметь тип kkPluginGUIWindowType::_parameters
	virtual void SetPluginGUIWindow( kkPluginGUIWindow* ) = 0;

	virtual void SetMaterialImplementation( kkMaterialImplementation * material ) = 0;
	virtual kkMaterialImplementation* GetMaterialImplementation() = 0;

	virtual void SetMaterial( const char16_t* name ) = 0;
	virtual bool IsMaterial( const char16_t* name ) = 0; // вернёт истину если у объекта материал с указанным именем
	virtual const char16_t* GetMaterial() = 0;
	//virtual void UpdateMaterial() = 0;
	virtual void SaveShaderParameter() = 0;
	virtual void RestoreShaderParameter() = 0;
};

#endif