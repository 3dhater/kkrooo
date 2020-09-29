#include "kkrooo.engine.h"
#include "../Common.h"

#include "ViewportOptimizations.h"

#include "Classes/Math/kkVector4.h"
#include "Classes/Containers/kkArray.h"
#include "SceneSystem/kkCamera.h"

#include "../Scene3D/Scene3DObject.h"

//f32 planeDotCoord( const v4f& coord, const v4f& plane )
//{
//	return f32( plane.x * coord.x + plane.y * coord.y + plane.z * coord.z + plane.w );
//}

bool PolygonInFrustum( int numpoints, v4f* pointlist, kkCameraFrustum * frustum )
{
   int f, p;

   for( f = 0; f < 6; f++ )
   {
      for( p = 0; p < numpoints; p++ )
      {
         if( frustum->m_planes[f].KK_X * pointlist[p].x + 
			 frustum->m_planes[f].KK_Y * pointlist[p].y + 
			 frustum->m_planes[f].KK_Z * pointlist[p].z + 
			 frustum->m_planes[f].KK_W > 0 )
            break;
      }
      if( p == numpoints )
         return false;
   }
   return true;
}

//bool CheckRectangle( kkCameraFrustum * frustum, float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
//{
//	int i;
//
//	for(i=0; i<6; i++)
//	{
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		if(planeDotCoord(frustum->m_planes[i], v4f((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
//		{
//			continue;
//		}
//
//		return false;
//	}
//
//	return true;
//}


bool OBBInFrustum( const kkObb & obb, kkCameraFrustum* frustum )
{ 
	static v4f polygon[] = 
	{
		v4f(),
		v4f(),
		v4f(),
		v4f()
	};

	polygon[ 0 ] = obb.v1;
	polygon[ 1 ] = obb.v5;
	polygon[ 2 ] = obb.v8;
	polygon[ 3 ] = obb.v4;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	polygon[ 0 ] = obb.v1;
	polygon[ 1 ] = obb.v3;
	polygon[ 2 ] = obb.v6;
	polygon[ 3 ] = obb.v5;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	polygon[ 0 ] = obb.v4;
	polygon[ 1 ] = obb.v8;
	polygon[ 2 ] = obb.v2;
	polygon[ 3 ] = obb.v7;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	polygon[ 0 ] = obb.v5;
	polygon[ 1 ] = obb.v6;
	polygon[ 2 ] = obb.v2;
	polygon[ 3 ] = obb.v8;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	polygon[ 0 ] = obb.v1;
	polygon[ 1 ] = obb.v4;
	polygon[ 2 ] = obb.v7;
	polygon[ 3 ] = obb.v3;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	polygon[ 0 ] = obb.v7;
	polygon[ 1 ] = obb.v2;
	polygon[ 2 ] = obb.v6;
	polygon[ 3 ] = obb.v3;

	if( PolygonInFrustum( 4, polygon, frustum ) )
		return true;

	return false;
}

//bool AABBInFrustum( const kkAabb & aabb, kkCamera* camera )
//{ 
//	auto frustum = camera->getFrustum();
//	CheckRectangle()
//}

bool sphereInFrustum( kkCameraFrustum * frustum, f32 radius, const v4f& position )
{
	for( u32 i = 0u; i < 6u; ++i )
	{
		if( ( frustum->m_planes[ i ].KK_X * position.x + frustum->m_planes[ i ].KK_Y * position.y + frustum->m_planes[ i ].KK_Z * position.z
			+ frustum->m_planes[ i ].KK_W ) <= -radius)
		{
			return false;
		}
	}
	return true;
}

void sortObjectsInFrustum( std::basic_string<Scene3DObject*>& sortedObjects )
{
	std::sort(sortedObjects.begin(),sortedObjects.end(),
		[](Scene3DObject* first, Scene3DObject* second)
		{
			return first->GetDistanceToCamera() > second->GetDistanceToCamera();
		}
	);
}

void sortMouseHoverObjects( std::basic_string<Scene3DObject*>& sortedObjects )
{
	std::sort(sortedObjects.begin(),sortedObjects.end(),
		[](Scene3DObject* first, Scene3DObject* second)
		{
			return first->GetDistanceToCamera() > second->GetDistanceToCamera();
		}
	);
}