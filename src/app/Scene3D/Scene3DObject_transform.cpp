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

struct node_hash{
	std::size_t operator()(const std::pair<kkMesh*,kkSMesh*>& _node) const {
		kkStringA str;
		str += (u64)_node.first;
		str += (u64)_node.second;
		return std::hash<std::string>()(str.data());
	}
};

void Scene3DObject::moveVerts(const kkVector4& v, std::unordered_set<kkVertex*>& verts)
{
	struct verts_points{  v3f _pos;  v4f _col;  };
	kkVector4 V = v;

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

	for(auto vertex : verts)
	{
		// сначала меняю координату полигональной модели
		vertex->m_position = vertex->m_positionFix + V;

		// потом меняю сетку для software модели и потом кидаю её в hardware
		// нужно взять правельную m_SoftwareModels
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
		{
			verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
			verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
			verts_ptr->Position = vertex->m_position;
			hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
		}
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
		{
			verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
			verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
			verts_lines_ptr->_position = vertex->m_position;
			hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
		}

		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ vertex->m_pointsModelIndex ]->m_vertices;
		verts_points_ptr = verts_points_ptr + vertex->m_pointsVertexIndex;
		verts_points_ptr->_pos = vertex->m_position;
		hardware_models_for_update_points.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_points[ vertex->m_pointsModelIndex ],
			m_SoftwareModels_points[ vertex->m_pointsModelIndex ]));
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

void Scene3DObject::rotateVerts(const kkMatrix4& m, std::unordered_set<kkVertex*>& verts, const kkVector4& selectionCenter)
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

	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;

	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	for(auto vertex : verts)
	{
		// сначала меняю координату полигональной модели
		auto V = vertex->m_positionFix - C;
		vertex->m_position = math::mul(V,M) + C;

		// потом меняю сетку для software модели и потом кидаю её в hardware
		// нужно взять правельную m_SoftwareModels
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
		{
			verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
			verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
			verts_ptr->Position = vertex->m_position;
			hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
		}
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
		{
			verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
			verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
			verts_lines_ptr->_position = vertex->m_position;
			hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
		}
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ vertex->m_pointsModelIndex ]->m_vertices;
		verts_points_ptr = verts_points_ptr + vertex->m_pointsVertexIndex;
		verts_points_ptr->_pos = vertex->m_position;
		hardware_models_for_update_points.insert(
			std::pair<kkMesh*,kkSMesh*>(
				m_HardwareModels_points[ vertex->m_pointsModelIndex ],
				m_SoftwareModels_points[ vertex->m_pointsModelIndex ]
			)
		);
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

void Scene3DObject::scaleVerts(const kkMatrix4& m, std::unordered_set<kkVertex*>& verts, const kkVector4& selectionCenter )
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
	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;

	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	for(auto vertex : verts)
	{
		// сначала меняю координату полигональной модели
		vertex->m_position = math::mul( vertex->m_positionFix - C, M) + C;

		// потом меняю сетку для software модели и потом кидаю её в hardware
		// нужно взять правельную m_SoftwareModels
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
		{
			verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
			verts_ptr = verts_ptr + vertex->m_vertexIndexForSoftware[k].first;
			verts_ptr->Position = vertex->m_position;
			hardware_models_for_update.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ],m_SoftwareModels_polys[ vertex->m_vertexIndexForSoftware[k].second ]));
		}
		for( u64 k = 0, ks = vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
		{
			verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
			verts_lines_ptr = verts_lines_ptr + vertex->m_vertexIndexForSoftware_lines[k].first;
			verts_lines_ptr->_position = vertex->m_position;
			hardware_models_for_update_lines.insert(std::pair<kkMesh*,kkSMesh*>(m_HardwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ],m_SoftwareModels_edges[ vertex->m_vertexIndexForSoftware_lines[k].second ]));
		}
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ vertex->m_pointsModelIndex ]->m_vertices;
		verts_points_ptr = verts_points_ptr + vertex->m_pointsVertexIndex;
		verts_points_ptr->_pos = vertex->m_position;
		hardware_models_for_update_points.insert(
			std::pair<kkMesh*,kkSMesh*>(
				m_HardwareModels_points[ vertex->m_pointsModelIndex ],
				m_SoftwareModels_points[ vertex->m_pointsModelIndex ]
				)
		);
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
	auto current_vertex = m_polyModel->m_verts;
	for( u64 i = 0; i < m_polyModel->m_vertsCount; ++i )
	{
		current_vertex->m_positionFix = current_vertex->m_position;
		current_vertex = current_vertex->m_mainNext;
	}
	updateAABB_vertex();
}

void Scene3DObject::RestorePosition()
{
	// чтобы восстановить предыдущую позицию, нужно так-же изменить сами hardware буферы
	struct verts_points{  v3f _pos;  v4f _col;  };

	DefaultVert*    verts_ptr     = nullptr;
	LineModelVertex*    verts_lines_ptr     = nullptr;
	verts_points       *  verts_points_ptr     = nullptr;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_lines;
	std::unordered_set<std::pair<kkMesh*,kkSMesh*>, node_hash> hardware_models_for_update_points;
	hardware_models_for_update.reserve(4000);
	hardware_models_for_update_lines.reserve(4000);
	hardware_models_for_update_points.reserve(4000);

	auto current_vertex = m_polyModel->m_verts;
	for( u64 i = 0; i < m_polyModel->m_vertsCount; ++i )
	{
		current_vertex->m_position = current_vertex->m_positionFix;
		for( u64 k = 0, ks = current_vertex->m_vertexIndexForSoftware.size(); k < ks; ++k )
		{
			verts_ptr       = (DefaultVert*)m_SoftwareModels_polys[ current_vertex->m_vertexIndexForSoftware[k].second ]->m_vertices;
			verts_ptr = verts_ptr + current_vertex->m_vertexIndexForSoftware[k].first;
			verts_ptr->Position = current_vertex->m_position;
			hardware_models_for_update.insert(
				std::pair<kkMesh*,kkSMesh*>(
					m_HardwareModels_polys[ current_vertex->m_vertexIndexForSoftware[k].second ],
					m_SoftwareModels_polys[ current_vertex->m_vertexIndexForSoftware[k].second ]
					)
			);
		}
		for( u64 k = 0, ks = current_vertex->m_vertexIndexForSoftware_lines.size(); k < ks; ++k )
		{
			verts_lines_ptr       = (LineModelVertex*)m_SoftwareModels_edges[ current_vertex->m_vertexIndexForSoftware_lines[k].second ]->m_vertices;
			verts_lines_ptr = verts_lines_ptr + current_vertex->m_vertexIndexForSoftware_lines[k].first;
			verts_lines_ptr->_position = current_vertex->m_position;
			hardware_models_for_update_lines.insert(
				std::pair<kkMesh*,kkSMesh*>(
					m_HardwareModels_edges[ current_vertex->m_vertexIndexForSoftware_lines[k].second ],
					m_SoftwareModels_edges[ current_vertex->m_vertexIndexForSoftware_lines[k].second ]
					)
			);
		}
	
		verts_points_ptr = (verts_points*)m_SoftwareModels_points[ current_vertex->m_pointsModelIndex ]->m_vertices;
		verts_points_ptr = verts_points_ptr + current_vertex->m_pointsVertexIndex;
		verts_points_ptr->_pos = current_vertex->m_position;
		hardware_models_for_update_points.insert(
			std::pair<kkMesh*,kkSMesh*>(
				m_HardwareModels_points[ current_vertex->m_pointsModelIndex ],
				m_SoftwareModels_points[ current_vertex->m_pointsModelIndex ]
			)
		);
		current_vertex = current_vertex->m_mainNext;
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

