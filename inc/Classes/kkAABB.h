// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_AABB_H__
#define __KKROO_AABB_H__


#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkRay.h"

#include <float.h>
#include <algorithm>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

		// Axis-Aligned Bounding Box
class kkAabb
{
public:

	kkAabb(){ }
	kkAabb( const kkVector4& min, const kkVector4& max ):m_min( min ), m_max( max ){ }

	void add( const kkVector4& point )
	{
		if( point.KK_X < m_min.KK_X ) m_min.KK_X = point.KK_X;
		if( point.KK_Y < m_min.KK_Y ) m_min.KK_Y = point.KK_Y;
		if( point.KK_Z < m_min.KK_Z ) m_min.KK_Z = point.KK_Z;

		if( point.KK_X > m_max.KK_X ) m_max.KK_X = point.KK_X;
		if( point.KK_Y > m_max.KK_Y ) m_max.KK_Y = point.KK_Y;
		if( point.KK_Z > m_max.KK_Z ) m_max.KK_Z = point.KK_Z;
	}

	void add( const kkAabb& box )
	{
		if( box.m_min.KK_X < m_min.KK_X ) m_min.KK_X = box.m_min.KK_X;
		if( box.m_min.KK_Y < m_min.KK_Y ) m_min.KK_Y = box.m_min.KK_Y;
		if( box.m_min.KK_Z < m_min.KK_Z ) m_min.KK_Z = box.m_min.KK_Z;

		if( box.m_max.KK_X > m_max.KK_X ) m_max.KK_X = box.m_max.KK_X;
		if( box.m_max.KK_Y > m_max.KK_Y ) m_max.KK_Y = box.m_max.KK_Y;
		if( box.m_max.KK_Z > m_max.KK_Z ) m_max.KK_Z = box.m_max.KK_Z;
	}

	bool rayTest( const kkRay& r )
	{
		float t1 = (m_min._f32[0] - r.m_origin._f32[0])*r.m_invDir._f32[0];
		float t2 = (m_max._f32[0] - r.m_origin._f32[0])*r.m_invDir._f32[0];
		float t3 = (m_min._f32[1] - r.m_origin._f32[1])*r.m_invDir._f32[1];
		float t4 = (m_max._f32[1] - r.m_origin._f32[1])*r.m_invDir._f32[1];
		float t5 = (m_min._f32[2] - r.m_origin._f32[2])*r.m_invDir._f32[2];
		float t6 = (m_max._f32[2] - r.m_origin._f32[2])*r.m_invDir._f32[2];

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
		
		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}

	void center( kkVector4& v ) const
	{
		v = kkVector4( m_min + m_max );
		v *= 0.5f;
	}

	f32 radius(const kkVector4& aabb_center )
	{
		return aabb_center.distance(m_max);
	}

	void extent( kkVector4& v ) { v = kkVector4( m_max - m_min ); }
	bool isEmpty() const  { return (m_min == m_max); }
	void reset()          { m_min = kkVector4(FLT_MAX); m_max = kkVector4(-FLT_MAX); }

	kkVector4 m_min = kkVector4(FLT_MAX);
	kkVector4 m_max = kkVector4(-FLT_MAX);
};


#endif