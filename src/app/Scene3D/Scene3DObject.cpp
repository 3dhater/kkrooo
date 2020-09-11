// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Classes/Math/kkRay.h"
#include "Classes/Containers/kkArray.h"

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "GraphicsSystem/kkMesh.h"
#include "../Geometry/PolygonalModel.h"
#include "../Geometry/Polygon3D.h"
#include "../Geometry/Vertex.h"
#include "Scene3DObject.h"
#include "Scene3D.h"
#include "../Functions.h"

#include "../Application.h"
#include "../Plugins/PluginGUIWindow.h"
#include "Renderer/kkRenderer.h"

#include <thread>
#include <unordered_set>

struct node_hash{
    std::size_t operator()(const std::pair<kkMesh*,kkSMesh*>& _node) const {
		kkStringA str;
		str += (u64)_node.first;
		str += (u64)_node.second;

        return std::hash<std::string>()(str.data());
    }
};

Scene3DObjectShaderParameter Scene3DObject::m_globalShaderParameter;

class kkDefaultMaterial;
extern kkDefaultMaterial * g_defaultMaterial;

Scene3DObject::Scene3DObject(kkScene3DObjectType t, PolygonalModel * m)
	:
	m_PolyModel(m)
{
	m_scene3DObjectType = t;

	m_GS = kkGetGS();

	//m_pointsInScreen.setWordSize( 0xffff );
	//m_pointsInScreen_IDs.setWordSize( 0xffff );

	this->SetMaterialImplementation((kkMaterialImplementation*)g_defaultMaterial);
}

Scene3DObject::~Scene3DObject()
{

	_destroyHardwareModels();
	_destroySoftwareModels();
	if( m_PolyModel )
	{
		kkDestroy(m_PolyModel);
	}
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

void Scene3DObject::_countNumOfLines()
{
	m_numOfLines = 0;

	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		Polygon3D * p = (Polygon3D *)m_PolyModel->m_polygons[i];
		m_numOfLines += (u32)p->m_verts.size();
	}
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

	_countNumOfLines();
	
	_createSoftwareModel_points();
	for(u64 i = 0, sz = m_SoftwareModels_points.size(); i < sz; ++i )
	{
		auto hm = m_GS->createMesh(m_SoftwareModels_points[i], kkMeshType::Points );
		if(!hm){ return false;}
		m_HardwareModels_points.push_back(hm);
	}
	
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
	Polygon3D * polygon;
	u32 num_of_verts;
	u32 triangleCount = 0;
	kkSMesh*     softwareModel = nullptr;
	DefaultVert* verts_ptr     = nullptr;
	u16     *   inds_ptr     = nullptr;
	u16 index = 0;
	Vertex* vertex1;
	Vertex* vertex2;
	Vertex* vertex3;

	// очищаю m_indexForSoftware
	for(u64 i = 0, sz = m_PolyModel->m_verts.size(); i < sz; ++i )
	{
		((Vertex*)m_PolyModel->m_verts[ i ])->m_vertexIndexForSoftware.clear();
	}

	u32 softwareModelIndex = 0;

	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		polygon = (Polygon3D *)m_PolyModel->m_polygons[i];

		num_of_verts = (u32)polygon->m_verts.size();

		u32 index2, index3;
		for( u64 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
		{
			if( triangleCount == 0 )
			{
				softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_TRIANGLES);
				verts_ptr     = (DefaultVert*)softwareModel->m_vertices;
				inds_ptr      = softwareModel->m_indices;
				m_SoftwareModels_polys.push_back(softwareModel);
				softwareModelIndex = (u32)m_SoftwareModels_polys.size() - 1;
				index = 0;
			}
		
			index2  = (u32)i2+1;
			index3  = (u32)index2 + 1;
			if( index3 == num_of_verts )
				index3 = 0;

			vertex2 = (Vertex*)polygon->m_verts[0];
			vertex1 = (Vertex*)polygon->m_verts[index2];
			vertex3 = (Vertex*)polygon->m_verts[index3];

			verts_ptr->Position.x = vertex1->m_Position._f32[0];
			verts_ptr->Position.y = vertex1->m_Position._f32[1];
			verts_ptr->Position.z = vertex1->m_Position._f32[2];

			verts_ptr->Normal.x = vertex1->m_Normal._f32[0];
			verts_ptr->Normal.y = vertex1->m_Normal._f32[1];
			verts_ptr->Normal.z = vertex1->m_Normal._f32[2];

			v4f color(1.f,1.f,1.f,1.f);
			if( kkSingleton<Application>::s_instance->getEditMode() == EditMode::Polygon )
			{
				if( polygon->m_isSelected )
				{
					color.set(1.f, 0.f, 0.f, 2.f);
				}
			}

			verts_ptr->Color = color;

			verts_ptr->UV.x = vertex1->m_UV._f32[0];
			verts_ptr->UV.y = vertex1->m_UV._f32[1];
			
			m_aabbOriginal.add( vertex1->m_Position );

			vertex1->m_vertexIndexForSoftware.push_back( std::pair<u32,u32>(index,softwareModelIndex) );
			*inds_ptr = index; 
			++index;
			++inds_ptr;
			++verts_ptr;

			verts_ptr->Position.x = vertex2->m_Position._f32[0];
			verts_ptr->Position.y = vertex2->m_Position._f32[1];
			verts_ptr->Position.z = vertex2->m_Position._f32[2];
			
			verts_ptr->Normal.x = vertex2->m_Normal._f32[0];
			verts_ptr->Normal.y = vertex2->m_Normal._f32[1];
			verts_ptr->Normal.z = vertex2->m_Normal._f32[2];
			verts_ptr->Color = color;

			verts_ptr->UV.x = vertex2->m_UV._f32[0];
			verts_ptr->UV.y = vertex2->m_UV._f32[1];

			m_aabbOriginal.add( vertex2->m_Position );

			vertex2->m_vertexIndexForSoftware.push_back( std::pair<u32,u32>(index,softwareModelIndex) );
			*inds_ptr = index; 
			++index;
			++inds_ptr;
			++verts_ptr;

			verts_ptr->Position.x = vertex3->m_Position._f32[0];
			verts_ptr->Position.y = vertex3->m_Position._f32[1];
			verts_ptr->Position.z = vertex3->m_Position._f32[2];

			verts_ptr->Normal.x = vertex3->m_Normal._f32[0];
			verts_ptr->Normal.y = vertex3->m_Normal._f32[1];
			verts_ptr->Normal.z = vertex3->m_Normal._f32[2];
			verts_ptr->Color = color;
			
			verts_ptr->UV.x = vertex3->m_UV._f32[0];
			verts_ptr->UV.y = vertex3->m_UV._f32[1];

			m_aabbOriginal.add( vertex3->m_Position );

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
		}
	}
}

void        Scene3DObject::updateModelPointsColors()
{
	struct verts_points{  v3f _pos;  v4f _col;  };
	verts_points       *  verts_ptr     = nullptr;
	ControlVertex * CV = nullptr;
	for(u64 i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		CV = (ControlVertex *)m_PolyModel->m_controlPoints[i];
		auto SM = m_SoftwareModels_points[CV->m_vertexIndexForSoftware_points.second];

		verts_ptr     = (verts_points*)SM->m_vertices;
		verts_ptr += CV->m_vertexIndexForSoftware_points.first;

		if( CV->m_isSelected )
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
	u32         pointCountForLimit = 0;
	kkSMesh*      softwareModel = nullptr;
	u16     *   inds_ptr      = nullptr;
	u16         index         = 0;
	struct verts{  v3f _pos;  v4f _col;  };
	verts       *  verts_ptr     = nullptr;
	u32 softwareModelIndex = 0;
	ControlVertex * CV = nullptr;
	for(u64 i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		CV = (ControlVertex *)m_PolyModel->m_controlPoints[i];

		if( pointCountForLimit == 0 )
		{
			softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_POINTS);
			verts_ptr     = (verts*)softwareModel->m_vertices;
			inds_ptr      = softwareModel->m_indices;
			m_SoftwareModels_points.push_back(softwareModel);
			softwareModelIndex = (u32)m_SoftwareModels_points.size() - 1;
			index = 0;
		}

		verts_ptr->_pos.x   = ((Vertex*)CV->m_verts[0])->m_Position[0];
		verts_ptr->_pos.y   = ((Vertex*)CV->m_verts[0])->m_Position[1];
		verts_ptr->_pos.z   = ((Vertex*)CV->m_verts[0])->m_Position[2];
		verts_ptr->_col.x   = 0.f;
		verts_ptr->_col.y   = 0.f;
		verts_ptr->_col.z   = 1.f;
		verts_ptr->_col.w   = 1.f;
		*inds_ptr = index;
		
		CV->m_vertexIndexForSoftware_points.first = index;
		CV->m_vertexIndexForSoftware_points.second = softwareModelIndex;

		++verts_ptr;
		++inds_ptr;
		++index;
		++softwareModel->m_vCount;
		++softwareModel->m_iCount;


		++pointCountForLimit;
		if( pointCountForLimit == m_pointLimit )
			pointCountForLimit = 0;
	}
}

void Scene3DObject::_createSoftwareModel_edges()
{
	u32         num_of_verts  = 0;
	u32         lineCount = 0;
	u16         index         = 0;
	Polygon3D *   polygon       = nullptr;
	kkSMesh*      softwareModel = nullptr;

	LineModelVertex*  verts_ptr     = nullptr;
	u16     *   inds_ptr      = nullptr;

	for(u64 i = 0, sz = m_PolyModel->m_verts.size(); i < sz; ++i )
		((Vertex*)m_PolyModel->m_verts[ i ])->m_vertexIndexForSoftware_lines.clear();

	u32 softwareModelIndex = 0;
	std::unordered_map<u64, u64> map;     // 2 объединённых адреса как ключ и индекс на сам массив хранящий Edge

	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		polygon = (Polygon3D *)m_PolyModel->m_polygons[i];

		for( u64 o = 0, sz2 = polygon->m_verts.size(); o < sz2; ++o )
		{
			u64 o2 = o + 1;
			if(o2 == sz2) o2=0;
			auto cv1 = (ControlVertex*)((Vertex*)polygon->m_verts[o])->m_controlVertex;
			auto cv2 = (ControlVertex*)((Vertex*)polygon->m_verts[o2])->m_controlVertex;
			
			// нужно определить контрольную вершину с адресом, значение которого меньше чем у другой вершины
			ControlVertex* cv_first  = cv1;
			ControlVertex* cv_second = cv2;
			if( cv2 < cv1 )
			{
				cv_first   = cv2;
				cv_second  = cv1;
			}

			u64 key_val = (u64)cv_first;
			key_val <<= 32;
			key_val |= ((u64)cv_second & 0x00000000FFFFFFFF);

			if( map.find(key_val) == map.end() )
			{
				map[key_val] = 0;
				if( lineCount == 0 )
				{
					softwareModel = _createNewSoftwareModel(_NEW_SOFTWARE_MODEL_TYPE::ENSMT_LINES);
					verts_ptr     = (LineModelVertex*)softwareModel->m_vertices;
					inds_ptr      = softwareModel->m_indices;
					m_SoftwareModels_edges.push_back(softwareModel);
					softwareModelIndex = (u32)m_SoftwareModels_edges.size() - 1;
					index = 0;
				}
				
				verts_ptr->_position.x  = ((Vertex*)cv_first->m_verts[0])->m_Position.KK_X;
				verts_ptr->_position.y  = ((Vertex*)cv_first->m_verts[0])->m_Position.KK_Y;
				verts_ptr->_position.z  = ((Vertex*)cv_first->m_verts[0])->m_Position.KK_Z;
				
				v4f color(1.f,1.f,1.f,1.f);
				if( kkSingleton<Application>::s_instance->getEditMode() == EditMode::Edge )
				{
					if( cv_first->m_edgeWith.size() )
					{
						if( std::find(cv_first->m_edgeWith.begin(), cv_first->m_edgeWith.end(), cv_second ) != cv_first->m_edgeWith.end() )
						{
							color = v4f(1.f,0.f,0.f,1.f);
						}
					}
				}

				verts_ptr->_color = color;
				*inds_ptr = index;
				
				((Vertex*)cv_first->m_verts[0])->m_vertexIndexForSoftware_lines.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

				++verts_ptr;
				++inds_ptr;
				++index;

				verts_ptr->_position.x  = ((Vertex*)cv_second->m_verts[0])->m_Position.KK_X;
				verts_ptr->_position.y  = ((Vertex*)cv_second->m_verts[0])->m_Position.KK_Y;
				verts_ptr->_position.z  = ((Vertex*)cv_second->m_verts[0])->m_Position.KK_Z;
				verts_ptr->_color = color;
				*inds_ptr = index;
				
				((Vertex*)cv_second->m_verts[0])->m_vertexIndexForSoftware_lines.push_back( std::pair<u32,u32>(index,softwareModelIndex) );

				++verts_ptr;
				++inds_ptr;
				++index;

				softwareModel->m_vCount += 2;
				softwareModel->m_iCount += 2;

				++lineCount;
				if( lineCount == m_lineLimit )
				{
					lineCount = 0;
				}
			}
		}
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

u64 Scene3DObject::GetPolygonCount()
{
	return m_PolyModel->getPolygonCount();
}

kkPolygon * Scene3DObject::GetPolygon(u64 i)
{
	return m_PolyModel->getPolygon(i);
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



void Scene3DObject_isRayIntersect( 
	int* out_result,
	int* stop_flag,
	u64 start_index,
	u64 end_index,
	Scene3DObject* object,
	kkRay* ray,
	kkRayTriangleIntersectionResultSimple* iResult,
	kkRayTriangleIntersectionAlgorithm alg
	)
{
	Polygon3D * polygon;
	Vertex*     vertex1;
	Vertex*     vertex2;
	Vertex*     vertex3;
	u32       num_of_verts;

	auto M = object->m_matrix;

	/*kkVector4 rayDir = ray->m_end - ray->m_origin;
	rayDir.normalize();*/

	for(u64 i = start_index; i < end_index; ++i )
	{
		polygon = (Polygon3D *)object->m_PolyModel->m_polygons.at(i);

		num_of_verts = (u32)polygon->m_verts.size();
		u32 index2, index3;
		for( u32 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
		{
			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == num_of_verts )
				index3 = 0;

			vertex2 = (Vertex*)polygon->m_verts[0];
			vertex1 = (Vertex*)polygon->m_verts[index2];
			vertex3 = (Vertex*)polygon->m_verts[index3];

			kkTriangle t;
			t.v1 = math::mul( vertex2->m_Position, M ) + object->m_pivot;
			t.v2 = math::mul( vertex1->m_Position, M ) + object->m_pivot;
			t.v3 = math::mul( vertex3->m_Position, M ) + object->m_pivot;
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
					iResult->m_polygonIndex = i;
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
					iResult->m_polygonIndex = i;
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
		}
	}
}

bool Scene3DObject::IsRayIntersect( const kkRay& ray, kkRayTriangleIntersectionResultSimple& result, kkRayTriangleIntersectionAlgorithm alg )
{
	kkRay r = ray;
	int thread_stop_flag = 0;
	int thread_result    = 0;

	if( m_PolyModel->m_polygons.size() > 3000 )
	{

		std::thread t1( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag,
			0, m_PolyModel->m_polygons.size() / 4, this, &r, &result, alg );

		std::thread t2( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag, 
			m_PolyModel->m_polygons.size() / 4, (m_PolyModel->m_polygons.size() / 4)*2,  this, &r, &result, alg  );

		std::thread t3( Scene3DObject_isRayIntersect,  &thread_result, &thread_stop_flag, 
			(m_PolyModel->m_polygons.size() / 4)*2, (m_PolyModel->m_polygons.size() / 4)*3,  this, &r, &result, alg  );

		Scene3DObject_isRayIntersect( &thread_result, &thread_stop_flag,
			(m_PolyModel->m_polygons.size() / 4)*3, m_PolyModel->m_polygons.size(),  this, &r, &result, alg  );
		t1.join();
		t2.join();
		t3.join();
	}
	else
	{
		Scene3DObject_isRayIntersect( &thread_result, &thread_stop_flag,
			0, m_PolyModel->m_polygons.size(), this, &r, &result, alg  );
	}

	return thread_result == 1;
}

bool Scene3DObject::IsRayIntersectMany( const kkRay& r, std::vector<kkRayTriangleIntersectionResultSimple>& result, kkRayTriangleIntersectionAlgorithm alg )
{
	auto viewport = kkSingleton<Application>::s_instance->getActiveViewport();
	auto camera   = viewport->getCamera();
	auto frustum  = camera->getFrustum();
	
	bool ret = false;

	Polygon3D * polygon;
	Vertex*     vertex1;
	Vertex*     vertex2;
	Vertex*     vertex3;
	u32       num_of_verts;

	kkRayTriangleIntersectionResultSimple iResult;
	iResult.m_object = this;
	auto M = m_matrix;

	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		polygon = (Polygon3D *)m_PolyModel->m_polygons.at(i);

		num_of_verts = (u32)polygon->m_verts.size();
		u32 index2, index3;
		for( u32 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
		{
			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == num_of_verts )
				index3 = 0;

			vertex2 = (Vertex*)polygon->m_verts[0];
			vertex1 = (Vertex*)polygon->m_verts[index2];
			vertex3 = (Vertex*)polygon->m_verts[index3];
			vertex1->m_Position.setW(1.f);
			vertex2->m_Position.setW(1.f);
			vertex3->m_Position.setW(1.f);

			kkTriangle t;
			t.v1 = math::mul( vertex2->m_Position, M ) + m_pivot;
			t.v2 = math::mul( vertex1->m_Position, M ) + m_pivot;
			t.v3 = math::mul( vertex3->m_Position, M ) + m_pivot;
			t.update();

			t.v1.setW(1.f);
			t.v2.setW(1.f);
			t.v3.setW(1.f);


			/*if( frustum->pointInFrustum( vertex2->m_Position )
				|| frustum->pointInFrustum( vertex1->m_Position )
				|| frustum->pointInFrustum( vertex3->m_Position ))*/
			if( kkrooo::pointOnFrontSideCamera(t.v1, camera->getViewProjectionMatrix())
				|| kkrooo::pointOnFrontSideCamera(t.v2, camera->getViewProjectionMatrix())
				|| kkrooo::pointOnFrontSideCamera(t.v3, camera->getViewProjectionMatrix()))
			{

				//auto r = ray;
				//r.update();

			//	std::cout << r.m_origin <<"\n";
			//	std::cout << r.m_end <<"\n\n";

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
						iResult.m_polygonIndex = i;
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
						iResult.m_polygonIndex = i;
						result.push_back(iResult);
						ret = true;
					}
					break;
				}
			}

		}
	}

	return ret;
}

void Scene3DObject::moveVerts(const kkVector4& v, std::basic_string<ControlVertex*>& verts)
{
	struct verts_points{  v3f _pos;  v4f _col;  };
	kkVector4 V = v;

	ControlVertex * cv = nullptr;
	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;

	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	auto M = m_matrix;
	M.invert();
	V.KK_W = 1.f;
	V = math::mul(V,M);

	for( size_t i = 0, sz = verts.size(); i < sz; ++i )
	{
		cv = verts[ i ];

		// сначала меняю координату полигональной модели
		for( u64 i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id   = cv->m_vertexIndex[i2];
			//auto vertex = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex = (Vertex*)cv->m_verts[i2];

			vertex->m_Position = vertex->m_Position_fix + V;

			// потом меняю сетку для software модели и потом кидаю её в hardware
			// нужно взять правельную m_SoftwareModels
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
			{
				verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
				verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
				verts_ptr->Position.x = vertex->m_Position.KK_X;
				verts_ptr->Position.y = vertex->m_Position.KK_Y;
				verts_ptr->Position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
			}
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
			{
				verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
				verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
				verts_lines_ptr->_position.x = vertex->m_Position.KK_X;
				verts_lines_ptr->_position.y = vertex->m_Position.KK_Y;
				verts_lines_ptr->_position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
			}
		}

		auto vertex  = (Vertex*)cv->m_verts[0];
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]->m_vertices;
		verts_points_ptr = verts_points_ptr + cv->m_vertexIndexForSoftware_points.first;
		verts_points_ptr->_pos.x = vertex->m_Position.KK_X;
		verts_points_ptr->_pos.y = vertex->m_Position.KK_Y;
		verts_points_ptr->_pos.z = vertex->m_Position.KK_Z;
		hardware_models_for_update_points.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_points[ cv->m_vertexIndexForSoftware_points.second ],
			m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]));
	}

	for( auto o : hardware_models_for_update )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_lines )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_points )
	{
		u8 * vptr = nullptr;
		auto HW = o.first;
		auto SW = o.second;
		HW->mapVerts(&vptr);
		memcpy(vptr, SW->m_vertices,  SW->m_stride * SW->m_vCount );
		HW->unmapVerts();
	}
}

void Scene3DObject::rotateVerts(const kkMatrix4& m, std::basic_string<ControlVertex*>& verts, const kkVector4& selectionCenter)
{
	auto M = m;
	M[ 3u ].KK_X = 0.f;
	M[ 3u ].KK_Y = 0.f;
	M[ 3u ].KK_Z = 0.f;

	auto W = m_matrixOnlyRotation;

	auto WT = W;
	WT.transpose();

	auto WI = W;
	WI.invert();


	// Есть матрица вращения m которой нужно повернуть вершины
	// Эту матрицу нужно повернуть текущей матрицей поворота.
	M = M * W;
	M = WT * M;

	kkVector4 C = selectionCenter;
	C.KK_W = 1.f;

	auto W2 = m_matrix;
	W2.invert();
	C = math::mul( C, W2 );

	struct verts_points{  v3f _pos;  v4f _col;  };

	ControlVertex * cv = nullptr;
	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;

	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	for( size_t i = 0, sz = verts.size(); i < sz; ++i )
	{
		cv = verts[ i ];

		// сначала меняю координату полигональной модели
		for( u64 i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id   = cv->m_vertexIndex[i2];
			//auto vertex = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex = (Vertex*)cv->m_verts[ i2 ];

			auto V = vertex->m_Position_fix - C;
			V.KK_W = 1.f;
			vertex->m_Position = math::mul(V,M) + C;

			// потом меняю сетку для software модели и потом кидаю её в hardware
			// нужно взять правельную m_SoftwareModels
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
			{
				verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
				verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
				verts_ptr->Position.x = vertex->m_Position.KK_X;
				verts_ptr->Position.y = vertex->m_Position.KK_Y;
				verts_ptr->Position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
			}
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
			{
				verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
				verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
				verts_lines_ptr->_position.x = vertex->m_Position.KK_X;
				verts_lines_ptr->_position.y = vertex->m_Position.KK_Y;
				verts_lines_ptr->_position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
			}
		}
		auto vertex  = (Vertex*)cv->m_verts[0];
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]->m_vertices;
		verts_points_ptr = verts_points_ptr + cv->m_vertexIndexForSoftware_points.first;
		verts_points_ptr->_pos.x = vertex->m_Position.KK_X;
		verts_points_ptr->_pos.y = vertex->m_Position.KK_Y;
		verts_points_ptr->_pos.z = vertex->m_Position.KK_Z;
		hardware_models_for_update_points.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_points[ cv->m_vertexIndexForSoftware_points.second ],
			m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]));
	}

	for( auto o : hardware_models_for_update )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_lines )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_points )
	{
		u8 * vptr = nullptr;
		auto HW = o.first;
		auto SW = o.second;
		HW->mapVerts(&vptr);
		memcpy(vptr, SW->m_vertices,  SW->m_stride * SW->m_vCount );
		HW->unmapVerts();
	}
}

void Scene3DObject::scaleVerts(const kkMatrix4& m, std::basic_string<ControlVertex*>& verts, const kkVector4& selectionCenter )
{
	auto M = m;
	M[ 3u ].KK_X = 0.f;
	M[ 3u ].KK_Y = 0.f;
	M[ 3u ].KK_Z = 0.f;

	auto W = m_matrixOnlyRotation;

	auto WT = W;
	WT.transpose();

	auto WI = W;
	WI.invert();


	// Есть матрица вращения m которой нужно повернуть вершины
	// Эту матрицу нужно повернуть текущей матрицей поворота.
	M = M * W;
	M = WT * M;

	kkVector4 C = selectionCenter;
	C.KK_W = 1.f;

	auto W2 = m_matrix;
	W2.invert();
	C = math::mul( C, W2 );

	struct verts_points{  v3f _pos;  v4f _col;  };
	ControlVertex * cv = nullptr;
	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;

	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	for( size_t i = 0, sz = verts.size(); i < sz; ++i )
	{
		cv = verts[ i ];

		// сначала меняю координату полигональной модели
		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id   = cv->m_vertexIndex[i2];
			//auto vertex = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex = (Vertex*)cv->m_verts[i2];
			
			vertex->m_Position = math::mul( vertex->m_Position_fix - C, M) + C;

			// потом меняю сетку для software модели и потом кидаю её в hardware
			// нужно взять правельную m_SoftwareModels
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
			{
				verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
				verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
				verts_ptr->Position.x = vertex->m_Position.KK_X;
				verts_ptr->Position.y = vertex->m_Position.KK_Y;
				verts_ptr->Position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
			}
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
			{
				verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
				verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
				verts_lines_ptr->_position.x = vertex->m_Position.KK_X;
				verts_lines_ptr->_position.y = vertex->m_Position.KK_Y;
				verts_lines_ptr->_position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
			}
		}
		auto vertex  = (Vertex*)cv->m_verts[0];
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]->m_vertices;
		verts_points_ptr = verts_points_ptr + cv->m_vertexIndexForSoftware_points.first;
		verts_points_ptr->_pos.x = vertex->m_Position.KK_X;
		verts_points_ptr->_pos.y = vertex->m_Position.KK_Y;
		verts_points_ptr->_pos.z = vertex->m_Position.KK_Z;
		hardware_models_for_update_points.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_points[ cv->m_vertexIndexForSoftware_points.second ],
			m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]));
	}

	for( auto o : hardware_models_for_update )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_lines )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_points )
	{
		u8 * vptr = nullptr;
		auto HW = o.first;
		auto SW = o.second;
		HW->mapVerts(&vptr);
		memcpy(vptr, SW->m_vertices,  SW->m_stride * SW->m_vCount );
		HW->unmapVerts();
	}
}

void Scene3DObject::ApplyPosition()
{
	ControlVertex * cv;
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];

		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id   = cv->m_vertexIndex[i2];
			//auto vertex = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex = (Vertex*)cv->m_verts[i2];

			vertex->m_Position_fix = vertex->m_Position;
		}
	}
	updateAABB_vertex();
}

void Scene3DObject::RestorePosition()
{
	// чтобы восстановить предыдущую позицию, нужно так-же изменить сами hardware буферы
	struct verts_points{  v3f _pos;  v4f _col;  };

	ControlVertex * cv;
	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
		
		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id    = cv->m_vertexIndex[i2];
			//auto vertex  = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex  = (Vertex*)cv->m_verts[i2];

			vertex->m_Position = vertex->m_Position_fix;

			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
			{
				verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
				verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
				verts_ptr->Position.x = vertex->m_Position.KK_X;
				verts_ptr->Position.y = vertex->m_Position.KK_Y;
				verts_ptr->Position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
			}
			for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
			{
				verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
				verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
				verts_lines_ptr->_position.x = vertex->m_Position.KK_X;
				verts_lines_ptr->_position.y = vertex->m_Position.KK_Y;
				verts_lines_ptr->_position.z = vertex->m_Position.KK_Z;
				hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
			}
		}

		auto vertex  = (Vertex*)cv->m_verts[0];
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]->m_vertices;
		verts_points_ptr = verts_points_ptr + cv->m_vertexIndexForSoftware_points.first;
		verts_points_ptr->_pos.x = vertex->m_Position.KK_X;
		verts_points_ptr->_pos.y = vertex->m_Position.KK_Y;
		verts_points_ptr->_pos.z = vertex->m_Position.KK_Z;
		hardware_models_for_update_points.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_points[ cv->m_vertexIndexForSoftware_points.second ],
			m_SoftwareModels_points[ cv->m_vertexIndexForSoftware_points.second ]));
	}

	for( auto o : hardware_models_for_update )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_lines )
	{
		u8 * vptr = nullptr;
		o.first->mapVerts(&vptr);
		memcpy(vptr, o.second->m_vertices,  o.second->m_stride * o.second->m_vCount );
		o.first->unmapVerts();
	}
	for( auto o : hardware_models_for_update_points )
	{
		u8 * vptr = nullptr;
		auto HW = o.first;
		auto SW = o.second;
		HW->mapVerts(&vptr);
		memcpy(vptr, SW->m_vertices,  SW->m_stride * SW->m_vCount );
		HW->unmapVerts();
	}
}

void        Scene3DObject::updateAABB_vertex()
{
	m_aabbOriginal.reset();

	ControlVertex * cv;
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];

		//auto V_id   = cv->m_vertexIndex[0];
		//auto vertex = (Vertex*)m_PolyModel->m_verts[ V_id ];
		auto vertex = (Vertex*)cv->m_verts[0];

		m_aabbOriginal.add( vertex->m_Position_fix );
	}
}

void Scene3DObject::applyMatrices()
{
	auto TIM = m_matrix;
	TIM.invert();
	TIM.transpose();

	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex * cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];

		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			//auto V_id    = cv->m_vertexIndex[i2];
			//auto vertex  = (Vertex*)m_PolyModel->m_verts[ V_id ];
			auto vertex  = (Vertex*)cv->m_verts[i2];
		
			vertex->m_Position = math::mul(vertex->m_Position_fix,m_matrix);
			vertex->m_Position_fix = vertex->m_Position;

			vertex->m_Normal = math::mul(vertex->m_Normal_fix, TIM);
			vertex->m_Normal_fix = vertex->m_Normal;
		}
	}

	resetMatrices();
	//updateMatrixPosition();
	
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

	//updateMatrixPosition();
	UpdateAabb();
}

void Scene3DObject::generateNormals()
{
	m_PolyModel->generateNormals(false);
	_rebuildModel();
}

void Scene3DObject::deleteSelectedPolys()
{
	bool need_delete = false;
	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		auto polygon = (Polygon3D *)m_PolyModel->m_polygons.at(i);
		if(polygon->m_isSelected)
		{
			polygon->MarkToDelete();
			need_delete = true;
		}
	}

	if( need_delete )
	{
		m_PolyModel->deleteMarkedPolygons();
		m_PolyModel->createControlPoints();
		_rebuildModel();
		updatePolygonModel();
		UpdateAabb();
	}
}

void Scene3DObject::deleteSelectedEdges()
{
	bool need_delete = false;
	for(u64 i = 0, sz = m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = m_PolyModel->m_edges[i];
		
		for( auto ECV : E->m_firstPoint->m_edgeWith )
		{
			if( ECV == E->m_secondPoint )
			{
				m_PolyModel->m_polygons[ E->m_polygonIndex[0] ]->MarkToDelete();
				if(E->m_polygonIndex[1] != 0xffffffffffffffff)
					m_PolyModel->m_polygons[ E->m_polygonIndex[1] ]->MarkToDelete();
				need_delete = true;
				break;
			}
		}
		
	}
	
	if( need_delete )
	{
		m_PolyModel->deleteMarkedPolygons();
		m_PolyModel->createControlPoints();
		_rebuildModel();
		updateEdgeModel();
		UpdateAabb();
	}
}

void Scene3DObject::deleteSelectedVerts()
{
	bool need_delete = false;
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		auto cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
		if( cv->m_isSelected )
		{
			for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
			{
				auto vertex = (Vertex*)cv->m_verts[i2];

				vertex->m_parentPolygon->MarkToDelete();
				need_delete = true;
			}
		}
	}

	if( need_delete )
	{
		m_PolyModel->deleteMarkedPolygons();
		m_PolyModel->createControlPoints();
		_rebuildModel();
		UpdateAabb();
	}
}

kkArray<kkVertex*>&   Scene3DObject::GetVertexArray()
{
	return m_PolyModel->m_verts;
}

kkArray<kkControlVertex*>&  Scene3DObject::GetControlVertexArray()
{
	return m_PolyModel->m_controlPoints;
}

kkArray<kkPolygon*>&  Scene3DObject::GetPolygonArray()
{
	return m_PolyModel->m_polygons;
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
	m_PolyModel->prepareForRaytracing(m_matrix, m_pivot, ri );
}

void Scene3DObject::FinishRaytracing()
{
	m_PolyModel->finishRaytracing();
}

void Scene3DObject::RayTest( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial )
{
	m_PolyModel->rayTest(outTriangles, ray, renderObjectMaterial);
}

void Scene3DObject::RayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation* renderObjectMaterial )
{
	m_PolyModel->rayTestGrid(outTriangles, point, ray, renderObjectMaterial);
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
	auto V = m_pivotFixed - position;
	auto M = m_matrix;
	M.invert();
	V.KK_W = 1.f;
	V = math::mul(V,M);
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex * cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];

		for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
		{
			auto vertex  = (Vertex*)cv->m_verts[i2];
			vertex->m_Position = vertex->m_Position_fix + V;
			vertex->m_Position_fix = vertex->m_Position;
		}
	}
	_rebuildModel();

	ApplyPivot();
	UpdateAabb();
}

void Scene3DObject::SelecPolygonsByAdd()
{
	m_isObjectHaveSelectedPolys = false;
	for(auto P : m_PolyModel->m_polygons)
	{
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
		updatePolygonModel();
}

void Scene3DObject::SelecVertsByAdd()
{
	m_isObjectHaveSelectedVerts = false;
	std::vector<ControlVertex*> vertsToSelect;
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
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
		updateModelPointsColors();
}

void Scene3DObject::SelecVertsBySub()
{
	std::vector<ControlVertex*> vertsToDeselect;
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
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
	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
		if(cv->m_isSelected)
		{
			m_isObjectHaveSelectedVerts = true;
			break;
		}
	}
	updateModelPointsColors();
}

void Scene3DObject::AttachObject(kkScene3DObject* object)
{
	if( object->GetType() == kkScene3DObjectType::PolygonObject )
	{
		auto MI = m_matrix;
		MI.invert();
		
		auto polyObject = (Scene3DObject*)object;
		m_PolyModel->addModel(polyObject->m_PolyModel, MI, polyObject->GetMatrix(), m_pivot, polyObject->GetPivot());

		m_PolyModel->createControlPoints();
		this->_rebuildModel();
	}
}

void Scene3DObject::BreakVerts()
{
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}
	}

	for( size_t i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_PolyModel->m_controlPoints[ i ];
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
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
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
	updateModelPointsColors();
}


void Scene3DObject::Weld(kkControlVertex* CV1, kkControlVertex* CV2)
{
	ControlVertex* cv1 = (ControlVertex*)CV1;
	ControlVertex* cv2 = (ControlVertex*)CV2;

	bool is_edge = false;
	Edge * edge = nullptr;
	for( auto e : cv1->m_edges )
	{
		edge = e;
		if(e->m_firstPoint == cv2 || e->m_secondPoint == cv2)
		{
			is_edge = true;
			break;
		}
	}


	// есть 2 случая, когда надо делать weld
	// 1. когда точки образуют ребро
	// 2. когда хоть 1 ребро у каждой точки имеет только 1 полигон на стороне (то есть точка как бы снаружи)

	// сначала перемещу нужную вершину к другой вершине
	// потом, нахожу 1 или 2 полигона
	// из полигонов удаляю вершину, если вершины три то удаляю сам полигон
	// удаляю вершины из главного массива с вершинами m_verts, 
	Vertex* targetVertex = (Vertex*)cv2->m_verts[0];
	Vertex* pickVertex = (Vertex*)cv1->m_verts[0];

	if(is_edge || (cv1->m_onEdge && cv2->m_onEdge))
	{
		auto half = (targetVertex->m_Position - pickVertex->m_Position) * 0.5f;
		for( auto V : cv1->m_verts )
		{
			//Vertex* vertex = (Vertex*)m_PolyModel->m_verts[ vertexIndex ];
			Vertex* vertex = (Vertex*)V;
			vertex->m_Boneinds = targetVertex->m_Boneinds;
			vertex->m_Color = targetVertex->m_Color;
			vertex->m_Normal = targetVertex->m_Normal;
			vertex->m_Normal_fix = targetVertex->m_Normal_fix;
				vertex->m_Position = targetVertex->m_Position;
				vertex->m_Position_fix = targetVertex->m_Position_fix;
			vertex->m_UV = targetVertex->m_UV;
			vertex->m_Weights = targetVertex->m_Weights;
			vertex->m_weld = true;
			targetVertex->m_weld = true;
		}

	}
	else
	{
		return;
	}
	
	std::unordered_set<Polygon3D*> polysForNormRecalculate;
	for( auto V : cv1->m_verts )
	{
		Vertex* vertex = (Vertex*)V;
		polysForNormRecalculate.insert(vertex->m_parentPolygon);
	}

	if(is_edge)
	{

		Polygon3D* P1 = (Polygon3D*)m_PolyModel->m_polygons[edge->m_polygonIndex[0]];
		Polygon3D* P2 = nullptr;
		if(edge->m_polygonIndex[1] != (u64)-1 )
		{
			P2 = (Polygon3D*)m_PolyModel->m_polygons[edge->m_polygonIndex[1]];
		}

		// удалить вершину из полигона и из основного массива
		if(P1->m_verts.size() > 3)
		{
			// теперь нужно удалить вершину из полигона и из массива с вершинами
			// нужно найти вершину внутри контрольной вершины, которая принадлежит данному полигону
			kkVertex * vertex_for_delete = nullptr;
			for( u64 i = 0, sz = P1->m_verts.size(); i < sz; ++i )
			{
				//for( auto vertexIndex_CV : cv1->m_vertexIndex )
				for( u64 j = 0, jsz = cv1->m_verts.size(); j < jsz; ++j )
				{
					if( cv1->m_verts[ j ] == P1->m_verts[ i ] )
					{
						vertex_for_delete = P1->m_verts[ i ];
						goto end;
					}
				}
			}
			end:;
			if(vertex_for_delete)
			{
				P1->m_verts.erase_first(vertex_for_delete);
				m_PolyModel->m_verts.erase_first(vertex_for_delete);
				kkDestroy(vertex_for_delete);
			}
		}
		else
		{
			for( u64 i = 0, sz = P1->m_verts.size(); i < sz; ++i )
			{
				auto V = P1->m_verts[i];
				m_PolyModel->m_verts.erase_first(V);
				kkDestroy(V);
			}

			m_PolyModel->m_polygons.erase_first(P1);
			kkDestroy(P1);
			polysForNormRecalculate.erase(P1);
		}

		if(P2)
		{
			if(P2->m_verts.size() > 3)
			{
				kkVertex * vertex_for_delete = nullptr;
				for( u64 i = 0, sz = P2->m_verts.size(); i < sz; ++i )
				{
					for( u64 j = 0, jsz = cv1->m_verts.size(); j < jsz; ++j )
					{
						if( cv1->m_verts[ j ] == P2->m_verts[ i ] )
						{
							vertex_for_delete = P2->m_verts[ i ];
							goto end2;
						}
					}
				}
				end2:;
				if(vertex_for_delete)
				{
					P2->m_verts.erase_first(vertex_for_delete);
					m_PolyModel->m_verts.erase_first(vertex_for_delete);
					kkDestroy(vertex_for_delete);
				}
			}
			else
			{
				for( u64 i = 0, sz = P2->m_verts.size(); i < sz; ++i )
				{
					auto V = P2->m_verts[i];
					m_PolyModel->m_verts.erase_first(V);
					kkDestroy(V);
				}
				m_PolyModel->m_polygons.erase_first(P2);
				kkDestroy(P2);
				polysForNormRecalculate.erase(P2);
			}
		}
	}

	for(auto P : polysForNormRecalculate)
	{
		P->CalculateNormals();
	}
	
	for( u64 i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i];

		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}

	}

	m_PolyModel->createControlPoints();

	for( u64 i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i];
		if(((Vertex*)CV->m_verts[0])->m_isCVSelected)
		{
			CV->select();
		}
	}

	this->_rebuildModel();
	updateModelPointsColors();
}

void Scene3DObject::WeldSelectedVerts(f32 len)
{
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}
	}

	//беру каждую вершину, и проверяю со специальными вершинами которых нужно придумать
	struct _weld_vertex
	{
		kkVector4 m_center; // единая позиция
		std::vector<Vertex*> m_verts; // вершины которые добавляются в зависимости от расстояния
	};
	kkArray<_weld_vertex*> weld_verts = kkArray<_weld_vertex*>(0xffff);

	for( u64 i = 0, sz = m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i];
		if(!CV->m_isSelected)
			continue;

		Vertex* V = (Vertex*)CV->m_verts[0];

		bool vertex_found = false;
		for( u64 k = 0, ksz = weld_verts.size(); k < ksz; ++k )
		{
			auto WV = weld_verts[k];
			if( V->m_Position.distance(WV->m_center) <= len )
			{
				vertex_found = true;
				k = ksz;

				kkVector4 vp = WV->m_center - V->m_Position;
				vp *= 0.5f;

				for(auto v : CV->m_verts)
				{
					WV->m_verts.emplace_back((Vertex*)v);
				}
				WV->m_center -= vp; 
				break;
			}
		}
		if(!vertex_found)
		{
			_weld_vertex * WV = new _weld_vertex;
			WV->m_center = V->m_Position;
			for(auto v : CV->m_verts)
			{
				WV->m_verts.emplace_back((Vertex*)v);
			}
			weld_verts.push_back(WV);
		}
	}

	// изменяю координаты вершин
	for( u64 i = 0, sz = weld_verts.size(); i < sz; ++i )
	{
		auto WV = weld_verts[i];
		for( u64 j = 0, jsz = WV->m_verts.size(); j < jsz; ++j )
		{
			auto V = WV->m_verts[j];
			V->m_Position = WV->m_center;
			V->m_Position_fix = WV->m_center;
			V->m_weld = true;
		}
		delete weld_verts[i];
	}

	// проверяю полигоны. если координаты точки равны то лишнюю нужно удалить
	// если остаётся 2 и менее вершин то надо удалить сам полигон
	auto isPolygonDelete = [&](Polygon3D* P)->bool
	{
		begin:
		for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
		{
			Vertex* V1 = (Vertex*)P->m_verts[i];
			for( u64 i2 = i+1, sz2 = P->m_verts.size(); i2 < sz2; ++i2 )
			{
				Vertex* V2 = (Vertex*)P->m_verts[i2];
				if(V1->m_Position == V2->m_Position)
				{
					V2->m_isToDelete = true;
					P->m_verts.erase_first(V2);
					goto begin;
				}
			}
		}

		return P->m_verts.size() < 3;
	};
	
	for( u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];
		if( isPolygonDelete(P) )
		{
			P->MarkToDelete();
		}
	}

	// удаление.
	m_PolyModel->deleteMarkedPolygons();

	m_PolyModel->createControlPoints();
	this->_rebuildModel();
	updateModelPointsColors();
}

void Scene3DObject::ConnectVerts()
{
	// сначала надо пометить реальные вершины, какие выбраны какие нет
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}
	}

	std::basic_string<Vertex*> vertex_buffer;
	kkArray<Polygon3D*> new_polygons = kkArray<Polygon3D*>(0xff);
	kkArray<Vertex*> new_verts = kkArray<Vertex*>(0xff);

	// прохожусь по полигонам
	for( u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		vertex_buffer.clear();

		Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];

		// прохожусь по полигону в поисках выбранной вершины.
		// если вершина находится, но kcnt должен сброситься, чтобы пройтись по массиву в полном объёме
		// соответственно, нужно сделать проверку на выход за пределы
		bool start = false;
		bool needToConnect = false;
		for( u64 ki = 0, kcnt = 0, kisz = P->m_verts.size(); kcnt < kisz; ++kcnt)
		{
			Vertex* vertex = (Vertex*)P->m_verts[ki];
			if(vertex->m_isCVSelected)
			{
				if(!start)
				{
					// нашли первую выделенную вершину
					start = true;
					kcnt = 0;
				}
				else
				{
					// есть вторая или более выбранных вершин
					needToConnect = true;
				}
			}
			++ki;
			if(start)
			{
				vertex_buffer.push_back(vertex);
			}
			if(ki == kisz)
				ki = 0;
		}
		if(needToConnect)
		{
			P->MarkToDelete();

			Vertex* baseVertex = vertex_buffer[0];
			Polygon3D* newPolygon = nullptr;
			Vertex* newVertex = nullptr;

			for( size_t j = 1, jsz = vertex_buffer.size(), vertex_counter = 0; j < jsz; ++j )
			{
				if(!newPolygon)
				{
					vertex_counter = 1;
					newPolygon = kkCreate<Polygon3D>();
					newVertex = kkCreate<Vertex>();
					newVertex->set(baseVertex);
					newVertex->m_parentPolygon = newPolygon;
					newVertex->m_isCVSelected = baseVertex->m_isCVSelected;
					newPolygon->addVertex(newVertex);
					new_verts.push_back(newVertex);
				}

				auto nextVertex = vertex_buffer[j];

				newVertex = kkCreate<Vertex>();
				newVertex->set(nextVertex);
				newVertex->m_parentPolygon = newPolygon;
				newVertex->m_isCVSelected = nextVertex->m_isCVSelected;
				newPolygon->addVertex(newVertex);
				new_verts.push_back(newVertex);
				++vertex_counter;

				bool last = j == jsz - 1;
				if((nextVertex->m_isCVSelected && vertex_counter > 2) || last )
				{
					if(j != 1)
					{
						new_polygons.push_back(newPolygon);
						newPolygon = nullptr;
						if(!last)
							--j;
					}
				}
			}
		}
	}

	auto newPolygonsSize = new_polygons.size();
	u64  currentNewPolygon = 0;
	// помечу вершины полигонов чтобы потом их удалить
	for(u64 i = 0, sz = m_PolyModel->m_polygons.size(); i < sz; ++i)
	{
		Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i];
		if(P->IsToDelete())
		{
			for(u64 j = 0, jsz = P->m_verts.size(); j < jsz; ++j)
			{
				Vertex* V = (Vertex*)P->m_verts[j];
				V->m_isToDelete = true;
			}
			kkDestroy(P);
			m_PolyModel->m_polygons[i] = new_polygons[currentNewPolygon];
			++currentNewPolygon;
		}
	}
	
	while(currentNewPolygon < newPolygonsSize)
	{
		m_PolyModel->m_polygons.push_back(new_polygons[currentNewPolygon]);
		++currentNewPolygon;
	}

	auto newVertssSize = new_verts.size();
	u64  currentNewVertex = 0;
	for(u64 i = 0, sz = m_PolyModel->m_verts.size(); i < sz; ++i)
	{
		Vertex* V = (Vertex*)m_PolyModel->m_verts[i];
		if(V->m_isToDelete)
		{
			kkDestroy(V);

			m_PolyModel->m_verts[i] = new_verts[currentNewVertex];
			++currentNewVertex;
		}
	}
	while(currentNewVertex < newVertssSize)
	{
		m_PolyModel->m_verts.push_back(new_verts[currentNewVertex]);
		++currentNewVertex;
	}

	m_PolyModel->createControlPoints();
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];

		for(auto v : CV->m_verts)
		{
			if(((Vertex*)v)->m_isCVSelected)
			{
				CV->select();
			}
		}

	}
	this->_rebuildModel();
	updateModelPointsColors();
}

void Scene3DObject::ChamferVerts(f32 len, bool addPolygon)
{
	if(len == 0.f)
		return;

	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
		bool sel = CV->isSelected();
		for( auto V : CV->m_verts )
		{
			Vertex* vertex = (Vertex*)V;
			vertex->m_isCVSelected = sel;
		}
	}

	struct _new_face
	{
		ControlVertex* m_base_cv = nullptr;
		kkArray<Vertex*> m_verts = kkArray<Vertex*>(4); // for copy
	};
	kkArray<_new_face*> new_faces = kkArray<_new_face*>(0xff);


	kkArray<Polygon3D*> new_polygons = kkArray<Polygon3D*>(0xff);
	for( u64 i2 = 0, sz2 = m_PolyModel->m_polygons.size(); i2 < sz2; ++i2 )
	{
		bool withSelectedVerts = false;
		Polygon3D* P = (Polygon3D*)m_PolyModel->m_polygons[i2];
		for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
		{
			Vertex* vertex = (Vertex*)P->m_verts[i];
			if(vertex->m_isCVSelected)
			{
				withSelectedVerts = true;
				break;
			}
		}

		if(!withSelectedVerts)
			continue;
	
		P->MarkToDelete();

		Polygon3D* new_P = kkCreate<Polygon3D>();
		new_polygons.push_back(new_P);

		for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
		{
			Vertex* vertex = (Vertex*)P->m_verts[i];
			if(vertex->m_isCVSelected)
			{
				Vertex* next_vertex = nullptr;
				if( i + 1 == sz )
				{
					next_vertex = (Vertex*)P->m_verts[0];
				}
				else
				{
					next_vertex = (Vertex*)P->m_verts[i+1];
				}

				Vertex* prev_vertex = nullptr;
				if(i == 0)
				{
					prev_vertex = (Vertex*)P->m_verts[sz-1];
				}
				else
				{
					prev_vertex = (Vertex*)P->m_verts[i-1];
				}

				Vertex* new_vertex1 = kkCreate<Vertex>();
				Vertex* new_vertex2 = kkCreate<Vertex>();
				new_vertex1->set(vertex);
				new_vertex2->set(vertex);
				new_vertex1->m_parentPolygon = new_P;
				new_vertex2->m_parentPolygon = new_P;
				new_P->addVertex(new_vertex1);
				new_P->addVertex(new_vertex2);
		 
				// T.intersectionPoint = ray.m_origin + ray.m_direction * len;
				// дальше нужно попробовать поставить вершины на позиции
				auto dir = prev_vertex->m_Position - vertex->m_Position;
				dir.normalize2();

				new_vertex1->m_Position = vertex->m_Position + dir * len;
				new_vertex1->m_Position_fix = new_vertex1->m_Position;

				dir = next_vertex->m_Position - vertex->m_Position;
				dir.normalize2();
				new_vertex2->m_Position = vertex->m_Position + dir * len;
				new_vertex2->m_Position_fix = new_vertex2->m_Position;
				
				if(addPolygon)
				{
					_new_face * nf_ptr = nullptr;
					for( u64 o = 0, osz = new_faces.size(); o < osz; ++o )
					{
						auto nf = new_faces[o];
						if(nf->m_base_cv == vertex->m_controlVertex)
						{
							nf_ptr = nf;
							break;
						}
					}
					if(!nf_ptr)
					{
						_new_face * nf = new _new_face;
						nf->m_base_cv = vertex->m_controlVertex;
						nf_ptr = nf;
						new_faces.push_back(nf);
					}
					nf_ptr->m_verts.push_back(new_vertex1);
				}
			}
			else
			{
				Vertex* new_vertex = kkCreate<Vertex>();
				new_vertex->set(vertex);
				new_vertex->m_parentPolygon = new_P;
				new_P->addVertex(new_vertex);
			}
		}

	}
	m_PolyModel->deleteMarkedPolygons();

	if(addPolygon)
	{
		for( u64 i = 0, sz = new_faces.size(); i < sz; ++i )
		{
			auto nf = new_faces[i];

			Polygon3D* new_P = kkCreate<Polygon3D>();
			new_polygons.push_back(new_P);

			for( u64 k = 0, ksz = nf->m_verts.size(); k < ksz; ++k )
			{
				Vertex* new_vertex = kkCreate<Vertex>();
				new_vertex->set(nf->m_verts[k]);
				new_vertex->m_parentPolygon = new_P;
				new_P->addVertex(new_vertex);
			}
			
			new_P->CalculateNormals();
			/*if(  )
			{
				new_P->Flip();
				new_P->CalculateNormals();
			}*/
			
			delete nf;
		}
	}

	for(auto P : new_polygons)
	{
		m_PolyModel->m_polygons.push_back(P);
		for(auto V : P->m_verts)
		{
			m_PolyModel->m_verts.push_back(V);
		}
	}
	m_PolyModel->createControlPoints();
	for( u64 i2 = 0, sz2 = m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	{
		ControlVertex* CV = (ControlVertex*)m_PolyModel->m_controlPoints[i2];
		for(auto v : CV->m_verts)
		{
			if(((Vertex*)v)->m_isCVSelected)
			{
				CV->select();
			}
		}
	}
	this->_rebuildModel();
	updateModelPointsColors();
}
