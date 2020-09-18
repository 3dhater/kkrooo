#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "kkrooo.engine.h"
#include "../Common.h"
#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMath.h"
#include "SceneSystem/kkCamera.h"
#include "SceneSystem/kkDummy.h"

#include "../Functions.h"
#include "Viewport.h"
#include "ViewportCamera.h"

void ViewportCameraUpdateCallback( kkCamera* c, void * data )
{
	if(!data) return;
	
	ViewportObject* viewport = (ViewportObject*)data;
	auto viewportCamera = viewport->getActiveViewportCamera();

	if(!viewportCamera) return;

	auto vprct = viewport->m_rect_modified;
	auto vpsz  = vprct.getWidthAndHeight();

    kkMatrix4 Pr, Ro, Vi;

    float W = vpsz.x * 0.01f;
    float H = vpsz.y * 0.01f;
	
	Pr[ 0u ] = kkVector4( 2.f / W, 0.f, 0.f, 0.f );
	Pr[ 1u ] = kkVector4( 0.f, 2.f / H, 0.f, 0.f );
	Pr[ 2u ] = kkVector4( 0.f, 0.f, 1.f / (-100.f - 100), 0.f );
	Pr[ 3u ] = kkVector4( 0.f, 0.f, Pr[ 2u ].KK_Z * -100, 1.f );

    auto rot = c->getRotation();

	kkQuaternion qPitch( kkVector4( rot.KK_X, 0.f, 0.f, 1.f ) );
	kkQuaternion qYaw( kkVector4( 0.f, rot.KK_Y, 0.f, 1.f ) );
	kkQuaternion qRoll( kkVector4( 0.f, 0.f, rot.KK_Z, 1.f ) );
		
	kkMatrix4 Roll;
	kkMatrix4 P;
	kkMatrix4 Y;
	math::makeRotationMatrix( Roll, qRoll );
	math::makeRotationMatrix( P, qPitch );
	math::makeRotationMatrix( Y, qYaw );

	Ro = Roll * P * Y;

	auto zoom = viewportCamera->getZoomOrt();

	kkVector4 position;
	switch(viewportCamera->getType() )
	{
	default:
		return;
	case ViewportCameraType::Back:
	case ViewportCameraType::Top:
	case ViewportCameraType::Front:
	case ViewportCameraType::Bottom:
	case ViewportCameraType::Left:
	case ViewportCameraType::Right:
	{
		position = viewportCamera->getPositionBase();
	}break;
	}

	kkMatrix4 translationMatrix;
	translationMatrix[ 3u ].KK_X = -position.KK_X;
	translationMatrix[ 3u ].KK_Y = -position.KK_Y;
	translationMatrix[ 3u ].KK_Z = -position.KK_Z;

	kkMatrix4 scaleMatrix;
	math::makeScaleMatrix( kkVector4(zoom,zoom,zoom,1.f), scaleMatrix );

	Vi = scaleMatrix * Ro * translationMatrix;

    c->setProjectionMatrix(Pr);
    c->setViewMatrix(Vi);
    c->setWorldMatrix(translationMatrix);
}

