﻿// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Vertex.h"

#include "Renderer/kkRenderer.h"
#include "GraphicsSystem/kkTexture.h"

#include "../Functions.h"
#include "../Application.h"
#include "Polygon3D.h"
#include "PolygonalModel.h"

PolygonalModel::PolygonalModel(){}

PolygonalModel::~PolygonalModel()
{
	_deleteEdges();

	for( u64  i = 0; i < m_polygons.size(); ++i )
	{
		kkDestroy( m_polygons[i] );
	}
	m_polygons.clear();

	for( u64  i = 0; i < m_controlPoints.size(); ++i )
	{
		kkDestroy( m_controlPoints[i] );
	}

	for( u64  i = 0; i < m_verts.size(); ++i )
	{
		kkDestroy( m_verts[i] );
	}
}

void PolygonalModel::addPolygon(Polygon3D* p, bool weld, bool triangulate, bool flip)
{
	Vertex * V;

	u64 num_of_tris = p->m_verts.size() - 2;
	
	for( u64 i = 0, sz = p->m_verts.size(); i < sz; ++i )
	{
		V = (Vertex*)p->m_verts[i];
		V->m_weld = weld;
	}

	if( triangulate && num_of_tris > 1 )
	{

		u64 index2;
		u64 index3;

		Polygon3D* new_polygon = nullptr;
		for( u64 i = 0, sz = p->m_verts.size(); i < sz; ++i )
		{
			new_polygon = kkCreate<Polygon3D>();

			index2  = i+2;
			index3  = index2 + 1;
			if( index3 == sz )
				index3 = 0;

			V = kkCreate<Vertex>();
			*V = *(Vertex*)p->m_verts[1];
			V->m_weld = weld;
			new_polygon->addVertex( V );

			V = kkCreate<Vertex>();
			*V = *(Vertex*)p->m_verts[index2];
			V->m_weld = weld;
			new_polygon->addVertex( V );

			V = kkCreate<Vertex>();
			*V = *(Vertex*)p->m_verts[index3];
			V->m_weld = weld;
			new_polygon->addVertex( V );

			if( index3 == 0 )
				break;

			addPolygon(new_polygon, true, false, flip);
		}

		kkDestroy(p);
		p = new_polygon;
	}

	//p->m_vertsInds.clear();
	//p->m_weld = weld;

	// Полигон нужно сохранить в модели
	m_polygons.push_back(p);

	/*s32 free_verts_sz = (s32)m_free_verts.size();
	s32 free_verts_i  = 0;*/

	// передаю вершины полигона в модель
	// и даю полигонам индексы на вершины
	
	for( u64 i = 0, sz = p->m_verts.size() ; i < sz; ++i )
	{
		V = (Vertex*)p->m_verts[ i ];

		if( flip )
		{
			V->m_Normal.KK_X = -V->m_Normal.KK_X;
			V->m_Normal.KK_Y = -V->m_Normal.KK_Y;
			V->m_Normal.KK_Z = -V->m_Normal.KK_Z;
			V->m_Normal_fix = V->m_Normal;
		}

		V->m_parentPolygon = p;

		//if( free_verts_i < free_verts_sz )
		//{
		//	auto ind = m_free_verts[0];
		//	m_free_verts.erase(0);
		//	m_verts[ ind ] = V;
		//	//p->m_vertsInds.push_back( ind );
		//	++free_verts_i;
		//}
		//else
		//{
		//	//p->m_vertsInds.push_back( (u32)m_verts.size() );
		//	m_verts.push_back( V );
		//}
		m_verts.push_back( V );


	}
	if( flip )
	{
		for( u64 i = 0, sz = p->m_verts.size(); i < sz/2; ++i )
		{
			auto v = p->m_verts[ i ];
			auto index = sz - (1+i);
			p->m_verts[ i ] = p->m_verts[ index ];
			p->m_verts[ index ] = v;
		}
	}
}

void PolygonalModel::calculateTriangleCount()
{
	m_triangleCount = 0;
	for( u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		m_triangleCount += (u32)((Polygon3D *)m_polygons[ i ])->m_verts.size() - 2;
	}
}

u64 PolygonalModel::getPolygonCount()const
{
	return m_polygons.size();
}

kkPolygon* PolygonalModel::getPolygon(u64 i)const
{
	return m_polygons[i];
}

bool PolygonalModel::_isNeedToWeld(ControlVertex* cv, Vertex* V, f32 len)
{
	//auto & vertInds = cv->getVertInds(); // получу индексы на m_verts
	//auto firstV = (Vertex*)m_verts[vertInds[0]];  // должно быть достаточно первой вершины
	auto & verts = cv->getVerts();
	auto firstV = (Vertex*)verts[0];  // должно быть достаточно первой вершины
					
	if( V->m_Position.distance(firstV->m_Position) > len ) return false;

	return true;
}
// здесь будет вычисление контрольных точек.
void PolygonalModel::createControlPoints()
{
	for( u64 i = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		kkDestroy( m_controlPoints[ i ] );
	}
	m_controlPoints.clear();

	
	ControlVertexHash h;
	for( u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		Polygon3D * P = (Polygon3D *)m_polygons[ i ];
		P->m_controlVerts.clear();

		// прохожусь по вершинам полигона.
		// смотрю, нужно ли сунуть вершину в уже существующий controlVertex,
		//  или нужно создать новый
		for( u64 k = 0, ksz = P->m_verts.size(); k < ksz; ++k )
		{
			Vertex* V = (Vertex*)P->m_verts[k]; // получение самой вершины

			bool needBetterSolution = false;

			h.set(&V->m_Position);
			ControlVertex * cv = nullptr;

			if( V->m_weld )
			{
				if( m_map.find(h.str) == m_map.end() )
				{
					cv = kkCreate<ControlVertex>();
					m_controlPoints.push_back(cv);
					//cv->m_index = (u32)m_controlPoints.size()-1;
					cv->m_verts.push_back(V);
					m_map[ h.str ] = cv;
				}
				else
				{
					cv = m_map[ h.str ];

					bool create_new_CV = false;
					for( u64 o = 0, osz = cv->m_verts.size(); o < osz; ++o )
					{
						Vertex* vertex = (Vertex*)cv->m_verts[o];
						if( vertex->m_parentPolygon == P )
						{
							create_new_CV = true;
							break;
						}
					}
					if(!create_new_CV)
					{
						for(auto N : P->m_neighbors)
						{
							for( u64 o = 0, osz = cv->m_verts.size(); o < osz; ++o )
							{
								Vertex* vertex = (Vertex*)cv->m_verts[o];
								if( vertex->m_parentPolygon == N )
								{
									create_new_CV = true;
									goto end;
								}
							}
						}
						end:;
					}

					if(create_new_CV)
					{
						cv = kkCreate<ControlVertex>();
						m_controlPoints.push_back(cv);
						//cv->m_index = (u32)m_controlPoints.size()-1;
						cv->m_verts.push_back(V);
					}
					else
					{
						cv->m_verts.push_back(V);
					}
				}
			}
			else
			{
				// если не сваривать вершины друг с другом то это значит просто создаются контрольные вершины
				cv = kkCreate<ControlVertex>();
				m_controlPoints.push_back(cv);
				//cv->m_index = (u32)m_controlPoints.size()-1;
				cv->m_verts.push_back(V);
				m_map[ h.str ] = cv;
			}
			cv->m_faceNormal += P->m_facenormal;
			V->m_controlVertex = cv;
			P->m_controlVerts.push_back(cv);
		}
		P->m_neighbors.clear();
	}
	m_map.clear();

	
	for( u64 i = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		auto CV = (ControlVertex*)m_controlPoints[i];
		CV->m_faceNormal /= CV->m_verts.size();
		CV->m_faceNormal.normalize2();
	}

	for( u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		Polygon3D * P = (Polygon3D *)m_polygons[ i ];
		for( u64 k = 0, ksz = P->m_verts.size(); k < ksz; ++k )
		{
			Vertex* V = (Vertex*)P->m_verts[k];
			P->m_neighbors.insert(V->m_parentPolygon);
		}
	}
	_createEdges();
}

// нужно всего лишь изменить координаты близких друг к другу вершин
void PolygonalModel::weldByLen(f32 len)
{
	for( u64 i = 0, sz = m_verts.size(); i < sz; ++i )
	{
		auto V1 = (Vertex*)m_verts[i];
		for( u64 i2 = 0, sz2 = m_verts.size(); i2 < sz2; ++i2 )
		{
			auto V2 = (Vertex*)m_verts[i2];
			if(V1!=V2)
			{
				if( V1->m_Position.distance(V2->m_Position) <= len )
				{
					V1->m_Position = (V2->m_Position + V1->m_Position) * 0.5f;
					V2->m_Position = V1->m_Position;
				}
			}
		}
	}
}


void PolygonalModel::onEndCreation()
{
	calculateTriangleCount();
}

void PolygonalModel::generateNormals(bool flat)
{
	// flat
	u64       num_of_verts;
	Polygon3D * polygon;
	Vertex* vertex1;
	Vertex* vertex2;
	Vertex* vertex3;

	kkVector4 e1, e2, no;

	for( u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		polygon      = (Polygon3D *)m_polygons[ i ];
		//num_of_verts = (u64)polygon->m_vertsInds.size();
		num_of_verts = (u64)polygon->m_verts.size();
		
		for( u64 i2 = 0, sz2 = num_of_verts; i2 < sz2; ++i2 )
		{
			((Vertex*)polygon->m_verts[ i2 ])->m_Normal.set( 0.f, 0.f, 0.f );
		}

		u64 index2, index3;

		for( u64 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
		{
			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == num_of_verts )
				index3 = 0;

			vertex2 = (Vertex*)polygon->m_verts[ 0 ];
			vertex1 = (Vertex*)polygon->m_verts[ index2 ];
			vertex3 = (Vertex*)polygon->m_verts[ index3 ];

			e1 = vertex2->m_Position - vertex1->m_Position;
			e2 = vertex3->m_Position - vertex1->m_Position;
			//no;
			e1.cross(e2, no);

			vertex1->m_Normal -= no;
			vertex2->m_Normal -= no;
			vertex3->m_Normal -= no;
		}

		for( u64 i2 = 0, sz2 = num_of_verts; i2 < sz2; ++i2 )
		{
			((Vertex*)polygon->m_verts[ i2 ])->m_Normal.normalize2();
		}
		polygon->m_facenormal = ((Vertex*)polygon->m_verts[ 0 ])->m_Normal;
	}


	if( flat )
		return;

	// smooth
	ControlVertex* CV;
	u64 sz2;
	for( u64 i = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		CV = (ControlVertex*)m_controlPoints[ i ];
		
		sz2 = CV->m_verts.size();

		kkVector4 N;

		for( u64 i2 = 0; i2 < sz2; ++i2 )
		{
			//auto V_id       = CV->m_vertexIndex[i2];
			//auto vertex     = (Vertex*)m_verts[ V_id ];
			auto vertex     = (Vertex*)CV->m_verts[i2];
		
			N += vertex->m_Normal;
		}

		N = N / (float)sz2;
		N.normalize2();

		for( u64 i2 = 0; i2 < sz2; ++i2 )
		{
			auto vertex     = (Vertex*)CV->m_verts[i2];
			vertex->m_Normal = N;
		}
	}
}

void PolygonalModel::deleteMarkedPolygons()
{
	// удалить полигоны
	u64 numOfPolys = 0;
	for( u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		Polygon3D * P = (Polygon3D *)m_polygons[i];
		if(P->IsToDelete())
		{
			for( u64 i = 0, sz = P->m_verts.size(); i < sz; ++i )
			{
				((Vertex*)P->m_verts[i])->m_isToDelete = true;
			}
			kkDestroy(P);
			m_polygons[i] = nullptr;
		}
		else
		{
			++numOfPolys;
		}
	}

	// обновить массив с полигонами
	bool start = false;
	for( u64 i = 0, A = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		auto P = m_polygons[i];
		if(!P)
		{
			if(!start)
			{
				start = true;
				A = i;
			}
		}
		else
		{
			if(start)
			{
				m_polygons[A] = P;
				++A;
			}
		}
	}
	m_polygons.setSize(numOfPolys);

	// удалить ненужные вершины и обновить массив с вершинами
	u64 numOfVerts = 0;
	start = false;
	for( u64 i = 0, A = 0, sz = m_verts.size(); i < sz; ++i )
	{
		auto V = (Vertex*)m_verts[i];
		if(V->m_isToDelete)
		{
			if(!start)
			{
				start = true;
				A = i;
			}

			// обновление контрольных вершин
			if(V->m_controlVertex->m_verts.size() == 1)
			{
				V->m_controlVertex->m_toDelete = true;
			}
			V->m_controlVertex->m_verts.erase_first(V);
			
			kkDestroy(V);
		}
		else
		{
			++numOfVerts;
			if(start)
			{
				m_verts[A] = V;
				++A;
			}
		}
	}
	m_verts.setSize(numOfVerts);

	// обновление контрольных вершин
	start = false;
	u64 numOfCVerts = 0;
	for( u64 i = 0, A = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		auto CV = (ControlVertex*)m_controlPoints[i];
		if(CV->m_toDelete)
		{
			if(!start)
			{
				start = true;
				A = i;
			}

			kkDestroy(CV);
		}
		else
		{
			++numOfCVerts;
			if(start)
			{
				m_controlPoints[A] = CV;
				++A;
			}
		}
	}
	m_controlPoints.setSize(numOfCVerts);
	this->_createEdges();
	calculateTriangleCount();
}

void PolygonalModel::_createGridAccel(kkRenderInfo* ri)
{
	// нужно вычислить количество линий
	s32 cell_size = 15;  // размер ячейки 10x10
	m_gridRadius = v2f(0.f,0.f).distance(v2f((f32)cell_size * 0.5f));
	
	s32 rows = ri->image->m_height / cell_size;
	if( ri->image->m_height % cell_size )
		++rows;
	s32 cols = ri->image->m_width / cell_size;
	if( ri->image->m_width % cell_size )
		++cols;
	m_gridAccelRows.reserve(rows);
	s32 _x = 0;
	s32 _y = 0;
	for( s32 i = 0; i < rows; ++i )
	{
		auto row = new GridAcceleratorRow;
		m_gridAccelRows.push_back(row);
		row->m_cells.reserve(cols);
		for( s32 j = 0; j < cols; ++j )
		{
			auto cell = new GridAcceleratorCell;
			cell->left_top.x     = (f32)_x;
			cell->left_top.y     = (f32)_y;
			cell->right_bottom.x = (f32)_x + (f32)cell_size;
			cell->right_bottom.y = (f32)_y + (f32)cell_size;

			cell->center.x = cell->left_top.x + ((cell->right_bottom.x - cell->left_top.x)*0.5f);
			cell->center.y = cell->left_top.y + ((cell->right_bottom.y - cell->left_top.y)*0.5f);

			row->m_cells.push_back(cell);

			_x += cell_size;
		}
		_y += cell_size;
		_x = 0;
	}
}

struct AABB2D
{
	v2f m_min = v2f(FLT_MAX, FLT_MAX);
	v2f m_max = v2f(-FLT_MAX, -FLT_MAX);

	void add(const v2f & point )
	{
		if( point.x < m_min.x ) m_min.x = point.x;
		if( point.y < m_min.y ) m_min.y = point.y;

		if( point.x > m_max.x ) m_max.x = point.x;
		if( point.y > m_max.y ) m_max.y = point.y;
	}

	void center( v2f& out )
	{
		out.x = (m_min.x + m_max.x) * 0.5f;
		out.y = (m_min.y + m_max.y) * 0.5f;
	}
};
void PolygonalModel::_addTriangleToGrid(kkRenderInfo* ri, kkTriangle* tri, u32 triIndex)
{
	v2i P1, P2, P3;
	auto viewport = v2f((f32)ri->image->m_width, (f32)ri->image->m_height);
	auto offset   = v2f(0.f,0.f);
	auto v1 = tri->v1;
	auto v2 = tri->v2;
	auto v3 = tri->v3;
	P1 = kkrooo::worldToScreen( ri->VP, v1, viewport, offset );
	P2 = kkrooo::worldToScreen( ri->VP, v2, viewport, offset );
	P3 = kkrooo::worldToScreen( ri->VP, v3, viewport, offset );
	
	v2f p1(P1.x,P1.y);
	v2f p2(P2.x,P2.y);
	v2f p3(P3.x,P3.y);

	/*v2f triangle_center = p1 + p2 + p3;
	triangle_center.x *= 0.3333333f;
	triangle_center.y *= 0.3333333f;*/

	AABB2D tri_aabb;
	tri_aabb.add(p1);
	tri_aabb.add(p2);
	tri_aabb.add(p3);

	v2f aabb_center;
	tri_aabb.center(aabb_center);

	f32 tri_radius = aabb_center.distance(tri_aabb.m_max);
	f32 tri_grid_len = tri_radius + m_gridRadius;

	//printf("%i %i : %i %i : [%f %f]\n", P1.x, P1.y, P2.x, P2.y, viewport.x, viewport.y);
	// сначала надо найти нужную линию, потом нужный столбик
	bool lineFound = false;
	for( u64 i = 0, sz = m_gridAccelRows.size(); i < sz; ++i )
	{

		bool currentLineFound = false;

		auto row = m_gridAccelRows[i];
		auto first_cell = row->m_cells[0];
		
		bool cellFound = false;
		for( u64 j = 0, jsz = row->m_cells.size(); j < jsz; ++j )
		{
			auto cell = row->m_cells[j];
			
			f32 len = cell->center.distance(aabb_center);
			if( len <= tri_grid_len )
			{
				cellFound = true;
				goto add;
			}
			if( cellFound )
			{
				break;
			}
			continue;
			add:
			cell->m_inds.push_back(triIndex);
			cellFound = true;
			lineFound = true;
			currentLineFound = true;
		}
		
		if( lineFound && !currentLineFound )
		{
			break;
		}
	}
}

void PolygonalModel::prepareForRaytracing(const kkMatrix4& matrix, const kkVector4& pivot, kkRenderInfo* ri)
{
	kkCameraFrustum frust;
	frust.calculateFrustum(ri->P, ri->V);

	auto ms = kkGetMainSystem();
	auto t = ms->getTime();

	if( m_isPreparedForRaytracing )
	{
		printf("WARNING! `prepareForRaytracing` called more than once! Now programm will call finishRaytracing...");
		finishRaytracing();
	}
	
	calculateTriangleCount();

	_createGridAccel(ri);

	//printf("m_triangleCount %u\n",m_triangleCount);
	m_trianglesForRendering.clear();
	m_trianglesForRendering.reserve(m_triangleCount);
	Polygon3D * polygon;
	Vertex*     vertex1;
	Vertex*     vertex2;
	Vertex*     vertex3;
	u32       num_of_verts;

	u32 freeIndsCount = 0;
	for(u64 polygonI = 0, polygon_sz = m_polygons.size(); polygonI < polygon_sz; ++polygonI )
	{
		polygon = (Polygon3D *)m_polygons.at(polygonI);
		num_of_verts = (u32)polygon->m_verts.size();
		u32 index2, index3;
		for( u32 i2 = 0, sz2 = num_of_verts - 2; i2 < sz2; ++i2 )
		{
			index2  = i2+1;
			index3  = index2 + 1;
			if( index3 == num_of_verts )
				index3 = 0;
			vertex2 = (Vertex*)polygon->m_verts[ 0 ];
			vertex1 = (Vertex*)polygon->m_verts[ index2 ];
			vertex3 = (Vertex*)polygon->m_verts[ index3 ];
			
			kkVector4 e1( vertex2->m_Position._f32[0] - vertex1->m_Position._f32[0],
				vertex2->m_Position._f32[1] - vertex1->m_Position._f32[1],
				vertex2->m_Position._f32[2] - vertex1->m_Position._f32[2],
				vertex2->m_Position._f32[3] - vertex1->m_Position._f32[3]);
			
			kkVector4 e2( vertex3->m_Position._f32[0] - vertex1->m_Position._f32[0],
				vertex3->m_Position._f32[1] - vertex1->m_Position._f32[1],
				vertex3->m_Position._f32[2] - vertex1->m_Position._f32[2],
				vertex3->m_Position._f32[3] - vertex1->m_Position._f32[3]);

			kkTriangleRayTestResult rtr;
			rtr.triangle.v1 = math::mul( vertex1->m_Position, matrix) + pivot;
			rtr.triangle.v2 = math::mul( vertex2->m_Position, matrix) + pivot;
			rtr.triangle.v3 = math::mul( vertex3->m_Position, matrix) + pivot;
			rtr.triangle.update();
			/*rtr.triangle.e1 = e1;
			rtr.triangle.e2 = e2;*/

			rtr.triangle.normal1 = vertex1->m_Normal;
			rtr.triangle.normal2 = vertex2->m_Normal;
			rtr.triangle.normal3 = vertex3->m_Normal;
			rtr.triangle.t1 = vertex1->m_UV;
			rtr.triangle.t2 = vertex2->m_UV;
			rtr.triangle.t3 = vertex3->m_UV;

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

			if( frust.sphereInFrustum(taabb.radius(triCnter), triCnter) )
			{
				m_trianglesForRendering.push_back(rtr);
				_addTriangleToGrid(ri, &rtr.triangle, freeIndsCount);
				
				m_BVH_root.m_inds.push_back(freeIndsCount++);
				m_BVH_root.m_aabb.add(rtr.triangle.v1);
				m_BVH_root.m_aabb.add(rtr.triangle.v2);
				m_BVH_root.m_aabb.add(rtr.triangle.v3);
			}
		}
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
	//printf("SPLIT TIME: [%llu]\n",ms->getTime()-t);
}

void PolygonalModel::_deleteGridAccel()
{
	for( u64 i = 0, sz = m_gridAccelRows.size(); i < sz; ++i )
	{
		delete m_gridAccelRows[i];
	}
	m_gridAccelRows.clear();
}

void PolygonalModel::finishRaytracing()
{
	if( m_isPreparedForRaytracing )
	{
		_deleteGridAccel();
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
		//triangle->object = 
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

void PolygonalModel::rayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial )
{
	// надо найти нужную ячейку по данным координатам
	for( u64 i_r = 0, rsz = m_gridAccelRows.size(); i_r < rsz; ++i_r )
	{
		auto row = m_gridAccelRows[ i_r ];
		auto first_cell = row->m_cells[0];
		if( point.y >= first_cell->left_top.y && point.y <= first_cell->right_bottom.y ) // найти нужную линию
		{
			for( u64 i_c = 0, csz = row->m_cells.size(); i_c < csz; ++i_c )
			{
				auto cell = row->m_cells[i_c];
				if( point.x >= cell->left_top.x && point.x <= cell->right_bottom.x )
				{
					for( u64 i_t = 0, tsz = cell->m_inds.size(); i_t < tsz; ++i_t )
					{
						auto  triangle = &m_trianglesForRendering[ cell->m_inds.at(i_t) ];
						_rayTestTriangle(outTriangles, ray, renderObjectMaterial, triangle);
					}

					return;
				}
			}
		}
	}
}

void PolygonalModel::addModel(PolygonalModel* other, const kkMatrix4& invertMatrix, const kkMatrix4& matrix_other, const kkVector4& pivot, const kkVector4& pivot_other)
{
	auto TIM = matrix_other;
	TIM.invert();
	TIM.transpose();

	auto old_size = m_verts.size();

	m_verts.reserve(old_size + other->m_verts.size());
	for( u64 i = 0, sz = other->m_verts.size(); i < sz; ++i )
	{
		m_verts.push_back(other->m_verts[i]);
	}
	
	for( auto P : other->m_polygons )
	{
		auto polygon = (Polygon3D*)P;
		for( size_t i = 0, sz = polygon->m_verts.size(); i < sz; ++i )
		{
			auto V = (Vertex*)polygon->m_verts[i];
			
			//polygon->m_vertsInds[i] += old_size;

			V->m_Normal		= math::mul(V->m_Normal_fix, TIM);
			V->m_Normal_fix = V->m_Normal;

			V->m_Position	= math::mul(V->m_Position, matrix_other)+ pivot_other - pivot;
			V->m_Position	= math::mul(V->m_Position, invertMatrix) ;
			V->m_Position_fix = V->m_Position;
		}
		m_polygons.push_back(polygon);
	}
	other->m_verts.clear();
	other->m_polygons.clear();
}

void PolygonalModel::_deleteEdges()
{
	for( size_t i = 0, sz = m_edges.size(); i < sz; ++i )
	{
		delete m_edges[i];
	}
	m_edges.clear();
}

void PolygonalModel::_createEdges()
{
	_deleteEdges();

	for( size_t i = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		ControlVertex* cv = (ControlVertex*)m_controlPoints[ i ];
		cv->m_edges.clear();
	}
	
	std::unordered_map<u64, u64> map;     // 2 объединённых адреса как ключ и индекс на сам массив хранящий Edge

	for(u64 i = 0, sz = m_polygons.size(); i < sz; ++i )
	{
		auto polygon = (Polygon3D *)m_polygons.at(i);

		// беру полигон. прохожусь по вершинам. беру контрольные точки. текущую и следующую. это должно быть ребром
		for( u64 o = 0, sz2 = polygon->m_controlVerts.size(); o < sz2; ++o )
		{
			u64 o2 = o + 1;
			if(o2 == sz2) o2=0;
			auto cv1 = (ControlVertex*)polygon->m_controlVerts[o];
			auto cv2 = (ControlVertex*)polygon->m_controlVerts[o2];
			
			// нужно определить контрольную вершину с адресом, значение которого меньше чем у другой вершины
			ControlVertex* cv_first  = cv1;
			ControlVertex* cv_second = cv2;
			if( cv2 < cv1 )
			{
				cv_first   = cv2;
				cv_second  = cv1;
			}

			// текущее ребро
			Edge * edge = new Edge;
			edge->m_firstPoint  = cv_first;
			edge->m_secondPoint = cv_second;

			

			// надо найти, было ли данное ребро добавлено ранее
			// в качестве уникального ключа берутся 4 байта с одного адреса и 4 байта с другого
			u64 key_val = (u64)cv_first;
			key_val <<= 32;
			key_val |= ((u64)cv_second & 0x00000000FFFFFFFF);
			
			auto search = map.find(key_val);
			if( search != map.end() )
			{ // ребро было добавлено ранее
				// значит нужно этому ребру дать индекс полигона....
				// индекс полигона нужен чтобы потом найти нужное ребро, так как выбор основан на взятии треугольника лучем.
				auto E = m_edges[ search->second ];
				if( E->m_polygonIndex[0] == 0xFFFFFFFFFFFFFFFF )
					E->m_polygonIndex[0] = i;
				else
					E->m_polygonIndex[1] = i;

				delete edge;
				edge = E;
			}
			else
			{ // не найдено, значит надо добавить в массив и в map
				if( edge->m_polygonIndex[0] == 0xFFFFFFFFFFFFFFFF )
					edge->m_polygonIndex[0] = i;
				else
					edge->m_polygonIndex[1] = i;

				m_edges.push_back(edge);
				map[key_val] = m_edges.size()-1;

				cv_first->m_edges.emplace_back(edge);
				cv_second->m_edges.emplace_back(edge);
			}
			
			for(u64 k = 0, ksz = cv_first->m_verts.size(); k < ksz; ++k )
			{
				Vertex* V = (Vertex*)cv_first->m_verts[k];
				V->m_edge = edge;
			}
			for(u64 k = 0, ksz = cv_second->m_verts.size(); k < ksz; ++k )
			{
				Vertex* V = (Vertex*)cv_second->m_verts[k];
				V->m_edge = edge;
			}

		}
	}

	// если ребро имеет только 1 полигон на стороне
	for( size_t i = 0, sz = m_edges.size(); i < sz; ++i )
	{
		auto E = m_edges[i];
		if(E->m_polygonIndex[1] == 0xFFFFFFFFFFFFFFFF)
		{
			E->m_firstPoint->m_onEdge = true;
			E->m_secondPoint->m_onEdge = true;
		}
	}
}

bool PolygonalModel::deleteSelectedVerts()
{
	bool result = false;
	for( size_t i = 0, sz = m_controlPoints.size(); i < sz; ++i )
	{
		auto cv = (ControlVertex*)m_controlPoints[ i ];
		if( cv->m_isSelected )
		{
			for( size_t i2 = 0, sz2 = cv->m_verts.size(); i2 < sz2; ++i2 )
			{
				auto vertex = (Vertex*)cv->m_verts[i2];

				vertex->m_parentPolygon->MarkToDelete();
				result = true;
			}
		}
	}
	if(result)
	{
		deleteMarkedPolygons();
	}
	return result;
}
