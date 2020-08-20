// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO__SCENE_SYSTEM_H__
#define __KKROO__SCENE_SYSTEM_H__

#include "kkCamera.h"
#include "kkDummy.h"


class kkSceneSystem
{
public:

	virtual kkCamera * createCamera() = 0;
	virtual kkDummy *  createDummy() = 0;
	
};

#endif