// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_3D_SCENE_H__
#define __KK_3D_SCENE_H__

#include "Common/kkForward.h"

enum class kkScene3DObjectType : u32
{
	PolygonObject
};

class kkScene3D
{
public:
	kkScene3D(){}
	virtual ~kkScene3D(){}

	virtual u32 getNumOfObjectsOnScene() = 0;
	virtual u32 getNumOfSelectedObjects() = 0;
	virtual kkScene3DObject* getObjectOnScene( u32 ) = 0;
	virtual kkScene3DObject* getSelectedObject( u32 ) = 0;

	virtual void deleteObject( kkScene3DObject* ) = 0;
	
	virtual void selectAll() = 0;
	virtual void deselectAll() = 0;

	virtual void selectObject(kkScene3DObject*) = 0;
	virtual void deselectObject(kkScene3DObject*) = 0;

	//virtual void beginTMPObject( kkScene3DObjectType ) = 0;
	//virtual void endTMPObject() = 0;
};

#endif