#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "kkrooo.engine.h"
#include "../Common.h"
#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMath.h"
#include "Classes/kkAABB.h"
#include "SceneSystem/kkCamera.h"
#include "SceneSystem/kkDummy.h"

#include "../Scene3D/Scene3DObject.h"
#include "ViewportCamera.h"
#include "Viewport.h"

#include "GraphicsSystem/kkGraphicsSystem.h"

void ViewportCameraUpdateCallback( kkCamera* c, void* viewport);

ViewportCamera::ViewportCamera(ViewportCameraType type,void * owner)
{
    m_type = type;
    m_owner = owner;
    for( s32 i = 0; i < _Camera::_number; ++i )
    {
        m_camera_parts[i] = nullptr;
    }
}

ViewportCamera::~ViewportCamera()
{
    _destroy();
}

void ViewportCamera::init()
{
    switch (m_type)
    {
    case ViewportCameraType::Perspective:
        m_fov = math::degToRad(60.f);
        break;
    case ViewportCameraType::Front:
    case ViewportCameraType::Back:
    case ViewportCameraType::Left:
    case ViewportCameraType::Right:
    case ViewportCameraType::Top:
    case ViewportCameraType::Bottom:
    default:
        m_fov = 0.1f;
        break;
    }
    reset();
}

void ViewportCamera::_destroy()
{
    for( s32 i = 0; i < _Camera::_number; ++i )
    {
        if( m_camera_parts[i] )
        {
            m_camera_parts[i]->removeAll();
            kkDestroy(m_camera_parts[i]);
            m_camera_parts[i] = nullptr;
        }
    }

    if( m_kk_camera )
    {
        kkDestroy(m_kk_camera);
        m_kk_camera = nullptr;
    }

    /*if( m_kk_axisCamera )
    {
        kkDestroy(m_kk_axisCamera);
        m_kk_axisCamera = nullptr;
    }*/
}


void ViewportCamera::reset()
{
    _destroy();
    for( s32 i = 0; i < _Camera::_number; ++i )
    {
        m_camera_parts[i] = kkCreateDummy();
    }

    m_all_rotation.set(0.f,0.f,0.f,0.f);

    const f32 start_camera_h = 15.f;
    const f32 start_camera_x = -45.f * 0.5f; // умножение * 0.5f стоит чтобы поставить на место, так как вращение ускорено в 2х
    const f32 start_camera_y = 0.f;

    m_kk_camera = kkCreateCamera();
    m_kk_camera->setNear(0.1f);
    m_kk_camera->setFar(5000.f);
    m_kk_camera->setFOV(m_fov);

    
    /*m_kk_axisCamera = kkCreateCamera();
    m_kk_axisCamera->setNear(0.1f);
    m_kk_axisCamera->setFar(10.f);
    m_kk_axisCamera->setFOV((math::degToRad(50.f)));*/

    m_kk_camera->setRotation(kkVector4(math::degToRad(-90),0.f,0.f));
    //m_kk_axisCamera->setRotation(kkVector4(math::degToRad(-90),0.f,0.f));


    m_camera_parts[_Camera::RotY_]->setParent( m_camera_parts[_Camera::Base_] );
    m_camera_parts[_Camera::RotX_]->setParent( m_camera_parts[_Camera::RotY_] );
    m_camera_parts[_Camera::ObjectRotation]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::BaseMoveYP]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::BaseMoveYN]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::BaseMoveXP]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::BaseMoveXN]->setParent( m_camera_parts[_Camera::RotX_] );

    m_camera_parts[_Camera::BaseMoveYP]->setPosition(kkVector4(0.f,0.f,m_panMoveSpeed_base));
    m_camera_parts[_Camera::BaseMoveYN]->setPosition(kkVector4(0.f,0.f,-m_panMoveSpeed_base));
    m_camera_parts[_Camera::BaseMoveXP]->setPosition(kkVector4(m_panMoveSpeed_base,0.f,0.0f));
    m_camera_parts[_Camera::BaseMoveXN]->setPosition(kkVector4(-m_panMoveSpeed_base,0.f,0.0f));
    m_camera_parts[_Camera::CamPos_]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::CamPos_]->setPosition(kkVector4(0.f,start_camera_h,0.f));
    m_camera_parts[_Camera::CamPos_Ax]->setParent( m_camera_parts[_Camera::RotX_] );
    m_camera_parts[_Camera::CamPos_Ax]->setPosition(kkVector4(0.f,3.f,0.f));
    

 //   m_kk_axisCamera->setCameraType(kkCameraType::FPS);

    switch(m_type)
    {
    case ViewportCameraType::Perspective:
        m_kk_camera->setCameraType(kkCameraType::FPS);

        rotateX( start_camera_x );
        rotateY( start_camera_y );
        break;
    case ViewportCameraType::Front:
    case ViewportCameraType::Back:
    case ViewportCameraType::Left:
    case ViewportCameraType::Right:
    case ViewportCameraType::Top:
    case ViewportCameraType::Bottom:
    default:
        m_cameraPos_ort.set(0.f,0.f,0.f);
	    m_zoomOrt = 1.f;
        m_kk_camera->setCameraType(kkCameraType::Custom);
        m_kk_camera->setUpdateCallback(ViewportCameraUpdateCallback, m_owner);
        m_kk_camera->setNear(0.001f);
        
        break;
    }

    switch(m_type)
    {
    case ViewportCameraType::Perspective:
    default:
        break;
    case ViewportCameraType::Top:
        rotateX( 0.f ); // OK
        break;
    case ViewportCameraType::Front:
        rotateX( 135.0f ); // OK
        break;
    case ViewportCameraType::Back:
        rotateX( 135.0f ); //ok
        rotateY( 90.0f );
        break;
    case ViewportCameraType::Left:
        rotateX( 135.0f ); //ok
        rotateY( 45.f );
        break;
    case ViewportCameraType::Right:
        rotateX( 135.0f ); //ok
        rotateY( -45.f );
        break;
    case ViewportCameraType::Bottom:
        rotateX( -90.f ); // OK
        break;
    }

    update();

    _updatePanMoveSpeed(AppState_keyboard::None);
}

void ViewportCamera::setOwner( ViewportObject* v )
{
    m_owner = v;

    switch(m_type)
    {
    case ViewportCameraType::Perspective:
        break;
    case ViewportCameraType::Front:
    case ViewportCameraType::Back:
    case ViewportCameraType::Left:
    case ViewportCameraType::Right:
    case ViewportCameraType::Top:
    case ViewportCameraType::Bottom:
    default:
        m_kk_camera->setUpdateCallback(ViewportCameraUpdateCallback, m_owner);
        break;
    }
}

ViewportObject* ViewportCamera::getOwner()
{
    return (ViewportObject*)m_owner;
}


void ViewportCamera::update()
{
    m_camera_parts[_Camera::Base_]->update();
    m_kk_camera->setPosition(m_camera_parts[_Camera::CamPos_]->getPositionInSpace());
    m_kk_camera->update();

    //m_kk_axisCamera->setPosition(m_camera_parts[_Camera::CamPos_Ax]->getPositionInSpace() - m_camera_parts[_Camera::Base_]->getPositionInSpace());
    //m_kk_axisCamera->update();
}

kkCamera * ViewportCamera::getCamera()
{
    return m_kk_camera;
}
const kkVector4& ViewportCamera::getPositionBase()
{
    return m_camera_parts[_Camera::Base_]->getPositionInSpace();
}

const kkVector4& ViewportCamera::getPositionCamera()
{
   return m_camera_parts[_Camera::CamPos_]->getPositionInSpace();
}

f32 ViewportCamera::getCameraHeight()
{
    auto pos = m_camera_parts[_Camera::CamPos_]->getPosition(); /// local position
    return pos.KK_Y;
}

void ViewportCamera::setCameraHeight(f32 v)
{
    m_camera_parts[_Camera::CamPos_]->setPosition( kkVector4(0.f,v,0.f) );
}


void ViewportCamera::rotateX( f32 x )
{
    x *= 2.f;
    m_all_rotation.x += x;


    if( m_all_rotation.x > 180.f )
        m_all_rotation.x = -180.f;

    if( m_all_rotation.x < -180.f )
        m_all_rotation.x = 180.f;

    auto rot = m_camera_parts[_Camera::RotX_]->getRotation();
    m_camera_parts[_Camera::RotX_]->setRotation(kkVector4(rot.KK_X + math::degToRad(x),0.f,0.f));

    m_kk_camera->setRotation(m_kk_camera->getRotation() - kkVector4(math::degToRad(x),0.f,0.f));
    //m_kk_axisCamera->setRotation(m_kk_axisCamera->getRotation() - kkVector4(math::degToRad(x),0.f,0.f));
}

void ViewportCamera::rotateY( f32 y )
{
    y *= 2.f;
    m_all_rotation.y += y;

    if( m_all_rotation.y > 180.f )
        m_all_rotation.y = -180.f;

    else if( m_all_rotation.y < -180.f )
        m_all_rotation.y = 180.f;

    auto rot = m_camera_parts[_Camera::RotY_]->getRotation();
    m_camera_parts[_Camera::RotY_]->setRotation(kkVector4(math::degToRad(0.f),rot.KK_Y + math::degToRad(-y),0.f));

    m_kk_camera->setRotation( m_kk_camera->getRotation() + kkVector4(0.f,math::degToRad(y),0.f));
    //m_kk_axisCamera->setRotation( m_kk_axisCamera->getRotation() + kkVector4(0.f,math::degToRad(y),0.f));
}

void ViewportCamera::zoomIn( AppState_keyboard key, s32 wheel_delta )
{
    auto pos = m_camera_parts[_Camera::CamPos_]->getPosition(); /// local position
    
    f32 another_value = 0.05f;
    f32 value = 1.f;

    auto len = pos.KK_Y;

    if( key == AppState_keyboard::Ctrl )
    {
        another_value = 0.5f;
        /*        if( len < 0.5f ){ value *= 0.015f; }
        else if( len < 1.0f ){ value *= 0.05f;  }
        else if( len < 2.0f ){ value *= 0.15f;  }
        else if( len < 3.0f ){ value *= 0.25f;  }*/
    }

    if( m_type != ViewportCameraType::Perspective )
    {
        m_zoomOrt += (float)wheel_delta * (m_zoomOrt * 0.1f);

        if( m_zoomOrt > 8000.f )
            m_zoomOrt = 8000.f;

        //printf("m_zoomOrt in [%f] [%f]\n",m_zoomOrt, this->getZoomOrtZValue() );
    }


    if( wheel_delta )
        value *= wheel_delta;

    auto h = getCameraHeight();
    pos.KK_Y -= value * (h*another_value);

    if( pos.KK_Y < 0.1f )
        pos.KK_Y = 0.1f;

    m_camera_parts[_Camera::CamPos_]->setPosition(kkVector4(0.f,pos.KK_Y,0.f));
    _updatePanMoveSpeed(AppState_keyboard::None);
}

void ViewportCamera::zoomOut( AppState_keyboard key, s32 wheel_delta )
{
    auto pos = m_camera_parts[_Camera::CamPos_]->getPosition(); /// local position
    auto len = pos.KK_Y;
    
    f32 another_value = 0.05f;
    f32 value = 1.f;
   

    if( key == AppState_keyboard::Ctrl )
    {
        another_value = 0.5f;
        /*        if( len < 0.5f ){ value *= 0.015f; }
        else if( len < 1.0f ){ value *= 0.05f;  }
        else if( len < 2.0f ){ value *= 0.15f;  }
        else if( len < 3.0f ){ value *= 0.25f;  }
        else if( len < 5.0f ){ value *= 0.35f;  }
        else if( len < 7.0f ){ value *= 0.45f;  }
        else if( len < 10.0f ){ value *= 0.7f;  }
        else if( len < 20.0f ){ value *= 0.9f;  }
        else if( len < 50.0f ){ value *= 1.f;  }*/
    }
    
    if( m_type != ViewportCameraType::Perspective )
    {
        m_zoomOrt += (float)wheel_delta * (m_zoomOrt * 0.1f);

        if( m_zoomOrt < 0.00005f )
            m_zoomOrt = 0.00005f;

       // printf("m_zoomOrt out [%f] [%f]\n",m_zoomOrt, this->getZoomOrtZValue());
    }

    if( wheel_delta )
        value *= wheel_delta;

    auto h = getCameraHeight();
    pos.KK_Y -= value * (h*another_value);

    m_camera_parts[_Camera::CamPos_]->setPosition(kkVector4(0.f,pos.KK_Y,0.f));
    _updatePanMoveSpeed(AppState_keyboard::None);
}

void ViewportCamera::setPositionBase(const kkVector4& p)
{
     m_camera_parts[_Camera::Base_]->setPosition(p);
}

void ViewportCamera::movePan( AppState_keyboard key, f32 x, f32 y )
{

    if( key == AppState_keyboard::Ctrl )
    {
        _updatePanMoveSpeed(key);
    }

    kkVector4 g_vec;

    auto Xabs = std::abs(x);
    auto Yabs = std::abs(y);

    auto base_position = m_camera_parts[_Camera::Base_]->getPosition();
    auto XP = m_camera_parts[_Camera::BaseMoveXP]->getPositionInSpace() - base_position;
    auto XN = m_camera_parts[_Camera::BaseMoveXN]->getPositionInSpace() - base_position;
    auto YP = m_camera_parts[_Camera::BaseMoveYP]->getPositionInSpace() - base_position;
    auto YN = m_camera_parts[_Camera::BaseMoveYN]->getPositionInSpace() - base_position;

    if( x != 0.f )
    {
        for( int i = 0, sz  = (int)Xabs; i < sz; ++i )
        {
            if( x > 0.f )
            {
                g_vec -= XP;
            }
            else
            {
                g_vec -= XN;
            }
        }
    }

    if( y != 0.f )
    {
        for( int i = 0, sz  = (int)Yabs; i < sz; ++i )
        {
            if( y > 0.f )
            {
                g_vec -= YP;
            }
            else
            {
                g_vec -= YN;
            }
        }
    }
    
    m_camera_parts[_Camera::Base_]->setPosition( g_vec + base_position );
    m_cameraPos_ort = g_vec + base_position;
}


void ViewportCamera::_updatePanMoveSpeed( AppState_keyboard key )
{
    f32 panMoveSpeed = 0.f;

    if( m_type != ViewportCameraType::Perspective )
    {
        panMoveSpeed = 0.01f / m_zoomOrt;
    }
    else
    {
        auto H = getCameraHeight();
        panMoveSpeed = H * 0.001f;
    }

    m_camera_parts[_Camera::BaseMoveYP]->setPosition(kkVector4(0.f,0.f,m_panMoveSpeed_base + panMoveSpeed));
    m_camera_parts[_Camera::BaseMoveYN]->setPosition(kkVector4(0.f,0.f,-m_panMoveSpeed_base - panMoveSpeed));
    m_camera_parts[_Camera::BaseMoveXP]->setPosition(kkVector4(m_panMoveSpeed_base + panMoveSpeed,0.f,0.0f));
    m_camera_parts[_Camera::BaseMoveXN]->setPosition(kkVector4(-m_panMoveSpeed_base - panMoveSpeed,0.f,0.0f));
}

void ViewportCamera::centerToAabb( const kkAabb& aabb )
{
	kkVector4 center;
    aabb.center(center);
	setPositionBase( center );
    auto H = center.distance(aabb.m_max)*1.8f;
    if( H < 0.1f )
        H = 0.1f;
	setCameraHeight(H);

    _updatePanMoveSpeed(AppState_keyboard::None);
}

void ViewportCamera::setObjectRotationAngle( f32 v )
{
    m_camera_parts[_Camera::ObjectRotation]->setRotation(kkVector4(0.f,v,0.f,1.f));
}
kkMatrix4 ViewportCamera::getObjectRotationMatrix()
{
    kkMatrix4 m1 = m_camera_parts[_Camera::ObjectRotation]->getWorldMatrix();
    m1[ 3u ].KK_X = 0.f;
	m1[ 3u ].KK_Y = 0.f;
	m1[ 3u ].KK_Z = 0.f;

    auto m2 = m_camera_parts[_Camera::RotX_]->getWorldMatrix();
    m2[ 3u ].KK_X = 0.f;
	m2[ 3u ].KK_Y = 0.f;
	m2[ 3u ].KK_Z = 0.f;

    m2.invert();
    return m1*m2;
}