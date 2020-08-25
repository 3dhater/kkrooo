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
	//u64 m_firstPolygonIndex  = 0;
	//u64 m_secondPolygonIndex = -1;
	u64 m_polygonIndex[2] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
};
//struct Edge

class Polygon3D : public kkPolygon
{
public:
	Polygon3D();
	virtual ~Polygon3D();

	//kkVertex* getVertex( u32 );
	//u32     getVertexCount();

	kkArray<u32>& GetVertInds();
	kkArray<u32>& GetControlVertInds();
	void MarkToDelete();
	void RemoveMarkToDelete();
	bool IsToDelete();
	bool IsSelected();
	void Select();
	void Deselect();

public:

	///// по этим указателям нужно будет освобождать память
	// теперь этот массив нужен только лишь для создания модели
	// необходимо передать эти вершины в модель, попутно заполняя m_verts
	kkArray<Vertex*> m_verts = kkArray<Vertex*>(4);


	kkArray<u32> m_vertsInds  = kkArray<u32>(4);
	kkArray<u32> m_controlVertsInds  = kkArray<u32>(4);

	void      addVertex(Vertex*v);

	// когда удаляются вершины и т.д., то удобно пометить полигон так, чтобы знать что он идёт на удаление
	// значение устанавливается как истина только в методе для удаления
	bool m_toDelete = false;
	bool m_isSelected = false;

	// используется при создании модели
	//bool m_weld = false;

};

#endif