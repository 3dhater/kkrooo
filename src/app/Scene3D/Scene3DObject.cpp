#include "kkrooo.engine.h"
#include "../Common.h"

#include "Classes/Math/kkRay.h"
#include "Classes/Containers/kkArray.h"

#include "Geometry/kkPolygonalModel.h"

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "GraphicsSystem/kkMesh.h"
#include "../Geometry/PolygonalModel.h"
#include "Scene3DObject.h"
#include "Scene3D.h"
#include "../Functions.h"

#include "../Application.h"
#include "../Plugins/PluginGUIWindow.h"
#include "Renderer/kkRenderer.h"
#include "../Viewport/Viewport.h"
#include "../Viewport/ViewportCamera.h"

#include <thread>
#include <unordered_set>

struct verts_points
{  
	v3f _pos;  
	v4f _col;  
};



Scene3DObjectShaderParameter Scene3DObject::m_globalShaderParameter;

class kkDefaultMaterial;
extern kkDefaultMaterial * g_defaultMaterial;

Scene3DObject::Scene3DObject(kkScene3DObjectType t, PolygonalModel * m)
	:
	m_polyModel(m)
{
	m_scene3DObjectType = t;
	m_GS = kkGetGS();
	this->SetMaterialImplementation((kkMaterialImplementation*)g_defaultMaterial);
	m_polyModel->m_object = this;
}

Scene3DObject::~Scene3DObject()
{
	_destroyHardwareModels();
	_destroySoftwareModels();
	if( m_polyModel )
	{
		kkDestroy(m_polyModel);
	}
}

kkPolygonalModel* Scene3DObject::GetModel()
{
	return m_polyModel;
}

bool Scene3DObject::_rebuildModel()
{
	switch(m_scene3DObjectType)
	{
	case kkScene3DObjectType::PolygonObject:
		return _buildModel_polymodel();
	default:
		break;
	}
	return false;
}

void Scene3DObject::updatePolygonModel()
{
	_destroySoftwareModels_polys();
	_destroyHardwareModels_polys();
	_createSoftwareModel_polys();
	_createHardwareModel_polys();
}

void Scene3DObject::updateEdgeModel()
{
	_destroySoftwareModels_edges();
	_destroyHardwareModels_edges();
	_createSoftwareModel_edges();
	_createHardwareModel_edges();
}

bool Scene3DObject::_createHardwareModel_edges()
{
	for(u64 i = 0, sz = m_SoftwareModels_edges.size(); i < sz; ++i )
	{
		auto hm = m_GS->createMesh(m_SoftwareModels_edges[i], kkMeshType::Lines );
		if(!hm){ return false;}
		m_HardwareModels_edges.push_back(hm);
	}
	return true;
}

bool Scene3DObject::_createHardwareModel_polys()
{
	for(u64 i = 0, sz = m_SoftwareModels_polys.size(); i < sz; ++i )
	{
		auto hm = m_GS->createMesh(m_SoftwareModels_polys[i], kkMeshType::Triangles );
		if(!hm){return false;}
		m_HardwareModels_polys.push_back(hm);
	}
	return true;
}

bool Scene3DObject::_buildModel_polymodel()
{
	// на основе m_PolyModel создаётся kkSMesh
	_destroySoftwareModels();
	_destroyHardwareModels();

	m_aabbOriginal.reset();

	_createSoftwareModel_points();
	for(u64 i = 0, sz = m_SoftwareModels_points.size(); i < sz; ++i )
	{
		auto hm = m_GS->createMesh(m_SoftwareModels_points[i], kkMeshType::Points );
		if(!hm){ return false;}
		m_HardwareModels_points.push_back(hm);
	}
	
	m_polyModel->updateEdges();
	_createSoftwareModel_edges();
	if( !_createHardwareModel_edges() )
		return false;

	// на основе kkSMesh создаётся kkMesh
	_createSoftwareModel_polys();
	if( !_createHardwareModel_polys() )
		return false;

	UpdateAabb();

	return true;
}

void Scene3DObject::_createSoftwareModel_polys()
{
	if(!m_polyModel->m_polygons)
		return;
	auto V = m_polyModel->m_verts;
	for(u64 i = 0; i < m_polyModel->m_vertsCount; ++i)
	{
		V->m_vertexIndexForSoftware.clear();
		V = V->m_mainNext;
	}
	auto current_polygon = m_polyModel->m_polygons;
	u32 triangleCount = 0;
	kkSMesh*     softwareModel = nullptr;
	DefaultVert* verts_ptr     = nullptr;
	u16     *   inds_ptr     = nullptr;
	u32 softwareModelIndex = 0;
	u16 index = 0;
	for(u64 i = 0; i < m_polyModel->m_polygonsCount; ++i)
	{
		kkVertex * base_vertex = current_polygon->m_verts->m_element;
		v4f color(1.f,1.f,1.f,1.f);
		if( kkSingleton<Application>::s_instance->getEditMode() == EditMode::Polygon )
		{
			if( current_polygon->m_flags & current_polygon->EF_SELECTED )
			{
				m_isObjectHaveSelectedPolys = true;
				color.set(0.8f, 0.f, 0.f, 2.f);
			}
		}
		auto v_node1 = current_polygon->m_verts->m_right;
		auto v_node2 = v_node1->m_right;
		for( u64 i2 = 0, sz2 = current_polygon->m_vertexCount - 2; i2 < sz2; ++i2 )
		{
			kkVertex * vertex3 = v_node1->m_element;
			kkVertex * vertex2 = v_node2->m_element;
			if( triangleCount == 0 )
			{
				softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_TRIANGLES);
				verts_ptr     = (DefaultVert*)softwareModel->m_vertices;
				inds_ptr      = softwareModel->m_indices;
				m_SoftwareModels_polys.push_back(softwareModel);
				softwareModelIndex = (u32)m_SoftwareModels_polys.size() - 1;
				index = 0;
			}
			
			

			verts_ptr->Color = color;
			verts_ptr->Position.x = base_vertex->m_position._f32[0];
			verts_ptr->Position.y = base_vertex->m_position._f32[1];
			verts_ptr->Position.z = base_vertex->m_position._f32[2];
			if(current_polygon->m_normals)
			{
				verts_ptr->Normal   = current_polygon->m_normals[0];
			}
			if(current_polygon->m_tcoords)
			{
				verts_ptr->TCoords   = current_polygon->m_tcoords[0];
			}
			m_aabbOriginal.add( base_vertex->m_position );
			base_vertex->m_vertexIndexForSoftware.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

			*inds_ptr = index; 
			++index;
			++inds_ptr;
			++verts_ptr;

			verts_ptr->Color = color;
			verts_ptr->Position.x = vertex2->m_position._f32[0];
			verts_ptr->Position.y = vertex2->m_position._f32[1];
			verts_ptr->Position.z = vertex2->m_position._f32[2];
			if(current_polygon->m_normals)
			{
				verts_ptr->Normal   = current_polygon->m_normals[i2+2];
			}
			if(current_polygon->m_tcoords)
			{
				verts_ptr->TCoords   = current_polygon->m_tcoords[i2+2];
			}
			m_aabbOriginal.add( vertex2->m_position );
			vertex2->m_vertexIndexForSoftware.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

			*inds_ptr = index; 
			++index;
			++inds_ptr;
			++verts_ptr;

			verts_ptr->Color = color;
			verts_ptr->Position.x = vertex3->m_position._f32[0];
			verts_ptr->Position.y = vertex3->m_position._f32[1];
			verts_ptr->Position.z = vertex3->m_position._f32[2];
			if(current_polygon->m_normals)
			{
				verts_ptr->Normal   = current_polygon->m_normals[i2+1];
			}
			if(current_polygon->m_tcoords)
			{
				verts_ptr->TCoords   = current_polygon->m_tcoords[i2+1];
			}
			m_aabbOriginal.add( vertex3->m_position );
			vertex3->m_vertexIndexForSoftware.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

			*inds_ptr = index; 
			++index;
			++inds_ptr;
			++verts_ptr;

			softwareModel->m_vCount += 3;
			softwareModel->m_iCount += 3;
			++triangleCount;
			if( triangleCount == m_triLimit )
			{
				triangleCount = 0;
			}

			v_node1 = v_node2;
			v_node2 = v_node2->m_right;
		}
		current_polygon = current_polygon->m_mainNext;
	}
}

void        Scene3DObject::updateModelPointsColors()
{
	verts_points       *  verts_ptr     = nullptr;
	auto current_vertex = m_polyModel->m_verts;
	for(u64 i = 0; i < m_polyModel->m_vertsCount; ++i)
	{
		auto SM = m_SoftwareModels_points[current_vertex->m_pointsModelIndex];

		verts_ptr = (verts_points*)SM->m_vertices;
		verts_ptr += current_vertex->m_pointsVertexIndex;

		if( current_vertex->m_flags & current_vertex->EF_SELECTED )
		{
			verts_ptr->_col.x = 1.f;
			verts_ptr->_col.z = 0.f;
		}
		else
		{
			verts_ptr->_col.x = 0.1f;
			verts_ptr->_col.y = 0.1f;
			verts_ptr->_col.z = 1.f;
		}
		current_vertex = current_vertex->m_mainNext;
	}
	for( u64 i = 0, sz = m_SoftwareModels_points.size(); i < sz; ++i )
	{
		u8 * vptr = nullptr;
		auto HW = m_HardwareModels_points[i];
		auto SW = m_SoftwareModels_points[i];
		HW->mapVerts(&vptr);
		memcpy(vptr, SW->m_vertices,  SW->m_stride * SW->m_vCount );
		HW->unmapVerts();
	}
}

void Scene3DObject::_createSoftwareModel_points()
{
	if(!m_polyModel->m_verts)
		return;
	auto current_vertex = m_polyModel->m_verts;
	u32         pointCountForLimit = 0;
	kkSMesh*      softwareModel = nullptr;
	u16     *   inds_ptr      = nullptr;
	u16         index         = 0;
	verts_points       *  verts_ptr     = nullptr;
	u32 softwareModelIndex = 0;
	for(u64 i = 0; i < m_polyModel->m_vertsCount; ++i)
	{
		if( pointCountForLimit == 0 )
		{
			softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_POINTS);
			verts_ptr     = (verts_points*)softwareModel->m_vertices;
			inds_ptr      = softwareModel->m_indices;
			m_SoftwareModels_points.push_back(softwareModel);
			softwareModelIndex = (u32)m_SoftwareModels_points.size() - 1;
			index = 0;
		}

		verts_ptr->_pos = current_vertex->m_position;
		verts_ptr->_col = v4f(0.f,0.f,1.f,1.f);
		if(current_vertex->m_flags & current_vertex->EF_SELECTED)
			verts_ptr->_col = v4f(1.f,0.f,0.f,1.f);
		*inds_ptr = index;

		current_vertex->m_pointsModelIndex = softwareModelIndex;
		current_vertex->m_pointsVertexIndex = index;

		++verts_ptr;
		++inds_ptr;
		++index;
		++softwareModel->m_vCount;
		++softwareModel->m_iCount;

		++pointCountForLimit;
		if( pointCountForLimit == m_pointLimit )
			pointCountForLimit = 0;

		current_vertex = current_vertex->m_mainNext;
	}
}

void Scene3DObject::_createSoftwareModel_edges()
{
	if(!m_polyModel->m_edges)
		return;
	auto V = m_polyModel->m_verts;
	for(u64 i = 0; i < m_polyModel->m_vertsCount; ++i)
	{
		V->m_vertexIndexForSoftware_lines.clear();
		V = V->m_mainNext;
	}
	u32 lineCount = 0;
	kkSMesh*      softwareModel = nullptr;
	LineModelVertex*  verts_ptr     = nullptr;
	u16     *   inds_ptr      = nullptr;
	u32 softwareModelIndex = 0;
	u16         index         = 0;
	auto E = m_polyModel->m_edges;
	for(u64 i = 0; i < m_polyModel->m_edgesCount; ++i)
	{
		if( lineCount == 0 )
		{
			softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_LINES);
			verts_ptr     = (LineModelVertex*)softwareModel->m_vertices;
			inds_ptr      = softwareModel->m_indices;
			m_SoftwareModels_edges.push_back(softwareModel);
			softwareModelIndex = (u32)m_SoftwareModels_edges.size() - 1;
			index = 0;
		}
		
		verts_ptr->_position.x  = E->m_v1->m_position.KK_X;
		verts_ptr->_position.y  = E->m_v1->m_position.KK_Y;
		verts_ptr->_position.z  = E->m_v1->m_position.KK_Z;

		v4f color(1.f,1.f,1.f,1.f);
		if( kkSingleton<Application>::s_instance->getEditMode() == EditMode::Edge )
		{
			if(E->m_flags & E->EF_SELECTED)
			{
				color = v4f(1.f,0.f,0.f,1.f);
				this->m_isObjectHaveSelectedEdges = true;
			}
		}
		// ********************

		verts_ptr->_color = color;
		*inds_ptr = index;
		
		E->m_v1->m_vertexIndexForSoftware_lines.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

		++verts_ptr;
		++inds_ptr;
		++index;

		verts_ptr->_position.x  = E->m_v2->m_position.KK_X;
		verts_ptr->_position.y  = E->m_v2->m_position.KK_Y;
		verts_ptr->_position.z  = E->m_v2->m_position.KK_Z;
		verts_ptr->_color = color;
		*inds_ptr = index;
		
		E->m_v2->m_vertexIndexForSoftware_lines.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

		++verts_ptr;
		++inds_ptr;
		++index;
		softwareModel->m_vCount += 2;
		softwareModel->m_iCount += 2;
		++lineCount;
		if( lineCount == m_lineLimit )
			lineCount = 0;

		E = E->m_mainNext;
	}
}

bool Scene3DObject::init()
{
	return _rebuildModel();
}


void Scene3DObject::SetName( const char16_t * name )
{
	/// Нужно проверять имена. Нельзя создавать дубликаты.
	if( !m_scene3D->nameIsFree(name))
	{
		auto tmp = m_scene3D->nameCreateNew(name);
		m_name = tmp;
	}
	else
	{
		m_name = name;
	}
}

void        Scene3DObject::setPosition(const kkVector4& p)
{
	m_pivot = p;
}

void       Scene3DObject::_destroyHardwareModels_polys()
{
	for( u64 i = 0, sz = m_HardwareModels_polys.size(); i < sz; ++i )
		kkDestroy(m_HardwareModels_polys[i]);
	m_HardwareModels_polys.clear();
}

void       Scene3DObject::_destroySoftwareModels_polys()
{
	for( u64 i = 0, sz = m_SoftwareModels_polys.size(); i < sz; ++i )
		kkDestroy(m_SoftwareModels_polys[i]);
	m_SoftwareModels_polys.clear();
}

void       Scene3DObject::_destroySoftwareModels_edges()
{
	for( u64 i = 0, sz = m_SoftwareModels_edges.size(); i < sz; ++i )
		kkDestroy(m_SoftwareModels_edges[i]);
	m_SoftwareModels_edges.clear();
}

void Scene3DObject::_destroySoftwareModels()
{
	_destroySoftwareModels_polys();
	_destroySoftwareModels_edges();

	for( u64 i = 0, sz = m_SoftwareModels_points.size(); i < sz; ++i )
		kkDestroy(m_SoftwareModels_points[i]);
	m_SoftwareModels_points.clear();
}

void       Scene3DObject::_destroyHardwareModels_edges()
{
	for( u64 i = 0, sz = m_HardwareModels_edges.size(); i < sz; ++i )
		kkDestroy(m_HardwareModels_edges[i]);
	m_HardwareModels_edges.clear();
}

void Scene3DObject::_destroyHardwareModels()
{
	_destroyHardwareModels_polys();
	_destroyHardwareModels_edges();

	for( u64 i = 0, sz = m_HardwareModels_points.size(); i < sz; ++i )
		kkDestroy(m_HardwareModels_points[i]);
	m_HardwareModels_points.clear();
}

kkSMesh *  Scene3DObject::_createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE t)
{
	kkSMesh * sm   = kkCreate<kkSMesh>();
	switch (t)
	{
	case Scene3DObject::ENSMT_TRIANGLES:
		sm->m_stride   = sizeof(DefaultVert);
		sm->m_vertices = (u8*)kkMemory::allocate(sm->m_stride * 3 * m_triLimit );
		sm->m_indices  = (u16*)kkMemory::allocate(m_triLimit * 3 * sizeof(u16));
		break;
	case Scene3DObject::ENSMT_LINES:
		sm->m_stride   = sizeof(LineModelVertex);
		sm->m_vertices = (u8*)kkMemory::allocate(sm->m_stride * 6 * m_lineLimit );
		sm->m_indices  = (u16*)kkMemory::allocate(m_lineLimit * 6 * sizeof(u16));
		break;
	case Scene3DObject::ENSMT_POINTS:
		sm->m_stride   = sizeof(v3f) + sizeof(v4f);
		sm->m_vertices = (u8*)kkMemory::allocate(sm->m_stride  * m_pointLimit );
		sm->m_indices  = (u16*)kkMemory::allocate(m_pointLimit * sizeof(u16));
		break;
	default:
		break;
	}
	return sm;
}

kkMesh    * Scene3DObject::getHardwareModel(u64 i)
{
	return m_HardwareModels_polys[i];
}

kkMesh    * Scene3DObject::getHardwareModel_lines(u64 i)
{
	return m_HardwareModels_edges[i];
}
kkMesh    * Scene3DObject::getHardwareModel_points(u64 i)
{
	return m_HardwareModels_points[i];
}


u64       Scene3DObject::getHardwareModelCount()
{
	return m_HardwareModels_polys.size();
}
u64       Scene3DObject::getHardwareModelCount_lines()
{
	return m_HardwareModels_edges.size();
}
u64       Scene3DObject::getHardwareModelCount_points()
{
	return m_HardwareModels_points.size();
}


// friend for `class Scene3DObject`
void Scene3DObject_isRayIntersect( 
	int* out_result,
	int* stop_flag,
	kkPolygon* begin,
	kkPolygon* end,
	Scene3DObject* object,
	kkRay* ray,
	kkRayTriangleIntersectionResultSimple* iResult,
	kkRayTriangleIntersectionAlgorithm alg
	)
{
	kkPolygon* polygon = begin;
	auto M = object->m_matrix;
	while(true)
	{
		auto base_vertex_node = polygon->m_verts;
		auto VN1 = base_vertex_node->m_right;
		auto VN2 = VN1->m_right;
		for( u64 i = 0, sz = polygon->m_vertexCount - 2; i < sz; ++i )
		{
			u64 index2  = i+1;
			u64 index3  = index2 + 1;
			if( index3 == polygon->m_vertexCount )
				index3 = 0;
			
			kkTriangle t;
			t.v1 = math::mul( base_vertex_node->m_element->m_position, M ) + object->m_pivot;
			t.v2 = math::mul( VN1->m_element->m_position, M ) + object->m_pivot;
			t.v3 = math::mul( VN2->m_element->m_position, M ) + object->m_pivot;
			t.update();

			auto r = *ray;
			r.update();
			float f1,f2,f3,f4;

			switch (alg)
			{
			default:
			case kkRayTriangleIntersectionAlgorithm::MollerTrumbore:
				if( t.rayTest_MT(r, true, f1, f2, f3, f4 ) )
				{
					iResult->m_object = object;
					iResult->m_T = f1;
					iResult->m_U = f2;
					iResult->m_V = f3;
					iResult->m_W = f4;
					iResult->m_intersectionPoint = r.m_origin + r.m_direction * f1;
					iResult->m_polygon = polygon;
					*out_result = 1;
					*stop_flag  = 1;
					return;
				}
				break;
			case kkRayTriangleIntersectionAlgorithm::Watertight:
				if( t.rayTest_Watertight(r, true, f1, f2, f3, f4 ) )
				{
					iResult->m_object = object;
					iResult->m_T = f1;
					iResult->m_U = f2;
					iResult->m_V = f3;
					iResult->m_W = f4;
					iResult->m_intersectionPoint = r.m_origin + r.m_direction * f1;
					iResult->m_polygon = polygon;
					*out_result = 1;
					*stop_flag  = 1;
					return;
				}
				break;
			}

			if( *stop_flag == 1 )
				return;

			if( *out_result == 1 )
				return;

			VN1 = VN2;
			VN2 = VN2->m_right;
		}
		if(polygon == end)
			break;
		polygon = polygon->m_mainNext;
	}
}

bool Scene3DObject::IsRayIntersect( const kkRay& ray, kkRayTriangleIntersectionResultSimple& result, kkRayTriangleIntersectionAlgorithm alg )
{
	kkRay r = ray;
	int thread_stop_flag = 0;
	int thread_result    = 0;

	if( m_polyModel->m_polygonsCount > 40000 )
	{
		kkPolygon* arr[8];
		arr[0] = m_polyModel->m_polygons;
		arr[1] = arr[2] = arr[3] = arr[4] = arr[5] = arr[6] = arr[0];

		u64 number = (u64)std::floor((f64)m_polyModel->m_polygonsCount * 0.25f);
		for( s32 i = 1; i < 7; )
		{
			u64 counter = 0;
			while(true)
			{
				arr[i] = arr[i]->m_mainNext;
				++counter;
				if(counter == number)
					break;
			}
			++i; // new begin
			arr[i] = arr[i-1]->m_mainNext;

			if(i == 6)
				break;

			++i;
			arr[i] = arr[i-1];
		}

		arr[7] = m_polyModel->m_polygons->m_mainPrev;

		std::thread t1( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag,
			arr[0], arr[1], this, &r, &result, alg );

		std::thread t2( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag, 
			arr[2], arr[3],  this, &r, &result, alg  );

		std::thread t3( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag, 
			arr[4], arr[5],  this, &r, &result, alg  );

		Scene3DObject_isRayIntersect( &thread_result, &thread_stop_flag,
			arr[6], arr[7],  this, &r, &result, alg  );
		t1.join();
		t2.join();
		t3.join();
	}
	else
	{
		Scene3DObject_isRayIntersect( &thread_result, &thread_stop_flag,
			m_polyModel->m_polygons, m_polyModel->m_polygons->m_mainPrev, this, &r, &result, alg  );
	}

	return thread_result == 1;
}

bool Scene3DObject::IsRayIntersectMany( const kkRay& r, std::vector<kkRayTriangleIntersectionResultSimple>& result, kkRayTriangleIntersectionAlgorithm alg )
{
	if(!m_polyModel->m_polygonsCount)
		return false;
	bool ret = false;

	kkRayTriangleIntersectionResultSimple iResult;
	iResult.m_object = this;
	auto M = m_matrix;

	kkPolygon* polygon = m_polyModel->m_polygons;
	auto end =  m_polyModel->m_polygons->m_mainPrev;
	while(true)
	{
		auto base_vertex_node = polygon->m_verts;
		auto VN1 = base_vertex_node->m_right;
		auto VN2 = VN1->m_right;
		for( u64 i = 0, sz = polygon->m_vertexCount - 2; i < sz; ++i )
		{
			u64 index2  = i+1;
			u64 index3  = index2 + 1;
			if( index3 == polygon->m_vertexCount )
				index3 = 0;
			
			kkTriangle t;
			t.v1 = math::mul( base_vertex_node->m_element->m_position, M ) + m_pivot;
			t.v2 = math::mul( VN1->m_element->m_position, M ) + m_pivot;
			t.v3 = math::mul( VN2->m_element->m_position, M ) + m_pivot;
			t.update();

			float f1,f2,f3,f4;
			switch (alg)
			{
			default:
			case kkRayTriangleIntersectionAlgorithm::MollerTrumbore:
				if( t.rayTest_MT(r, true, f1, f2, f3, f4 ) )
				{
					iResult.m_T = f1;
					iResult.m_U = f2;
					iResult.m_V = f3;
					iResult.m_W = f4;
					iResult.m_intersectionPoint = r.m_origin + r.m_direction * f1;
					iResult.m_polygon = polygon;
					result.push_back(iResult);
					ret = true;
				}
				break;
			case kkRayTriangleIntersectionAlgorithm::Watertight:
				if( t.rayTest_Watertight(r, true, f1, f2, f3, f4 ) )
				{
					iResult.m_T = f1;
					iResult.m_U = f2;
					iResult.m_V = f3;
					iResult.m_W = f4;
					iResult.m_intersectionPoint = r.m_origin + r.m_direction * f1;
					iResult.m_polygon = polygon;
					result.push_back(iResult);
					ret = true;
				}
				break;
			}
			VN1 = VN2;
			VN2 = VN2->m_right;
		}
		if(polygon == end)
			break;
		polygon = polygon->m_mainNext;
	}
	return ret;
}

void        Scene3DObject::updateAABB_vertex()
{
	m_aabbOriginal.reset();
	auto current_vertex = m_polyModel->m_verts;
	for( u64 i = 0; i < m_polyModel->m_vertsCount; ++i )
	{
		m_aabbOriginal.add( current_vertex->m_position );
		current_vertex = current_vertex->m_mainNext;
	}
}

void Scene3DObject::applyMatrices()
{
	auto TIM = m_matrix;
	TIM.invert();
	TIM.transpose();

	auto V = m_polyModel->m_verts;
	for( size_t i = 0; i < m_polyModel->m_vertsCount; ++i )
	{
		V->m_position = math::mul(V->m_positionFix,m_matrix);
		V->m_positionFix = V->m_position;
		V = V->m_mainNext;
	}
	auto P = m_polyModel->m_polygons;
	if(P->m_normals)
	{
		for( size_t i = 0; i < m_polyModel->m_polygonsCount; ++i )
		{
			auto PN = P->m_normals;
			for( size_t i2 = 0; i2 < P->m_vertexCount; ++i2 )
			{
				PN[i2] = math::mul(PN[i2], TIM);
			}
			P = P->m_mainNext;
		}
	}
	resetMatrices();
	_rebuildModel();
	UpdateAabb();
}

void Scene3DObject::resetMatrices()
{
	m_matrix			= kkMatrix4();
	m_matrixFixed		= m_matrix;
	m_matrixOnlyRotation= m_matrix;

	auto & s = GetScale();
	s.set(1.f,1.f,1.f,1.f);
	auto & r = GetRotationAngles();
	r.set(0.f,0.f,0.f,0.f);
	r = GetRotationPitchYawRoll();
	r.set(0.f,0.f,0.f,0.f);
	UpdateAabb();
}

void Scene3DObject::generateNormals()
{
	m_polyModel->generateNormals(false);
	_rebuildModel();
}

void Scene3DObject::deleteSelectedPolys()
{
	u64 size = 0;
	auto P = m_polyModel->m_polygons;
	for( size_t i = 0; i < m_polyModel->m_polygonsCount; ++i )
	{
		if(P->m_flags & P->EF_SELECTED)
		{
			++size;
		}
		P = P->m_mainNext;
	}
	if(size)
	{
		std::unordered_set<kkPolygon*> polys;
		polys.reserve(size);
		P = m_polyModel->m_polygons;
		for( size_t i = 0; i < m_polyModel->m_polygonsCount; ++i )
		{
			if(P->m_flags & P->EF_SELECTED)
			{
				polys.insert(P);
			}
			P = P->m_mainNext;
		}
		for(auto P : polys)
		{
			m_polyModel->DeletePolygon(P);
		}
		_rebuildModel();
		UpdateAabb();
	}
}

void Scene3DObject::deleteSelectedEdges()
{
	u64 size = 0;
	auto E = m_polyModel->m_edges;
	for( size_t i = 0; i < m_polyModel->m_edgesCount; ++i )
	{
		if(E->m_flags & E->EF_SELECTED)
		{
			size += 2;
		}
		E = E->m_mainNext;
	}
	if(size)
	{
		std::unordered_set<kkPolygon*> polys;
		polys.reserve(size);
		E = m_polyModel->m_edges;
		for( size_t i = 0; i < m_polyModel->m_edgesCount; ++i )
		{
			if(E->m_flags & E->EF_SELECTED)
			{
				if( E->m_p1 )
					polys.insert(E->m_p1);
				if( E->m_p2 )
					polys.insert(E->m_p2);
			}
			E = E->m_mainNext;
		}
		for(auto P : polys)
		{
			m_polyModel->DeletePolygon(P);
		}
		_rebuildModel();
		UpdateAabb();
	}
}

void Scene3DObject::deleteSelectedVerts()
{
	u64 size = 0;
	auto V = m_polyModel->m_verts;
	for( size_t i = 0; i < m_polyModel->m_vertsCount; ++i )
	{
		if(V->m_flags & V->EF_SELECTED)
		{
			size += V->m_polygonCount;
		}
		V = V->m_mainNext;
	}
	if(size)
	{
		std::unordered_set<kkPolygon*> polys;
		polys.reserve(size);
		V = m_polyModel->m_verts;
		for( size_t i = 0; i < m_polyModel->m_vertsCount; ++i )
		{
			if(V->m_flags & V->EF_SELECTED)
			{
				auto VP = V->m_polygons;
				for( size_t i2 = 0; i2 < V->m_polygonCount; ++i2 )
				{
					polys.insert(VP->m_element);
					VP = VP->m_right;
				}
			}
			V = V->m_mainNext;
		}
		for(auto P : polys)
		{
			m_polyModel->DeletePolygon(P);
		}
		_rebuildModel();
		UpdateAabb();
	}
}

kkPluginGUIWindow* Scene3DObject::GetPluginGUIWindow()
{
	return m_paramsWindow;
}

void Scene3DObject::SetPluginGUIWindow( kkPluginGUIWindow* w )
{
	m_paramsWindow = (PluginGUIWindow*)(w);
}

void Scene3DObject::PrepareForRaytracing(kkRenderInfo* ri)
{
	m_polyModel->prepareForRaytracing(ri );
}

void Scene3DObject::FinishRaytracing()
{
	m_polyModel->finishRaytracing();
}

void Scene3DObject::RayTest( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial )
{
	m_polyModel->rayTest(outTriangles, ray, renderObjectMaterial);
}

void Scene3DObject::RayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation* renderObjectMaterial )
{
	//m_polyModel->rayTestGrid(outTriangles, point, ray, renderObjectMaterial);
}

void Scene3DObject::setShaderParameter_diffuseColor( const kkColor& color )
{
	m_shaderParameter.m_diffuseColor = color;
}

void Scene3DObject::setShaderParameter_diffuseTexture( kkImageContainerNode* t )
{
	m_shaderParameter.m_diffuseTexture = t;
}



void Scene3DObject::ChangePivotPosition(const kkVector4& position)
{
	/*auto V = m_pivotFixed - position;
	auto M = m_matrix;
	M.invert();
	V.KK_W = 1.f;
	V = math::mul(V,M);
	for( size_t i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		ControlVertex * cv = (ControlVertex*)m_PolyModel->m_controlVerts[ i ];

		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			auto vertex  = (Vertex*)cv->m_verts[i2];
			vertex->m_Position = vertex->m_Position_fix + V;
			vertex->m_Position_fix = vertex->m_Position;
		}
	}
	_rebuildModel();

	ApplyPivot();
	UpdateAabb();*/
}
void Scene3DObject::SelecEdgesBySub()
{
	/*m_isObjectHaveSelectedEdges = false;
	std::unordered_set<Edge*> edgesToDeselect;
	for( size_t i = 0, sz = m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = m_PolyModel->m_edges[i];
		if(E->m_isSelected)
		{
			auto cv1 = E->m_firstPoint;
			auto cv2 = E->m_secondPoint;
			for(auto E2 : cv1->m_edges)
			{
				if(!E2->m_isSelected)
					edgesToDeselect.insert(E);
			}
			for(auto E2 : cv2->m_edges)
			{
				if(!E2->m_isSelected)
					edgesToDeselect.insert(E);
			}
		}
	}
	for(auto E : edgesToDeselect)
	{
		E->m_isSelected = false;
	}
	for( size_t i = 0, sz = m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = m_PolyModel->m_edges[i];
		if(E->m_isSelected)
		{
			m_isObjectHaveSelectedEdges = true;
			break;
		}
	}
	if(edgesToDeselect.size() > 0)
	{
		m_PolyModel->updateCVEdgeWith();
		updateEdgeModel();
	}*/
}
void Scene3DObject::SelecEdgesByAdd()
{
	/*m_isObjectHaveSelectedEdges = false;
	std::unordered_set<Edge*> edgesToSelect;
	for( size_t i = 0, sz = m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = m_PolyModel->m_edges[i];
		if(E->m_isSelected)
		{
			auto cv1 = E->m_firstPoint;
			auto cv2 = E->m_secondPoint;

			for(auto E2 : cv1->m_edges)
			{
				if(!E2->m_isSelected)
					edgesToSelect.insert(E2);
			}
			for(auto E2 : cv2->m_edges)
			{
				if(!E2->m_isSelected)
					edgesToSelect.insert(E2);
			}
		}
	}
	for(auto E : edgesToSelect)
	{
		E->m_isSelected = true;
	}	
	if(edgesToSelect.size() > 0)
	{
		m_isObjectHaveSelectedEdges = true;
		m_PolyModel->updateCVEdgeWith();
		updateEdgeModel();
	}*/
}
void Scene3DObject::SelecEdgesByLoop()
{
	/*bool need_update = false;
	bool is_sel = false;
	for( size_t i = 0, sz = m_PolyModel->m_edges.size(), last = m_PolyModel->m_edges.size()-1; i < sz;  )
	{
		auto E = m_PolyModel->m_edges[i];
		if(E->m_isSelected)
		{
			if( E->m_firstPoint->m_edges.size() == 4 )
			{
				u64 edge_index = 0;
				for( size_t i2 = 0, sz2 = E->m_firstPoint->m_edges.size(); i2 < sz2; ++i2 )
				{
					auto e = E->m_firstPoint->m_edges[i2];
					if(E == e)
					{
						edge_index = i2;
						break;
					}
				}
				switch(edge_index)
				{
				default:
				case 0: edge_index = 2; break;
				case 1: edge_index = 3; break;
				case 2: edge_index = 0; break;
				case 3: edge_index = 1; break;
				}
				auto E2 = E->m_firstPoint->m_edges[edge_index];
				if(!E2->m_isSelected)
				{
					E2->m_isSelected = true;
					need_update = true;
					is_sel = true;
				}
			}
			if( E->m_secondPoint->m_edges.size() == 4 )
			{
				u64 edge_index = 0;
				for( size_t i2 = 0, sz2 = E->m_secondPoint->m_edges.size(); i2 < sz2; ++i2 )
				{
					auto e = E->m_secondPoint->m_edges[i2];
					if(E == e)
					{
						edge_index = i2;
						break;
					}
				}
				switch(edge_index)
				{
				default:
				case 0: edge_index = 2; break;
				case 1: edge_index = 3; break;
				case 2: edge_index = 0; break;
				case 3: edge_index = 1; break;
				}
				auto E2 = E->m_secondPoint->m_edges[edge_index];
				if(!E2->m_isSelected)
				{
					E2->m_isSelected = true;
					need_update = true;
					is_sel = true;
				}
			}
		}
		++i;
		if(i == last && is_sel)
		{
			i = 0;
			is_sel = false;
		}
	}
	if(need_update)
	{
		m_isObjectHaveSelectedEdges = true;
		m_PolyModel->updateCVEdgeWith();
		updateEdgeModel();
	}*/
}
void Scene3DObject::SelecEdgesByRing()
{
	//bool need_update = false;
	//bool is_sel = false;
	//for( size_t i = 0, sz = m_PolyModel->m_edges.size(), last = m_PolyModel->m_edges.size()-1; i < sz;  )
	//{
	//	auto E = m_PolyModel->m_edges[i];
	//	if(E->m_isSelected)
	//	{
	//		Polygon3D* P1 = E->m_firstPolygon;
	//		// найду противоположное ребро
	//		if(P1->m_edges.size() == 4)
	//		{
	//			u64 edge_index = 0;
	//			for( size_t i2 = 0, sz2 = P1->m_edges.size(); i2 < sz2; ++i2 )
	//			{
	//				auto e = P1->m_edges[i2];
	//				if(E == e)
	//				{
	//					edge_index = i2;
	//					break;
	//				}
	//			}
	//			switch(edge_index)
	//			{
	//			default:
	//			case 0: edge_index = 2; break;
	//			case 1: edge_index = 3; break;
	//			case 2: edge_index = 0; break;
	//			case 3: edge_index = 1; break;
	//			}
	//			auto E2 = P1->m_edges[edge_index];
	//			if(!E2->m_isSelected)
	//			{
	//				E2->m_isSelected = true;
	//				need_update = true;
	//				is_sel = true;
	//			}
	//		}

	//		Polygon3D* P2 = nullptr;
	//		if(E->m_secondPolygon)
	//		{
	//			P2 = E->m_secondPolygon;
	//			if(P2->m_edges.size() == 4)
	//			{
	//				u64 edge_index = 0;
	//				for( size_t i2 = 0, sz2 = P2->m_edges.size(); i2 < sz2; ++i2 )
	//				{
	//					auto e = P2->m_edges[i2];
	//					if(E == e)
	//					{
	//						edge_index = i2;
	//						break;
	//					}
	//				}
	//				switch(edge_index)
	//				{
	//				default:
	//				case 0: edge_index = 2; break;
	//				case 1: edge_index = 3; break;
	//				case 2: edge_index = 0; break;
	//				case 3: edge_index = 1; break;
	//				}
	//				auto E2 = P2->m_edges[edge_index];
	//				if(!E2->m_isSelected)
	//				{
	//					E2->m_isSelected = true;
	//					need_update = true;
	//					is_sel = true;
	//				}
	//			}
	//		}
	//	}
	//	++i;
	//	if(i == last && is_sel)
	//	{
	//		i = 0;
	//		is_sel = false;
	//	}
	//}
	//if(need_update)
	//{
	//	m_isObjectHaveSelectedEdges = true;
	//	m_PolyModel->updateCVEdgeWith();
	//	updateEdgeModel();
	//}
}


void Scene3DObject::SelecPolygonsByAdd()
{
	/*m_isObjectHaveSelectedPolys = false;
	for(auto P : m_PolyModel->m_polygons)
	{
		if(P->IsSelected())
			m_isObjectHaveSelectedPolys = true;
	}
	std::unordered_set<Polygon3D*> polygonsToSelect;
	for( size_t i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		auto P = (Polygon3D*)m_PolyModel->m_polygons[i];
		if(P->m_isSelected)
		{
			for( auto NP : P->m_neighbors )
			{
				if(!NP->m_isSelected)
				{
					polygonsToSelect.insert(NP);
				}
			}
		}
	}
	for(auto P : polygonsToSelect)
	{
		m_isObjectHaveSelectedPolys = true;
		P->Select();
	}
	m_PolyModel->updateCVForPolygonSelect();
	if(m_isObjectHaveSelectedPolys)
		updatePolygonModel();*/
}

void Scene3DObject::SelecVertsByAdd()
{
	/*m_isObjectHaveSelectedVerts = false;
	std::vector<ControlVertex*> vertsToSelect;
	for( size_t i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlVerts[ i ];
		if(cv->m_isSelected)
		{
			m_isObjectHaveSelectedVerts = true;

			for( auto c : cv->m_edges )
			{
				if( !c->m_firstPoint->m_isSelected )
					vertsToSelect.emplace_back(c->m_firstPoint);
				if( !c->m_secondPoint->m_isSelected )
					vertsToSelect.emplace_back(c->m_secondPoint);
			}
		}
	}
	for( auto c : vertsToSelect )
	{
		c->select();
	}

	if(m_isObjectHaveSelectedVerts)
		updateModelPointsColors();*/
}

void Scene3DObject::SelecVertsBySub()
{
	/*std::vector<ControlVertex*> vertsToDeselect;
	for( size_t i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlVerts[ i ];
		if(!cv->m_isSelected)
		{
			for( auto c : cv->m_edges )
			{
				if( c->m_firstPoint->m_isSelected )
					vertsToDeselect.emplace_back(c->m_firstPoint);
				if( c->m_secondPoint->m_isSelected )
					vertsToDeselect.emplace_back(c->m_secondPoint);
			}
		}
	}
	for( auto c : vertsToDeselect )
	{
		c->deselect();
	}

	m_isObjectHaveSelectedVerts = false;
	for( size_t i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlVerts[ i ];
		if(cv->m_isSelected)
		{
			m_isObjectHaveSelectedVerts = true;
			break;
		}
	}
	updateModelPointsColors();*/
}

void Scene3DObject::AttachObject(kkScene3DObject* object)
{
	if( object->GetType() == kkScene3DObjectType::PolygonObject )
	{
		auto MI = m_matrix;
		MI.invert();
		auto polyObject = (Scene3DObject*)object;
		m_polyModel->attachModel(polyObject->m_polyModel, MI, polyObject->GetMatrix(), m_pivot, polyObject->GetPivot());
		this->_rebuildModel();
	}
}

void Scene3DObject::BreakVerts()
{
	/*for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}
	}

	for( size_t i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlVerts[ i ];
		if(cv->m_isSelected)
		{
			for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
			{
				auto vertex  = (Vertex*)cv->m_verts[i2];
				vertex->m_weld = false;
			}
		}
	}
	m_isObjectHaveSelectedVerts = false;
	m_PolyModel->createControlPoints();
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
		for(auto v : CV->m_verts)
		{
			if(((Vertex*)v)->m_isCVSelected)
			{
				CV->select();
				m_isObjectHaveSelectedVerts = true;
			}
		}
	}
	this->_rebuildModel();
	updateModelPointsColors();*/
}


void Scene3DObject::Weld(kkVertex* V1, kkVertex* V2)
{
	//ControlVertex* cv1 = (ControlVertex*)CV1;
	//ControlVertex* cv2 = (ControlVertex*)CV2;

	//bool is_edge = false;
	//Edge * edge = nullptr;
	//for( auto e : cv1->m_edges )
	//{
	//	edge = e;
	//	if(e->m_firstPoint == cv2 || e->m_secondPoint == cv2)
	//	{
	//		is_edge = true;
	//		break;
	//	}
	//}


	//// есть 2 случая, когда надо делать weld
	//// 1. когда точки образуют ребро
	//// 2. когда хоть 1 ребро у каждой точки имеет только 1 полигон на стороне (то есть точка как бы снаружи)

	//// сначала перемещу нужную вершину к другой вершине
	//// потом, нахожу 1 или 2 полигона
	//// из полигонов удаляю вершину, если вершины три то удаляю сам полигон
	//// удаляю вершины из главного массива с вершинами m_verts, 
	//Vertex* targetVertex = (Vertex*)cv2->m_verts[0];
	//Vertex* pickVertex = (Vertex*)cv1->m_verts[0];
	//bool onEdge = false;
	//for(auto cvedge : cv1->m_edges)
	//{
	//	if( cvedge->m_secondPolygon )
	//	{
	//		onEdge = true;
	//		break;
	//	}
	//}
	//if(!onEdge)
	//{
	//	for(auto cvedge : cv2->m_edges)
	//	{
	//		if( cvedge->m_secondPolygon )
	//		{
	//			onEdge = true;
	//			break;
	//		}
	//	}
	//}

	//if(is_edge || onEdge)
	//{
	//	auto half = (targetVertex->m_Position - pickVertex->m_Position) * 0.5f;
	//	for( auto V : cv1->m_verts )
	//	{
	//		//Vertex* vertex = (Vertex*)m_PolyModel->m_verts[ vertexIndex ];
	//		Vertex* vertex = (Vertex*)V;
	//		vertex->m_Boneinds = targetVertex->m_Boneinds;
	//		vertex->m_Color = targetVertex->m_Color;
	//		vertex->m_Normal = targetVertex->m_Normal;
	//		vertex->m_Normal_fix = targetVertex->m_Normal_fix;
	//			vertex->m_Position = targetVertex->m_Position;
	//			vertex->m_Position_fix = targetVertex->m_Position_fix;
	//		vertex->m_UV = targetVertex->m_UV;
	//		vertex->m_Weights = targetVertex->m_Weights;
	//		vertex->m_weld = true;
	//		targetVertex->m_weld = true;
	//	}

	//}
	//else
	//{
	//	return;
	//}
	//
	//std::unordered_set<Polygon3D*> polysForNormRecalculate;
	//for( auto V : cv1->m_verts )
	//{
	//	Vertex* vertex = (Vertex*)V;
	//	polysForNormRecalculate.insert(vertex->m_parentPolygon);
	//}

	//if(is_edge)
	//{

	//	Polygon3D* P1 = edge->m_firstPolygon;
	//	Polygon3D* P2 = nullptr;
	//	if(edge->m_secondPolygon )
	//	{
	//		P2 = edge->m_secondPolygon;
	//	}

	//	// удалить вершину из полигона и из основного массива
	//	if(P1->m_verts.size() > 3)
	//	{
	//		// теперь нужно удалить вершину из полигона и из массива с вершинами
	//		// нужно найти вершину внутри контрольной вершины, которая принадлежит данному полигону
	//		kkVertex * vertex_for_delete = nullptr;
	//		for( u64 i = 0, sz = P1->m_verts.size(); i < sz; ++i )
	//		{
	//			//for( auto vertexIndex_CV : cv1->m_vertexIndex )
	//			for( u64 j = 0, jsz = cv1->m_verts.size(); j < jsz; ++j )
	//			{
	//				if( cv1->m_verts[ j ] == P1->m_verts[ i ] )
	//				{
	//					vertex_for_delete = P1->m_verts[ i ];
	//					goto end;
	//				}
	//			}
	//		}
	//		end:;
	//		if(vertex_for_delete)
	//		{
	//			P1->m_verts.erase_first(vertex_for_delete);
	//			m_PolyModel->m_verts.erase_first(vertex_for_delete);
	//			kkDestroy(vertex_for_delete);
	//		}
	//	}
	//	else
	//	{
	//		for( u64 i = 0, sz = P1->m_verts.size(); i < sz; ++i )
	//		{
	//			auto V = P1->m_verts[i];
	//			m_PolyModel->m_verts.erase_first(V);
	//			kkDestroy(V);
	//		}

	//		m_PolyModel->m_polygons.erase_first(P1);
	//		kkDestroy(P1);
	//		polysForNormRecalculate.erase(P1);
	//	}

	//	if(P2)
	//	{
	//		if(P2->m_verts.size() > 3)
	//		{
	//			kkVertex * vertex_for_delete = nullptr;
	//			for( u64 i = 0, sz = P2->m_verts.size(); i < sz; ++i )
	//			{
	//				for( u64 j = 0, jsz = cv1->m_verts.size(); j < jsz; ++j )
	//				{
	//					if( cv1->m_verts[ j ] == P2->m_verts[ i ] )
	//					{
	//						vertex_for_delete = P2->m_verts[ i ];
	//						goto end2;
	//					}
	//				}
	//			}
	//			end2:;
	//			if(vertex_for_delete)
	//			{
	//				P2->m_verts.erase_first(vertex_for_delete);
	//				m_PolyModel->m_verts.erase_first(vertex_for_delete);
	//				kkDestroy(vertex_for_delete);
	//			}
	//		}
	//		else
	//		{
	//			for( u64 i = 0, sz = P2->m_verts.size(); i < sz; ++i )
	//			{
	//				auto V = P2->m_verts[i];
	//				m_PolyModel->m_verts.erase_first(V);
	//				kkDestroy(V);
	//			}
	//			m_PolyModel->m_polygons.erase_first(P2);
	//			kkDestroy(P2);
	//			polysForNormRecalculate.erase(P2);
	//		}
	//	}
	//}

	//for(auto P : polysForNormRecalculate)
	//{
	//	P->CalculateNormals();
	//}
	//
	//for( u64 i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i];

	//	bool sel = CV->isSelected();
	//	for( auto V : CV->m_verts )
	//	{
	//		Vertex* vertex = (Vertex*)V;
	//		vertex->m_isCVSelected = sel;
	//	}

	//}

	//m_PolyModel->createControlPoints();

	//for( u64 i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i];
	//	if(((Vertex*)CV->m_verts[0])->m_isCVSelected)
	//	{
	//		CV->select();
	//	}
	//}

	//this->_rebuildModel();
	//updateModelPointsColors();
}

void Scene3DObject::WeldSelectedVerts(f32 len)
{
	//for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
	//	bool sel = CV->isSelected();
	//	for( auto V : CV->m_verts )
	//	{
	//		Vertex* vertex = (Vertex*)V;
	//		vertex->m_isCVSelected = sel;
	//	}
	//}

	////беру каждую вершину, и проверяю со специальными вершинами которых нужно придумать
	//struct _weld_vertex
	//{
	//	kkVector4 m_center; // единая позиция
	//	std::vector<Vertex*> m_verts; // вершины которые добавляются в зависимости от расстояния
	//};
	//kkArray<_weld_vertex*> weld_verts = kkArray<_weld_vertex*>(0xffff);

	//for( u64 i = 0, sz = m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i];
	//	if(!CV->m_isSelected)
	//		continue;

	//	Vertex* V = (Vertex*)CV->m_verts[0];

	//	bool vertex_found = false;
	//	for( u64 k = 0, ksz = weld_verts.size(); k < ksz; ++k )
	//	{
	//		auto WV = weld_verts[k];
	//		if( V->m_Position.distance(WV->m_center) <= len )
	//		{
	//			vertex_found = true;
	//			k = ksz;

	//			kkVector4 vp = WV->m_center - V->m_Position;
	//			vp *= 0.5f;

	//			for(auto v : CV->m_verts)
	//			{
	//				WV->m_verts.emplace_back((Vertex*)v);
	//			}
	//			WV->m_center -= vp; 
	//			break;
	//		}
	//	}
	//	if(!vertex_found)
	//	{
	//		_weld_vertex * WV = new _weld_vertex;
	//		WV->m_center = V->m_Position;
	//		for(auto v : CV->m_verts)
	//		{
	//			WV->m_verts.emplace_back((Vertex*)v);
	//		}
	//		weld_verts.push_back(WV);
	//	}
	//}

	//// изменяю координаты вершин
	//for( u64 i = 0, sz = weld_verts.size(); i < sz; ++i )
	//{
	//	auto WV = weld_verts[i];
	//	for( u64 j = 0, jsz = WV->m_verts.size(); j < jsz; ++j )
	//	{
	//		auto V = WV->m_verts[j];
	//		V->m_Position = WV->m_center;
	//		V->m_Position_fix = WV->m_center;
	//		V->m_weld = true;
	//	}
	//	delete weld_verts[i];
	//}

	//// проверяю полигоны. если координаты точки равны то лишнюю нужно удалить
	//// если остаётся 2 и менее вершин то надо удалить сам полигон
	//auto isPolygonDelete = [&](Polygon3D* P)->bool
	//{
	//	begin:
	//	for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
	//	{
	//		Vertex* V1 = (Vertex*)P->m_verts[i];
	//		for( u64 i2 = i+1, sz2 = P->m_verts.size(); i2 < sz2; ++i2 )
	//		{
	//			Vertex* V2 = (Vertex*)P->m_verts[i2];
	//			if(V1->m_Position == V2->m_Position)
	//			{
	//				V2->m_isToDelete = true;
	//				P->m_verts.erase_first(V2);
	//				goto begin;
	//			}
	//		}
	//	}

	//	return P->m_verts.size() < 3;
	//};
	//
	//for( u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	//{
	//	Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];
	//	if( isPolygonDelete(P) )
	//	{
	//		P->MarkToDelete();
	//	}
	//}

	//// удаление.
	//m_PolyModel->deleteMarkedPolygons();

	//m_PolyModel->createControlPoints();
	//this->_rebuildModel();
	//updateModelPointsColors();
}

void Scene3DObject::ConnectVerts()
{
	//// сначала надо пометить реальные вершины, какие выбраны какие нет
	//for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
	//	bool sel = CV->isSelected();
	//	for( auto V : CV->m_verts )
	//	{
	//		Vertex* vertex = (Vertex*)V;
	//		vertex->m_isCVSelected = sel;
	//	}
	//}

	//std::basic_string<Vertex*> vertex_buffer;
	//kkArray<Polygon3D*> new_polygons = kkArray<Polygon3D*>(0xff);
	//kkArray<Vertex*> new_verts = kkArray<Vertex*>(0xff);

	//// прохожусь по полигонам
	//for( u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	//{
	//	vertex_buffer.clear();

	//	Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];

	//	// прохожусь по полигону в поисках выбранной вершины.
	//	// если вершина находится, но kcnt должен сброситься, чтобы пройтись по массиву в полном объёме
	//	// соответственно, нужно сделать проверку на выход за пределы
	//	bool start = false;
	//	bool needToConnect = false;
	//	for( u64 ki = 0, kcnt = 0, kisz = P->m_verts.size(); kcnt < kisz; ++kcnt)
	//	{
	//		Vertex* vertex = (Vertex*)P->m_verts[ki];
	//		if(vertex->m_isCVSelected)
	//		{
	//			if(!start)
	//			{
	//				// нашли первую выделенную вершину
	//				start = true;
	//				kcnt = 0;
	//			}
	//			else
	//			{
	//				// есть вторая или более выбранных вершин
	//				needToConnect = true;
	//			}
	//		}
	//		++ki;
	//		if(start)
	//		{
	//			vertex_buffer.push_back(vertex);
	//		}
	//		if(ki == kisz)
	//			ki = 0;
	//	}
	//	if(needToConnect)
	//	{
	//		P->MarkToDelete();

	//		Vertex* baseVertex = vertex_buffer[0];
	//		Polygon3D* newPolygon = nullptr;
	//		Vertex* newVertex = nullptr;

	//		for( size_t j = 1, jsz = vertex_buffer.size(), vertex_counter = 0; j < jsz; ++j )
	//		{
	//			if(!newPolygon)
	//			{
	//				vertex_counter = 1;
	//				newPolygon = kkCreate<Polygon3D>();
	//				newPolygon->m_model = m_PolyModel;
	//				newVertex = kkCreate<Vertex>();
	//				newVertex->set(baseVertex);
	//				newVertex->m_parentPolygon = newPolygon;
	//				newVertex->m_isCVSelected = baseVertex->m_isCVSelected;
	//				newPolygon->addVertex(newVertex);
	//				new_verts.push_back(newVertex);
	//			}

	//			auto nextVertex = vertex_buffer[j];

	//			newVertex = kkCreate<Vertex>();
	//			newVertex->set(nextVertex);
	//			newVertex->m_parentPolygon = newPolygon;
	//			newVertex->m_isCVSelected = nextVertex->m_isCVSelected;
	//			newPolygon->addVertex(newVertex);
	//			new_verts.push_back(newVertex);
	//			++vertex_counter;

	//			bool last = j == jsz - 1;
	//			if((nextVertex->m_isCVSelected && vertex_counter > 2) || last )
	//			{
	//				if(j != 1)
	//				{
	//					new_polygons.push_back(newPolygon);
	//					newPolygon = nullptr;
	//					if(!last)
	//						--j;
	//				}
	//			}
	//		}
	//	}
	//}

	//auto newPolygonsSize = new_polygons.size();
	//u64  currentNewPolygon = 0;
	//// помечу вершины полигонов чтобы потом их удалить
	//for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i)
	//{
	//	Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];
	//	if(P->IsToDelete())
	//	{
	//		for(u64 j = 0, jsz = P->m_verts.size(); j < jsz; ++j)
	//		{
	//			Vertex* V = (Vertex*)P->m_verts[j];
	//			V->m_isToDelete = true;
	//		}
	//		kkDestroy(P);
	//		m_PolyModel->m_polygons[i] = new_polygons[currentNewPolygon];
	//		++currentNewPolygon;
	//	}
	//}
	//
	//while(currentNewPolygon < newPolygonsSize)
	//{
	//	m_PolyModel->m_polygons.push_back(new_polygons[currentNewPolygon]);
	//	++currentNewPolygon;
	//}

	//auto newVertssSize = new_verts.size();
	//u64  currentNewVertex = 0;
	//for(u64 i = 0, sz = m_PolyModel->m_verts.size(); i < sz; ++i)
	//{
	//	Vertex* V = (Vertex*)m_PolyModel->m_verts[i];
	//	if(V->m_isToDelete)
	//	{
	//		kkDestroy(V);

	//		m_PolyModel->m_verts[i] = new_verts[currentNewVertex];
	//		++currentNewVertex;
	//	}
	//}
	//while(currentNewVertex < newVertssSize)
	//{
	//	m_PolyModel->m_verts.push_back(new_verts[currentNewVertex]);
	//	++currentNewVertex;
	//}

	//m_PolyModel->createControlPoints();
	//for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];

	//	for(auto v : CV->m_verts)
	//	{
	//		if(((Vertex*)v)->m_isCVSelected)
	//		{
	//			CV->select();
	//		}
	//	}

	//}
	//this->_rebuildModel();
	//updateModelPointsColors();
}

void Scene3DObject::ChamferVerts(f32 len, bool addPolygon)
{
	//if(len == 0.f)
	//	return;

	//for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
	//	bool sel = CV->isSelected();
	//	for( auto V : CV->m_verts )
	//	{
	//		Vertex* vertex = (Vertex*)V;
	//		vertex->m_isCVSelected = sel;
	//	}
	//}

	//struct _new_face
	//{
	//	ControlVertex* m_base_cv = nullptr;
	//	kkArray<Vertex*> m_verts = kkArray<Vertex*>(4); // for copy
	//	kkVector4 m_cv_normal;
	//	kkVector4 m_cv_position;
	//};
	//kkArray<_new_face*> new_faces = kkArray<_new_face*>(0xff);


	//kkArray<Polygon3D*> new_polygons = kkArray<Polygon3D*>(0xff);
	//for( u64 i2 = 0, sz2 = m_PolyModel->m_polygons.size(); i2 < sz2; ++i2 )
	//{
	//	bool withSelectedVerts = false;
	//	Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i2];
	//	for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
	//	{
	//		Vertex* vertex = (Vertex*)P->m_verts[i];
	//		if(vertex->m_isCVSelected)
	//		{
	//			withSelectedVerts = true;
	//			break;
	//		}
	//	}

	//	if(!withSelectedVerts)
	//		continue;
	//
	//	P->MarkToDelete();

	//	Polygon3D* new_P = kkCreate<Polygon3D>();
	//	new_polygons.push_back(new_P);

	//	for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
	//	{
	//		Vertex* vertex = (Vertex*)P->m_verts[i];
	//		if(vertex->m_isCVSelected)
	//		{
	//			Vertex* next_vertex = nullptr;
	//			if( i + 1 == sz )
	//			{
	//				next_vertex = (Vertex*)P->m_verts[0];
	//			}
	//			else
	//			{
	//				next_vertex = (Vertex*)P->m_verts[i+1];
	//			}

	//			Vertex* prev_vertex = nullptr;
	//			if(i == 0)
	//			{
	//				prev_vertex = (Vertex*)P->m_verts[sz-1];
	//			}
	//			else
	//			{
	//				prev_vertex = (Vertex*)P->m_verts[i-1];
	//			}

	//			Vertex* new_vertex1 = kkCreate<Vertex>();
	//			Vertex* new_vertex2 = kkCreate<Vertex>();
	//			new_vertex1->set(vertex);
	//			new_vertex2->set(vertex);
	//			new_vertex1->m_parentPolygon = new_P;
	//			new_vertex2->m_parentPolygon = new_P;
	//			new_P->addVertex(new_vertex1);
	//			new_P->addVertex(new_vertex2);
	//	 
	//			// T.intersectionPoint = ray.m_origin + ray.m_direction * len;
	//			// дальше нужно попробовать поставить вершины на позиции
	//			auto dir = prev_vertex->m_Position - vertex->m_Position;
	//			dir.normalize2();

	//			new_vertex1->m_Position = vertex->m_Position + dir * len;
	//			new_vertex1->m_Position_fix = new_vertex1->m_Position;

	//			dir = next_vertex->m_Position - vertex->m_Position;
	//			dir.normalize2();
	//			new_vertex2->m_Position = vertex->m_Position + dir * len;
	//			new_vertex2->m_Position_fix = new_vertex2->m_Position;
	//			
	//			if(addPolygon && vertex->m_controlVertex->m_verts.size() > 1)
	//			{
	//				_new_face * nf_ptr = nullptr;
	//				for( u64 o = 0, osz = new_faces.size(); o < osz; ++o )
	//				{
	//					auto nf = new_faces[o];
	//					if(nf->m_base_cv == vertex->m_controlVertex)
	//					{
	//						nf_ptr = nf;
	//						break;
	//					}
	//				}
	//				if(!nf_ptr)
	//				{
	//					_new_face * nf = new _new_face;
	//					nf->m_base_cv = vertex->m_controlVertex;
	//					nf->m_cv_normal = vertex->m_controlVertex->m_faceNormal;
	//					nf->m_cv_position = vertex->m_controlVertex->m_verts[0]->getPosition();
	//					nf_ptr = nf;
	//					new_faces.push_back(nf);
	//				}
	//				nf_ptr->m_verts.push_back(new_vertex1);
	//			}
	//		}
	//		else
	//		{
	//			Vertex* new_vertex = kkCreate<Vertex>();
	//			new_vertex->set(vertex);
	//			new_vertex->m_parentPolygon = new_P;
	//			new_P->addVertex(new_vertex);
	//		}
	//	}

	//}
	//m_PolyModel->deleteMarkedPolygons();

	//if(addPolygon)
	//{
	//	for( u64 i = 0, sz = new_faces.size(); i < sz; ++i )
	//	{
	//		auto nf = new_faces[i];

	//		Polygon3D* new_P = kkCreate<Polygon3D>();
	//		new_polygons.push_back(new_P);

	//		kkAabb aabb;
	//		for( u64 k = 0, ksz = nf->m_verts.size(); k < ksz; ++k )
	//		{
	//			Vertex* new_vertex = kkCreate<Vertex>();
	//			new_vertex->set(nf->m_verts[k]);
	//			new_vertex->m_parentPolygon = new_P;
	//			new_P->addVertex(new_vertex);
	//			aabb.add(new_vertex->m_Position);
	//		}
	//		
	//		new_P->CalculateNormals();

	//		kkVector4 P0 = nf->m_cv_position;
	//		kkVector4 P1;
	//		aabb.center(P1);
	//		kkVector4 N0 = nf->m_cv_normal + P0;
	//		kkVector4 N1 = new_P->m_facenormal + P1;
	//		kkVector4 delta = P1 - P0;
	//		f32 dp0 = delta.dot(N0);
	//		f32 dp1 = delta.dot(N1);

	//		//printf("%f %f\n", dp0, dp1);
	//		if( dp0 < 0.f && dp1 > 0.f )
	//		{
	//			new_P->Flip();
	//			new_P->CalculateNormals();
	//		}
	//		
	//		delete nf;
	//	}
	//}

	//for(auto P : new_polygons)
	//{
	//	P->m_model = m_PolyModel;
	//	m_PolyModel->m_polygons.push_back(P);
	//	for(auto V : P->m_verts)
	//	{
	//		m_PolyModel->m_verts.push_back(V);
	//	}
	//}
	//m_PolyModel->createControlPoints();
	//for( u64 i2 = 0, sz2 = m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
	//{
	//	ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlVerts[i2];
	//	for(auto v : CV->m_verts)
	//	{
	//		if(((Vertex*)v)->m_isCVSelected)
	//		{
	//			CV->select();
	//		}
	//	}
	//}
	//this->_rebuildModel();
	//updateModelPointsColors();
}

void Scene3DObject::ConnectEdges()
{
	////kkArray<Polygon3D*> new_polygons = kkArray<Polygon3D*>(0xff);
	//for( u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	//{
	//	auto P = (Polygon3D*)m_PolyModel->m_polygons[i];
	//	bool start = false;
	//	for( u64 i2 = 0, sz2 = P->m_edges.size(), num = 0; i2 < sz2; ++i2 )
	//	{
	//		auto E = P->m_edges[i2];
	//		if(E->m_isSelected && !start)
	//		{
	//			++num;
	//		}
	//		if(!start)
	//		{
	//			if(num==2)
	//			{
	//				start = true;
	//				++num;
	//				i2 = -1;
	//			}
	//		}
	//		else
	//		{
	//			if(E->m_isSelected && !E->m_isAlreadyConnected)
	//			{
	//				kkVertex * V1 = nullptr, * V2  = nullptr;
	//				Polygon3D* P1 = E->m_firstPolygon;
	//				Polygon3D* P2 = E->m_secondPolygon;

	//				for(auto V : E->m_firstPoint->m_verts)
	//				{
	//					auto v = (Vertex*)V;
	//					if( v->m_parentPolygon == P1 )
	//					{
	//						V1 = V;
	//						break;
	//					}
	//				}
	//				for(auto V : E->m_secondPoint->m_verts)
	//				{
	//					auto v = (Vertex*)V;
	//					if( v->m_parentPolygon == P1 )
	//					{
	//						V2 = V;
	//						break;
	//					}
	//				}
	//				if(V1 && V2)
	//				{
	//					P1->InsertVertex(V1, V2);
	//					E->m_isAlreadyConnected = true;
	//				}

	//				if(P2)
	//				{
	//					for(auto V : E->m_firstPoint->m_verts)
	//					{
	//						auto v = (Vertex*)V;
	//						if( v->m_parentPolygon == P2 )
	//						{
	//							V1 = V;
	//							break;
	//						}
	//					}
	//					for(auto V : E->m_secondPoint->m_verts)
	//					{
	//						auto v = (Vertex*)V;
	//						if( v->m_parentPolygon == P2 )
	//						{
	//							V2 = V;
	//							break;
	//						}
	//					}
	//					if(V1 && V2)
	//					{
	//						P2->InsertVertex(V1, V2);
	//						E->m_isAlreadyConnected = true;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	///*m_PolyModel->deleteMarkedPolygons();
	//for(auto P : new_polygons)
	//{
	//	m_PolyModel->m_polygons.push_back(P);
	//	for(auto V : P->m_verts)
	//	{
	//		m_PolyModel->m_verts.push_back(V);
	//	}
	//}*/
	//m_PolyModel->createControlPoints();
	//this->_rebuildModel();
	//for( u64 i = 0, sz = m_PolyModel->m_edges.size(); i < sz; ++i )
	//{
	//	auto e = m_PolyModel->m_edges[i];
	//	e->m_isAlreadyConnected = false;
	//}
}
