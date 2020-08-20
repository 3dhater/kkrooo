// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_CAMERA_H__
#define __KKROO_CAMERA_H__

#include "kkSceneObject.h"

enum class kkCameraType
{
	LookAt,
	Free,
	FPS,
	Camera_2D,
	Custom
};

struct kkCameraFrustum
{
	kkVector4 m_planes[ 6u ];
	
	float frustum[16];

	// перед использованием обязательно нужно сделать point.KK_W = 1.f
	bool pointInFrustum( const kkVector4& point )
	{
		for( s32 i = 0; i < 6; ++i )
		{
			if( m_planes[i].dot(point) < 0 )
				return false;
		}

		return true;
	}

	bool sphereInFrustum( f32 radius, const v4f& position )
	{
		for( u32 i = 0u; i < 6u; ++i )
		{
			if( ( m_planes[ i ].KK_X * position.x + m_planes[ i ].KK_Y * position.y + m_planes[ i ].KK_Z * position.z
				+ m_planes[ i ].KK_W ) <= -radius)
			{
				return false;
			}
		}
		return true;
	}

	void calculateFrustum( kkMatrix4& P, kkMatrix4& V )
	{
		f32 *proj = P.getPtr();
		f32 *modl = V.getPtr();
		float   clip[16]; //clipping planes

		clip[0] = modl[0] * proj[0]   + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
		clip[1] = modl[0] * proj[1]   + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
		clip[2] = modl[0] * proj[2]   + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
		clip[3] = modl[0] * proj[3]   + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

		clip[4] = modl[4] * proj[0]   + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
		clip[5] = modl[4] * proj[1]   + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
		clip[6] = modl[4] * proj[2]   + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
		clip[7] = modl[4] * proj[3]   + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

		clip[8] = modl[8] * proj[0]   + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
		clip[9] = modl[8] * proj[1]   + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
		clip[10] = modl[8] * proj[2]  + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[8] * proj[3]  + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];


		//RIGHT 
		m_planes[ 0u ].KK_X = ( clip[ 3 ] - clip[ 0 ]);
		m_planes[ 0u ].KK_Y = ( clip[ 7 ] - clip[ 4 ]);
		m_planes[ 0u ].KK_Z = ( clip[ 11u ] - clip[ 8 ]);
		m_planes[ 0u ].KK_W = ( clip[ 15u ] - clip[ 12u ]);
		m_planes[ 0u ].normalize();

		//LEFT  
		m_planes[ 1 ].setX( clip[ 3 ] + clip[ 0 ]);
		m_planes[ 1 ].setY( clip[ 7 ] + clip[ 4 ]);
		m_planes[ 1 ].setZ( clip[ 11u ] + clip[ 8 ]);
		m_planes[ 1 ].setW( clip[ 15u ] + clip[ 12u ]);
		m_planes[ 1 ].normalize();

		//BOTTOM  
		m_planes[ 2 ].setX( clip[ 3 ] + clip[ 1 ]);
		m_planes[ 2 ].setY( clip[ 7 ] + clip[ 5 ]);
		m_planes[ 2 ].setZ( clip[ 11u ] + clip[ 9 ]);
		m_planes[ 2 ].setW( clip[ 15u ] + clip[ 13u ]);
		m_planes[ 2 ].normalize();

		//TOP  
		m_planes[ 3 ].setX( clip[ 3 ] - clip[ 1 ]);
		m_planes[ 3 ].setY( clip[ 7 ] - clip[ 5 ]);
		m_planes[ 3 ].setZ( clip[ 11u ] - clip[ 9 ]);
		m_planes[ 3 ].setW( clip[ 15u ] - clip[ 13u ]);
		m_planes[ 3 ].normalize();

		//FAR  
		m_planes[ 4 ].setX( clip[ 3 ] - clip[ 2 ]);
		m_planes[ 4 ].setY( clip[ 7 ] - clip[ 6 ]);
		m_planes[ 4 ].setZ( clip[ 11u ] - clip[ 10u ]);
		m_planes[ 4 ].setW( clip[ 15u ] - clip[ 14u ]);
		m_planes[ 4 ].normalize();

		//NEAR  
		m_planes[ 5 ].setX( clip[ 3 ] + clip[ 2 ]);
		m_planes[ 5 ].setY( clip[ 7 ] + clip[ 6 ]);
		m_planes[ 5 ].setZ( clip[ 11u ] + clip[ 10u ]);
		m_planes[ 5 ].setW( clip[ 15u ] + clip[ 14u ]);
		m_planes[ 5 ].normalize();
	}
};

class kkCamera : public kkSceneObject
{
protected:
	void(*m_updateCallback)(kkCamera*,void*data) = nullptr;
	void* m_userData = nullptr;

	kkVector4		m_up     = kkVector4(0.f,1.f,0.f,1.f);
	f32		m_fov    = 0.785398185f;
	f32		m_near   = 1.f;
	f32		m_far    = 100.f;
	f32		m_aspect = 1.333333f;
	kkCameraType	m_cameraType = kkCameraType::LookAt;
	kkMatrix4		m_viewMatrix;
	kkMatrix4		m_viewProjectionMatrix;
	kkMatrix4		m_rotationMatrix;
	kkMatrix4		m_viewProjectionInvertMatrix;
	kkMatrix4		m_projectionMatrix;
	kkVector4		m_viewport;
	kkVector4		m_target;
	kkVector4		m_direction;
	kkCameraFrustum m_frustum;

public:

	kkCamera()
	{
		m_objectType = kkSceneObjectType::Camera;
	}
	virtual ~kkCamera(){}

	f32			getAspect()const{return m_aspect;}
	kkCameraType		getCameraType(){return m_cameraType;}
	f32			getFar()const{return m_far;}
	f32			getFOV()const{return m_fov;}
	kkCameraFrustum*    getFrustum(){return &m_frustum;}
	f32			getNear()const{return m_near;}
	const kkMatrix4&	getProjectionMatrix()const{return m_projectionMatrix;}
	const kkVector4&	getTarget()const{return m_target;}
	const kkVector4&	getDirection()const{return m_direction;}
	const kkVector4&	getUpVector()const{return m_up;}
	const kkMatrix4&	getViewMatrix()const{return m_viewMatrix;}
	const kkMatrix4&	getViewProjectionMatrix()const{return m_viewProjectionMatrix;}
	const kkMatrix4&	getViewProjectionInvertMatrix()const{return m_viewProjectionInvertMatrix;}

	void				setAspect( f32 aspect ){m_aspect = aspect;}
	void				setCameraType( kkCameraType type ){m_cameraType=type;}
	void				setFar( f32 Far ){m_far=Far;}
	void				setFOV( f32 fov ){m_fov=fov;}
	void				setNear( f32 Near ){m_near=Near;}
	void				setTarget( const kkVector4& target ){m_target=target;}
	void	            setViewMatrix( const kkMatrix4& m){m_viewMatrix=m;}
	void	            setProjectionMatrix( const kkMatrix4& m){m_projectionMatrix=m;}
		
	// Only for custom camera
	void                setUpdateCallback( void(*callback)(kkCamera*,void*), void * data ){m_updateCallback=callback;m_userData=data;}
		
	void				setUpVector( const kkVector4& up ){m_up=up;}
	void				setViewPort( const kkVector4& v ){m_viewport=v;}
};

#endif