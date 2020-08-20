// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_UTILITIES_H__
#define __KK_UTILITIES_H__

#include "Classes/Math/kkVector4.h"
#include "FileSystem/kkFileSystem.h"

#include <vector>

//	Some helper functions here
namespace util
{

	KK_FORCE_INLINE bool pointInRect( const v2i& coord, const v4i& rect )
	{
		if( coord.x >= rect.x ){
			if( coord.x <= rect.z ){
				if( coord.y >= rect.y ){
					if( coord.y <= rect.w ){
						return true;
					}
				}
			}
		}
		return false;
	}

	template<typename char_type>
	bool isDigit( char_type c )
	{
		if( c < 0x7B )
		{
			if( c >= (char_type)'0' && c <= (char_type)'9' )
				return true;
		}
		return false;
	}

	template<typename char_type>
	bool isAlpha( char_type c )
	{
		if( c < 0x7B ){
			if( (c >= (char_type)'a' && c <= (char_type)'z')
				|| (c >= (char_type)'A' && c <= (char_type)'Z') )
				return true;
		}else if( c >= 0xC0 && c <= 0x2AF ){
			return true;
		}else if( c >= 0x370 && c < 0x374 ){
			return true;
		}else if( c >= 0x376 && c < 0x378 ){
			return true;
		}else if( c >= 0x376 && c < 0x378 ){
			return true;
		}else if( c == 0x37F || c == 0x386 ){
			return true;
		}else if( c > 0x387 && c < 0x38B ){
			return true;
		}else if( c == 0x38C ){
			return true;
		}else if( c > 0x38D && c < 0x3A2 ){
			return true;
		}else if( c > 0x3A2 && c < 0x482 ){
			return true;
		}else if( c > 0x489 && c < 0x530 ){
			return true;
		}else if( c > 0x530 && c < 0x557 ){
			return true;
		}else if( c > 0x560 && c < 0x588 ){
			return true;
		}else if( c >= 0x5D0 && c < 0x5EB ){
			return true;
		}/*
			Mey be for isAlphaUnicode( c )
			else if( c >= 0x630 && c < 0x64B ){
			return true;
		}else if( c > 0x66D && c < 0x6D4 ){
			return true;
		}else if( c > 0x6F9 && c < 0x6FD ){
			return true;
		}else if( c >= 0x710 && c < 0x730 ){
			return true;
		}else if( c > 0x74C && c < 0x7A6 ){
			return true;
		}else if( c == 0x7B0 || c == 0x7B1 ){
			return true;
		}else if( c > 0x7C9 && c < 0x7EB ){
			return true;
		}else if( c > 0x7FF && c < 0x816 ){
			return true;
		}else if( c > 0x83F && c < 0x859 ){
			return true;
		}else if( c > 0x85F && c < 0x86B ){
			return true;
		}else if( c > 0x89F && c < 0x8B5 ){
			return true;
		}else if( c > 0x8B5 && c < 0x8BE ){
			return true;
		}else if( c > 0x903 && c < 0x93A ){
			return true;
		}else if( c > 0x957 && c < 0x962 ){
			return true;
		}else if( c > 0x971 && c < 0x980 ){
			return true;
		}else if( c > 0x984 && c < 0x98D ){
			return true;
		}else if( c > 0x984 && c < 0x98D ){
			return true;
		}else if( c == 0x98F || c == 0x990 || c == 0x9B2 || c == 0x9DC || c == 0x9DD ){
			return true;
		}else if( c > 0x992 && c < 0x9A9 ){
			return true;
		}else if( c > 0x9A9 && c < 0x9B0 ){
			return true;
		}else if( c > 0x9B5 && c < 0x9BA ){
			return true;
		}else if( c > 0x9DE && c < 0x9E2 ){
			return true;
		}else if( c == 0x9F0 || c == 0x9F1 || c == 0xA0F || c == 0xA10 ){
			return true;
		}else if( c > 0xA04 && c < 0xA0B ){
			return true;
		}else if( c > 0xA13 && c < 0xA29 ){
			return true;
		}else if( c > 0xA29 && c < 0xA31 ){
			return true;
		}else if( c == 0xA32 || c == 0xA33 || c == 0xA35 || c == 0xA36 || c == 0xA38 || c == 0xA39 ){
			return true;
		}*/
		return false;
	}

	template<typename Type, typename allocator>
	void _stringGetWords( std::vector<kkString_base<Type,allocator>> * out_array,
		const kkString_base<Type,allocator>& string,
		bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false )
	{
		kkString_base<Type,allocator> word;
		u64 sz = string.size();
		for( u64 i = 0; i < sz; ++i ){
			auto ch = string[ i ];
			if( ch < 256 && (u8)ch == ' ' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_space ){
					out_array->push_back( kkString_base<Type,allocator>( (Type)ch ) );
				}
			}else if( ch < 256 && (u8)ch == '\t' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_tab ){
					out_array->push_back( kkString_base<Type,allocator>( (Type)ch ) );
				}
			}else if( ch < 256 && (u8)ch == '\n' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_newLine ){
					out_array->push_back( kkString_base<Type,allocator>( (Type)ch ) );
				}
			}else{
				word += ch;
			}
		}
		if( word.size() ){
			out_array->push_back( word );
			word.clear();
		}
	}
	
	template<typename Type>
	void stringGetWords( std::vector<Type> * out_array,
		const Type& string,
		bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false )
	{
		 _stringGetWords( out_array, string, add_space, add_tab, add_newLine);
	}


	template<typename Type>
	inline void stringFlipSlash( Type& str )
	{
		u64 sz = str.size();
		for( u64 i = 0u; i < sz; ++i )
		{
			if( str[ i ] == '\\' )
				str[ i ] = '/';
		}
	}

		//	abc -> cba
	template<typename Type>
	inline void stringFlip( Type& str )
	{
		Type flippedStr;
		for( u64 i = str.size() - 1u; i >= 0u; --i )
		{
			flippedStr += str[ i ];
			if( !i ) break;
		}
		str = flippedStr;
	}

	template<typename Type>
	inline void stringPopBackBefore( Type& str, s8 c )
	{
		if( str.size() )
			str.pop_back();
		if( str.size() )
		{
			for( u64 i = str.size() - 1u; i >= 0u; --i )
			{
				if( str[ i ] == c ) 
					break;
				else
					str.pop_back();
				if( !i ) 
					break;
			}
		}
	}

	template<typename Type>
	inline Type stringGetExtension( const Type& str, bool with_dot = false)
	{
		Type ret;

		for( u64 i = str.size() - 1u; i >= 0u; --i )
		{
			auto c = str[ i ];
			if( c == '/' || c == '.' )
			{
				if(c=='.' && with_dot)
					ret += c;
				break;
			}
			else 
				ret += c;
			if( !i ) 
				break;
		}

		stringFlip<Type>( ret );

		return ret;
	}

		//	"AbC" -> "abc"
	template<typename Type>
	inline void stringToLower( Type& str )
	{
		u64 sz = str.size();

		for( u64 i = 0u; i < sz; ++i )
		{
			auto c = str[ i ];
			if( c <= 'Z' && c >= 'A' )
				str[ i ] += 32;
		}

	}

		//	"AbC" -> "ABC"
	template<typename Type>
	inline void stringToUpper( Type& str )
	{
		u64 sz = str.size();

		for( u64 i = 0u; i < sz; ++i )
		{
			auto c = str[ i ];
			if( c >= 'a' && c <= 'z' )
				str[ i ] -= 32;
		}

	}

		//	" asd " -> "asd"
	template<typename Type>
	inline void stringTrimSpace( Type& str )
	{
		while( true )
		{
			if( str.is_space( 0u ) )
				str.pop_front();
			else 
				break;
		}

		while( true )
		{
			if( str.is_space( str.size() - 1u ) )
				str.pop_back();
			else 
				break;
		}

	}

	template<typename Type>
	inline void stringTrimFrontSpace( Type& str )
	{
		while( true )
		{
			if( str.is_space( 0u ) )
				str.pop_front();
			else 
				break;
		}
	}

	template<typename Type, typename AnotherType, typename charType>
	inline void stringAppend( Type& str, AnotherType& other, charType /*c*/ )
	{
		u64 sz = 0u;
		auto * p = &other[0];
		while( *p++ ) 
			sz++;
		for( u64 i = 0u; i < sz; ++i )
			str += static_cast<charType>( other[ i ] );
	}

	KK_FORCE_INLINE kkString stringGetFirstWord( const kkString& string, bool with_ = true )
	{
		kkString ret;
		kkString str = string;
		util::stringTrimSpace(str);
		for( auto i : str )
		{
			if( util::isAlpha( i ) )
			{
				ret += i;
			}
			else if( with_ && i == u'_' )
			{
				ret += i;
			}
			else
			{
				break;
			}
		}
		return ret;
	}


	KK_FORCE_INLINE bool readTextFromFileForUnicode( const kkString& fileName, kkString& utf16 )
	{
		kkFile* file = util::openFileForReadBin( fileName );
		if( !file )
		{
			KK_PRINT_FAILED;
			kkDestroy(file);
			return false;
		}

		u64 sz = file->size();
		if( sz < 4 )
		{
			KK_PRINT_FAILED;
			kkDestroy(file);
			return false;
		}

		u8 bom[ 3u ];
		file->read( bom, 3u );
		file->seek( 0u, kkFileSeekPos::Begin );

		bool isUTF8 = false;
		bool isBE = false;

		if( bom[ 0u ] == 0xEF ){
			file->seek( 3u, kkFileSeekPos::Begin );
			isUTF8 = true;
			sz -= 3u;
		}else if( bom[ 0u ] == 0xFE ){ // utf16 BE
			file->seek( 2u, kkFileSeekPos::Begin );
			isBE = true;
			sz -= 2u;
		}else if( bom[ 0u ] == 0xFF ){
			file->seek( 2u, kkFileSeekPos::Begin );
			sz -= 2u;
		}else{
			// else - utf8 w/o bom
			isUTF8 = true;
		}

		kkStringA textBytes;
		textBytes.reserve( (u32)sz );
		textBytes.setSize( (u32)sz );
		file->read( (u8*)textBytes.data(), sz );

		if( !isUTF8 ){
			union{
				char16_t unicode;
				char b[ 2u ];
			}un;
			for( u32 i = 0u; i < sz; i += 2u ){
				/*char16_t ch16 = textBytes[ i ];

				if( isBE ){
					ch16 <<= kkConst8U;
					ch16 |= textBytes[ i + 1u ];
				}else{
					char16_t ch16_2 = textBytes[ i + 1u ];
					ch16_2 <<= kkConst8U;
					ch16 |= ch16_2;
				}*/

				if( isBE ){
					un.b[ 0u ] = textBytes[ i + 1u ];
					un.b[ 1u ] = textBytes[ i ];
				}else{
					un.b[ 0u ] = textBytes[ i ];
					un.b[ 1u ] = textBytes[ i + 1u ];
				}

				utf16 += un.unicode;
			}

		}else
		{
			util::string_UTF8_to_UTF16( utf16, textBytes );
		}
		kkDestroy(file);
		return true;
	}

	template<typename T>
	void stringReplaseSubString( kkString_base<T,kkDefault_allocator>& source, const kkString_base<T,kkDefault_allocator>& target, const kkString_base<T,kkDefault_allocator>& text )
	{
		kkString_base<T,kkDefault_allocator> result;

		u64 source_sz = source.size();
		u64 target_sz = target.size();
		u64 text_sz   = text.size();

		for( u64 i = 0u; i < source_sz; ++i )
		{
			if( (source_sz - i) < target_sz )
			{
				for( u64 i2 = i; i2 < source_sz; ++i2 )
				{
					result += source[ i2 ];
				}
				break;
			}

			bool comp = false;
			for( u64 o = 0u; o < target_sz; ++o )
			{
				if( source[ i + o ] == target[ o ] )
				{
					if( !comp )
					{
						comp = true;
					}
				}else
				{
					comp = false;
					break;
				}
			}

			if( comp )
			{
				for( u64 o = 0u; o < text_sz; ++o )
				{
					result += text[ o ];
				}
				i += target_sz - 1u;
			}else
			{
				result += source[ i ];
			}
		}

		if( result.size() )
		{
			source.clear();
			source.assign( result );
		}
	}

	template<typename type, typename char_type>
	type stringToInt( const kkString_base<char_type,kkDefault_allocator>& str )
	{
		type Integer = 0u;

		kkString_base<char_type,kkDefault_allocator> s = str;
		util::stringFlip( s );

		u32 mul = 1u;

		auto * ptr = s.data();

		while( *ptr )
		{
			u32 code = *ptr - 48u;

			if( code >= 0u && code <= 9 )
			{
				Integer += mul * code;
				mul *= 10u;
			}
			++ptr;
		}

		return Integer;
	}


	template<typename char_type>
	bool isSpace( char_type c )
	{
		if( c == (char_type)' ' ) return true;
		if( c == (char_type)'\r' ) return true;
		if( c == (char_type)'\n' ) return true;
		if( c == (char_type)'\t' ) return true;
		return false;
	}

	template<typename SourceType, typename TargetType>
	SourceType* getWordFromString( SourceType* source, kkString_base<TargetType,kkDefault_allocator>* target )
	{
		while( *source )
		{
			if( isSpace( *source ) || *source == (SourceType)',' )
			{
				++source;
			}else
				break;
		}

		while( *source )
		{
			if( isSpace( *source ) || *source == (SourceType)',' )
			{
				++source;
				break;
			}

			*target += (TargetType)*source;

			++source;
		}
		return source;
	}

	template<typename type>
	void getVec4iFromString( const kkString& str, type* vec )
	{
		auto * ptr = str.data();

		kkString word;

		u32 i = 0u;

		while( *ptr )
		{
			word.clear();

			ptr = getWordFromString( ptr, &word );

			if( word.size() ){
				switch( i ){
				case 0u:
					vec->x = stringToInt<u32>( word );
					break;
				case 1u:
					vec->y = stringToInt<u32>( word );
					break;
				case 2u:
					vec->z = stringToInt<u32>( word );
					break;
				case 3u:
					vec->w = stringToInt<u32>( word );
					break;
				}
				++i;
			}
		}
	}

	KK_FORCE_INLINE void getFilesFromDir( std::vector<kkString>* out, const kkString& dir, bool with_sub_dir )
	{
		if( !kkFileSystem::existDir( dir ) )
		{
			KK_PRINT_FAILED;
			return;
		}
		kkFileSystem::scanDirBegin( dir );
		kkFileSystem::DirObject obj;

		std::vector<kkString> dirs;

		while( kkFileSystem::getDirObject(&obj))
		{
			if( obj.type == kkFileSystem::DirObjectType::file )
				out->push_back( kkString( (char16_t*)obj.path ) );

			if( obj.type == kkFileSystem::DirObjectType::folder )
			{
				if( with_sub_dir )
					dirs.push_back(kkString( (char16_t*)obj.path ));
			}
		}
		kkFileSystem::scanDirEnd();

		if( with_sub_dir )
		{
			for( auto & o : dirs )
			{
				getFilesFromDir(out, o, with_sub_dir);
			}
		}
	}

	KK_FORCE_INLINE void getDirs( std::vector<kkString>* out, const kkString& dir, bool with_sub_dir )
	{
		if( !kkFileSystem::existDir( dir ) )
		{
			KK_PRINT_FAILED;
			return;
		}
		kkFileSystem::scanDirBegin( dir );
		kkFileSystem::DirObject obj;

		std::vector<kkString> dirs;

		while( kkFileSystem::getDirObject(&obj))
		{

			if( obj.type == kkFileSystem::DirObjectType::folder )
			{
				out->push_back( kkString( (char16_t*)obj.path ) );

				if( with_sub_dir )
					dirs.push_back(kkString( (char16_t*)obj.path ));
			}
		}
		kkFileSystem::scanDirEnd();

		if( with_sub_dir )
		{
			for( auto & o : dirs )
			{
				getDirs(out, o, with_sub_dir);
			}
		}
	}

}



#endif

