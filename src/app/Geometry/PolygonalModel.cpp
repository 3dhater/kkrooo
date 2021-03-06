#include "kkrooo.engine.h"
#include "../Common.h"

#include "Geometry/kkGeometryCreator.h"
#include "Geometry/kkPolygonalModel.h"

#include "Renderer/kkRenderer.h"
#include "GraphicsSystem/kkTexture.h"
#include "../Scene3D/Scene3DObject.h"

#include "../Functions.h"
#include "../Application.h"
#include "PolygonalModel.h"
#include "../SelectionFrust.h"

void kkPolygon_calculateNormals(kkPolygon* P)
{
	if(!P->m_normals.size())
		return;

	kkVector4 e1, e2, no;
	for( u16 i = 0; i < P->m_vertexCount; ++i )
	{
		P->m_normals[i].set(0.f,0.f,0.f);
	}
	kkVertex * base_vertex = P->m_verts->m_element;
	auto v_node1 = P->m_verts->m_right;
	auto v_node2 = v_node1->m_right;
	u16 index2, index3;
	for( u16 i2 = 0, sz2 = P->m_vertexCount - 2; i2 < sz2; ++i2 )
	{
		kkVertex * vertex3 = v_node1->m_element;
		kkVertex * vertex2 = v_node2->m_element;
		
		e1 = vertex2->m_position - base_vertex->m_position;
		e2 = vertex3->m_position - base_vertex->m_position;
		//no;
		e1.cross(e2, no);

		index2  = i2+1;
		index3  = index2 + 1;
		if( index3 == P->m_vertexCount )
			index3 = 0;

		P->m_normals[ 0 ].x -= no._f32[0];
		P->m_normals[ 0 ].y -= no._f32[1];
		P->m_normals[ 0 ].z -= no._f32[2];
		P->m_normals[ index2 ].x -= no._f32[0];
		P->m_normals[ index2 ].y -= no._f32[1];
		P->m_normals[ index2 ].z -= no._f32[2];
		P->m_normals[ index3 ].x -= no._f32[0];
		P->m_normals[ index3 ].y -= no._f32[1];
		P->m_normals[ index3 ].z -= no._f32[2];

		v_node1 = v_node2;
		v_node2 = v_node2->m_right;
	}

	for( u16 i2 = 0; i2 < P->m_vertexCount; ++i2 )
	{
		P->m_normals[i2].normalize2();
	}
	P->m_facenormal = P->m_normals[ 0 ];
}
void kkPolygon_replaceVertex(kkVertex* old_vertex, kkVertex* new_vertex, kkPolygon* p)
{
	auto vertex = p->m_verts;
	for(u16 i = 0; i < p->m_vertexCount; ++i)
	{
		if(vertex->m_element == old_vertex)
		{
			vertex->m_element = new_vertex;
			return;
		}
		vertex = vertex->m_right;
	}
}
// полигоны всё ещё будут содержать данную вершину
void kkVertex_removeOtherPolygons(kkVertex* v)
{
	if(v->m_polygonCount>1)
	{
		auto P = v->m_polygons->m_right;
		for(u16 i = 1; i < v->m_polygonCount; ++i)
		{
			auto next = P->m_right;
			kkDestroy(P);
			P = next;
		}
		v->m_polygonCount = 1;
		v->m_polygons->m_left  = v->m_polygons;
		v->m_polygons->m_right = v->m_polygons;
	}
}
void kkVertex_addPolygon(kkVertex* v, kkPolygon* p)
{
	auto new_loop = kkCreate<kkLoopNode<kkPolygon>>();
	new_loop->m_element = p;
	if(!v->m_polygonCount)
	{
		v->m_polygons = new_loop;
		v->m_polygons->m_left  = v->m_polygons;
		v->m_polygons->m_right = v->m_polygons;
	}
	else
	{
		new_loop->m_right = v->m_polygons;
		new_loop->m_left  = v->m_polygons->m_left;
		v->m_polygons->m_left->m_right = new_loop;
		v->m_polygons->m_left = new_loop;
	}
	++v->m_polygonCount;
}
void kkVertex_addEdge(kkVertex* v, kkEdge* e)
{
	auto new_loop = kkCreate<kkLoopNode<kkEdge>>();
	new_loop->m_element = e;
	if(!v->m_edgeCount)
	{
		v->m_edges = new_loop;
		v->m_edges->m_left  = v->m_edges;
		v->m_edges->m_right = v->m_edges;
	}
	else
	{
		new_loop->m_right = v->m_edges;
		new_loop->m_left  = v->m_edges->m_left;
		v->m_edges->m_left->m_right = new_loop;
		v->m_edges->m_left = new_loop;
	}
	++v->m_edgeCount;
}
void kkPolygon_addVertex(kkVertex* v, kkPolygon* p)
{
	auto new_loop = kkCreate<kkLoopNode<kkVertex>>();
	new_loop->m_element = v;
	if(!p->m_vertexCount)
	{
		p->m_verts = new_loop;
		p->m_verts->m_left  = p->m_verts;
		p->m_verts->m_right = p->m_verts;
	}
	else
	{
		new_loop->m_right = p->m_verts;
		new_loop->m_left  = p->m_verts->m_left;
		p->m_verts->m_left->m_right = new_loop;
		p->m_verts->m_left = new_loop;
	}
	++p->m_vertexCount;
}
void kkPolygon_addEdge(kkEdge* e, kkPolygon* p)
{
	auto new_loop = kkCreate<kkLoopNode<kkEdge>>();
	new_loop->m_element = e;
	if(!p->m_edgeCount)
	{
		p->m_edges = new_loop;
		p->m_edges->m_left  = p->m_edges;
		p->m_edges->m_right = p->m_edges;
	}
	else
	{
		new_loop->m_right = p->m_edges;
		new_loop->m_left  = p->m_edges->m_left;
		p->m_edges->m_left->m_right = new_loop;
		p->m_edges->m_left = new_loop;
	}
	++p->m_edgeCount;
}
void kkPolygon_removeVertex(kkLoopNode<kkVertex>* vn, kkPolygon* p)
{
	u32 vertex_index = 0;
	auto vn_curr = p->m_verts;
	for(u32 i = 0; i < p->m_vertexCount; ++i)
	{
		if(vn_curr == vn)
		{
			vertex_index = i;
			break;
		}
		vn_curr = vn_curr->m_right;
	}

	if(p->m_normals.size())
	{
		p->m_normals.erase(vertex_index);
		/*if(vertex_index < p->m_vertexCount - 1)
		{
			for(u32 i = vertex_index; i < p->m_vertexCount; ++i)
			{
				p->m_normals[i] = p->m_normals[i+1];
			}
		}*/
	}
	if(p->m_tcoords.size())
	{
		p->m_tcoords.erase(vertex_index);
		/*if(vertex_index < p->m_vertexCount - 1)
		{
			for(u32 i = vertex_index; i < p->m_vertexCount; ++i)
			{
				p->m_tcoords[i] = p->m_tcoords[i+1];
			}
		}*/
	}

	vn->m_left->m_right = vn->m_right;
	vn->m_right->m_left = vn->m_left;
	if(vn == p->m_verts)
		p->m_verts = vn->m_right;
	if(vn == p->m_verts)
		p->m_verts = nullptr;
	--p->m_vertexCount;
	kkDestroy(vn);
}
void kkPolygon_removeVertex(kkVertex* v, kkPolygon* p)
{
	kkLoopNode<kkVertex>* vn = nullptr;
	auto curr = p->m_verts;
	for(u64 i = 0; i < p->m_vertexCount; ++i)
	{
		if(curr->m_element == v)
		{
			vn = curr;
			break;
		}
		curr = curr->m_right;
	}
	if(!vn)
	{
		kkLogWriteWarning("`kkPolygon_removeVertex` - curr is null.\n");
		return;
	}
	kkPolygon_removeVertex(vn, p);
}
void kkVertex_removePolygon(kkVertex* v, kkPolygon* p)
{
	kkLoopNode<kkPolygon>* pn = nullptr;
	auto curr = v->m_polygons;
	for(u64 i = 0; i < v->m_polygonCount; ++i)
	{
		if(curr->m_element == p)
		{
			pn = curr;
			break;
		}
		curr = curr->m_right;
	}
	if(!pn)
	{
		kkLogWriteWarning("`kkVertex_removePolygon` - curr is null.\n");
		return;
	}
	pn->m_left->m_right = pn->m_right;
	pn->m_right->m_left = pn->m_left;
	if(pn == v->m_polygons)
		v->m_polygons = pn->m_right;
	if(pn == v->m_polygons)
		v->m_polygons = nullptr;
	--v->m_polygonCount;
	kkDestroy(pn);
}

PolygonalModel::PolygonalModel()
{
}
PolygonalModel::~PolygonalModel()
{
	ClearNewMesh();
	deleteEdges();
	while(m_polygons)
	{
		DeletePolygon(m_polygons);
	}
}

void PolygonalModel::_addVertexToList(kkVertex* v)
{
	++m_vertsCount;
	if(!m_verts)
	{
		m_verts = v;
		m_verts->m_mainNext = v;
		m_verts->m_mainPrev = v;
	}
	else
	{
		v->m_mainNext = m_verts;
		v->m_mainPrev = m_verts->m_mainPrev;
		m_verts->m_mainPrev->m_mainNext = v;
		m_verts->m_mainPrev = v;
	}
}

void PolygonalModel::_removeVertexFromList(kkVertex* v)
{
	--m_vertsCount;
	if(!m_vertsCount)
	{
		m_verts = nullptr;
	}
	else
	{
		v->m_mainPrev->m_mainNext = v->m_mainNext;
		v->m_mainNext->m_mainPrev = v->m_mainPrev;

		if(v == m_verts)
			m_verts = m_verts->m_mainNext;
	}
}

void PolygonalModel::_addPolygonToList(kkPolygon* p)
{
	++m_polygonsCount;
	if(!m_polygons)
	{
		m_polygons = p;
		m_polygons->m_mainNext = p;
	}
	else
	{
		p->m_mainNext = m_polygons;
		p->m_mainPrev = m_polygons->m_mainPrev;
		m_polygons->m_mainPrev->m_mainNext = p;
	}
	m_polygons->m_mainPrev = p;
}

void PolygonalModel::_removePolygonFromList(kkPolygon* p)
{
	--m_polygonsCount;
	if(!m_polygonsCount)
	{
		m_polygons = nullptr;
	}
	else
	{
		p->m_mainPrev->m_mainNext = p->m_mainNext;
		p->m_mainNext->m_mainPrev = p->m_mainPrev;

		if(p == m_polygons)
			m_polygons = m_polygons->m_mainNext;
	}
}
void PolygonalModel::_addEdgeToList(kkEdge* e)
{
	++m_edgesCount;
	if(!m_edges)
	{
		m_edges = e;
		m_edges->m_mainNext = e;
	}
	else
	{
		e->m_mainNext = m_edges;
		e->m_mainPrev = m_edges->m_mainPrev;
		m_edges->m_mainPrev->m_mainNext = e;
	}
	m_edges->m_mainPrev = e;
}
void PolygonalModel::_removeEdgeFromList(kkEdge* e)
{
	--m_edgesCount;
	if(!m_edgesCount)
	{
		m_edges = nullptr;
	}
	else
	{
		e->m_mainPrev->m_mainNext = e->m_mainNext;
		e->m_mainNext->m_mainPrev = e->m_mainPrev;
	}
}
kkPolygon* PolygonalModel::GetPolygons()
{
	return m_polygons;
}

void PolygonalModel::DeletePolygon(kkPolygon* p)
{
	assert(p);
	if(!p)
	{
		kkLogWriteWarning("`DeletePolygon` - p is null.\n");
		return;
	}

	auto VN = p->m_verts;
	while(p->m_vertexCount)
	{
		auto next = VN->m_right;
		auto V = VN->m_element;
		// удалить вершину из полигона
		kkPolygon_removeVertex(VN, p);
		kkVertex_removePolygon(V, p);
		VN = next;
		
		if( !V->m_polygonCount )
		{
			_removeVertexFromList(V);
			kkDestroy(V);
		}
	}

	if(p == m_polygons)
		m_polygons = m_polygons->m_mainNext;
	_removePolygonFromList(p);

	kkDestroy(p);
}

void PolygonalModel::AddPolygon(kkGeometryInformation* gi, bool weld, bool flip)
{
	if(!gi)
	{
		kkLogWriteWarning("`AddPolygon` - GeometryInformation is null.\n");
		return;
	}
	kkPolygon* new_polygon = kkCreate<kkPolygon>();
	
	u16 positionSize = (u16)gi->m_position.size();
	bool withNormals = gi->m_normal.size() > 0;
	bool withTCoords = gi->m_tcoords.size() > 0;

	for( u16 i = 0; i < positionSize; ++i )
	{
		auto & pos = gi->m_position[i];
		VertexHash vh;
		vh.set(&pos);

		if(withNormals)
		{
			new_polygon->m_normals.push_back( gi->m_normal[i] );
			if( flip )
			{
				new_polygon->m_normals[i].x = -new_polygon->m_normals[i].x;
				new_polygon->m_normals[i].y = -new_polygon->m_normals[i].y;
				new_polygon->m_normals[i].z = -new_polygon->m_normals[i].z;
			}
		}
		if(withTCoords)
		{
			new_polygon->m_tcoords.push_back( gi->m_tcoords[i] );
		}

		kkVertex * new_vertex = nullptr;
		if(weld)
		{
			auto find_result = m_weldMap.find(vh.str);
			if( find_result == m_weldMap.end() )
			{
				new_vertex = kkCreate<kkVertex>();
				new_vertex->m_position.set(pos.x, pos.y, pos.z);
				new_vertex->m_positionFix = new_vertex->m_position;
				m_weldMap[vh.str] = new_vertex;
				_addVertexToList(new_vertex);
			}
			else
			{
				new_vertex = find_result->second;
			}
		}
		else
		{
			new_vertex = kkCreate<kkVertex>();
			new_vertex->m_position.set(pos.x, pos.y, pos.z);
			new_vertex->m_positionFix = new_vertex->m_position;
			_addVertexToList(new_vertex);
		}
		kkVertex_addPolygon(new_vertex, new_polygon);
		kkPolygon_addVertex(new_vertex, new_polygon);
	}
	_addPolygonToList(new_polygon);
}
void PolygonalModel::deleteEdges()
{
	if(!m_edgesCount)
		return;
	auto E = m_edges;
	for(u64 i = 0; i < m_edgesCount; ++i)
	{
		auto next = E->m_mainNext;
		kkDestroy(E);
		E = next;
	}
	m_edges = nullptr;
	m_edgesCount = 0;
	auto P = m_polygons;
	for(u64 i = 0; i < m_polygonsCount; ++i)
	{
		if(P->m_edgeCount)
		{
			auto PE_node = P->m_edges;
			for(u16 o = 0; o < P->m_edgeCount; ++o)
			{
				auto next = PE_node->m_right;
				kkDestroy(PE_node);
				PE_node = next;
			}
			P->m_edgeCount = 0;
			P->m_edges = nullptr;
		}
		P = P->m_mainNext;
	}
	auto V = m_verts;
	for(u64 i = 0; i < m_vertsCount; ++i)
	{
		if(V->m_edgeCount)
		{
			auto VE_node = V->m_edges;
			for(u16 o = 0; o < V->m_edgeCount; ++o)
			{
				auto next = VE_node->m_right;
				kkDestroy(VE_node);
				VE_node = next;
			}
			V->m_edgeCount = 0;
			V->m_edges = nullptr;
		}
		V = V->m_mainNext;
	}
}
void PolygonalModel::updateEdges()
{
	if(m_edges)
		deleteEdges();
	if(!m_polygons)
		return;
	auto polygon = m_polygons;
	for(u64 i = 0; i < m_polygonsCount; ++i )
	{
		if(polygon->m_flags & polygon->EF_OLD)
		{
			polygon = polygon->m_mainNext;
			continue;
		}
		auto vertex_node1 = polygon->m_verts;
		auto vertex_node2 = vertex_node1->m_right;
		for(u64 o = 0; o < polygon->m_vertexCount; ++o )
		{
			auto v1 = vertex_node1->m_element;
			auto v2 = vertex_node2->m_element;

			// пусть вершина с адресом значение которого меньше
			//  будет на первом месте.
			if(v2 < v1)
			{
				v1 = vertex_node2->m_element;
				v2 = vertex_node1->m_element;
			}

			kkEdge* new_edge = nullptr;
			bool create_new = true;
			if( v1->m_edgeCount )
			{
				auto edge_node = v1->m_edges;
				for(u64 k = 0; k < v1->m_edgeCount; ++k)
				{
					if(edge_node->m_element->m_v1 == v1 
						&& edge_node->m_element->m_v2 == v2)
					{
						new_edge = edge_node->m_element;
						create_new = false;
						break;
					}
					edge_node = edge_node->m_right;
				}
			}

			if(create_new)
			{
				new_edge = kkCreate<kkEdge>();
				new_edge->m_v1 = v1;
				new_edge->m_v2 = v2;
				_addEdgeToList(new_edge);
			}
			if(!new_edge->m_p1)
				new_edge->m_p1 = polygon;
			else
				new_edge->m_p2 = polygon;
			kkVertex_addEdge(v1, new_edge);
			kkVertex_addEdge(v2, new_edge);
			kkPolygon_addEdge(new_edge, polygon);

			vertex_node1 = vertex_node2;
			vertex_node2 = vertex_node2->m_right;
		}
		
		polygon = polygon->m_mainNext;
	}
}

u64 PolygonalModel::GetPolygonsCount()
{
	return m_polygonsCount;
}

void PolygonalModel::calculateTriangleCount()
{
	m_triangleCount = 0;
	if(m_polygons)
	{
		auto last = m_polygons->m_mainPrev;
		auto P = m_polygons;
		while(true)
		{
			m_triangleCount += P->m_vertexCount - 2;
			if(P == last)
				break;
			P = P->m_mainNext;
		}
	}
}

// нужно всего лишь изменить координаты близких друг к другу вершин
void PolygonalModel::weldByLen(f32 len)
{
	//for( u64 i = 0, sz = m_verts.size(); i < sz; ++i )
	//{
	//	auto V1 = (Vertex*)m_verts[i];
	//	for( u64 i2 = 0, sz2 = m_verts.size(); i2 < sz2; ++i2 )
	//	{
	//		auto V2 = (Vertex*)m_verts[i2];
	//		if(V1!=V2)
	//		{
	//			if( V1->m_Position.distance(V2->m_Position) <= len )
	//			{
	//				V1->m_Position = (V2->m_Position + V1->m_Position) * 0.5f;
	//				V2->m_Position = V1->m_Position;
	//			}
	//		}
	//	}
	//}
}


void PolygonalModel::onEndCreation()
{
	calculateTriangleCount();
	//updateEdges();
	m_weldMap.clear();
}

void PolygonalModel::generateBT()
{
}

void PolygonalModel::generateNormals(bool flat)
{
	kkLogWriteInfo("Generate normals.\n");
	// flat
	v3f e1, e2, no;

	auto current_polygon = m_polygons;
	for( size_t i = 0; i < m_polygonsCount; ++i )
	{
		if(current_polygon->m_normals.size() != current_polygon->m_vertexCount)
		{
			while (true)
			{
				current_polygon->m_normals.push_back(v3f());
				if(current_polygon->m_normals.size() == current_polygon->m_vertexCount)
					break;
			}
		}
		for( u64 i2 = 0; i2 < current_polygon->m_vertexCount; ++i2 )
		{
			current_polygon->m_normals[ i2 ].set( 0.f, 0.f, 0.f );
		}
		kkVertex * base_vertex = current_polygon->m_verts->m_element;
		auto v_node1 = current_polygon->m_verts->m_right;
		auto v_node2 = v_node1->m_right;
		u64 index2, index3;
		for( u64 i2 = 0, sz2 = current_polygon->m_vertexCount - 2; i2 < sz2; ++i2 )
		{
			kkVertex * vertex3 = v_node1->m_element;
			kkVertex * vertex2 = v_node2->m_element;

			e1 = vertex2->m_position - base_vertex->m_position;
			e2 = vertex3->m_position - base_vertex->m_position;
			//no;
			no = e1.cross_return(e2);

			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == current_polygon->m_vertexCount )
				index3 = 0;

			current_polygon->m_normals[ 0 ] -= no;
			current_polygon->m_normals[ index2 ] -= no;
			current_polygon->m_normals[ index3 ] -= no;

			base_vertex->m_normal -= no;
			vertex2->m_normal -= no;
			vertex3->m_normal -= no;

			v_node1 = v_node2;
			v_node2 = v_node2->m_right;
		}

		for( u64 i2 = 0; i2 < current_polygon->m_vertexCount; ++i2 )
		{
			current_polygon->m_normals[i2].normalize2();
		}
		current_polygon->m_facenormal = current_polygon->m_normals[ 0 ];
		current_polygon = current_polygon->m_mainNext;
	}

	auto V = m_verts;
	for( size_t i = 0; i < m_vertsCount; ++i )
	{
		V->m_normal = V->m_normal / (f32)V->m_polygonCount;
		V->m_normal.normalize2();
		V = V->m_mainNext;
	}

	if( flat )
		return;

	// smooth
	current_polygon = m_polygons;
	for( size_t i = 0; i < m_polygonsCount; ++i )
	{
		auto V = current_polygon->m_verts;
		for( u64 i2 = 0; i2 < current_polygon->m_vertexCount; ++i2 )
		{
			current_polygon->m_normals[i2] = V->m_element->m_normal;
			V = V->m_right;
		}
		current_polygon = current_polygon->m_mainNext;
	}
}

//void PolygonalModel::_createGridAccel(kkRenderInfo* ri)
//{
//	// нужно вычислить количество линий
//	s32 cell_size = 15;  // размер ячейки 10x10
//	m_gridRadius = v2f(0.f,0.f).distance(v2f((f32)cell_size * 0.5f));
//	
//	s32 rows = ri->image->m_height / cell_size;
//	if( ri->image->m_height % cell_size )
//		++rows;
//	s32 cols = ri->image->m_width / cell_size;
//	if( ri->image->m_width % cell_size )
//		++cols;
//	m_gridAccelRows.reserve(rows);
//	s32 _x = 0;
//	s32 _y = 0;
//	for( s32 i = 0; i < rows; ++i )
//	{
//		auto row = new GridAcceleratorRow;
//		m_gridAccelRows.push_back(row);
//		row->m_cells.reserve(cols);
//		for( s32 j = 0; j < cols; ++j )
//		{
//			auto cell = new GridAcceleratorCell;
//			cell->left_top.x     = (f32)_x;
//			cell->left_top.y     = (f32)_y;
//			cell->right_bottom.x = (f32)_x + (f32)cell_size;
//			cell->right_bottom.y = (f32)_y + (f32)cell_size;
//
//			cell->center.x = cell->left_top.x + ((cell->right_bottom.x - cell->left_top.x)*0.5f);
//			cell->center.y = cell->left_top.y + ((cell->right_bottom.y - cell->left_top.y)*0.5f);
//
//			row->m_cells.push_back(cell);
//
//			_x += cell_size;
//		}
//		_y += cell_size;
//		_x = 0;
//	}
//}
//
//struct AABB2D
//{
//	v2f m_min = v2f(FLT_MAX, FLT_MAX);
//	v2f m_max = v2f(-FLT_MAX, -FLT_MAX);
//
//	void add(const v2f & point )
//	{
//		if( point.x < m_min.x ) m_min.x = point.x;
//		if( point.y < m_min.y ) m_min.y = point.y;
//
//		if( point.x > m_max.x ) m_max.x = point.x;
//		if( point.y > m_max.y ) m_max.y = point.y;
//	}
//
//	void center( v2f& out )
//	{
//		out.x = (m_min.x + m_max.x) * 0.5f;
//		out.y = (m_min.y + m_max.y) * 0.5f;
//	}
//};
//void PolygonalModel::_addTriangleToGrid(kkRenderInfo* ri, kkTriangle* tri, u32 triIndex)
//{
//	v2i P1, P2, P3;
//	auto viewport = v2f((f32)ri->image->m_width, (f32)ri->image->m_height);
//	auto offset   = v2f(0.f,0.f);
//	auto v1 = tri->v1;
//	auto v2 = tri->v2;
//	auto v3 = tri->v3;
//	P1 = kkrooo::worldToScreen( ri->VP, v1, viewport, offset );
//	P2 = kkrooo::worldToScreen( ri->VP, v2, viewport, offset );
//	P3 = kkrooo::worldToScreen( ri->VP, v3, viewport, offset );
//	
//	v2f p1(P1.x,P1.y);
//	v2f p2(P2.x,P2.y);
//	v2f p3(P3.x,P3.y);
//
//	/*v2f triangle_center = p1 + p2 + p3;
//	triangle_center.x *= 0.3333333f;
//	triangle_center.y *= 0.3333333f;*/
//
//	AABB2D tri_aabb;
//	tri_aabb.add(p1);
//	tri_aabb.add(p2);
//	tri_aabb.add(p3);
//
//	v2f aabb_center;
//	tri_aabb.center(aabb_center);
//
//	f32 tri_radius = aabb_center.distance(tri_aabb.m_max);
//	f32 tri_grid_len = tri_radius + m_gridRadius;
//
//	//printf("%i %i : %i %i : [%f %f]\n", P1.x, P1.y, P2.x, P2.y, viewport.x, viewport.y);
//	// сначала надо найти нужную линию, потом нужный столбик
//	bool lineFound = false;
//	for( u64 i = 0, sz = m_gridAccelRows.size(); i < sz; ++i )
//	{
//
//		bool currentLineFound = false;
//
//		auto row = m_gridAccelRows[i];
//		auto first_cell = row->m_cells[0];
//		
//		bool cellFound = false;
//		for( u64 j = 0, jsz = row->m_cells.size(); j < jsz; ++j )
//		{
//			auto cell = row->m_cells[j];
//			
//			f32 len = cell->center.distance(aabb_center);
//			if( len <= tri_grid_len )
//			{
//				cellFound = true;
//				goto add;
//			}
//			if( cellFound )
//			{
//				break;
//			}
//			continue;
//			add:
//			cell->m_inds.push_back(triIndex);
//			cellFound = true;
//			lineFound = true;
//			currentLineFound = true;
//		}
//		
//		if( lineFound && !currentLineFound )
//		{
//			break;
//		}
//	}
//}

void PolygonalModel::prepareForRaytracing(kkRenderInfo* ri)
{
	kkLogWriteInfo("Build raytracer accelerator...\n");
	auto T1 = kkGetMainSystem()->getTime();
	//kkCameraFrustum frust;
	//frust.calculateFrustum(ri->P, ri->V);

	auto ms = kkGetMainSystem();
	auto t = ms->getTime();

	if( m_isPreparedForRaytracing )
	{
		kkLogWriteWarning("`prepareForRaytracing` called more than once! Now programm will call finishRaytracing...\n");
		finishRaytracing();
	}
	
	calculateTriangleCount();
	//_createGridAccel(ri);

	//printf("m_triangleCount %u\n",m_triangleCount);
	m_trianglesForRendering.clear();
	m_trianglesForRendering.reserve(m_triangleCount);
	auto & matrix = m_object->GetMatrix();
	auto & pivot = m_object->GetPivot();
	auto  current_polygon = m_polygons;
	u32 freeIndsCount = 0;
	for(u64 i = 0; i < m_polygonsCount; ++i )
	{
		kkVertex * base_vertex = current_polygon->m_verts->m_element;
		auto v_node1 = current_polygon->m_verts->m_right;
		auto v_node2 = v_node1->m_right;

		u64 index2, index3;
		for( u64 i2 = 0, sz2 = current_polygon->m_vertexCount - 2; i2 < sz2; ++i2 )
		{
			
			kkVertex * vertex3 = v_node1->m_element;
			kkVertex * vertex2 = v_node2->m_element;

			kkTriangleRayTestResult rtr;
			rtr.triangle.v1 = math::mul( base_vertex->m_position, matrix) + pivot;
			rtr.triangle.v2 = math::mul( vertex2->m_position, matrix) + pivot;
			rtr.triangle.v3 = math::mul( vertex3->m_position, matrix) + pivot;
			rtr.triangle.update();

			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == current_polygon->m_vertexCount )
				index3 = 0;

			if(current_polygon->m_normals.size())
			{
				rtr.triangle.normal1._f32[0] = current_polygon->m_normals[ 0 ].x;
				rtr.triangle.normal1._f32[1] = current_polygon->m_normals[ 0 ].y;
				rtr.triangle.normal1._f32[2] = current_polygon->m_normals[ 0 ].z;
				rtr.triangle.normal2._f32[0] = current_polygon->m_normals[ index3 ].x;
				rtr.triangle.normal2._f32[1] = current_polygon->m_normals[ index3 ].y;
				rtr.triangle.normal2._f32[2] = current_polygon->m_normals[ index3 ].z;
				rtr.triangle.normal3._f32[0] = current_polygon->m_normals[ index2 ].x;
				rtr.triangle.normal3._f32[1] = current_polygon->m_normals[ index2 ].y;
				rtr.triangle.normal3._f32[2] = current_polygon->m_normals[ index2 ].z;
			}
			if(current_polygon->m_tcoords.size())
			{
				rtr.triangle.t1._f32[0] = current_polygon->m_tcoords[ 0 ].x;
				rtr.triangle.t1._f32[1] = current_polygon->m_tcoords[ 0 ].y;
				rtr.triangle.t2._f32[0] = current_polygon->m_tcoords[ index3 ].x;
				rtr.triangle.t2._f32[1] = current_polygon->m_tcoords[ index3 ].y;
				rtr.triangle.t3._f32[0] = current_polygon->m_tcoords[ index2 ].x;
				rtr.triangle.t3._f32[1] = current_polygon->m_tcoords[ index2 ].y;
			}

			kkAabb taabb;
			taabb.add(rtr.triangle.v1);
			taabb.add(rtr.triangle.v2);
			taabb.add(rtr.triangle.v3);
			kkVector4 triCnter;
			taabb.center(triCnter);

			/*if( kkrooo::pointOnFrontSideCamera( rtr.triangle.v1, ri->VP )
				|| kkrooo::pointOnFrontSideCamera( rtr.triangle.v2, ri->VP )
				|| kkrooo::pointOnFrontSideCamera( rtr.triangle.v3, ri->VP ) )
			{
				m_trianglesForRendering.push_back(rtr);
				_addTriangleToGrid(ri, &rtr.triangle, freeIndsCount);
				
				m_BVH_root.m_inds.push_back(freeIndsCount++);
				m_BVH_root.m_aabb.add(rtr.triangle.v1);
				m_BVH_root.m_aabb.add(rtr.triangle.v2);
				m_BVH_root.m_aabb.add(rtr.triangle.v3);
			}*/

			//if( frust.sphereInFrustum(taabb.radius(triCnter), triCnter) )
			{
			//	_addTriangleToGrid(ri, &rtr.triangle, freeIndsCount);
				m_trianglesForRendering.push_back(rtr);
				m_BVH_root.m_inds.push_back(freeIndsCount++);
				m_BVH_root.m_aabb.add(rtr.triangle.v1);
				m_BVH_root.m_aabb.add(rtr.triangle.v2);
				m_BVH_root.m_aabb.add(rtr.triangle.v3);
			}

			v_node1 = v_node2;
			v_node2 = v_node2->m_right;
		}
		current_polygon = current_polygon->m_mainNext;
	}

	/*printf("Grid rows: [%llu]\n", m_gridAccelRows.size());
	for( u64 i = 0; i < m_gridAccelRows.size(); ++i )
	{
		printf("   Row [%llu]:\t\t[%llu] cells\n", i, m_gridAccelRows[i]->m_cells.size());
		for( u64 j = 0; j < m_gridAccelRows[i]->m_cells.size(); ++j )
		{
			printf("   \t Cell [%llu]:\t\t[%llu] tris\n", j, m_gridAccelRows[i]->m_cells[j]->m_inds.size());
		}
	}*/
	// бокс целой модели
	m_aabbRayTestAabb.reset();
	m_aabbRayTestAabb.add(m_BVH_root.m_aabb);

	// РАЗБИЕНИЕ
	// m_BVH_root.m_inds содержит индексы всех треугольников
	// разбивая, нужно передать индексы новым узлам, и потом удалить m_inds
	if( m_triangleCount > numOfIndsPerBVHNode )
	{
		kkArray<BVH_node*> node_buffer = kkArray<BVH_node*>(0xffff); // замена рекурсии
		node_buffer.push_back(&m_BVH_root);
		bool ignorenext = false;
		for( size_t i = 0; i < node_buffer.size(); ++i )
		{
			if( ignorenext )
			{
				ignorenext = false;
				continue;
			}
			auto new_node = node_buffer[i];

			// сначала нужно понять, надо ли разбивать часть
			if( new_node->m_inds.size() > numOfIndsPerBVHNode )
			{
			// если надо, создаю две новые части, и передаю в них нужные m_freeInds
				BVH_node * first_bvh  = nullptr;
				BVH_node * second_bvh = nullptr;
				
				kkVector4 aabbCenter;
				new_node->m_aabb.center(aabbCenter);

				// узнаю, в каком месте у бокса больше скопления вершин
				int trisCount_Xp = 0;
				int trisCount_Xn = 0;
				int trisCount_Yp = 0;
				int trisCount_Yn = 0;
				int trisCount_Zp = 0;
				int trisCount_Zn = 0;
				for( u64 o = 0, osz = new_node->m_inds.size(); o < osz; ++o )
				{
					auto & triangle = m_trianglesForRendering[new_node->m_inds.at(o)];
					kkVector4 triangleCenter;
					triangle.triangle.center(triangleCenter);
					if( triangleCenter.KK_X < aabbCenter.KK_X ) ++trisCount_Xn; else ++trisCount_Xp;
					if( triangleCenter.KK_Y < aabbCenter.KK_Y ) ++trisCount_Yn; else ++trisCount_Yp;
					if( triangleCenter.KK_Z < aabbCenter.KK_Z ) ++trisCount_Zn; else ++trisCount_Zp;
				}

				// разница
				auto trisDiffX = std::abs(trisCount_Xp-trisCount_Xn);
				auto trisDiffY = std::abs(trisCount_Yp-trisCount_Yn);
				auto trisDiffZ = std::abs(trisCount_Zp-trisCount_Zn);
				
				// сторона для разделения
				enum class _side{side_X, side_Y, side_Z};
				_side _side_ = _side::side_X;
				
				// нужно использовать ту сторону, в которой наименьшая разница
				if( trisDiffY < trisDiffX )
				{
					_side_ = _side::side_Y;
					if( trisDiffZ < trisDiffY ) 
						_side_ = _side::side_Z;
				}
				else if( trisDiffZ < trisDiffX )
				{
					_side_ = _side::side_Z;
					if( trisDiffY < trisDiffZ ) 
						_side_ = _side::side_Y;
				}

				int vectorComponent = 0;
				switch (_side_)
				{
				case _side::side_Y:
					vectorComponent = 1;
					break;
				case _side::side_Z:
					vectorComponent = 2;
					break;
				case _side::side_X:
				default:
					break;
				}

				for( u64 o = 0, osz = new_node->m_inds.size(); o < osz; ++o )
				{
					auto index = new_node->m_inds.at(o);
					auto & triangle = m_trianglesForRendering[index];

					bool add_to_first = false;


					kkVector4 triangleCenter;
					triangle.triangle.center(triangleCenter);
					if( triangleCenter._f32[vectorComponent] > aabbCenter._f32[vectorComponent] )
						add_to_first = true;

					BVH_node * node_to_add = nullptr;

					if( add_to_first )
					{
						if( !first_bvh )
						{
							first_bvh  = new BVH_node;
							//first_bvh->m_inds = new kkArray<u32>(0xff);
						}

						node_to_add = first_bvh;
					}
					else
					{
						if( !second_bvh )
						{
							second_bvh  = new BVH_node;
							//second_bvh->m_inds = new kkArray<u32>(0xff);
						}
						node_to_add = second_bvh;
					}

					node_to_add->m_inds.push_back(index);
					node_to_add->m_aabb.add(triangle.triangle.v1);
					node_to_add->m_aabb.add(triangle.triangle.v2);
					node_to_add->m_aabb.add(triangle.triangle.v3);

				}


				// если нет какого-то узла, это значит что все индексы были добавлены в один узел
				// значит нужно будет уметь игнорировать эту ноду при дальнейшем разбиении
				if( !first_bvh || !second_bvh )
				{
					ignorenext = true;
				}

				if( first_bvh )
					node_buffer.push_back(first_bvh);

				if( second_bvh )
					node_buffer.push_back(second_bvh);

				new_node->first  = first_bvh;
				new_node->second = second_bvh;
			}
		}
	}

	m_isPreparedForRaytracing = true;
	kkLogWriteInfo("[%llu]\n", kkGetMainSystem()->getTime() - T1);
}

//void PolygonalModel::_deleteGridAccel()
//{
//	for( u64 i = 0, sz = m_gridAccelRows.size(); i < sz; ++i )
//	{
//		delete m_gridAccelRows[i];
//	}
//	m_gridAccelRows.clear();
//}

void PolygonalModel::finishRaytracing()
{
	if( m_isPreparedForRaytracing )
	{
		//_deleteGridAccel();
		m_BVH_root.clear();
		m_trianglesForRendering.clear();
		m_isPreparedForRaytracing = false;
	}
}

bool PolygonalModel::_intersectBVHNode(BVH_node* node, const kkRay& ray)
{
	return node->m_aabb.rayTest(ray);
}

void PolygonalModel::rayTestBVH( BVH_node* node, std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, 
	kkMaterialImplementation* renderObjectMaterial )
{
	bool noChildren = true;
	if( node->first )
	{
		noChildren = false;
		//if( node->first->m_aabb.rayTest(ray) )
		if( _intersectBVHNode(node->first, ray ) )
		{
			rayTestBVH( node->first, outTriangles, ray, renderObjectMaterial );
		}
	}
	if( node->second )
	{
		noChildren = false;
		//if( node->second->m_aabb.rayTest(ray) )
		if( _intersectBVHNode(node->second, ray ) )
		{
			rayTestBVH( node->second, outTriangles, ray, renderObjectMaterial );
		}
	}
	if(noChildren)
	{
		//for( u32 i2 = 0; i2 < node->m_noumOfInds; ++i2 )
		for( u32 i2 = 0; i2 < node->m_inds.size(); ++i2 )
		{
			auto  triangle = &m_trianglesForRendering[ node->m_inds.at(i2) ];
			_rayTestTriangle(outTriangles, ray, renderObjectMaterial, triangle);
		}
	}
}

void PolygonalModel::_rayTestTriangle( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial, kkTriangleRayTestResult* triangle )
{
	f32 len = 0.f;
	f32 U = 0.f;
	f32 V = 0.f;
	f32 W = 0.f;
	triangle->triangle.update();
	//if(triangle->triangle.rayTest_Watertight(ray, true, len, U, V, W))
	if(triangle->triangle.rayTest_MT(ray, true, len, U, V, W))
	{

		kkTriangleRayTestResult T;
		T.pointNormal = W * triangle->triangle.normal1 + U * triangle->triangle.normal2 + V * triangle->triangle.normal3;
		T.pointTcoord = W * triangle->triangle.t1 + U * triangle->triangle.t2 + V * triangle->triangle.t3;
		T.intersectionPoint = ray.m_origin + ray.m_direction * len;
		T.length = len;
		T.material = renderObjectMaterial;
		outTriangles.push_back(T);
	}
}

void PolygonalModel::rayTest( std::vector<kkTriangleRayTestResult>& outTriangle, const kkRay& ray, 
	kkMaterialImplementation * renderObjectMaterial )
{
	// Object AABB
	if( !m_aabbRayTestAabb.rayTest(ray) )		return;
	
	rayTestBVH(&m_BVH_root, outTriangle, ray, renderObjectMaterial);
}

//void PolygonalModel::rayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial )
//{
//	// надо найти нужную ячейку по данным координатам
//	for( u64 i_r = 0, rsz = m_gridAccelRows.size(); i_r < rsz; ++i_r )
//	{
//		auto row = m_gridAccelRows[ i_r ];
//		auto first_cell = row->m_cells[0];
//		if( point.y >= first_cell->left_top.y && point.y <= first_cell->right_bottom.y ) // найти нужную линию
//		{
//			for( u64 i_c = 0, csz = row->m_cells.size(); i_c < csz; ++i_c )
//			{
//				auto cell = row->m_cells[i_c];
//				if( point.x >= cell->left_top.x && point.x <= cell->right_bottom.x )
//				{
//					for( u64 i_t = 0, tsz = cell->m_inds.size(); i_t < tsz; ++i_t )
//					{
//						auto  triangle = &m_trianglesForRendering[ cell->m_inds.at(i_t) ];
//						_rayTestTriangle(outTriangles, ray, renderObjectMaterial, triangle);
//					}
//
//					return;
//				}
//			}
//		}
//	}
//}

void PolygonalModel::attachModel(PolygonalModel* other, const kkMatrix4& invertMatrix, const kkMatrix4& matrix_other, const kkVector4& pivot, const kkVector4& pivot_other)
{
	kkLogWriteInfo("Attach model\n");
	if(!other)
	{
		kkLogWriteWarning("Attach model: `other` is null\n");
		return;
	}
	auto TIM = matrix_other;
	TIM.invert();
	TIM.transpose();

	auto other_polygon = other->m_polygons;
	for( u64 i = 0; i < other->m_polygonsCount; ++i )
	{
		if(other_polygon->m_normals.size())
		{
			for( u64 i2 = 0; i2 < other_polygon->m_vertexCount; ++i2 )
			{
				other_polygon->m_normals[i2] = math::mul(other_polygon->m_normals[i2], TIM);
			}
		}
		other_polygon = other_polygon->m_mainNext;
	}
	
	auto other_vertex = other->m_verts;
	for( u64 i = 0; i < other->m_vertsCount; ++i )
	{
		other_vertex->m_position	= math::mul(other_vertex->m_position, matrix_other)+ pivot_other - pivot;
		other_vertex->m_position	= math::mul(other_vertex->m_position, invertMatrix) ;
		other_vertex->m_positionFix = other_vertex->m_position;
		other_vertex = other_vertex->m_mainNext;
	}

	other->deleteEdges();

	auto verts = other->m_verts;
	auto polys = other->m_polygons;

	{
		auto begin_1 = verts;
		auto end_1 = verts->m_mainPrev;

		auto begin_2 = m_verts;
		auto end_2 = m_verts->m_mainPrev;

		end_2->m_mainNext = begin_1;
		begin_1->m_mainPrev = end_2;

		end_1->m_mainNext = begin_2;
		begin_2->m_mainPrev = end_1;
	}
	{
		auto begin_1 = polys;
		auto end_1 = polys->m_mainPrev;

		auto begin_2 = m_polygons;
		auto end_2 = m_polygons->m_mainPrev;

		end_2->m_mainNext = begin_1;
		begin_1->m_mainPrev = end_2;

		end_1->m_mainNext = begin_2;
		begin_2->m_mainPrev = end_1;
	}
	m_polygonsCount += other->m_polygonsCount;
	m_vertsCount += other->m_vertsCount;

	other->m_polygons = nullptr;
	other->m_polygonsCount = 0;
	other->m_verts = nullptr;
	other->m_vertsCount = 0;
}

bool PolygonalModel::breakVerts()
{
	std::vector<kkVertex*> newVerts;
	auto vertex = m_verts;
	for( u64 i = 0; i < m_vertsCount; ++i )
	{
		if(vertex->m_flags & vertex->EF_SELECTED)
		{
			if(vertex->m_polygonCount > 1)
			{
				auto polygon = vertex->m_polygons->m_right;
				for(u32 i2 = 1; i2 < vertex->m_polygonCount; ++i2)
				{
					kkVertex * new_vertex = nullptr;
					new_vertex = kkCreate<kkVertex>();
					new_vertex->m_position = vertex->m_position;
					new_vertex->m_positionFix = vertex->m_position;
					new_vertex->m_normal = vertex->m_normal;
					newVerts.push_back(new_vertex);

					kkVertex_addPolygon(new_vertex, polygon->m_element);
					kkPolygon_replaceVertex(vertex, new_vertex, polygon->m_element);

					polygon = polygon->m_right;
				}

				kkVertex_removeOtherPolygons(vertex);
			}
		}
		vertex = vertex->m_mainNext;
	}
	for( size_t i = 0, sz = newVerts.size(); i < sz; ++i )
	{
		_addVertexToList(newVerts[i]);
	}
	return newVerts.size() != 0;
}

kkEdge* PolygonalModel::isEdge(kkVertex* V1, kkVertex* V2)
{
	auto edge = m_edges;
	for(u32 i = 0; i < m_edgesCount; ++i)
	{
		if((edge->m_v1 == V1 && edge->m_v2 == V2)
			|| (edge->m_v1 == V2 && edge->m_v2 == V1))
		{
			return edge;
		}
		edge = edge->m_mainNext;
	}
	return nullptr;
}
bool PolygonalModel::isOnEdge(kkVertex* V)
{
	auto E = V->m_edges;
	for(u32 i = 0; i < V->m_edgeCount; ++i)
	{
		if((E->m_element->m_p1 && !E->m_element->m_p2)
			|| (!E->m_element->m_p1 && E->m_element->m_p2))
			return true;
		E = E->m_right;
	}
	return false;
}
bool PolygonalModel::weld(kkVertex* V1, kkVertex* V2)
{
	//// есть 2 случая, когда надо делать weld
	//// 1. когда точки образуют ребро
	//// 2. когда хоть 1 ребро у каждой точки имеет только 1 полигон на стороне (то есть точка как бы снаружи)
	bool result = false;
	auto edge = isEdge(V1, V2);
	if(edge)
	{
		// нужно взять полигоны вершины, такие,
		// которые не являются частью ребра
		// и заменить вершину полигона которая потом
		// будетт удалена (V1) на вершину V2
		auto V1PolygonNode = V1->m_polygons;
		for(u32 i = 0; i < V1->m_polygonCount; ++i)
		{
			if(V1PolygonNode->m_element != edge->m_p1
				&& V1PolygonNode->m_element != edge->m_p2)
			{
				// заменить вершину
				kkPolygon_replaceVertex(V1, V2, V1PolygonNode->m_element);
				// и добавить полигон в вершину V2
				kkVertex_addPolygon(V2,V1PolygonNode->m_element);
				kkPolygon_calculateNormals(V1PolygonNode->m_element);
				result = true;
			}
			V1PolygonNode = V1PolygonNode->m_right;
		}
		// потом, нужно удалить V1 из полигонов ребра
		if(edge->m_p1)
		{
			kkPolygon_removeVertex(V1,edge->m_p1);
			// если у полигона остаётся 2 вершины то нужно удалить полигон
			if(edge->m_p1->m_vertexCount < 3)
				DeletePolygon(edge->m_p1);
			result = true;
		}
		if(edge->m_p2)
		{
			kkPolygon_removeVertex(V1,edge->m_p2);
			if(edge->m_p2->m_vertexCount < 3)
				DeletePolygon(edge->m_p2);
			result = true;
		}
		_removeVertexFromList(V1);
		kkDestroy(V1);
	}
	else
	{
		// если вершины на краю
		if(isOnEdge(V1) && isOnEdge(V2))
		{
			// беру полигоны вершины V1 и заменяю в нём вершину V1 на вершину V2
			auto P = V1->m_polygons;
			for(u32 i = 0; i < V1->m_polygonCount; ++i)
			{
				kkPolygon_replaceVertex(V1, V2, P->m_element);
				kkVertex_addPolygon(V2,P->m_element); // потом добавить этот полигон в вершину V2
				kkPolygon_calculateNormals(P->m_element);
				result = true;
				P = P->m_right;
			}

			_removeVertexFromList(V1);
			kkDestroy(V1);
		}
	}
	return result;
}

bool PolygonalModel::ClearNewMesh()
{
	bool res = false;
	auto P = m_polygons;
	auto End = P->m_mainPrev;
	u32 count = 0;
	deleteEdges();
	while(true)
	{
		auto next = P->m_mainNext;
		if(P->m_flags & P->EF_OLD)
		{
			P->m_flags ^= P->EF_OLD;
			res = true;
		}
		if(P->m_flags & P->EF_NEW)
		{
			P->m_flags ^= P->EF_NEW;
			res = true;
			DeletePolygon(P);
			++count;
		}

		if(P == End)
			break;

		P = next;
	}
	if(count)
	{
		kkLogWriteInfo("Clear new mesh: [%u] polygons deleted\n", count);
	}
	return res;
}

void PolygonalModel::_createNewMesh()
{
	auto V = m_verts;
	u32 count = 0;
	for(u64 i = 0; i < m_vertsCount; ++i)
	{
		if(V->m_flags & V->EF_CREATENEW)
		{
			// беру полигоны текущей вершины
			auto VP = V->m_polygons;
			for(u64 k = 0; k < V->m_polygonCount; ++k)
			{
				// если ранее я не брал этот полигон то беру
				if(!(VP->m_element->m_flags & VP->m_element->EF_MODIF))
				{
					VP->m_element->m_flags |= VP->m_element->EF_MODIF;
					VP->m_element->m_flags |= VP->m_element->EF_OLD;
					// теперь создаю новый полигон на основе этого

					kkPolygon* new_polygon = kkCreate<kkPolygon>();
					// нужно пройтись по вершинам VP->m_element
					// если вершина не EF_CREATENEW то это место
					// где новый полигон соединяется с главной моделью
					auto PV = VP->m_element->m_verts;
					for(u64 o = 0; o < VP->m_element->m_vertexCount; ++o)
					{
						kkVertex* new_vertex = nullptr;
						if(PV->m_element->m_flags & PV->m_element->EF_CREATENEW)
						{
							VertexHash vh;
							vh.set(&PV->m_element->m_position);

							auto find_result = m_weldMap.find(vh.str);
							if( find_result == m_weldMap.end() )
							{
								new_vertex = kkCreate<kkVertex>();
								new_vertex->m_normal = PV->m_element->m_normal;
								new_vertex->m_position = PV->m_element->m_position;
								new_vertex->m_positionFix = PV->m_element->m_positionFix;
								m_weldMap[vh.str] = new_vertex;
								_addVertexToList(new_vertex);
							}
							else
							{
								new_vertex = find_result->second;
							}
						}
						else
						{
							new_vertex = PV->m_element;
						}
						kkPolygon_addVertex(new_vertex, new_polygon);
						kkVertex_addPolygon(new_vertex, new_polygon);
						PV = PV->m_right;
					}
					new_polygon->m_flags |= new_polygon->EF_NEW;
					_addPolygonToList(new_polygon);
					++count;
				}
				VP = VP->m_right;
			}
		}
		V = V->m_mainNext;
	}
	if(count)
	{
		kkLogWriteInfo("Create new mesh: [%u] polygons created\n", count);
	}
	V = m_verts;
	for(u64 i = 0; i < m_vertsCount; ++i)
	{
		if(V->m_flags & V->EF_CREATENEW)
			V->m_flags ^= V->EF_CREATENEW;
		V = V->m_mainNext;
	}

	auto P = m_polygons;
	for(u64 i = 0; i < m_polygonsCount; ++i)
	{
		if( P->m_flags & P->EF_MODIF )
			P->m_flags ^= P->EF_MODIF;
		P = P->m_mainNext;
	}

	m_weldMap.clear();
}
// создать новые полигоны на основе полигонов выбранных вершин
// новые полигоны должны будут иметь флаг EF_NEW
// старые полигоны должны будут иметь флаг EF_OLD
// после работы над новой сеткой, нужно вызвать ApplyNewMesh
// или удалить вызвав ClearNewMesh
bool PolygonalModel::CreateNewMeshFromSelectedVerts()
{
	if(GetNumOfSelectedVerts() < 2)
		return false;
	auto V = m_verts;
	for(u64 i = 0; i < m_vertsCount; ++i)
	{
		if(V->m_flags & V->EF_SELECTED)
		{
			V->m_flags |= V->EF_CREATENEW;
		}
		V = V->m_mainNext;
	}
	_createNewMesh();
	return true;
}

void PolygonalModel::ApplyNewMesh()
{
}

bool PolygonalModel::WeldSelectedVerts(f32 len)
{
	if( CreateNewMeshFromSelectedVerts() )
	{
		return true;
	}
	return false;
}
u64 PolygonalModel::GetNumOfSelectedVerts()
{
	u64 result = 0;
	auto V = m_verts;
	for(u64 i = 0; i < m_vertsCount; ++i)
	{
		if(V->m_flags & V->EF_SELECTED)
			++result;
		V = V->m_mainNext;
	}
	return result;
}