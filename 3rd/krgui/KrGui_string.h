#ifndef __KK_KRGUI_STRING_H__
#define __KK_KRGUI_STRING_H__
#include <string>
#include <sstream>
#include <cstdarg>
namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			inline void deformat( const char16_t* fmt,	va_list& args, std::u16string& message )
			{
				unsigned int len = 0U;
				const char16_t* p = fmt;
				do	{		++len;	} while(*p++);
				--len;

				va_list list = (va_list)args;

				bool S = false;
				for( unsigned int i(0); i < len; ++i )
				{
					std::wostringstream ss;
					if( S )
					{
						if( fmt[ i ] == u'f' )
						{
							ss << va_arg( list, double );
							message += (char16_t*)ss.str().c_str();
							continue;
						}
						else if( fmt[ i ] == u'i' )
						{
							ss << va_arg( list, int );
							message += (char16_t*)ss.str().c_str();
							continue;
						}
						else if( fmt[ i ] == u'u' )
						{
							ss << va_arg( list, unsigned int );
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
							unsigned int len2( 0U );
							do{ ++len2; } while(*p2++);
							p2 -= len2;
							for( unsigned int o(0U); o < len2-1; ++o )
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

			inline bool is_space(char16_t c){
				switch (c)
				{
				case u' ':
				case u'\t':
				case u'\r':
				case u'\n':
					return true;
				default:
					break;
				}
				return false;
			}
			inline bool is_digit(char16_t c){
				switch (c)
				{
				case u'0': case u'1': case u'2': case u'3': case u'4': case u'5':
				case u'6': case u'7': case u'8': case u'9':
					return true;
				default:
					break;
				}
				return false;
				//return c == u'0' || c == u'1' || c == u'2' || c == u'3' || c == u'4' || c == u'5'
				//	 || c == u'6' || c == u'7' || c == u'8' || c == u'9'
				//	/*|| c == 0x07C0*/;
			}
			inline bool is_alpha(char16_t c){
				return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
				//return c == u'0' || c == u'1' || c == u'2' || c == u'3' || c == u'4' || c == u'5'
				//	 || c == u'6' || c == u'7' || c == u'8' || c == u'9'
				//	/*|| c == 0x07C0*/;
			}
			template<typename char_type>
			int stl_len(const char_type* str){
				int len = 0u;
				if(str[0]!=0){
					const char_type* p = &str[ 0u ];
					while( (int)*p++ ) len++;
				}
				return len;
			}
			inline int to_int(const char16_t* str, int size = -1){
				int len = size;
				if( size == -1 )
					len = stl_len(str);
				int result = 0;
				int mul_val = 1;
				bool is_neg = str[0]==u'-';
				for(int i = 0, last = len-1; i < len; ++i)
				{
					int char_value = (int)str[last] - 0x30;
					if( char_value >= 0 && char_value <= 9 )
					{
						result += char_value * mul_val;
						mul_val *= 10;
					}
					--last;
				}
				if( is_neg )
					result = -result;
				return result;
			}
			inline void UTF16_to_UTF8(std::u16string& utf16, std::string& utf8){
				size_t sz = utf16.size();
				for( size_t i = 0u; i < sz; ++i ){
					char16_t ch16 = utf16[ i ];
					if( ch16 < 0x80 ){
						utf8 += (char)ch16;
					}else if( ch16 < 0x800 ){
						utf8 += (char)((ch16>>6)|0xc0);
						utf8 += (char)((ch16&0x3f)|0x80);
					}
				}
			}
			const float string_to_float_table[17] = 
			{
				0.f,
				0.1f,
				0.01f,
				0.001f,
				0.0001f,
				0.00001f,
				0.000001f,
				0.0000001f,
				0.00000001f,
				0.000000001f,
				0.0000000001f,
				0.00000000001f,
				0.000000000001f,
				0.0000000000001f,
				0.00000000000001f,
				0.000000000000001f,
				0.0000000000000001f,
			};
			inline float string_to_float(const char16_t* str, size_t len)
			{
				assert(str);
				float result = 0.f;
				bool is_negative = *str == u'-';

				if(is_negative)
					++str;

				int i = 0;
				while(*str >= u'0' && *str <= u'9')
				{
					i *= 10;
					i += *str - u'0';
					++str;
				}
				result = (float)i;
				
				i = 0;

				if(*str == u'.')
					++str;

				int part_2_count = 0;
				while(*str >= u'0' && *str <= u'9')
				{
					i *= 10;
					i += *str - u'0';
					++str;
					++part_2_count;
				}
				result += (float)i * string_to_float_table[part_2_count];

				return is_negative ? -result : result;
			}
		}
	}
}

#endif