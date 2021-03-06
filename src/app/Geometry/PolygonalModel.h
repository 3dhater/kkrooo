#ifndef __POLYGONAL_MODEL_H__
#define __POLYGONAL_MODEL_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/kkAABB.h"
#include "Classes/kkOBB.h"

#include <unordered_map>

#include "../Plugins/PluginCommonInterface.h"

struct VertexHash
{
	VertexHash()
	{
		bytes[ 0 ] = 0;
		bytes[ 1 ] = 0;
		bytes[ 2 ] = 0;
		bytes[ 3 ] = 0;
		bytes[ 4 ] = 0;
		bytes[ 5 ] = 0;
		bytes[ 6 ] = 0;
		bytes[ 7 ] = 0;
		bytes[ 8 ] = 0;
		bytes[ 9 ] = 0;
		bytes[ 10 ] = 0;
		bytes[ 11 ] = 0;
	}

	bool operator==(const VertexHash& o)const
    {
		if( bytes[ 0 ] != o.bytes[ 0 ] ) return false;
		if( bytes[ 1 ] != o.bytes[ 1 ] ) return false;
		if( bytes[ 2 ] != o.bytes[ 2 ] ) return false;
		if( bytes[ 3 ] != o.bytes[ 3 ] ) return false;
		if( bytes[ 4 ] != o.bytes[ 4 ] ) return false;
		if( bytes[ 5 ] != o.bytes[ 5 ] ) return false;
		if( bytes[ 6 ] != o.bytes[ 6 ] ) return false;
		if( bytes[ 7 ] != o.bytes[ 7 ] ) return false;
		if( bytes[ 8 ] != o.bytes[ 8 ] ) return false;
		if( bytes[ 9 ] != o.bytes[ 9 ] ) return false;
		if( bytes[ 10 ] != o.bytes[ 10 ] ) return false;
		if( bytes[ 11 ] != o.bytes[ 11 ] ) return false;
        
		return true;
    }

	void set(kkVertex* v)
	{
		char * ptr = (char *)v->m_position.data();
		bytes[ 0 ] = ptr[ 0 ]; 
		bytes[ 1 ] = ptr[ 1 ]; 
		bytes[ 2 ] = ptr[ 2 ]; 
		bytes[ 3 ] = ptr[ 3 ]; 
		bytes[ 4 ] = ptr[ 4 ]; 
		bytes[ 5 ] = ptr[ 5 ]; 
		bytes[ 6 ] = ptr[ 6 ]; 
		bytes[ 7 ] = ptr[ 7 ]; 
		bytes[ 8 ] = ptr[ 8 ]; 
		bytes[ 9 ] = ptr[ 9 ]; 
		bytes[ 10 ] = ptr[ 10 ]; 
		bytes[ 11 ] = ptr[ 11 ]; 
		if( bytes[ 0 ] == 0 ) bytes[ 0 ] = 1;
		if( bytes[ 1 ] == 0 ) bytes[ 1 ] = 1;
		if( bytes[ 2 ] == 0 ) bytes[ 2 ] = 1;
		if( bytes[ 3 ] == 0 ) bytes[ 3 ] = 1;
		if( bytes[ 4 ] == 0 ) bytes[ 4 ] = 1;
		if( bytes[ 5 ] == 0 ) bytes[ 5 ] = 1;
		if( bytes[ 6 ] == 0 ) bytes[ 6 ] = 1;
		if( bytes[ 7 ] == 0 ) bytes[ 7 ] = 1;
		if( bytes[ 8 ] == 0 ) bytes[ 8 ] = 1;
		if( bytes[ 9 ] == 0 ) bytes[ 9 ] = 1;
		if( bytes[ 10 ] == 0 ) bytes[ 10 ] = 1;
		if( bytes[ 11 ] == 0 ) bytes[ 11 ] = 1;
		bytes[12] = 0;
		str = bytes;
	}
	void set(v3f* v)
	{
		char * ptr = (char *)v->data();
		bytes[ 0 ] = ptr[ 0 ];
		bytes[ 1 ] = ptr[ 1 ];
		bytes[ 2 ] = ptr[ 2 ];
		bytes[ 3 ] = ptr[ 3 ];
		bytes[ 4 ] = ptr[ 4 ];
		bytes[ 5 ] = ptr[ 5 ];
		bytes[ 6 ] = ptr[ 6 ];
		bytes[ 7 ] = ptr[ 7 ];
		bytes[ 8 ] = ptr[ 8 ];
		bytes[ 9 ] = ptr[ 9 ];
		bytes[ 10 ] = ptr[ 10 ];
		bytes[ 11 ] = ptr[ 11 ];
		if( bytes[ 0 ] == 0 ) bytes[ 0 ] = 1;
		if( bytes[ 1 ] == 0 ) bytes[ 1 ] = 1;
		if( bytes[ 2 ] == 0 ) bytes[ 2 ] = 1;
		if( bytes[ 3 ] == 0 ) bytes[ 3 ] = 1;
		if( bytes[ 4 ] == 0 ) bytes[ 4 ] = 1;
		if( bytes[ 5 ] == 0 ) bytes[ 5 ] = 1;
		if( bytes[ 6 ] == 0 ) bytes[ 6 ] = 1;
		if( bytes[ 7 ] == 0 ) bytes[ 7 ] = 1;
		if( bytes[ 8 ] == 0 ) bytes[ 8 ] = 1;
		if( bytes[ 9 ] == 0 ) bytes[ 9 ] = 1;
		if( bytes[ 10 ] == 0 ) bytes[ 10 ] = 1;
		if( bytes[ 11 ] == 0 ) bytes[ 11 ] = 1;
		bytes[12] = 0;
		str = bytes;
	}
	void set(kkVector4* v)
	{
		char * ptr = (char *)v->data();
		bytes[ 0 ] = ptr[ 0 ];
		bytes[ 1 ] = ptr[ 1 ];
		bytes[ 2 ] = ptr[ 2 ];
		bytes[ 3 ] = ptr[ 3 ];
		bytes[ 4 ] = ptr[ 4 ];
		bytes[ 5 ] = ptr[ 5 ];
		bytes[ 6 ] = ptr[ 6 ];
		bytes[ 7 ] = ptr[ 7 ];
		bytes[ 8 ] = ptr[ 8 ];
		bytes[ 9 ] = ptr[ 9 ];
		bytes[ 10 ] = ptr[ 10 ];
		bytes[ 11 ] = ptr[ 11 ];
		if( bytes[ 0 ] == 0 ) bytes[ 0 ] = 1;
		if( bytes[ 1 ] == 0 ) bytes[ 1 ] = 1;
		if( bytes[ 2 ] == 0 ) bytes[ 2 ] = 1;
		if( bytes[ 3 ] == 0 ) bytes[ 3 ] = 1;
		if( bytes[ 4 ] == 0 ) bytes[ 4 ] = 1;
		if( bytes[ 5 ] == 0 ) bytes[ 5 ] = 1;
		if( bytes[ 6 ] == 0 ) bytes[ 6 ] = 1;
		if( bytes[ 7 ] == 0 ) bytes[ 7 ] = 1;
		if( bytes[ 8 ] == 0 ) bytes[ 8 ] = 1;
		if( bytes[ 9 ] == 0 ) bytes[ 9 ] = 1;
		if( bytes[ 10 ] == 0 ) bytes[ 10 ] = 1;
		if( bytes[ 11 ] == 0 ) bytes[ 11 ] = 1;
		bytes[12] = 0;
		str = bytes;
	}

	// 3 координаты по 4 байта
	char bytes[13];

	std::string str;
	long long id = 0;
};

//KK_FORCE_INLINE
//std::size_t hash_value(const ControlVertexHash& b)
//{
//    std::hash<char[12]> hasher();
//    return hasher(b.bytes);
//}

constexpr u32 numOfIndsPerBVHNode = 15;
struct BVH_node
{
	BVH_node(){}
	~BVH_node()
	{
		if( first )  delete first;
		if( second ) delete second;
		//if( m_inds ){ m_inds->clear(); delete m_inds; }
	}

	kkArray<u32> m_inds = kkArray<u32>(0xff);
	//std::vector<u32> m_inds;

	BVH_node * first = nullptr;
	BVH_node * second = nullptr;

	kkAabb m_aabb;

	void clear()
	{
		m_inds.clear();
		m_aabb.reset();
		if( first ){  delete first; first = nullptr; }
		if( second ){ delete second; second = nullptr; }
		//if( m_inds ){ m_inds->clear(); delete m_inds; }
	}

};

struct BVH_sphere
{
	float radius = 0.f;
};

struct GridAcceleratorCell
{
	v2f left_top;
	v2f right_bottom;
	v2f center;

	kkArray<u32> m_inds = kkArray<u32>(0xff);
};

struct GridAcceleratorRow
{
	GridAcceleratorRow(){}
	~GridAcceleratorRow()
	{
		for( u64 i = 0, sz = m_cells.size(); i < sz; ++i )
		{
			delete m_cells[i];
		}
		m_cells.clear();
	}
	kkArray<GridAcceleratorCell*> m_cells;
};


struct kkRenderInfo;
class PolygonalModel : public kkPolygonalModel
{
	std::size_t m_triangleCount = 0;

	kkAabb    m_aabbRayTestAabb;

	bool _intersectBVHNode(BVH_node* node, const kkRay& ray);
	void rayTestBVH( BVH_node* node, std::vector<kkTriangleRayTestResult>& outTriangle, const kkRay& ray, 
	kkMaterialImplementation* renderObjectMaterial );

	//f32 m_gridRadius = 0.f;
	//kkArray<GridAcceleratorRow*> m_gridAccelRows;
	//void _deleteGridAccel();
	//void _createGridAccel(kkRenderInfo* ri);
	//void _addTriangleToGrid(kkRenderInfo* ri, kkTriangle*, u32 triIndex);

	void _rayTestTriangle( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial, kkTriangleRayTestResult* );

	std::unordered_map<std::string,kkVertex*> m_weldMap;
	
	void _addVertexToList(kkVertex*);
	void _removeVertexFromList(kkVertex*);
	void _addPolygonToList(kkPolygon*);
	void _removePolygonFromList(kkPolygon*);
	void _addEdgeToList(kkEdge*);
	void _removeEdgeFromList(kkEdge*);
	void _createNewMesh();

public:
	PolygonalModel();
	virtual ~PolygonalModel();
	virtual kkPolygon* GetPolygons();
	virtual u64 GetPolygonsCount();
	virtual void DeletePolygon(kkPolygon*);
	//virtual void DeleteVertex(kkVertex*);

	void deleteEdges();
	void  AddPolygon(kkGeometryInformation* gi,bool weld, bool flip);

	void calculateTriangleCount();

	void onEndCreation();
	void generateNormals(bool flat);
	void generateBT();

	std::vector<kkTriangleRayTestResult> m_trianglesForRendering;
	
	void prepareForRaytracing(kkRenderInfo* );
	void finishRaytracing();
	bool m_isPreparedForRaytracing = false;

	BVH_node m_BVH_root;
	void rayTest( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial );
	//void rayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial );

	void weldByLen(f32 len);

	void attachModel(PolygonalModel*, const kkMatrix4& invertMatrix, const kkMatrix4& matrix_other, const kkVector4& pivot, const kkVector4& pivot_other);

	kkPolygon* m_polygons = nullptr;
	kkVertex*  m_verts    = nullptr;
	kkEdge*    m_edges    = nullptr;
	u64 m_polygonsCount = 0;
	u64 m_vertsCount    = 0;
	u64 m_edgesCount    = 0;
	
	void updateEdges();
	bool breakVerts();
	bool weld(kkVertex* V1, kkVertex* V2);
	kkEdge* isEdge(kkVertex* V1, kkVertex* V2);
	bool isOnEdge(kkVertex* V);

	bool CreateNewMeshFromSelectedVerts();
	bool ClearNewMesh();
	void ApplyNewMesh();

	bool WeldSelectedVerts(f32 len);

	u64 GetNumOfSelectedVerts();

	Scene3DObject* m_object = nullptr;
};

#endif