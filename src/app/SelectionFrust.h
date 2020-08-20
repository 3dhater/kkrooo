#ifndef __SELECTION_FRUST_H__
#define __SELECTION_FRUST_H__

#include "Classes/Math/kkMatrix.h"
#include "Functions.h"

// При выборе рамкой (просто когда зажимается ЛКМ и мышка тянется куда-то)
// должны создаваться плоскости - верх низ лево право
/*
e   end
2---3
|   |
1---0
o   origin
*/
struct SelectionFrust
{
	kkVector4 m_left[4];
	kkVector4 m_top[4];
	kkVector4 m_right[4];
	kkVector4 m_bottom[4];

	// center
	kkVector4 m_LC;
	kkVector4 m_TC;
	kkVector4 m_RC;
	kkVector4 m_BC;

	// normal
	kkVector4 m_LN;
	kkVector4 m_TN;
	kkVector4 m_RN;
	kkVector4 m_BN;

	void createWithFrame(const v4i& frame, const v4f& vp_rect, const kkMatrix4& VP_invert)
	{
		// get 4 rays from screen
		kkRay ray1, ray2, ray3, ray4;

		kkrooo::getRay(ray1, v2i(frame.x, frame.y), vp_rect, vp_rect.getWidthAndHeight(),VP_invert);
		kkrooo::getRay(ray2, v2i(frame.z, frame.y), vp_rect, vp_rect.getWidthAndHeight(),VP_invert);
		kkrooo::getRay(ray3, v2i(frame.x, frame.w), vp_rect, vp_rect.getWidthAndHeight(),VP_invert);
		kkrooo::getRay(ray4, v2i(frame.z, frame.w), vp_rect, vp_rect.getWidthAndHeight(),VP_invert);
		ray1.update();
		ray2.update();
		ray3.update();
		ray4.update();

		m_top[0] = ray1.m_origin;
		m_top[1] = ray2.m_origin;
		m_top[2] = ray1.m_end;
		m_top[3] = ray2.m_end;

		m_right[0] = ray2.m_origin;
		m_right[1] = ray4.m_origin;
		m_right[2] = ray2.m_end;
		m_right[3] = ray4.m_end;

		m_bottom[0] = ray4.m_origin;
		m_bottom[1] = ray3.m_origin;
		m_bottom[2] = ray4.m_end;
		m_bottom[3] = ray3.m_end;

		m_left[0] = ray3.m_origin;
		m_left[1] = ray1.m_origin;
		m_left[2] = ray3.m_end;
		m_left[3] = ray1.m_end;

		kkVector4 e1, e2;
	
		e1 = m_right[1] - m_right[0];
		e2 = m_right[2] - m_right[0];
		e1.cross(e2, m_RN);
		m_RC = m_right[0] + m_right[1] + m_right[2] + m_right[3];
		m_RC *= 0.25;

		e1 = m_bottom[1] - m_bottom[0];
		e2 = m_bottom[2] - m_bottom[0];
		e1.cross(e2, m_BN);
		m_BC = m_bottom[0] + m_bottom[1] + m_bottom[2] + m_bottom[3];
		m_BC *= 0.25;

		e1 = m_top[1] - m_top[0];
		e2 = m_top[2] - m_top[0];
		e1.cross(e2, m_TN);
		m_TC = m_top[0] + m_top[1] + m_top[2] + m_top[3];
		m_TC *= 0.25;

		e1 = m_left[1] - m_left[0];
		e2 = m_left[2] - m_left[0];
		e1.cross(e2, m_LN);
		m_LC = m_left[0] + m_left[1] + m_left[2] + m_left[3];
		m_LC *= 0.25;
	}

	bool pointInFrust(const kkVector4& v)const
	{
		if( m_TN.dot(m_TC - v) < 0.f ) return false;
		if( m_BN.dot(m_BC - v) < 0.f ) return false;
		if( m_RN.dot(m_RC - v) < 0.f ) return false;
		if( m_LN.dot(m_LC - v) < 0.f ) return false;
		return true;
	}

	bool lineInFrust(const kkVector4& p1, const kkVector4& p2)const
	{
		// обе точки за пределами
		if( m_TN.dot(m_TC - p1) < 0.f && m_TN.dot(m_TC - p2) < 0.f ) return false;
		if( m_BN.dot(m_BC - p1) < 0.f && m_BN.dot(m_BC - p2) < 0.f ) return false;
		if( m_RN.dot(m_RC - p1) < 0.f && m_RN.dot(m_RC - p2) < 0.f ) return false;
		if( m_LN.dot(m_LC - p1) < 0.f && m_LN.dot(m_LC - p2) < 0.f ) return false;
	
		// обе точки внутри
		if( m_TN.dot(m_TC - p1) > 0.f && m_TN.dot(m_TC - p2) > 0.f )
		{
			if( m_RN.dot(m_RC - p1) > 0.f && m_RN.dot(m_RC - p2) > 0.f )
			{
				if( m_BN.dot(m_BC - p1) > 0.f && m_BN.dot(m_BC - p2) > 0.f )
				{
					if( m_LN.dot(m_LC - p1) > 0.f && m_LN.dot(m_LC - p2) > 0.f )
					{
						return true;
					}
				}
			}
		}

		// try ray-triangle
		if( rayTest_MT(p1, p2, m_top[0], m_top[1], m_top[2]) ) return true;
		if( rayTest_MT(p1, p2, m_top[0], m_top[2], m_top[3]) ) return true;
		if( rayTest_MT(p1, p2, m_left[0], m_left[1], m_left[2]) ) return true;
		if( rayTest_MT(p1, p2, m_left[0], m_left[2], m_left[3]) ) return true;
		if( rayTest_MT(p1, p2, m_right[0], m_right[1], m_right[2]) ) return true;
		if( rayTest_MT(p1, p2, m_right[0], m_right[2], m_right[3]) ) return true;
		if( rayTest_MT(p1, p2, m_bottom[0], m_bottom[1], m_bottom[2]) ) return true;
		if( rayTest_MT(p1, p2, m_bottom[0], m_bottom[2], m_bottom[3]) ) return true;

		return false;
	}

	bool rayTest_MT( const kkVector4& ray_origin, const kkVector4& ray_end, const kkVector4& v1, const kkVector4& v2, const kkVector4& v3 )const
	{
		kkVector4 e1 = v2 - v1;
		kkVector4 e2 = v3 - v1;
		kkVector4 ray_dir = ray_end - ray_origin;
		ray_dir.normalize2();
		ray_dir.setW(1.f);
		kkVector4  pvec = ray_dir.cross_return(e2);
		f32 det  = e1.dot(pvec);
		
		if( std::fabs(det) < Epsilon ) return false;

		kkVector4 tvec(
			ray_origin._f32[0] - v1._f32[0],
			ray_origin._f32[1] - v1._f32[1],
			ray_origin._f32[2] - v1._f32[2],
			0.f);

		f32 inv_det = 1.f / det;
		f32 U = tvec.dot(pvec) * inv_det;

		if( U < 0.f || U > 1.f )
			return false;

		kkVector4  qvec = tvec.cross_return(e1);
		f32 V    = ray_dir.dot(qvec) * inv_det;

		if( V < 0.f || U + V > 1.f )
			return false;

		f32 T = e2.dot(qvec) * inv_det;
		
		//if( T < Epsilon ) return false;
		
		return true;
	}
};

#endif