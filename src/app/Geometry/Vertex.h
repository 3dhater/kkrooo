// SPDX-License-Identifier: GPL-3.0-only
#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <utility>

#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Geometry/kkVertex.h"
#include "../Plugins/PluginCommonInterface.h"

class Polygon3D;
class ControlVertex;

class Vertex : public kkVertex
{
public:
	Vertex();
	virtual ~Vertex();

	kkVector4    getPosition();
	kkVector4    getUV();
	kkVector4    getNormal();
	//kkVector4    getBinormal();
	//kkVector4    getTangent();
	kkVector4    getBoneInds();
	kkVector4    getWeights();

	Polygon3D *     m_parentPolygon = nullptr; // для быстрого доступа к нужному полигону

	//ControlVertex*  m_parentControlVertex = nullptr; //родительский ControlVertex
	//bool            m_isControl = false; // если это ControlVertex то истина

	// при удалении вершин, разумно не урезать массив с вершинами, а просто поставить маркер что данная вершина более не нужна
	// это будет удобно при редактировании модели, не нужно пересчитывать индексы для полигонов.
	//   стоит лишь вопрос в том, когда урезать этот массив(со всеми пересчётами), возможно при сохранении...
	//   но пока будет так...
	// При удалении так-же нужно будет добавить вершину в полигональную_модель->m_free_verts
	bool m_isFree = false;


	// при создании software модели нужно запомнить индекс вершины (в software буфере)
	//   чтобы потом иметь возможно быстро найти нужную вершину
	// значение устанавливается при перестройке модели
	std::vector<std::pair<u32,u32>> m_vertexIndexForSoftware      ; // вершина полигона может создавать множество hardware точек
	                                             // перед перестройкой модели нужно очищать список
	                                             // индекс в мешбуфере, индекс софтваре модели
	std::vector<std::pair<u32,u32>> m_vertexIndexForSoftware_lines;
	
	// индекс части модели, которая строит себя из этой вершины
	//kkArray<u32> m_softwareModelIndex = 0;
	//u32  m_SoftwareModels_lines_ID = 0;

public:

	//при добавлении полей нужно будет дополнить копирование внутри PolygonalModel::addModel(PolygonalModel* other)
	kkVector4    m_Position;
	kkVector4    m_Position_fix;
	kkVector4    m_UV;
	kkVector4    m_Normal;
	kkVector4    m_Normal_fix;
	//kkVector4    m_Binormal;
	kkVector4    m_Color;
	//kkVector4    m_Tangent;
	kkVector4    m_Boneinds;
	kkVector4    m_Weights;

	// каждая вершина может быть частью контрольной вершины, или она сама может быть контрольной вершиной...
	// контрольная вершина может быть разбита на несколько... значит нужно уметь не соединять их когда они имеют равную позицию
	bool m_weld = false;
};


struct Edge;
class ControlVertex : public kkControlVertex
{
public:
	ControlVertex();
	~ControlVertex();


	// индексы тех вершин, которые будут подчинены этой контрольной вершине
	// должна быть минимум 1 вершина
	kkArray<u32> m_vertexIndex  = kkArray<u32>(4);

	std::pair<u32,u32> m_vertexIndexForSoftware_points;


	u32 m_index = 0;

	bool m_isSelected = false;
	bool m_isSelected_edge = false;
	bool m_isSelected_poly =false;

	// образует edge с такими-то ControlVertex
	std::vector<ControlVertex*> m_edgeWith; // когда выделяется ребро, то обоим вершинам нужно указать друг друга
	
	// created in Scene3DObject::createEdges
	std::vector<Edge*> m_edges;

	// когда выбран полигон, каждой контрольной вершине нужно добавить информацию об этом полигоне
	//std::vector<u64> m_selectedPolys;
	u32 m_selectedPolysCounter = 0; // возможно это-же можно сделать и с рёбрами
	
	f32 m_distanceToCamera = 0.f;

	// когда удаляются вершины будет полезно знать что конкретно идёт на удаление
	// значение устанавливается как истина только в методе для удаления
	bool m_toDelete = false;

	kkArray<u32>& getVertInds();
	bool isSelected();
	bool isSelectedEdge();
	bool isSelectedPoly();
	void select();
	void deselect();
	u32 getIndex();
};


//struct ControlVertexCompare
//{
//	bool operator() (const ControlVertex& lhs, const ControlVertex& rhs) const
//	{
//		return lhs.m_index < rhs.m_index;
//	}
//};

#endif