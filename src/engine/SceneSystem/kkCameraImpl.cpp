// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "kkCameraImpl.h"

kkCameraImpl::kkCameraImpl()
{
}

kkCameraImpl::~kkCameraImpl()
{
}
	
void		kkCameraImpl::update()
{
	kkMatrix4 translationMatrix;
	math::makeTranslationMatrix( -m_position, translationMatrix );

	m_worldMatrix = translationMatrix;

	if( m_parent )
	{
		m_worldMatrix = m_parent->getAbsoluteWorldMatrix() * m_worldMatrix;
	}

	m_worldMatrixAbsolute = m_worldMatrix;

	auto * childs = &getChildList();
	{
		auto it = childs->begin();
		for(; it != childs->end(); ++it)
		{
			(*it)->update();
		}
	}

	switch( m_cameraType ){
	case kkCameraType::Custom:
	{
		if( m_updateCallback )
		{
			m_updateCallback( this, m_userData );
		}
	}break;
	case kkCameraType::LookAt:
	{
		math::makePerspectiveRHMatrix(
			m_projectionMatrix,
			m_fov,
			m_aspect,
			m_near,
			m_far );
		math::makeLookAtRHMatrix(
			m_position,
			m_target,
			m_up,
			m_viewMatrix
		);
	}break;

	case kkCameraType::Camera_2D:
	{

		math::makeOrthoRHMatrix(
			m_projectionMatrix,
			10,
			8,
			m_near,
			m_far
		);


		kkQuaternion qPitch( kkVector4( m_rotation.KK_X, 0.f, 0.f, 1.f ) );
		kkQuaternion qYaw( kkVector4( 0.f, m_rotation.KK_Y, 0.f, 1.f ) );
		kkQuaternion qRoll( kkVector4( 0.f, 0.f, m_rotation.KK_Z, 1.f ) );
		
		kkMatrix4 R;
		kkMatrix4 P;
		kkMatrix4 Y;
		math::makeRotationMatrix( R, qRoll );
		math::makeRotationMatrix( P, qPitch );
		math::makeRotationMatrix( Y, qYaw );

		m_rotationMatrix = R * P * Y;

		//m_projectionMatrix = m_rotationMatrix * m_projectionMatrix;
		
		m_viewMatrix = m_rotationMatrix * m_worldMatrixAbsolute;

	}break;

	case kkCameraType::Free:
	{
		math::makePerspectiveRHMatrix(
			m_projectionMatrix,
			m_fov,
			m_aspect,
			m_near,
			m_far );
		

		kkQuaternion qPitch( kkVector4( m_rotation.KK_X, 0.f, 0.f, 1.f ) );
		kkQuaternion qYaw( kkVector4( 0.f, m_rotation.KK_Y, 0.f, 1.f ) );
		kkQuaternion qRoll( kkVector4( 0.f, 0.f, m_rotation.KK_Z, 1.f ) );
		
		kkMatrix4 R;
		kkMatrix4 P;
		kkMatrix4 Y;
		math::makeRotationMatrix( R, qRoll );
		math::makeRotationMatrix( P, qPitch );
		math::makeRotationMatrix( Y, qYaw );

		m_rotationMatrix = R * P * Y;

		m_viewMatrix = m_rotationMatrix * m_worldMatrixAbsolute;


	}break;
	case kkCameraType::FPS:{
		math::makePerspectiveRHMatrix(
			m_projectionMatrix,
			m_fov,
			m_aspect,
			m_near,
			m_far );

		kkQuaternion qPitch( kkVector4( m_rotation.KK_X, 0.f, 0.f, 1.f ) );
		kkQuaternion qYaw( kkVector4( 0.f, m_rotation.KK_Y, 0.f, 1.f ) );
		kkQuaternion qRoll( kkVector4( 0.f, 0.f, m_rotation.KK_Z, 1.f ) );

		m_orientation = qYaw * qPitch * qRoll;
		m_orientation.normalize();

		math::makeRotationMatrix( m_rotationMatrix, m_orientation );

		m_viewMatrix = m_rotationMatrix * m_worldMatrixAbsolute;

	}break;
	}

	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;

	m_viewProjectionInvertMatrix = m_viewProjectionMatrix;
	m_viewProjectionInvertMatrix.invert();

	// calculate target
	switch( m_cameraType )
	{
	case kkCameraType::FPS:
	case kkCameraType::Free:
	case kkCameraType::Custom:
	{
		
		m_target  = math::mul(kkVector4(0.f,0.f,1.f,1.f), m_viewProjectionInvertMatrix);
		m_target.KK_W = 1.0f / m_target.KK_W;
		m_target.KK_X *= m_target.KK_W;
		m_target.KK_Y *= m_target.KK_W;
		m_target.KK_Z *= m_target.KK_W;
		//m_target = math::mul( kkVector4(m_near * 5.15f, 0.f, 0.f, 0.f), R );
		
		//m_target += getPositionInSpace();
	}
		break;
	}

	m_direction = m_target - m_worldMatrix[3];//m_worldMatrixAbsolute[3];
	m_direction.setW(0.f);
	m_direction = m_direction.normalize();

	//std::cout << m_direction << "\n";
	m_frustum.calculateFrustum( m_projectionMatrix, m_viewMatrix );
//	calculateFrustum();
}



