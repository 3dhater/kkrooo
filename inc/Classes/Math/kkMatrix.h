// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_MAT_H__
#define __KKROO_MAT_H__

#include <cmath>

#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkQuaternion.h"

class kkMatrix4
{
	kkVector4 m_data[ 4u ]; //< components

public:

		// c-tor
	kkMatrix4(){
		identity();
	}

		// c-tor with other matrix
	kkMatrix4( const kkMatrix4& m ){
		*this = m;
	}

		//	fill matrix with one value
	kkMatrix4( f32 v ){
		fill( v );
	}

		//	set matrix 4 vectors
	kkMatrix4( const kkVector4& x, const kkVector4& y, const kkVector4& z, const kkVector4& w ){
		m_data[ 0u ] = x;
		m_data[ 1u ] = y;
		m_data[ 2u ] = z;
		m_data[ 3u ] = w;
	}

		//	fill matrix with one value
	void		fill( f32 v ){
		m_data[ 0u ].fill( v );
		m_data[ 1u ].fill( v );
		m_data[ 2u ].fill( v );
		m_data[ 3u ].fill( v );
	}

		// set all components = 0
	void		zero(){
		fill( 0.f );
	}

	void		identity(){
		auto * p = this->getPtr();
		p[ 0u ] = 1.f;
		p[ 1u ] = 0.f;
		p[ 2u ] = 0.f;
		p[ 3u ] = 0.f;

		p[ 4u ] = 0.f;
		p[ 5u ] = 1.f;
		p[ 6u ] = 0.f;
		p[ 7u ] = 0.f;

		p[ 8u ] = 0.f;
		p[ 9u ] = 0.f;
		p[ 10u ] = 1.f;
		p[ 11u ] = 0.f;

		p[ 12u ] = 0.f;
		p[ 13u ] = 0.f;
		p[ 14u ] = 0.f;
		p[ 15u ] = 1.f;

	}

	f32 * getPtr(){ return reinterpret_cast<f32*>(&m_data); }
		
	kkVector4&       operator[]( u32 i ){ return m_data[ i ]; }
	const kkVector4& operator[]( u32 i ) const { return m_data[ i ]; }

		//	add
		// \param m: other matrix
		// \return new matrix
	kkMatrix4 operator+( const kkMatrix4& m ) const {
		kkMatrix4 out = *this;

		out[ 0u ] += m[ 0u ];
		out[ 1u ] += m[ 1u ];
		out[ 2u ] += m[ 2u ];
		out[ 3u ] += m[ 3u ];

		return out;
	}

		//	substract
		// \param m: other matrix
		// \return new matrix
	kkMatrix4 operator-( const kkMatrix4& m ) const {
		kkMatrix4 out = *this;

		out[ 0u ] -= m[ 0u ];
		out[ 1u ] -= m[ 1u ];
		out[ 2u ] -= m[ 2u ];
		out[ 3u ] -= m[ 3u ];

		return out;
	}

		//	multiplication
		// \param m: other matrix
		// \return new matrix
	kkMatrix4 operator*( const kkMatrix4& m ) const
	{
		return kkMatrix4(
			m_data[ 0u ] * m[ 0u ].KK_X + m_data[ 1u ] * m[ 0u ].KK_Y + m_data[ 2u ] * m[ 0u ].KK_Z + m_data[ 3u ] * m[ 0u ].KK_W,
			m_data[ 0u ] * m[ 1u ].KK_X + m_data[ 1u ] * m[ 1u ].KK_Y + m_data[ 2u ] * m[ 1u ].KK_Z + m_data[ 3u ] * m[ 1u ].KK_W,
			m_data[ 0u ] * m[ 2u ].KK_X + m_data[ 1u ] * m[ 2u ].KK_Y + m_data[ 2u ] * m[ 2u ].KK_Z + m_data[ 3u ] * m[ 2u ].KK_W,
			m_data[ 0u ] * m[ 3u ].KK_X + m_data[ 1u ] * m[ 3u ].KK_Y + m_data[ 2u ] * m[ 3u ].KK_Z + m_data[ 3u ] * m[ 3u ].KK_W
		);
	}

	// возможно тут нужно по другому.
	kkVector4 operator*( const kkVector4& v ) const {
		return kkVector4
		(
			v.KK_X * m_data[0].KK_X + v.KK_Y * m_data[1].KK_X + v.KK_Z * m_data[2].KK_X + v.KK_W * m_data[2].KK_X,
			v.KK_X * m_data[0].KK_Y + v.KK_Y * m_data[1].KK_Y + v.KK_Z * m_data[2].KK_Y + v.KK_W * m_data[2].KK_Y,
			v.KK_X * m_data[0].KK_Z + v.KK_Y * m_data[1].KK_Z + v.KK_Z * m_data[2].KK_Z + v.KK_W * m_data[2].KK_Z,
			v.KK_X * m_data[0].KK_W + v.KK_Y * m_data[1].KK_W + v.KK_Z * m_data[2].KK_W + v.KK_W * m_data[2].KK_W
		);
	}

		//	divide
		// \param m: other matrix
		// \return new matrix
	kkMatrix4 operator/( const kkMatrix4& m ) const {
		kkMatrix4 out = *this;

		out[ 0u ] /= m[ 0u ];
		out[ 1u ] /= m[ 1u ];
		out[ 2u ] /= m[ 2u ];
		out[ 3u ] /= m[ 3u ];

		return out;
	}

		//	add
		// \param m: other matrix
		// \return this matrix
	kkMatrix4& operator+=( const kkMatrix4& m ){
		m_data[ 0u ] += m[ 0u ];
		m_data[ 1u ] += m[ 1u ];
		m_data[ 2u ] += m[ 2u ];
		m_data[ 3u ] += m[ 3u ];
		return *this;
	}

		//	substract
		// \param m: other matrix
		// \return this matrix
	kkMatrix4& operator-=( const kkMatrix4& m ){
		m_data[ 0u ] -= m[ 0u ];
		m_data[ 1u ] -= m[ 1u ];
		m_data[ 2u ] -= m[ 2u ];
		m_data[ 3u ] -= m[ 3u ];
		return *this;
	}

		//	multiplication
		// \param m: other matrix
		// \return this matrix
	kkMatrix4& operator*=( const kkMatrix4& m ){
		(*this) = (*this) * m;
		return *this;
	}

		//	divide
		// \param m: other matrix
		// \return this matrix
	kkMatrix4& operator/=( const kkMatrix4& m ){
		m_data[ 0u ] /= m[ 0u ];
		m_data[ 1u ] /= m[ 1u ];
		m_data[ 2u ] /= m[ 2u ];
		m_data[ 3u ] /= m[ 3u ];
		return *this;
	}

		//	transpose
	void transpose()
	{
		kkMatrix4 tmp;
		tmp[ 0u ].setX( this->m_data[ 0u ].getX() ); //0
		tmp[ 0u ].setY( this->m_data[ 1u ].getX() ); //1
		tmp[ 0u ].setZ( this->m_data[ 2u ].getX() ); //2
		tmp[ 0u ].setW( this->m_data[ 3u ].getX() ); //3

		tmp[ 1u ].setX( this->m_data[ 0u ].getY() ); //4
		tmp[ 1u ].setY( this->m_data[ 1u ].getY() ); //5
		tmp[ 1u ].setZ( this->m_data[ 2u ].getY() ); //6
		tmp[ 1u ].setW( this->m_data[ 3u ].getY() ); //7

		tmp[ 2u ].setX( this->m_data[ 0u ].getZ() ); //8
		tmp[ 2u ].setY( this->m_data[ 1u ].getZ() ); //9
		tmp[ 2u ].setZ( this->m_data[ 2u ].getZ() ); //10
		tmp[ 2u ].setW( this->m_data[ 3u ].getZ() ); //11

		tmp[ 3u ].setX( this->m_data[ 0u ].getW() ); //12
		tmp[ 3u ].setY( this->m_data[ 1u ].getW() ); //13
		tmp[ 3u ].setZ( this->m_data[ 2u ].getW() ); //14
		tmp[ 3u ].setW( this->m_data[ 3u ].getW() ); //15
		this->m_data[ 0u ] = tmp[ 0u ];
		this->m_data[ 1u ] = tmp[ 1u ];
		this->m_data[ 2u ] = tmp[ 2u ];
		this->m_data[ 3u ] = tmp[ 3u ];
	}

	bool invert()
	{
		kkMatrix4 mat; 
		auto ptr = this->getPtr();
		for(unsigned column = 0; column < 4; ++column) 
		{ 
			// Swap row in case our pivot point is not working
			if(m_data[column]._f32[column] == 0)
			{ 
				unsigned big = column; 
				for (unsigned row = 0; row < 4; ++row) 
					if(fabs(m_data[row]._f32[column]) > fabs(m_data[big]._f32[column]))
						big = row; 
				// Print this is a singular matrix, return identity ?
				if (big == column)
					fprintf(stderr, "Singular matrix\n"); 
				// Swap rows                               
				else for (unsigned j = 0; j < 4; ++j) 
				{ 
					std::swap(m_data[column]._f32[j], m_data[big]._f32[j]); 
					std::swap(mat.m_data[column]._f32[j], mat.m_data[big]._f32[j]); 
				} 
			} 

			// Set each row in the column to 0  
			for (unsigned row = 0; row < 4; ++row)
			{ 
				if (row != column) 
				{ 
					f32 coeff = m_data[row]._f32[column] / m_data[column]._f32[column]; 
					if (coeff != 0) 
					{ 
						for (unsigned j = 0; j < 4; ++j) 
						{ 
							m_data[row]._f32[j] -= coeff * m_data[column]._f32[j]; 
							mat.m_data[row]._f32[j] -= coeff * mat.m_data[column]._f32[j]; 
						} 
						// Set the element to 0 for safety
						m_data[row]._f32[column] = 0; 
					} 
				} 
			}
		} 

		// Set each element of the diagonal to 1
		for (unsigned row = 0; row < 4; ++row) 
		{ 
			for (unsigned column = 0; column < 4; ++column) 
			{ 
				mat.m_data[row]._f32[column] /= m_data[row]._f32[row]; 
			} 
		} 

		*this = mat;
		return true;
	}

};

namespace math
{

		//	create perspective matrix for left hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
	KK_FORCE_INLINE void  makePerspectiveLHMatrix( kkMatrix4& out, f32 FOV, f32 aspect,
		f32 Near, f32 Far)
	{
		f32 S	=	std::sin( 0.5f * FOV );
		f32 C	=	std::cos( 0.5f * FOV );
		f32 H = C / S;
		f32 W = H / aspect;
		out[ 0 ] = kkVector4( W, 0.f, 0.f, 0.f );
		out[ 1 ] = kkVector4( 0.f, H, 0.f, 0.f );
		out[ 2 ] = kkVector4( 0.f, 0.f, Far/(Far-Near), 1.f );
		out[ 3 ] = kkVector4( 0.f, 0.f, -out[2].KK_Z * Near, 0.f );
	}

		//	create perspective matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
	KK_FORCE_INLINE void  makePerspectiveRHMatrix( kkMatrix4& out, f32 FOV, f32 aspect,
		f32 Near, f32 Far)
	{
		f32 S	=	std::sin( 0.5f * FOV );
		f32 C	=	std::cos( 0.5f * FOV );
		f32 H = C / S;
		f32 W = H / aspect;
		out[ 0u ] = kkVector4( W, 0.f, 0.f, 0.f );
		out[ 1u ] = kkVector4( 0.f, H, 0.f, 0.f );
		out[ 2u ] = kkVector4( 0.f, 0.f, Far / (Near - Far), -1.f );
		out[ 3u ] = kkVector4( 0.f, 0.f, out[ 2u ].KK_Z * Near, 0.f );
	}

		//	create orthogonal matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param width: width of viewport
		// \param height: height of viewport
		// \param Near: near clip plane
		// \param Far: far clip plane
	KK_FORCE_INLINE void  makeOrthoRHMatrix( kkMatrix4& out, f32 width, f32 height,
		f32 Near, f32 Far )
	{
		out[ 0u ] = kkVector4( 2.f / width, 0.f, 0.f, 0.f );
		out[ 1u ] = kkVector4( 0.f, 2.f / height, 0.f, 0.f );
		out[ 2u ] = kkVector4( 0.f, 0.f, 1.f / (Near - Far), 0.f );
		out[ 3u ] = kkVector4( 0.f, 0.f, out[ 2u ].KK_Z * Near, 1.f );
	}
		
		//	create \a look \a at matrix for right hand coordinate system
		// \param in_out: view matrix
		// \param eye: camera position
		// \param center: camera target
		// \param up: up vector
	KK_FORCE_INLINE void  makeLookAtRHMatrix( const kkVector4& eye, const kkVector4& center, const kkVector4& up, kkMatrix4& out )
	{
		kkVector4 f( center - eye );
		f.normalize();
			
		kkVector4 s;
		f.cross( up, s );
		s.normalize();

		kkVector4 u;
		s.cross( f, u );

		out.identity();

		out[ 0u ].KK_X = s.KK_X;
		out[ 1u ].KK_X = s.KK_Y;
		out[ 2u ].KK_X = s.KK_Z;
		out[ 0u ].KK_Y = u.KK_X;
		out[ 1u ].KK_Y = u.KK_Y;
		out[ 2u ].KK_Y = u.KK_Z;
		out[ 0u ].KK_Z = -f.KK_X;
		out[ 1u ].KK_Z = -f.KK_Y;
		out[ 2u ].KK_Z = -f.KK_Z;
		out[ 3u ].KK_X = -s.dot( eye );
		out[ 3u ].KK_Y = -u.dot( eye );
		out[ 3u ].KK_Z = f.dot( eye );
	}

	KK_FORCE_INLINE void  makeLookAtLHMatrix( const kkVector4& eye, const kkVector4& center,const kkVector4& up, kkMatrix4& out)
	{
		kkVector4 f( center - eye );
		f.normalize();
			
		kkVector4 s;
		f.cross( up, s );
		s.normalize();

		kkVector4 u;
		s.cross( f, u );

		out.identity();

		out[ 0u ].KK_X = s.KK_X;
		out[ 1u ].KK_X = s.KK_Y;
		out[ 2u ].KK_X = s.KK_Z;
		out[ 0u ].KK_Y = u.KK_X;
		out[ 1u ].KK_Y = u.KK_Y;
		out[ 2u ].KK_Y = u.KK_Z;
		out[ 0u ].KK_Z = f.KK_X;
		out[ 1u ].KK_Z = f.KK_Y;
		out[ 2u ].KK_Z = f.KK_Z;
		out[ 3u ].KK_X = -s.dot( eye );
		out[ 3u ].KK_Y = -u.dot( eye );
		out[ 3u ].KK_Z = -f.dot( eye );
	}

	KK_FORCE_INLINE void makeTranslationMatrix( const kkVector4& position, kkMatrix4& out )
	{
		out[ 3u ].KK_X = position.KK_X;
		out[ 3u ].KK_Y = position.KK_Y;
		out[ 3u ].KK_Z = position.KK_Z;
	}

	KK_FORCE_INLINE void makeScaleMatrix( const kkVector4& scale, kkMatrix4& out )
	{
		out[ 0u ].KK_X = scale.KK_X;
		out[ 1u ].KK_Y = scale.KK_Y;
		out[ 2u ].KK_Z = scale.KK_Z;
	}

	KK_FORCE_INLINE void makeRotationMatrix( kkMatrix4& out, const kkQuaternion& p ){
			
		f32	wx, wy, wz;
		f32	xx, yy, yz;
		f32	xy, xz, zz;
		f32	x2, y2, z2;

		x2 = p.x + p.x;
		y2 = p.y + p.y;
		z2 = p.z + p.z;

		xx = p.x * x2;
		xy = p.x * y2;
		xz = p.x * z2;

		yy = p.y * y2;
		yz = p.y * z2;
		zz = p.z * z2;

		wx = p.w * x2;
		wy = p.w * y2;
		wz = p.w * z2;

		out[ 0u ].KK_X = ( 1.f - ( yy + zz ) );
		out[ 0u ].KK_Y = xy - wz;
		out[ 0u ].KK_Z = xz + wy;

		out[ 1u ].KK_X = ( xy + wz);
		out[ 1u ].KK_Y = ( 1.f - ( xx + zz ));
		out[ 1u ].KK_Z = ( yz - wx);

		out[ 2u ].KK_X = ( xz - wy);
		out[ 2u ].KK_Y = ( yz + wx);
		out[ 2u ].KK_Z = ( 1.f - ( xx + yy ));

		out[ 0u ].KK_W = 0.f;
		out[ 1u ].KK_W = 0.f;
		out[ 2u ].KK_W = 0.f;
		out[ 3u ].KK_X = 0.f;
		out[ 3u ].KK_Y = 0.f;
		out[ 3u ].KK_Z = 0.f;
		out[ 3u ].KK_W = 1.f;
	}

		//Vector-matrix product 
	KK_FORCE_INLINE kkVector4 mul( const kkVector4& vec, const kkMatrix4& mat )
	{
		return kkVector4
		(
			mat[ 0u ].KK_X * vec.KK_X + mat[ 1u ].KK_X * vec.KK_Y + mat[ 2u ].KK_X * vec.KK_Z + mat[ 3u ].KK_X * vec.KK_W,
			mat[ 0u ].KK_Y * vec.KK_X + mat[ 1u ].KK_Y * vec.KK_Y + mat[ 2u ].KK_Y * vec.KK_Z + mat[ 3u ].KK_Y * vec.KK_W,
			mat[ 0u ].KK_Z * vec.KK_X + mat[ 1u ].KK_Z * vec.KK_Y + mat[ 2u ].KK_Z * vec.KK_Z + mat[ 3u ].KK_Z * vec.KK_W,
			mat[ 0u ].KK_W * vec.KK_X + mat[ 1u ].KK_W * vec.KK_Y + mat[ 2u ].KK_W * vec.KK_Z + mat[ 3u ].KK_W * vec.KK_W
		);
	}

	KK_FORCE_INLINE v3f mul( const v3f& vec, const kkMatrix4& mat )
	{
		return v3f
		(
			mat[ 0u ].KK_X * vec.x + mat[ 1u ].KK_X * vec.y + mat[ 2u ].KK_X * vec.z + mat[ 3u ].KK_X,
			mat[ 0u ].KK_Y * vec.x + mat[ 1u ].KK_Y * vec.y + mat[ 2u ].KK_Y * vec.z + mat[ 3u ].KK_Y,
			mat[ 0u ].KK_Z * vec.x + mat[ 1u ].KK_Z * vec.y + mat[ 2u ].KK_Z * vec.z + mat[ 3u ].KK_Z
		);
	}

		//Matrix-vector product
	/*KK_FORCE_INLINE kkVector4 mul( const kkMatrix4& mat, const kkVector4& vec )
	{
		return kkVector4
		(
			mat[ 0u ].KK_X * vec.KK_X + mat[ 0u ].KK_Y * vec.KK_Y + mat[ 0u ].KK_Z * vec.KK_Z + mat[ 0u ].KK_W * vec.KK_W,
			mat[ 1u ].KK_X * vec.KK_X + mat[ 1u ].KK_Y * vec.KK_Y + mat[ 1u ].KK_Z * vec.KK_Z + mat[ 1u ].KK_W * vec.KK_W,
			mat[ 2u ].KK_X * vec.KK_X + mat[ 2u ].KK_Y * vec.KK_Y + mat[ 2u ].KK_Z * vec.KK_Z + mat[ 2u ].KK_W * vec.KK_W,
			mat[ 3u ].KK_X * vec.KK_X + mat[ 3u ].KK_Y * vec.KK_Y + mat[ 3u ].KK_Z * vec.KK_Z + mat[ 3u ].KK_W * vec.KK_W
		);
	}

	KK_FORCE_INLINE v3f mul( const kkMatrix4& mat, const v3f& vec )
	{
		return v3f
		(
			mat[ 0u ].KK_X * vec.x + mat[ 0u ].KK_Y * vec.y + mat[ 0u ].KK_Z * vec.z + mat[ 0u ].KK_W,
			mat[ 1u ].KK_X * vec.x + mat[ 1u ].KK_Y * vec.y + mat[ 1u ].KK_Z * vec.z + mat[ 1u ].KK_W,
			mat[ 2u ].KK_X * vec.x + mat[ 2u ].KK_Y * vec.y + mat[ 2u ].KK_Z * vec.z + mat[ 2u ].KK_W
		);
	}*/

}//end math::

#endif