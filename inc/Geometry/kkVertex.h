// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKVERTEX_H__
#define __KKVERTEX_H__

#include "Classes/Math/kkVector4.h"
#include "Plugins/kkPluginCommonInterface.h"

class kkControlVertex
{
public:
	kkControlVertex(){}
	virtual ~kkControlVertex(){}

	// �������� ������� �� ������� �� ������� ������� ����� �������� ����� kkScene3DObject::getVertexArray
	// kkControlVertex ��������� ����� ���������
	//virtual kkArray<u32>& getVertInds() = 0;
	virtual kkArray<kkVertex*>& getVerts() = 0;

	virtual bool isSelected() = 0;
	virtual bool isSelectedEdge() = 0;
	virtual bool isSelectedPoly() = 0;
	virtual void select() = 0;
	virtual void deselect() = 0;

	// ����� ������ (� ������� kkScene3DObject::getControlVertexArray) � ������� ��������� ������ �������
	virtual u32 getIndex() = 0;
};

class kkVertex
{
public:
	kkVertex(){}
	virtual ~kkVertex(){}

	virtual kkVector4 getPosition() = 0;
	virtual kkVector4 getUV() = 0;
	virtual kkVector4 getNormal() = 0;
	//virtual kkVector4 getBinormal() = 0;
	//virtual kkVector4 getTangent() = 0;
	virtual kkVector4 getBoneInds() = 0;
	virtual kkVector4 getWeights() = 0;
};
	

#endif