// SPDX-License-Identifier: GPL-3.0-only
/*
Используется часть кода из библиотеки Bullet

Copyright (c) 2003-2006 Gino van den Bergen / Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __KKROO_VEC4_H__
#define __KKROO_VEC4_H__

#include <cmath>

#define Infinity std::numeric_limits<f32>::infinity()
#define Epsilon std::numeric_limits<f32>::epsilon()

#ifdef KK_PLATFORM_WINDOWS
	#ifdef KK_USE_SIMD
		#include <xmmintrin.h>
		#include <emmintrin.h>
#define btCastfTo128i(a) (_mm_castps_si128(a))
#define btCastfTo128d(a) (_mm_castps_pd(a))
#define btCastiTo128f(a) (_mm_castsi128_ps(a))
#define btCastdTo128f(a) (_mm_castpd_ps(a))
#define btCastdTo128i(a) (_mm_castpd_si128(a))
#define btAssign128(r0, r1, r2, r3) _mm_setr_ps(r0, r1, r2, r3)
#define BT_SHUFFLE(x, y, z, w) (((w) << 6 | (z) << 4 | (y) << 2 | (x)) & 0xff)
//#define bt_pshufd_ps( _a, _mask ) (__m128) _mm_shuffle_epi32((__m128i)(_a), (_mask) )
#define bt_pshufd_ps(_a, _mask) _mm_shuffle_ps((_a), (_a), (_mask))
#define bt_splat3_ps(_a, _i) bt_pshufd_ps((_a), BT_SHUFFLE(_i, _i, _i, 3))
#define bt_splat_ps(_a, _i) bt_pshufd_ps((_a), BT_SHUFFLE(_i, _i, _i, _i))

#define btv3AbsiMask (_mm_set_epi32(0x00000000, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define btvAbsMask (_mm_set_epi32(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define btvFFF0Mask (_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
#define btv3AbsfMask btCastiTo128f(btv3AbsiMask)
#define btvFFF0fMask btCastiTo128f(btvFFF0Mask)
#define btvxyzMaskf btvFFF0fMask
#define btvAbsfMask btCastiTo128f(btvAbsMask)
#define ATTRIBUTE_ALIGNED16(a) __declspec(align(16)) a
	#endif
#endif

#ifndef ATTRIBUTE_ALIGNED16
#define ATTRIBUTE_ALIGNED16(a) a
#endif

ATTRIBUTE_ALIGNED16(class)
kkVector4
{
public:

	union
	{
		f32 _f32[4];
#ifdef KK_USE_SIMD
		__m128    _vec;
#endif
	};

	kkVector4()
	{
		_f32[ 0 ] = 0.f;
		_f32[ 1 ] = 0.f;
		_f32[ 2 ] = 0.f;
		_f32[ 3 ] = 0.f;
	}


	kkVector4(f32 v)
	{
		_f32[ 0 ] = v;
		_f32[ 1 ] = v;
		_f32[ 2 ] = v;
		_f32[ 3 ] = v;
	}

#ifdef KK_USE_SIMD
	kkVector4(__m128 v)
	{
		_vec = v;
	}
#endif



	kkVector4( const kkVector4& other )
	{
		_f32[ 0 ] = other._f32[ 0 ];
		_f32[ 1 ] = other._f32[ 1 ];
		_f32[ 2 ] = other._f32[ 2 ];
		_f32[ 3 ] = other._f32[ 3 ];
	}

	kkVector4( f32 x, f32 y, f32 z, f32 w )
	{
		_f32[ 0 ] = x;
		_f32[ 1 ] = y;
		_f32[ 2 ] = z;
		_f32[ 3 ] = w;
	}

	kkVector4( f32 x, f32 y, f32 z )
	{
		_f32[ 0 ] = x;
		_f32[ 1 ] = y;
		_f32[ 2 ] = z;
		_f32[ 3 ] = 1.f;
	}

	kkVector4( u32 x, u32 y, u32 z, u32 w )
	{
		_f32[ 0 ] = static_cast<f32>(x);
		_f32[ 1 ] = static_cast<f32>(y);
		_f32[ 2 ] = static_cast<f32>(z);
		_f32[ 3 ] = static_cast<f32>(w);
	}

	kkVector4( s32 x, s32 y, s32 z, s32 w )
	{
		_f32[ 0 ] = static_cast<f32>(x);
		_f32[ 1 ] = static_cast<f32>(y);
		_f32[ 2 ] = static_cast<f32>(z);
		_f32[ 3 ] = static_cast<f32>(w);
	}

	~kkVector4()
	{
	}

	const f32 * data() const
	{
		return &_f32[0];
	}

	f32 getX() const 
	{
		return _f32[0];
	}

	f32 getY() const 
	{
		return _f32[1];
	}

	f32 getZ() const 
	{
		return _f32[2];
	}

	f32 getW() const 
	{
		return _f32[3];
	}

	void set(f32 x, f32 y, f32 z)
	{
		_f32[0] = x;
		_f32[1] = y;
		_f32[2] = z;
		_f32[3] = 0.f;
	}

	void set(f32 x, f32 y, f32 z, f32 w)
	{
		_f32[0] = x;
		_f32[1] = y;
		_f32[2] = z;
		_f32[3] = w;
	}

	void setX( f32 v )
	{
		_f32[0] = v;
	}

	void setY( f32 v )
	{
		_f32[1] = v;
	}

	void setZ( f32 v )
	{
		_f32[2] = v;
	}

	void setW( f32 v )
	{
		_f32[3] = v;
	}

	friend std::ostream& operator<<(std::ostream& os, const kkVector4& dt);

#ifdef KK_USE_SIMD
	void setSimd( const __m128& v)
	{
		_vec = v;
	}
#endif

	void operator+=( const kkVector4& v )
	{
#ifdef KK_USE_SIMD
		_vec = _mm_add_ps( _vec, v._vec );
#else
		_f32[ 0 ] += v._f32[ 0 ];
		_f32[ 1 ] += v._f32[ 1 ];
		_f32[ 2 ] += v._f32[ 2 ];
		_f32[ 3 ] += v._f32[ 3 ];
#endif
	}

	void operator+=( f32 v )
	{
		_f32[ 0 ] += v;
		_f32[ 1 ] += v;
		_f32[ 2 ] += v;
		_f32[ 3 ] += v;
	}

	void operator-=( const kkVector4& v )
	{
#ifdef KK_USE_SIMD
		_vec = _mm_sub_ps( _vec, v._vec );
#else
		_f32[ 0 ] -= v._f32[ 0 ];
		_f32[ 1 ] -= v._f32[ 1 ];
		_f32[ 2 ] -= v._f32[ 2 ];
		_f32[ 3 ] -= v._f32[ 3 ];
#endif
	}

	void operator*=( const kkVector4& v )
	{
#ifdef KK_USE_SIMD
		_vec = _mm_mul_ps( _vec, v._vec );
#else
		_f32[ 0 ] *= v._f32[ 0 ];
		_f32[ 1 ] *= v._f32[ 1 ];
		_f32[ 2 ] *= v._f32[ 2 ];
		_f32[ 3 ] *= v._f32[ 3 ];
#endif
	}

	void operator*=( f32 v )
	{
#ifdef KK_USE_SIMD
		kkVector4 _v(v,v,v,v);
		_vec = _mm_mul_ps( _vec, _v._vec );
#else
		_f32[ 0 ] *= v;
		_f32[ 1 ] *= v;
		_f32[ 2 ] *= v;
		_f32[ 3 ] *= v;
#endif
	}

	void operator/=( const kkVector4& v )
	{
#ifdef KK_USE_SIMD
		_vec = _mm_div_ps( _vec, v._vec );
#else
		_f32[ 0 ] /= v._f32[ 0 ];
		_f32[ 1 ] /= v._f32[ 1 ];
		_f32[ 2 ] /= v._f32[ 2 ];
		_f32[ 3 ] /= v._f32[ 3 ];
#endif
	}

	kkVector4 operator+( const kkVector4& v ) const
	{
		kkVector4 r;
#ifdef KK_USE_SIMD
		r._vec = _mm_add_ps( _vec, v._vec );
#else
		r._f32[ 0 ] = _f32[0] + v._f32[ 0 ];
		r._f32[ 1 ] = _f32[1] + v._f32[ 1 ];
		r._f32[ 2 ] = _f32[2] + v._f32[ 2 ];
		r._f32[ 3 ] = _f32[3] + v._f32[ 3 ];
#endif
		return r;
	}

	kkVector4 operator-( const kkVector4& v ) const
	{
		kkVector4 r;
#ifdef KK_USE_SIMD
		r._vec = _mm_sub_ps( _vec, v._vec );
#else
		r._f32[ 0 ] = _f32[0] - v._f32[ 0 ];
		r._f32[ 1 ] = _f32[1] - v._f32[ 1 ];
		r._f32[ 2 ] = _f32[2] - v._f32[ 2 ];
		r._f32[ 3 ] = _f32[3] - v._f32[ 3 ];
#endif
		return r;
	}

	kkVector4 operator-() const
	{
		kkVector4 r;
		r._f32[ 0 ] = -_f32[ 0 ];
		r._f32[ 1 ] = -_f32[ 1 ];
		r._f32[ 2 ] = -_f32[ 2 ];
		r._f32[ 3 ] = -_f32[ 3 ];
		return r;
	}

	kkVector4 operator*( const kkVector4& v ) const
	{
		kkVector4 r;
#ifdef KK_USE_SIMD
		r._vec = _mm_mul_ps( _vec, v._vec );
#else
		r._f32[ 0 ] = _f32[0] * v._f32[ 0 ];
		r._f32[ 1 ] = _f32[1] * v._f32[ 1 ];
		r._f32[ 2 ] = _f32[2] * v._f32[ 2 ];
		r._f32[ 3 ] = _f32[3] * v._f32[ 3 ];
#endif
		return r;
	}

	kkVector4 operator*( f32 v ) const
	{
		kkVector4 r;
#ifdef KK_USE_SIMD
		kkVector4 _v(v,v,v,v);
		r._vec = _mm_mul_ps( _vec, _v._vec );
#else
		r._f32[ 0 ] = _f32[0] * v;
		r._f32[ 1 ] = _f32[1] * v;
		r._f32[ 2 ] = _f32[2] * v;
		r._f32[ 3 ] = _f32[3] * v;
#endif
		return r;
	}

	kkVector4 operator/( const kkVector4& v ) const
	{
		kkVector4 r;
#ifdef KK_USE_SIMD
		r._vec = _mm_div_ps( _vec, v._vec );
#else
		r._f32[ 0 ] = _f32[0] / v._f32[ 0 ];
		r._f32[ 1 ] = _f32[1] / v._f32[ 1 ];
		r._f32[ 2 ] = _f32[2] / v._f32[ 2 ];
		r._f32[ 3 ] = _f32[3] / v._f32[ 3 ];
#endif
		return r;
	}

	bool	operator==( const kkVector4& v ) const 
	{
		if( _f32[0] != v._f32[0] )
		{
			return false;
		}
		if( _f32[1] != v._f32[1] )
		{
			return false;
		}
		if( _f32[2] != v._f32[2] )
		{
			return false;
		}
		if( _f32[3] != v._f32[3] )
		{
			return false;
		}
		return true;
	}

	bool	operator!=( const kkVector4& v ) const 
	{
		if( _f32[0] != v._f32[0] )
		{
			return true;
		}
		if( _f32[1] != v._f32[1] )
		{
			return true;
		}
		if( _f32[2] != v._f32[2] )
		{
			return true;
		}
		if( _f32[3] != v._f32[3] )
		{
			return true;
		}
		return false;
	}

	f32&       operator[]( u32 i )
	{
		KK_ASSERT(i<4);
		return this->_f32[i];
	}
		
	const f32& operator[]( u32 i ) const 
	{ 
		KK_ASSERT(i<4);
		return this->_f32[i];
	}

	void fill(f32 v)
	{
		_f32[ 0 ] = v;
		_f32[ 1 ] = v;
		_f32[ 2 ] = v;
		_f32[ 3 ] = v;
	}

	void	normalize2(bool w_zero = false)
	{
		if( w_zero ) _f32[3] = 0.f;
		f32 len = std::sqrt(dot());
		if(len > 0)
		{
			len = 1.0f/len;
		}
		_f32[0] *= len;
		_f32[1] *= len;
		_f32[2] *= len;
		_f32[3] *= len;
	}

	f32	dot(const kkVector4& V2)const
	{
#ifdef KK_USE_SIMD
		kkVector4 vTemp2 = V2;
		kkVector4 vTemp  = _mm_mul_ps(_vec,vTemp2._vec);
		vTemp2 = _mm_shuffle_ps(vTemp2._vec,vTemp._vec,_MM_SHUFFLE(1,0,0,0));
		vTemp2 = _mm_add_ps(vTemp2._vec,vTemp._vec);
		vTemp  = _mm_shuffle_ps(vTemp._vec,vTemp2._vec,_MM_SHUFFLE(0,3,0,0));
		vTemp  = _mm_add_ps(vTemp._vec,vTemp2._vec);
		kkVector4 r = _mm_shuffle_ps(vTemp._vec,vTemp._vec,_MM_SHUFFLE(2,2,2,2));
		return r._f32[0];
#else
		return (KK_X * V2.KK_X ) + ( KK_Y * V2.KK_Y ) + ( KK_Z * V2.KK_Z ) + ( KK_W * V2.KK_W );
#endif
	}

	f32	dot()const
	{
#ifdef KK_USE_SIMD
		kkVector4 vTemp2 = kkVector4(_f32[0],_f32[1],_f32[2],_f32[3]);
		kkVector4 vTemp  = _mm_mul_ps(_vec,vTemp2._vec);
		vTemp2 = _mm_shuffle_ps(vTemp2._vec,vTemp._vec,_MM_SHUFFLE(1,0,0,0));
		vTemp2 = _mm_add_ps(vTemp2._vec,vTemp._vec);
		vTemp  = _mm_shuffle_ps(vTemp._vec,vTemp2._vec,_MM_SHUFFLE(0,3,0,0));
		vTemp  = _mm_add_ps(vTemp._vec,vTemp2._vec);
		kkVector4 r = _mm_shuffle_ps(vTemp._vec,vTemp._vec,_MM_SHUFFLE(2,2,2,2));
		return r._f32[0];
#else
		return ( KK_X * KK_X ) + (KK_Y * KK_Y ) + ( KK_Z * KK_Z ) + ( KK_W * KK_W );
#endif
	}

	void cross( const kkVector4& a, kkVector4& out )const{
		out.KK_X = (KK_Y * a.KK_Z) - (KK_Z * a.KK_Y);
		out.KK_Y = (KK_Z * a.KK_X) - (KK_X * a.KK_Z);
		out.KK_Z = (KK_X * a.KK_Y) - (KK_Y * a.KK_X);
	}

	kkVector4 cross_return( const kkVector4& a )const{
		kkVector4 r;
		r.KK_X = (KK_Y * a.KK_Z) - (KK_Z * a.KK_Y);
		r.KK_Y = (KK_Z * a.KK_X) - (KK_X * a.KK_Z);
		r.KK_Z = (KK_X * a.KK_Y) - (KK_Y * a.KK_X);
		return r;
	}

	f32 distance( const kkVector4& from )const
	{ 
		return kkVector4( _f32[0] - from._f32[0], _f32[1] - from._f32[1], _f32[2] - from._f32[2] ).length(); 
	}

	f32 lengthSqrt()const
	{
		return ( _f32[0] * _f32[0] ) + ( _f32[1] * _f32[1] ) + ( _f32[2] * _f32[2] ); 
	}

	f32 length()const
	{
		return std::sqrt( lengthSqrt() ); 
	}


	kkVector4	normalize(bool w_zero = false);
};

namespace math
{
	const kkVector4 v_Infinity(0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000);
	const kkVector4 v_NaN(0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000);
}

KK_FORCE_INLINE kkVector4 operator*(const f32& s, const kkVector4& v)
{
	return v * s;
}

KK_FORCE_INLINE std::ostream& operator<<(std::ostream& os, const kkVector4& v)
{
	os << v._f32[0] << '/' << v._f32[1] << '/' << v._f32[2];
    return os;
}

KK_FORCE_INLINE kkVector4	kkVector4::normalize(bool w_zero)
{
#ifdef KK_USE_SIMD
	kkVector4 vLengthSq = _mm_mul_ps(_vec,_vec);
	kkVector4 vTemp     = _mm_shuffle_ps(vLengthSq._vec,vLengthSq._vec,_MM_SHUFFLE(3,2,3,2));
	vLengthSq = _mm_add_ps(vLengthSq._vec,vTemp._vec);
	vLengthSq = _mm_shuffle_ps(vLengthSq._vec,vLengthSq._vec,_MM_SHUFFLE(1,0,0,0));
	vTemp = _mm_shuffle_ps(vTemp._vec,vLengthSq._vec,_MM_SHUFFLE(3,3,0,0));
	vLengthSq = _mm_add_ps(vLengthSq._vec,vTemp._vec);
	vLengthSq = _mm_shuffle_ps(vLengthSq._vec,vLengthSq._vec,_MM_SHUFFLE(2,2,2,2));
	kkVector4 vResult = _mm_sqrt_ps(vLengthSq._vec);
	kkVector4 vZeroMask = _mm_setzero_ps();
	vZeroMask = _mm_cmpneq_ps(vZeroMask._vec,vResult._vec);

		

	vLengthSq = _mm_cmpneq_ps(vLengthSq._vec,math::v_Infinity._vec);
	vResult = _mm_div_ps(_vec,vResult._vec);
	vResult = _mm_and_ps(vResult._vec,vZeroMask._vec);
	kkVector4 vTemp1 = _mm_andnot_ps(vLengthSq._vec,math::v_NaN._vec);
	kkVector4 vTemp2 = _mm_and_ps(vResult._vec,vLengthSq._vec);
	vResult = _mm_or_ps(vTemp1._vec,vTemp2._vec);
	return vResult;
#else
	if( w_zero ) _f32[3] = 0.f;
	f32 len = std::sqrt(dot());
	if(len > 0)
	{
		len = 1.0f/len;
	}
	_f32[0] *= len;
	_f32[1] *= len;
	_f32[2] *= len;
	_f32[3] *= len;
	return kkVector4(_f32[0],_f32[1],_f32[2],_f32[3]);
#endif
}



template<typename _type>
class kkv2
{
public:
	kkv2()
	{
	}

	kkv2( _type _x, _type _y )
		:
		x(_x),
		y(_y)
	{
	}

	template<typename other_type>
	kkv2( other_type _x, other_type _y )
		:
		x( static_cast<_type>(_x) ),
		y( static_cast<_type>(_y) )
	{
	}

	template<typename other_type>
	kkv2( other_type _v )
		:
		x( static_cast<_type>(_v) ),
		y( static_cast<_type>(_v) )
	{
	}

	kkv2( const kkv2& o )
		:
		x(o.x),
		y(o.y)
	{
	}

	kkv2( kkv2&& o ) noexcept
		:
		x(std::move(o.x)),
		y(std::move(o.y))
	{
	}

	~kkv2()
	{
	}

	kkv2& operator=( _type v )
	{
		x = v;
		y = v;

		return *this;
	}

	kkv2& operator=( const kkv2& o)
	{
		x = o.x;
		y = o.y;

		return *this;
	}

	kkv2& operator=(kkv2&& o)
	{
		x = std::move(o.x);
		y = std::move(o.y);

		return *this;
	}

	void set( _type _x, _type _y )
	{
		x = _x;
		y = _y;
	}

	kkv2 operator+( const kkv2& v ) const
	{
		kkv2 r;
		r.x = x + v.x;
		r.y = y + v.y;
		return r;
	}

	kkv2 operator-( const kkv2& v ) const
	{
		kkv2 r;
		r.x = x - v.x;
		r.y = y - v.y;
		return r;
	}

	bool	operator==( const kkv2& v ) const 
	{
		if( x != v.x )
		{
			return false;
		}
		if( y != v.y )
		{
			return false;
		}
		return true;
	}

	f32 lengthSqrt()const
	{
		return ( (f32)x * (f32)x ) + ( (f32)y * (f32)y ); 
	}

	f32 length()const
	{
		return std::sqrt( lengthSqrt() ); 
	}

	_type distance( const kkv2& from )const
	{ 
		return static_cast<_type>(kkv2( x - from.x, y - from.y ).length()); 
	}


	_type x = static_cast<_type>(0);
	_type y = static_cast<_type>(0);
};

using v2_s16 = kkv2<s16>;
using v2i = kkv2<s32>;
using v2u = kkv2<u32>;
using v2f = kkv2<f32>;

template<typename _type>
class kkv3
{
public:
	kkv3()
	{
	}

	kkv3( _type _v )
		:
		x(_v),
		y(_v),
		z(_v)
	{
	}

	kkv3( _type _x, _type _y, _type _z )
		:
		x(_x),
		y(_y),
		z(_z)
	{
	}

	kkv3( const kkVector4& v )
		:
		x(v._f32[0]),
		y(v._f32[1]),
		z(v._f32[2])
	{
	}

	~kkv3()
	{
	}

	void set( _type _x, _type _y, _type _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}

	_type * data() 
	{
		return &x;
	}
	
	kkv3 operator-( const kkv3& v ) const
	{
		kkv3 r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		return r;
	}

	f32	dot(const kkv3& V2)const
	{
		return (x* V2.x ) + ( y * V2.y ) + ( z * V2.z );
	}
	f32	dot()const
	{
		return (x*x ) + ( y*y ) + ( z*z );
	}

	kkv3 normalize()
	{
		f32 len = std::sqrt(dot());
		if(len > 0)
			len = 1.0f/len;
		x *= len;
		y *= len;
		z *= len;
		return kkv3(x,y,z);
	}

	void	normalize2()
	{
		f32 len = std::sqrt(dot());
		if(len > 0)
		{
			len = 1.0f/len;
		}
		x *= len;
		y *= len;
		z *= len;
	}

	kkv3 operator+( const kkv3& v ) const
	{
		kkv3 r;
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		return r;
	}

	kkv3 operator/( const kkv3& v ) const
	{
		kkv3 r;
		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;
		return r;
	}

	kkv3 operator*( const kkv3& v ) const
	{
		kkv3 r;
		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;
		return r;
	}

	kkv3 cross_return( const kkv3& a )const{
		kkv3 r;
		r.x = (y * a.z) - (z * a.y);
		r.y = (z * a.x) - (x * a.z);
		r.z = (x * a.y) - (y * a.x);
		return r;
	}

	f32 distance( const kkv3& from )const
	{ 
		return kkv3( x - from.x, y - from.y, z - from.z ).length(); 
	}

	f32 lengthSqrt()const
	{
		return ( x * x ) + ( y * y ) + ( z * z ); 
	}

	f32 length()const
	{
		return std::sqrt( lengthSqrt() ); 
	}

	_type x = static_cast<_type>(0);
	_type y = static_cast<_type>(0);
	_type z = static_cast<_type>(0);
};


using v3i = kkv3<s32>;
using v3u = kkv3<u32>;
using v3f = kkv3<f32>;

KK_FORCE_INLINE v3f operator*(const f32& s, const v3f& v){ return v * s; }
KK_FORCE_INLINE v3f operator/(const f32& s, const v3f& v){ return v / s; }

template<typename _type>
class kkv4
{
public:
	kkv4()
	{
	}

	kkv4( const kkVector4& v )
		:
		x(v.KK_X),
		y(v.KK_Y),
		z(v.KK_Z),
		w(v.KK_W)
	{
	}

	kkv4( _type _x, _type _y, _type _z, _type _w )
		:
		x(_x),
		y(_y),
		z(_z),
		w(_w)
	{
	}

	kkv4( _type _x, _type _y, _type _z )
		:
		x(_x),
		y(_y),
		z(_z),
		w(1)
	{
	}

	~kkv4()
	{
	}

	void set( _type _x, _type _y, _type _z, _type _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	kkv2<_type> getWidthAndHeight() const
	{
		return kkv2<_type>(z-x,w-y);
	}

	_type * data() 
	{
		return &x;
	}

	kkv4<_type> operator+( const kkv4<_type>& v ) const
	{
		kkv4<_type> r;
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		r.w = w + v.w;
		return r;
	}

	kkv4<_type> operator-( const kkv4<_type>& v ) const
	{
		kkv4<_type> r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		r.w = w - v.w;
		return r;
	}

	_type x = static_cast<_type>(0);
	_type y = static_cast<_type>(0);
	_type z = static_cast<_type>(0);
	_type w = static_cast<_type>(0);
};

using v4i = kkv4<s32>;
using v4u = kkv4<u32>;
using v4f = kkv4<f32>;

namespace math
{

	/*template<typename T>
	T planeDotCoord( const T& coord, const v4f& plane ){
		return T( plane.x * coord.x + plane.y * coord.y + plane.z * coord.z + plane.w );
	}

	template<typename T>
	T getTriNormal( const T& v1, const T& v2, const T& v3 ){
		T U, V;
		U = v2 - v1;
		V = v3 - v1;

		return T( (U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x) );
	}*/
}

#endif