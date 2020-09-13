// SPDX-License-Identifier: GPL-3.0-only
#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "Classes/Containers/kkArray.h"

#include "../Plugins/PluginCommonInterface.h"
#include "Geometry/kkPolygon.h"

class Vertex;
class ControlVertex;

// ребро это всего лишь 2 контролирующие вершины.
struct Edge
{
	ControlVertex* m_firstPoint  = nullptr;
	ControlVertex* m_secondPoint = nullptr;
	

	// ребро может быть как на одном полигоне так и на двух
	u64 m_polygonIndex[2] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
	u32 m_index[2] = {0,0}; // порядковый номер в полигоне. для выбора ring

	bool m_isSelected = false;
};
//struct Edge

class Polygon3D : public kkPolygon
{
public:
	Polygon3D();
	virtual ~Polygon3D();

	kkArray<kkVertex*>& GetVerts();
	void MarkToDelete();
	void RemoveMarkToDelete();
	bool IsToDelete();
	bool IsSelected();
	void Select();
	void Deselect();
	void CalculateNormals();
	void Flip();
	kkVector4& GetNormal();
public:

	// необходимо передать эти вершины в модель, попутно заполняя m_verts
	kkArray<kkVertex*> m_verts = kkArray<kkVertex*>(4);
	kkArray<Edge*> m_edges = kkArray<Edge*>(4);

	void      addVertex(Vertex*v);

	// когда удаляются вершины и т.д., то удобно пометить полигон так, чтобы знать что он идёт на удаление
	// значение устанавливается как истина только в методе для удаления
	bool m_toDelete = false;
	bool m_isSelected = false;

	// при редактировании нужно следить за соседями
	std::unordered_set<Polygon3D*> m_neighbors;

	kkVector4 m_facenormal;
};

#endif