// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_DUMMY_H__
#define __KKROO_DUMMY_H__

#include "kkSceneObject.h"

class kkDummy : public kkSceneObject
{
public:

	kkDummy()
	{
		m_objectType = kkSceneObjectType::Dummy;
	}
	virtual ~kkDummy(){}

	void update()
	{
		kkMatrix4 translationMatrix;
		math::makeTranslationMatrix( m_position, translationMatrix );

		kkMatrix4 rotationMatrix;
		math::makeRotationMatrix( rotationMatrix, m_orientation );

		kkMatrix4	scaleMatrix;
		//scaleMatrix[ 0 ].fill(1.f);
		//scaleMatrix[ 1 ].fill(1.f);
		//scaleMatrix[ 2 ].fill(1.f);
		scaleMatrix[ 0u ].KK_X = 1.f;
		scaleMatrix[ 1u ].KK_Y = 1.f;
		scaleMatrix[ 2u ].KK_Z = 1.f;

		m_worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		m_worldMatrixAbsolute = m_worldMatrix;
	
		//m_positionInSpace = m_position;

		if( m_parent )
		{
			//m_worldMatrixAbsolute = m_parent->getAbsoluteWorldMatrix() * m_worldMatrix;
			m_worldMatrix = m_parent->getWorldMatrix() * m_worldMatrix;
			//m_positionInSpace += m_parent->get_position_in_space();
		}



		auto * childs = &getChildList();
		{
			auto it = childs->begin();
			for(; it != childs->end(); ++it){
				(*it)->update();
			}
		}

	}
};

#endif