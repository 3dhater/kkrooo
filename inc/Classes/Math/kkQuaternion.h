// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_QUATERNION_H__
#define __KK_QUATERNION_H__

#include <cmath>
#include "kkVector4.h"

	//	quaternion
class kkQuaternion
{
public:

	f32 x, y, z, w;

	kkQuaternion()
	{
		identity();
	}

	kkQuaternion( const kkQuaternion& o ):
		x(o.x),
		y(o.y),
		z(o.z),
		w(o.w)
	{
	}

	kkQuaternion( f32 i1, f32 i2, f32 i3, f32 real )
		:
		x(i1),
		y(i2),
		z(i3),
		w(real)
	{
	}

		//	create quaternion from Euler angles
	kkQuaternion( f32 Euler_x, f32 Euler_y, f32 Euler_z )
	{
		set( Euler_x, Euler_y, Euler_z );
	}

		//	create quaternion from Euler angles
	kkQuaternion( const kkVector4& v ){ set( v ); }

		//	get quaternion in kkVector4f form
	kkVector4	get() const { return kkVector4( x, y, z, w ); }

		//	get real part
	f32	getReal() const { return w; }

		//	set quaternion with components
		//	\param i1: imagine1, first component
		//	\param i2: imagine2, second component
		//	\param i3: imagine3, third component
		//	\param real: real part, fourth component
	void set( f32 i1, f32 i2, f32 i3, f32 real ){
		x = i1;
		y = i2;
		z = i3;
		w = real;
	}

		//	set quaternion from Euler angles
	void set( const kkVector4& v ){ set( v.getX(), v.getY(), v.getZ() ); }

		//	set quaternion from Euler angles
		// \param Euler_x: x angle
		// \param Euler_y: y angle
		// \param Euler_z: z angle
	void set( f32 Euler_x, f32 Euler_y, f32 Euler_z ){
		f32 c1 = cos( Euler_x * 0.5f );
		f32 c2 = cos( Euler_y * 0.5f );
		f32 c3 = cos( Euler_z * 0.5f );
		f32 s1 = sin( Euler_x * 0.5f );
		f32 s2 = sin( Euler_y * 0.5f );
		f32 s3 = sin( Euler_z * 0.5f );
		w	=	(c1 * c2 * c3) + (s1 * s2 * s3);
		x	=	(s1 * c2 * c3) - (c1 * s2 * s3);
		y	=	(c1 * s2 * c3) + (s1 * c2 * s3);
		z	=	(c1 * c2 * s3) - (s1 * s2 * c3);
	}

		//	set real part
		//	\param r: real part
	void setReal( f32 r ){
		w = r;
	}

		//	reset quaternion
	void identity(){
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 1.f;
	}

		//	assing other
		//	\return this quaternion
	kkQuaternion& operator=( const kkQuaternion& o ){
		x = o.x;
		y = o.y;
		z = o.z;
		w = o.w;
		return *this;
	}

		//	multiplication with other
		//	\param q: other quaternion
		//	\return new quaternion
	kkQuaternion operator*( const kkQuaternion& q )const{
		return kkQuaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

		//	multiplication with other
		//	\param q: other quaternion
	void operator*=( const kkQuaternion& q ){
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
	}

		//	compare
		//	\param q: other quaternion
		// \return \b true if not equal
	bool operator!=( const kkQuaternion& q )const{
		if( x != q.x ) return true;
		if( y != q.y ) return true;
		if( z != q.z ) return true;
		if( w != q.w ) return true;
		return false;
	}

		//	compare
		//	\param q: other quaternion
		// \return \b true if equal
	bool operator==( const kkQuaternion& q )const{
		if( x != q.x ) return false;
		if( y != q.y ) return false;
		if( z != q.z ) return false;
		if( w != q.w ) return false;
		return true;
	}

		//	add other
		//	\param q: other quaternion
		// \return new quaternion
	kkQuaternion operator+( const kkQuaternion& o ) const {
		return kkQuaternion(
			x + o.x,
			y + o.y,
			z + o.z,
			w + o.w );
	}

		//	substract other
		//	\param q: other quaternion
		// \return new quaternion
	kkQuaternion operator-( const kkQuaternion& o ) const {
		return kkQuaternion(
			x - o.x,
			y - o.y,
			z - o.z,
			w - o.w );
	}

		//	invert
		// \return new quaternion
	kkQuaternion operator-(){
		x = -x;
		y = -y;
		z = -z;
		return kkQuaternion( x, y, z, w );
	}

		//	length
		// \return length
	f32	getLength()
	{
		return std::sqrt( x*x+y*y+z*z+w*w );
	}

		//	normalize
		// \return normalized quaternion
	kkQuaternion& normalize(){
		f32 len = this->getLength();
		if( len ){
			f32 len2 = 1.f / len;
			x *= len2;
			y *= len2;
			z *= len2;
			w *= len2;
		}
		return *this;
	}

		//	invert
	void invert()
	{
		x = -x;
		y = -y;
		z = -z;
	}

};

#endif