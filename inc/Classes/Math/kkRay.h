// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_RAY_H__
#define __KKROO_RAY_H__
#include "Classes/Math/kkVector4.h"

inline int max_dim(const kkVector4& v)
{
    return (v._f32[0] > v._f32[1]) ? ((v._f32[0] > v._f32[2]) 
		? 0 : 2) : ((v._f32[1] > v._f32[2]) ? 1 : 2);
}

class kkRay
{
public:
	kkRay(){}
	~kkRay(){}

	kkVector4 m_origin;
	kkVector4 m_end;
	kkVector4 m_direction;
	kkVector4 m_invDir;

	s32 m_kz = 0;
	s32 m_kx = 0;
	s32 m_ky = 0;

	f32 m_Sx = 0.f;
	f32 m_Sy = 0.f;
	f32 m_Sz = 0.f;

	mutable f32 m_tMax = Infinity;

	f32 distanceToLine(const kkVector4& lineP0, const kkVector4& lineP1)
	{
		kkVector4 u = m_end - m_origin;
		kkVector4 v = lineP1 - lineP0;
		kkVector4 w = m_origin - lineP0;
		u.setW(0.f);
		v.setW(0.f);
		w.setW(0.f);
		f32 a = u.dot();
		f32 b = u.dot(v);
		f32 c = v.dot();
		f32 d = u.dot(w);
		f32 e = v.dot(w);
		f32 D = a*c - b*b;
		f32 sc, tc;

		if( D < Epsilon )
		{
			sc = 0.f;
			tc = (b>c ? d/b : e/c);
		}
		else
		{
			sc = (b*e - c*d) / D;
			tc = (a*e - b*d) / D;
		}

		kkVector4 dP = w + (sc*u) - (tc*v);
		dP.setW(0.f);
		return std::sqrt(dP.dot());
	}

	void update()
	{
		m_direction._f32[ 0 ] = m_end._f32[0] - m_origin._f32[0];
		m_direction._f32[ 1 ] = m_end._f32[1] - m_origin._f32[1];
		m_direction._f32[ 2 ] = m_end._f32[2] - m_origin._f32[2];
		m_direction.normalize();
		m_direction._f32[ 3 ] = 1.f;

		m_invDir._f32[ 0 ] = 1.f / m_direction._f32[ 0 ];
		m_invDir._f32[ 1 ] = 1.f / m_direction._f32[ 1 ];
		m_invDir._f32[ 2 ] = 1.f / m_direction._f32[ 2 ];
		m_invDir._f32[ 3 ] = 1.f / m_direction._f32[ 3 ];
	
		m_kz = max_dim
		(
			kkVector4
			(
				std::abs(m_direction._f32[ 0 ]),
				std::abs(m_direction._f32[ 1 ]),
				std::abs(m_direction._f32[ 2 ]),
				1.f
			)
		);
		
		m_kx = m_kz + 1;
		if( m_kx == 3 )
			m_kx = 0;
		
		m_ky = m_kx + 1;
		if( m_ky == 3 )
			m_ky = 0;

		if( m_direction._f32[m_kz] )
			std::swap(m_kx, m_ky);

		m_Sx = m_direction._f32[m_kx] / m_direction._f32[m_kz];
		m_Sy = m_direction._f32[m_ky] / m_direction._f32[m_kz];
		m_Sz = 1.f / m_direction._f32[m_kz];
	}
};

#endif