// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_CAMERA_IMPL_H__
#define __KK_CAMERA_IMPL_H__

#include "SceneSystem/kkCamera.h"

class kkCameraImpl : public kkCamera
{

public:
	kkCameraImpl();
	~kkCameraImpl();
	
	void update();
};


#endif