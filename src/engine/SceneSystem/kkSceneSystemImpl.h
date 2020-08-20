// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_SCENE_SYSTEM_H__
#define __KK_SCENE_SYSTEM_H__

#include "SceneSystem/kkSceneSystem.h"

class kkSceneSystemImpl : public kkSceneSystem
{
public:
	kkSceneSystemImpl();
	~kkSceneSystemImpl();
	
	kkCamera * createCamera();
	kkDummy * createDummy();
	
};


#endif