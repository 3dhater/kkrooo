// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PLUGIN_SDK_H__
#define __KK_PLUGIN_SDK_H__

#define KK_EXPORTS
#define KK_LIBRARY


#include "kkrooo.engine.h"
#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMath.h"
#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Patterns/kkSingleton.h"
#include "FileSystem/kkFileSystem.h"
#include "SceneSystem/kkSceneSystem.h"
#include "MainSystem/kkMainSystem.h"
#include "Geometry/kkGeometryCreator.h"
#include "Geometry/kkPolygonalModel.h"
#include "GraphicsSystem/kkTexture.h"


#include "Scene3D/kkScene3D.h"
#include "Scene3D/kkScene3DObject.h"
#include "Plugins/kkPlugin.h"
#include "Plugins/kkPluginGUI.h"
#include "Plugins/kkPluginCommonInterface.h"
#include "Plugins/kkPluginObjectCategory.h"

#include "Material/kkMaterial.h"
#include "Renderer/kkRenderer.h"

#ifdef KK_PLATFORM_WINDOWS
#pragma comment(lib, "kkrooo.engine.lib")
#endif

#endif