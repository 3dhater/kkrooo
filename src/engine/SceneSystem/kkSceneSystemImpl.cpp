// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "kkSceneSystemImpl.h"
#include "kkCameraImpl.h"

kkSceneSystemImpl::kkSceneSystemImpl()
{
}

kkSceneSystemImpl::~kkSceneSystemImpl()
{
}

kkCamera * kkSceneSystemImpl::createCamera()
{
	kkCameraImpl * w = kkCreate<kkCameraImpl>();
	if( !w )
	{
		KK_PRINT_FAILED;
	}
	return w;
}

kkDummy * kkSceneSystemImpl::createDummy()
{
	kkDummy * w = kkCreate<kkDummy>();
	if( !w )
	{
		KK_PRINT_FAILED;
	}
	return w;
}