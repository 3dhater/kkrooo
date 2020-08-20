#ifndef __KK_KRGUI_VEC_H__
#define __KK_KRGUI_VEC_H__

namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			constexpr float makeColorValue = 0.00392156862745f;
			inline int strLen( const char16_t* str ){ int result = 0; auto p = str; while((int)*p++) ++result; return result; }
		}
		
		template<typename T>
		struct Vec2
		{
			Vec2():x( static_cast<T>(0) ),y( static_cast<T>(0) ){}
			template<typename T1, typename T2>
			Vec2( T1 _x, T2 _y ):x( static_cast<T>(_x) ),y( static_cast<T>(_y) ){}

			template<typename T2>
			Vec2( const Vec2<T2>& other ):x( static_cast<T>(other.x) ),y( static_cast<T>(other.y) ){}

			template<typename T1, typename T2>
			void set(T1 _x, T2 _y){x = static_cast<T>(_x);y = static_cast<T>(_y);}

			template<typename T2>
			void operator*=( T2 v ){x *= v;y *= v;}

			Vec2 operator+( const Vec2& v ) const{return Vec2(x + v.x, y + v.y);}
			Vec2 operator-( const Vec2& v ) const{return Vec2(x - v.x, y - v.y);}
			Vec2 operator*( const Vec2& v ) const{return Vec2(x * v.x, y * v.y);}
			Vec2 operator/( const Vec2& v ) const{return Vec2(x / v.x, y / v.y);}

			template<typename T2>
			Vec2 operator+( T2 v ) const{return Vec2(x + static_cast<T>(v), y + static_cast<T>(v));}
			template<typename T2>
			Vec2 operator-( T2 v ) const{return Vec2(x - static_cast<T>(v), y - static_cast<T>(v));}
			template<typename T2>
			Vec2 operator*( T2 v ) const{return Vec2(x * static_cast<T>(v), y * static_cast<T>(v));}
			template<typename T2>
			Vec2 operator/( T2 v ) const{return Vec2(x / static_cast<T>(v), y / static_cast<T>(v));}

			void operator+=( const Vec2& o ){x += o.x;y += o.y;}
			void operator-=( const Vec2& o ){x -= o.x;y -= o.y;}


			T x, y;
		};
		using Vec2f = Vec2<float>;
		//using Vec2i = Vec2<int>;

		template<typename T>
		struct Vec4
		{
			Vec4():x( static_cast<T>(0) ),y( static_cast<T>(0) ),z( static_cast<T>(0) ),w( static_cast<T>(0) ){}

			template<typename T1, typename T2, typename T3, typename T4>
			Vec4( T1 _x, T2 _y, T3 _z, T4 _w ):x( static_cast<T>(_x) ),y( static_cast<T>(_y) ),z( static_cast<T>(_z) ),w( static_cast<T>(_w) ){}

			template<typename T2>
			Vec4( const Vec4<T2>& other ):x( static_cast<T>(other.x) ),y( static_cast<T>(other.y) ),z( static_cast<T>(other.z) ),w( static_cast<T>(other.w) ){}
			Vec4( unsigned int color ){makeColor(color);}
		
			T* data(){return reinterpret_cast<T*>(&x);}

			// ARGB 0xFF001122
			void makeColor( unsigned int v )
			{
				x = static_cast<T>(static_cast<unsigned char>( v >> 16u )) * _internal::makeColorValue;
				y = static_cast<T>(static_cast<unsigned char>( v >> 8u )) * _internal::makeColorValue;
				z = static_cast<T>(static_cast<unsigned char>( v )) * _internal::makeColorValue;
				w = static_cast<T>(static_cast<unsigned char>( v >> 24u )) * _internal::makeColorValue;
			}
			const unsigned char getAsByteRed() const   { return static_cast<unsigned char>( x * 255. ); }
			const unsigned char getAsByteGreen() const { return static_cast<unsigned char>( y * 255. ); }
			const unsigned char getAsByteBlue() const  { return static_cast<unsigned char>( z * 255. ); }
			const unsigned char getAsByteAlpha() const { return static_cast<unsigned char>( w * 255. ); }
			Vec4 operator-( const Vec4& o ) const{return Vec4(x-o.x,y-o.y,z-o.z,w-o.w);}
			Vec4 operator+( const Vec4& o ) const{return Vec4(x+o.x,y+o.y,z+o.z,w+o.w);}

			template<typename T2>
			void operator*=( T2 v ){x *= v;y *= v;z *= v;w *= v;}

			void operator+=( const Vec4& o ){x += o.x;y += o.y;z += o.z;w += o.w;}

			T x, y, z, w;
		};
		using Vec4f = Vec4<float>;
		//using Vec4i = Vec4<int>;
	}
}

#endif