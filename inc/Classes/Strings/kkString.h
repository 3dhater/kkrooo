// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKSTRING_H__
#define __KKSTRING_H__

#include "Classes/Common/kkAllocator.h"

#include <cstring>
#include <sstream>
#include <cstdarg>

#include <vector>

#pragma warning( disable : 4996 )



template<typename char_type, typename allocator>
class kkString_base
{
		
	using pointer = char_type*;
	using const_pointer = const char_type*;
	using reference = char_type&;
	using const_reference = const char_type&;
	using this_type = kkString_base;
	using this_const_reference = const kkString_base&;
	
	allocator m_allocator;

	u64 m_stringWordSize = 16u;

	pointer m_data = nullptr;
	u64     m_allocated = 16u;
	u64     m_size = 0;

	void reallocate( u64 new_allocated )
	{
		//char_type * new_data = (char_type*)malloc( new_allocated * sizeof(char_type) );
		char_type * new_data = (char_type*)m_allocator.allocate( new_allocated * sizeof(char_type) );
		if( m_data )
		{
			std::memcpy( new_data, m_data, m_size * sizeof( char_type ) );
		//	free( m_data );
			m_allocator.free( m_data );
		}
		else
		{
			std::memset(new_data, 0, new_allocated);
		}
		m_data = new_data;
		m_allocated = new_allocated;
	}

	template<typename other_type>
	u64 getlen( const other_type* str )
	{
		unsigned int len = 0u;
		if(str[0]!=0)
		{
			const other_type* p = &str[ 0u ];
			while( (u32)*p++ ) 
				len++;
		}
		return len;
	}

	template<typename dst_type, typename src_type>
	void copy( dst_type * dst, src_type* src ) const
	{
		while( (u32)*src )
		{
			*dst = static_cast<dst_type>(*src);
			dst++;
			src++;
		}
	}


public:
	using character_type = char_type;
	
	kkString_base()
	{
		reallocate( m_allocated );
	}

	template<typename other_type>
	kkString_base( const other_type * str )
	{
		reallocate( m_allocated );
		assign( str );
	}

	kkString_base( this_const_reference str )
	{
		reallocate( m_allocated );
		assign( str );
	}

	kkString_base( this_type&& str )
	{
		reallocate( m_allocated );
		assign( str );
	}

	kkString_base( char_type c )
	{
		u64 new_size = 1u;
		reallocate( (new_size + 1u) + m_stringWordSize );
		m_data[ 0u ] = c;
		m_size = new_size;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	~kkString_base()
	{
		if( m_data )
		{
			m_allocator.free( m_data );
			//free( m_data );
		}
	}

	void setWordSize( u64 v )
	{
		m_stringWordSize = v;
	}

	void reserve( u64 size )
	{
		if( size > m_allocated )
		{
			reallocate( size );
			m_data[ m_size ] = 0;
		}
	}

	template<typename other_type>
	void assign( other_type str )
	{
		m_size = 0u;
		m_data[ m_size ] = static_cast<char_type>(0x0);
		append( str );
	}

	void assign( this_const_reference str )
	{
		m_size = 0u;
		m_data[ m_size ] = static_cast<char_type>(0x0);
		append( str );
	}

	template<typename other_type>
	void append( const other_type * str )
	{
		u64 new_size = getlen( str ) + m_size;

		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + m_stringWordSize );

		copy( &m_data[m_size], str );

		m_size = new_size;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	void append( this_const_reference str )
	{
		append( str.data() );
	}

	void append( char_type c ){
		u64 new_size = m_size + 1u;
		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + m_stringWordSize );
		m_data[ m_size ] = c;
		m_size = new_size;
		m_data[ m_size ] = 0;
	}

	void append( s32 c )
	{
		char buf[32u];
		::std::snprintf( buf, 32, "%i", c );
		append( buf );
	}

	void append( u32 c )
	{
		char buf[32u];
		::std::snprintf( buf, 32, "%u", c );
		append( buf );
	}

	void append( u64 c )
	{
		char buf[32u];
		::std::snprintf( buf, 32, "%llu", c );
		append( buf );
	}

	void append( f64 c )
	{
		char buf[32u];
		::std::snprintf( buf, 32, "%.12f", c );
		append( buf );
	}

	void append( f32 c )
	{
		char buf[32u];
		::std::snprintf( buf, 32, "%.4f", c );
		append( buf );
	}

	kkString_base<char,allocator> to_kkStringA() const
	{
		kkString_base<char,allocator> ret;
		ret.reserve(m_size+1);
		this->copy(ret.data(),m_data);
		ret.setSize(m_size);
		ret.data()[m_size]=0;
		return ret;
	}

	kkString_base<char16_t,allocator> to_kkString() const
	{
		kkString_base<char16_t,allocator> ret;
		ret.reserve(m_size);
		this->copy(ret.data(),m_data);
		return ret;
	}

	const_pointer c_str() const 
	{
		return m_data;
	}

	pointer data() const 
	{
		return m_data;
	}

	const u64 size() const 
	{
		return m_size;
	}

	this_type& operator=( this_const_reference str )
	{
		assign( str );
		return *this;
	}

	this_type& operator=( this_type&& str )
	{
		assign( str );
		return *this;
	}

	
	template<typename other_type>
	this_type& operator=( other_type * str )
	{
		assign( str );
		return *this;
	}

	this_type operator+( const_pointer str )
	{
		this_type ret( *this );
		ret.append( str );
		return ret;
	}

	this_type operator+( this_const_reference str )
	{
		return operator+(str.data());
	}

	this_type operator+( u32 num )
	{
		this_type r( *this );
		r.append( num );
		return r;
	}

	pointer begin() const 
	{
		return m_data;
	}

	pointer end() const 
	{
		return (m_data+(m_size));
	}

	const_reference operator[]( u64 i ) const 
	{
		return m_data[ i ];
	}

	reference operator[]( u64 i )
	{
		return m_data[ i ];
	}

	void operator+=( s32 i )
	{
		append( i );
	}

	void operator+=( u32 i )
	{
		append( i );
	}
	void operator+=( u64 i )
	{
		append( i );
	}
	void operator+=( f32 i )
	{
		append( i );
	}
	void operator+=( f64 i )
	{
		append( i );
	}

	void operator+=( char_type c )
	{
		append( c );
	}

	template<typename other_type>
	void operator+=( other_type * str )
	{
		append( str );
	}

	void operator+=( this_const_reference str )
	{
		append( str );
	}

	bool operator==( this_const_reference other ) const 
	{
		if( other.size() != m_size ) 
			return false;

		const u64 sz = other.size();
		const auto * ptr = other.data();
		for( u64 i = 0u; i < sz; ++i )
		{
			if( ptr[ i ] != m_data[ i ]  )
				return false;
		}

		return true;
	}

	bool operator!=( this_const_reference other ) const 
	{
		if( other.size() != m_size )
			return true;

		const u64 sz = other.size();
		const auto * ptr = other.data();
		for( u64 i = 0u; i < sz; ++i )
		{
			if( ptr[ i ] == m_data[ i ]  ) 
				return false;
		}

		return true;
	}

	void clear()
	{
		m_size = 0u;
		m_data[ m_size ] = 0;
	}

	void pop_back()
	{
		if( m_size )
		{
			--m_size;
			m_data[ m_size ] = 0;
		}
	}

	char_type pop_back_return()
	{
		char_type r = '?';
		if( m_size )
		{
			--m_size;
			r = m_data[ m_size ];
			m_data[ m_size ] = 0;
		}
		return r;
	}

	void shrink_to_fit()
	{
		if( m_size )
		{
			if( m_allocated > (m_size + m_stringWordSize) )
			{
				reallocate( m_size + 1u );
				m_data[ m_size ] = static_cast<char_type>(0x0);
			}
		}
	}

	void setSize( u64 size )
	{
		m_size = size;
	}

	void erase( u64 begin, u64 end )
	{
		u64 numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
		u64 next = end + 1u;
		for( u64 i = begin; i < m_size; ++i )
		{
			if( next < m_size )
			{
				m_data[ i ] = m_data[ next ];
				++next;
			}
		}
		m_size -= numCharsToDelete;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	void pop_front()
	{
		erase( 0u, 0u );
	}

	bool is_space( u64 index )
	{
		if( m_data[ index ] == (char_type)' ' ) return true;
		if( m_data[ index ] == (char_type)'\r' ) return true;
		if( m_data[ index ] == (char_type)'\n' ) return true;
		if( m_data[ index ] == (char_type)'\t' ) return true;
		return false;
	}
};

using kkString   = kkString_base<char16_t,kkDefault_allocator>;
using kkStringW  = kkString_base<wchar_t,kkDefault_allocator>;
using kkString32 = kkString_base<char32_t,kkDefault_allocator>;
using kkStringA  = kkString_base<char,kkDefault_allocator>;

//using kkStringPL = kkString_base<char16_t,kkDefault_allocator_stdlib>;
//using kkStringAPL  = kkString_base<char,kkDefault_allocator_stdlib>;

namespace util
{
	inline void deformat( const char16_t* fmt,	va_list& args, kkString& message )
	{
		u32 len = 0U;
		const char16_t* p = fmt;
		do	{		++len;	} while(*p++);
		--len;

		va_list list = (va_list)args;

		bool S = false;
		for( u32 i(0); i < len; ++i )
		{
			std::wostringstream ss;
			if( S )
			{
				if( fmt[ i ] == u'f' )
				{
					ss << va_arg( list, f64 );
					message += (char16_t*)ss.str().c_str();
					continue;
				}
				else if( fmt[ i ] == u'i' )
				{
					ss << va_arg( list, s32 );
					message += (char16_t*)ss.str().c_str();
					continue;
				}
				else if( fmt[ i ] == u'u' )
				{
					ss << va_arg( list, u32 );
					message += (char16_t*)ss.str().c_str();
					continue;
				}
				else if( fmt[ i ] == u'c' )
				{
					message += va_arg( list, /*char16_t*/int );
					continue;
				}
				else if( fmt[ i ] == u's' )
				{
					char16_t * p2 = va_arg( list, char16_t* );
					u32 len2( 0U );
					do{ ++len2; } while(*p2++);
					p2 -= len2;
					for( u32 o(0U); o < len2-1; ++o )
						message += p2[ o ];
					continue;
				}
			}

			if( fmt[ i ] == u'%' )
			{
				if( !S ) S = true;
				else S = false;
			}
			else S = false;

			if( !S )
				message += fmt[ i ];
		}
	}
	template<typename allocator>
	void _stringChangeEndian( kkString_base<char16_t,allocator>& string )
	{
		u64 sz = string.size();
		for( u64 i = 0u; i < sz; ++i ){
			u8* p1 = reinterpret_cast<u8*>( &string.data()[ i ]);
			u8* p2 = p1 + 1u;
			u8 c1 = *p1;
			*p1 = *p2;
			*p2 = c1;
		}
	}
	KK_FORCE_INLINE void stringChangeEndian( kkString_base<char16_t,kkDefault_allocator>& string )
	{
		_stringChangeEndian<kkDefault_allocator>(string);
	}
	KK_FORCE_INLINE void stringChangeEndian_PL( kkString_base<char16_t,kkDefault_allocator_stdlib>& string )
	{
		_stringChangeEndian<kkDefault_allocator_stdlib>(string);
	}


	template<typename allocator>
	void _string_UTF16_to_UTF8( kkString_base<char16_t,allocator>& utf16, kkString_base<char,allocator>& utf8 )
	{
		u64 sz = utf16.size();
		for( u64 i = 0u; i < sz; ++i ){
			char16_t ch16 = utf16[ i ];
			if( ch16 < 0x80 ){
				utf8 += (char)ch16;
			}else if( ch16 < 0x800 ){
				utf8 += (char)((ch16>>6)|0xc0);
				utf8 += (char)((ch16&0x3f)|0x80);
			}
		}
	}
	KK_FORCE_INLINE void string_UTF16_to_UTF8( kkString_base<char16_t,kkDefault_allocator>& utf16, kkString_base<char,kkDefault_allocator>& utf8)
	{
		_string_UTF16_to_UTF8<kkDefault_allocator>(utf16,utf8);
	}
	KK_FORCE_INLINE void string_UTF16_to_UTF8_PL( kkString_base<char16_t,kkDefault_allocator_stdlib>& utf16, kkString_base<char,kkDefault_allocator_stdlib>& utf8)
	{
		_string_UTF16_to_UTF8<kkDefault_allocator_stdlib>(utf16,utf8);
	}

	template<typename allocator>
	void _string_UTF8_to_UTF16( kkString_base<char16_t,allocator>& utf16, kkString_base<char,allocator>& utf8 )
	{
		std::vector<u32> unicode;
		u64 i = 0u;
		auto sz = utf8.size();
		while( i < sz ){
			u32 uni = 0u;
			u32 todo = 0u;
//				bool error = false;
			u8 ch = utf8[i++];
			if( ch <= 0x7F ){
				uni = ch;
				todo = 0;
			}else if( ch <= 0xBF ){
				//throw std::logic_error("not a UTF-8 string");
			}else if ( ch <= 0xDF ){
				uni = ch&0x1F;
				todo = 1;
			}else if( ch <= 0xEF ){
				uni = ch&0x0F;
				todo = 2;
			}else if( ch <= 0xF7 ){
				uni = ch&0x07;
				todo = 3;
			}else{
				//throw std::logic_error("not a UTF-8 string");
			}
			for( u32 j = 0; j < todo; ++j ){
				//if( i == utf8.size() )
					//throw std::logic_error("not a UTF-8 string");
				u8 ch2 = utf8[i++];
				//if( ch < 0x80 || ch > 0xBF )
					//throw std::logic_error("not a UTF-8 string");
				uni <<= 6;
				uni += ch2 & 0x3F;
			}
			//if( uni >= 0xD800 && uni <= 0xDFFF )
				//throw std::logic_error("not a UTF-8 string");
			//if( uni > 0x10FFFF )
				//throw std::logic_error("not a UTF-8 string");
			unicode.push_back(uni);
		}
		auto sz2 = unicode.size();
		for( u64 o = 0u; o < sz2; ++o ){
			u32 uni = unicode[o];
			if( uni <= 0xFFFF ){
				utf16 += (char16_t)uni;
			}else{
				uni -= 0x10000;
				utf16 += (wchar_t)((uni >> 10) + 0xD800);
				utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
			}
		}
	}
	KK_FORCE_INLINE void string_UTF8_to_UTF16( kkString_base<char16_t,kkDefault_allocator>& utf16, kkString_base<char,kkDefault_allocator>& utf8 )
	{
		_string_UTF8_to_UTF16<kkDefault_allocator>(utf16,utf8);
	}
	KK_FORCE_INLINE void string_UTF8_to_UTF16( kkString_base<char16_t,kkDefault_allocator_stdlib>& utf16, kkString_base<char,kkDefault_allocator_stdlib>& utf8 )
	{
		_string_UTF8_to_UTF16<kkDefault_allocator_stdlib>(utf16,utf8);
	}
}

#endif