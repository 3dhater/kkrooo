#ifndef __KKVERTEX_H__
#define __KKVERTEX_H__

#include "Classes/Math/kkVector4.h"
#include "Classes/Containers/kkList.h"

//void kkVertex_addPolygon(kkVertex*, kkPolygon*);

template<typename T>
struct kkLoopNode
{
	kkLoopNode<T> * m_left  = nullptr;
	kkLoopNode<T> * m_right = nullptr;

	T * m_element = nullptr;
};

struct kkEdge
{
	// предидущий\следующий в главном списке
	kkEdge* m_mainPrev = nullptr;
	kkEdge* m_mainNext = nullptr;

	// вершины образующие ребро
	kkVertex * m_v1 = nullptr;
	kkVertex * m_v2 = nullptr;

	// у ребра может быть 1 или 2 полигона
	kkPolygon* m_p1 = nullptr;
	kkPolygon* m_p2 = nullptr;
};

struct kkPolygon
{
	// предидущий\следующий в главном списке
	kkPolygon* m_mainPrev = nullptr;
	kkPolygon* m_mainNext = nullptr;

	kkLoopNode<kkVertex> * m_verts = nullptr;
	u64        m_vertexCount = 0;

	kkLoopNode<kkEdge> * m_edges = nullptr;
	u64 m_edgeCount = 0;

	v3f * m_normals = nullptr;
	v2f * m_tcoords = nullptr;
};

struct kkVertex
{
	kkVector4 m_position;
	kkVector4 m_positionFix;
	kkVector4 m_normal;

	// предидущий\следующий в главном списке
	kkVertex* m_mainPrev = nullptr;
	kkVertex* m_mainNext = nullptr;
	
	kkLoopNode<kkPolygon> * m_polygons = nullptr;
	u32 m_polygonCount = 0;
	
	kkLoopNode<kkEdge> * m_edges = nullptr;
	u32 m_edgeCount = 0;

	// Индекс software модели для точек
	u32 m_pointsModelIndex = 0;
	u32 m_pointsVertexIndex = 0;

	// при создании software модели нужно запомнить индекс вершины (в software буфере)
	//   чтобы потом иметь возможно быстро найти нужную вершину
	// значение устанавливается при перестройке модели
	std::vector<std::pair<u32,u32>> m_vertexIndexForSoftware      ; // вершина полигона может создавать множество hardware точек
	                                             // перед перестройкой модели нужно очищать список
	                                             // индекс в мешбуфере, индекс софтваре модели
	std::vector<std::pair<u32,u32>> m_vertexIndexForSoftware_lines;

	enum _flags
	{
		EF_SELECTED = 1
	};
	u32 m_flags = 0;
};

struct kkTriangle
{
	kkVector4 v1;
	kkVector4 v2;
	kkVector4 v3;
	//kkVector4 faceNormal;
	kkVector4 normal1;
	kkVector4 normal2;
	kkVector4 normal3;
	kkVector4 e1;
	kkVector4 e2;
	kkVector4 t1;
	kkVector4 t2;
	kkVector4 t3;

	void update()
	{
		e1 = kkVector4( v2._f32[0] - v1._f32[0],
			v2._f32[1] - v1._f32[1],
			v2._f32[2] - v1._f32[2],
			0.f);
		e2 = kkVector4( v3._f32[0] - v1._f32[0],
			v3._f32[1] - v1._f32[1],
			v3._f32[2] - v1._f32[2],
			0.f);
	//	e1.cross(e2, faceNormal);
	}

	void center(kkVector4& out)
	{
		out = (v1 + v2 + v3) * 0.3333333f;
	}

	bool rayTest_MT( const kkRay& ray, bool withBackFace, f32& T, f32& U, f32& V, f32& W )
	{
		kkVector4  pvec = ray.m_direction.cross_return(e2);
		f32 det  = e1.dot(pvec);
		
		if( withBackFace )
		{
			if( std::fabs(det) < Epsilon )
				return false;
		}
		else
		{
			if( det < Epsilon && det > -Epsilon )
				return false;
		}

		kkVector4 tvec(
			ray.m_origin._f32[0] - v1._f32[0],
			ray.m_origin._f32[1] - v1._f32[1],
			ray.m_origin._f32[2] - v1._f32[2],
			0.f);

		f32 inv_det = 1.f / det;
		U = tvec.dot(pvec) * inv_det;

		if( U < 0.f || U > 1.f )
			return false;

		kkVector4  qvec = tvec.cross_return(e1);
		V    = ray.m_direction.dot(qvec) * inv_det;

		if( V < 0.f || U + V > 1.f )
			return false;

		T = e2.dot(qvec) * inv_det;
		
		if( T < Epsilon ) return false;
		
		W = 1.f - U - V;
		return true;
	}

	bool rayTest_Watertight( const kkRay& ray, bool withBackFace, f32& T, f32& U, f32& V, f32& W )
	{
		v1._f32[3] = 1.f;
		v2._f32[3] = 1.f;
		v3._f32[3] = 1.f;
		const auto A = v2 - ray.m_origin;
		const auto B = v3 - ray.m_origin;
		const auto C = v1 - ray.m_origin;

		const f32 Ax = A[ray.m_kx] - (ray.m_Sx * A[ray.m_kz]);
		const f32 Ay = A[ray.m_ky] - (ray.m_Sy * A[ray.m_kz]);
		const f32 Bx = B[ray.m_kx] - (ray.m_Sx * B[ray.m_kz]);
		const f32 By = B[ray.m_ky] - (ray.m_Sy * B[ray.m_kz]);
		const f32 Cx = C[ray.m_kx] - (ray.m_Sx * C[ray.m_kz]);
		const f32 Cy = C[ray.m_ky] - (ray.m_Sy * C[ray.m_kz]);

		U = (Cx * By) - (Cy * Bx);
		V = (Ax * Cy) - (Ay * Cx);
		W = (Bx * Ay) - (By * Ax);

		if( U == 0.f || V == 0.f || W == 0.f )
		{
			f64 CxBy = (f64)Cx * (f64)By;
			f64 CyBx = (f64)Cy * (f64)Bx;
			U = (f32)(CxBy - CyBx);

			f64 AxCy = (f64)Ax * (f64)Cy;
			f64 AyCx = (f64)Ay * (f64)Cx;
			V = (f32)(AxCy - AyCx);

			f64 BxAy = (f64)Bx * (f64)Ay;
			f64 ByAx = (f64)By * (f64)Ax;
			W = (f32)(BxAy - ByAx);
		}

		if( withBackFace )
		{
			if( (U<0.f || V<0.f || W < 0.f) &&
				(U>0.f || V>0.f || W > 0.f) )
				return false;
		}
		else
		{
			if(U<0.f || V<0.f || W<0.f)
				return false;
		}

		f32 det = U+V+W;

		if(det == 0.f)
			return false;

		const f32 Az = ray.m_Sz * A[ray.m_kz];
		const f32 Bz = ray.m_Sz * B[ray.m_kz];
		const f32 Cz = ray.m_Sz * C[ray.m_kz];
		const f32 Ts = (U*Az) + (V*Bz) + (W*Cz);
		
		if( !withBackFace ) // CULL
		{
			if( Ts < 0.f || Ts > ray.m_tMax*det )
				return false;
		}
		else
		{
			if( det < 0.f && (Ts >= 0.f || Ts<ray.m_tMax*det))
				return false;
			else if(det > 0.f && (Ts<=0.f || Ts >ray.m_tMax*det))
				return false;
		}

		const f32 invDet = 1.f / det;
		U = U*invDet;
		V = V*invDet;
		W = W*invDet;
		T = Ts*invDet;
		if( T < Epsilon)
			return false;
		return true;
	}
};

// при добавлении новых элементов нужно сделать обновление в копировании внутри метода _rayTestTriangle
struct kkTriangleRayTestResult
{
	kkTriangle triangle;

	// read only
	// заполняются при rayTest
	// используется при уже самом рендеринге
	float length = 0.f;
	kkVector4 intersectionPoint;
	kkVector4 pointNormal;
	kkVector4 pointTcoord;
	kkMaterialImplementation* material = nullptr;

	u32 index = 0; // порядковый номер
};

class kkPolygonalModel
{
public:
	kkPolygonalModel(){}
	virtual ~kkPolygonalModel(){}

	virtual kkPolygon* GetPolygons() = 0;
	virtual u64 GetPolygonsCount() = 0;
	virtual void DeletePolygon(kkPolygon*) = 0;
	virtual void AddPolygon(kkGeometryInformation* p,bool weld, bool flip) = 0;
};

#endif