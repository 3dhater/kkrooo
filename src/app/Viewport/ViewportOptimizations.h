// SPDX-License-Identifier: GPL-3.0-only
#ifndef __VIEWPORT_OPT__H__
#define __VIEWPORT_OPT__H__

class kkObb;
struct kkCameraFrustum;
class Scene3DObject;

class kkVector4;

//#include "Classes/Containers/kkArray.h"

bool OBBInFrustum( const kkObb & obb, kkCameraFrustum* frustum );
void sortObjectsInFrustum( std::basic_string<Scene3DObject*>& sortedObjects );
void sortMouseHoverObjects( std::basic_string<Scene3DObject*>& sortedObjects );


#endif