// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_SCENE_OBJECT_COMMON_H__
#define __KKROO_SCENE_OBJECT_COMMON_H__

#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMatrix.h"
#include "Classes/Math/kkQuaternion.h"
#include "Classes/kkAABB.h"
#include "Classes/kkOBB.h"
#include "Classes/Strings/kkString.h"

#include <list>

enum class kkSceneObjectType : u32
{
	None,
	Camera,
	Static,
	Sprite,
	Dummy,

	PolyObject
};

enum class kkBoundingVolumeType : u32
{
	Sphere,
	Obb,
	Aabb,
	Convex
};

class kkSceneObject
{
protected:
	kkString		    m_name;
	s32			    m_id   = -1;
	kkSceneObject*      m_parent = nullptr;
	bool				m_isVisible = true;
	bool				m_isBV = true;
	bool				m_isFNormal = true; // ???
	bool				m_need_update = true;
	f32			m_sphereRadius = 1.f;	
	kkBoundingVolumeType m_BVType     = kkBoundingVolumeType::Sphere;
	kkSceneObjectType    m_objectType = kkSceneObjectType::None;

	std::list<kkSceneObject*> m_childs;

	kkMatrix4		m_worldMatrix, m_worldMatrixAbsolute;
	kkMatrix4		m_rotationMatrix;

	kkVector4		m_position/*, m_positionInSpace*/;
	kkVector4		m_rotation, m_old_rotation;
	kkVector4		m_scale;
	kkQuaternion	m_orientation;
	
	kkAabb			m_aabb;
	kkObb			m_obb;
public:
	kkSceneObject(){}
	virtual ~kkSceneObject(){}
	
	kkSceneObjectType	getType(){ return m_objectType; }
	const kkVector4&	getPosition()const{return m_position;}
	const kkVector4&	getPositionInSpace()const{return m_worldMatrix[3];}
	void				setPosition( const kkVector4& p ){m_position=p;m_need_update=true;}

	/// Чётко устанавливает ориентацию объекта
	/// Для более детальной настройки лучше использовать set_orientation
	void				setRotation( const kkVector4& rotation )
	{
		if( m_old_rotation != rotation )
		{
			this->m_rotation = rotation; 

			kkVector4 r =  rotation - m_old_rotation;

			kkQuaternion q(r);
			m_orientation = q * m_orientation;
			m_orientation.normalize();

			m_old_rotation = rotation;

			//recalculateBV();
			m_need_update=true;
		}
	}
	const kkVector4&	getRotation()const{return m_rotation;}
	const kkVector4&	getScale()const{return m_scale;}
	void				setScale( const kkVector4& s ){m_scale=s;m_need_update=true;}

	/// двойной вызов set_orientation(kkVector4(math::degToRad(45.f),0.f,0.f))
	/// повернёт объект на 90 градусов
	void				setOrientation( const kkQuaternion& q )
	{
		m_orientation = q * m_orientation;
		m_orientation.normalize();
		//recalculateBV();
		m_need_update=true;
	}

	const kkQuaternion& getOrientation()const{return m_orientation;}
	const kkMatrix4&	getAbsoluteWorldMatrix()const{return m_worldMatrixAbsolute;}
	const kkMatrix4&	getWorldMatrix()const{return m_worldMatrix;}
	void				setWorldMatrix( const kkMatrix4& m ){m_worldMatrix = m;}

	void                recalculateBV()
	{
		/// этот метод должен изменять ОББ\ААББ\радиус БВ после изменении трансформации
		/// ААББ создаётся в соответствии с установленной моделью.
		/// Если объект изменил позицию и т.д. то нужно вычислить новый ААББ. Брать точки оригинального ААББ, поворачивать их, и строить новый ААББ.
		/// как я понял ОББ это оригинальный ААББ, но повёрнут в соответствии с кватернионом
	}

	s32				getID()const{return m_id;}
	void			setID( s32 i ){m_id=i;}

	void	            setParent( kkSceneObject * parent = nullptr )
	{
		if( m_parent )
			m_parent->removeChild( this );

		if( parent )
		{
			parent->addChild( this );
			m_parent = parent;
		}
	}

	kkSceneObject *     getParent()const{return m_parent;}
	std::list<kkSceneObject*>&	getChildList(){return m_childs;}
	void	            addChild( kkSceneObject * child )
	{
		if( child && (child->getParent() != this) )
		{
			m_childs.push_back( child );
			child->m_parent = this;
		}
	}
	void	            removeChild( kkSceneObject * child )
	{
		auto it = m_childs.begin();
		auto it_end = m_childs.end();
		for(; it != it_end; ++it )
		{
			if( (*it) == child )
			{
				(*it)->m_parent = nullptr;
				m_childs.erase( it );
				return;
			}
		}
	}

	bool                isVisible(){return m_isVisible;}
	void                setVisible( bool v ){m_isVisible = v;}

	void                setBVType( kkBoundingVolumeType type ){m_BVType=type;}
	const f32&	getBVSphereRadius() const {return m_sphereRadius;}
	kkBoundingVolumeType getBVType() const  {return m_BVType;}
	kkAabb*				getAabb(){return &m_aabb;}
	kkObb*				getObb(){return &m_obb;}
		
	virtual void		update() = 0;
		
};

#endif