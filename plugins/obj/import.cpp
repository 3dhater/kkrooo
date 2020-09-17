// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include <cstdlib>
#include <cctype>
#include <map>
#include <unordered_map>

#include "common.h"

#ifdef _WIN32
#include <Windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch( ul_reason_for_call )
	{
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif



struct VERTEX
{
	v4f position;
	v4f texcoord;
	v4f normal;
};

extern "C" 
{

	u8 * nextLine( u8 * ptr );
	u8 * skipSpaces( u8 * ptr );
	u8 * readVec2( u8 * ptr, v4f& vec2 );
	u8 * readFloat( u8 * ptr, f32& value );
	u8 * readVec3( u8 * ptr, v4f& vec3 );
	u8 * readFace( u8 * ptr, face& f, char * s );
	u8 * readWord( u8 * ptr, kkStringA& str ); 
	void   reserveArrays( 
		kkArray<v4f,kkDefault_allocator_stdlib>& position, 
		kkArray<v4f,kkDefault_allocator_stdlib>& uv, 
		kkArray<v4f,kkDefault_allocator_stdlib>& normal, 
		u8 * ptr, u32& );
	
	KK_API int KK_C_DECL kkPlugin_onImportMesh( kkPluginCommonInterface* cInterface, const char16_t* file_path )
	{
		int result = 1; //good

		ImportData * importData = kkSingleton<ImportData>::s_instance;
		

		auto geometry_creator = cInterface->GetGeometryCreator();

		if( cInterface->FSFileExist( file_path ) )
		{
			auto file = cInterface->FSOpenFile( file_path );
			if( file )
			{
				auto file_size = file->size();
			
				if( file_size )
				{
					kkArray<u8,kkDefault_allocator_stdlib> file_byte_array;
					file_byte_array.reserve( (u32)file_size + 2 );
					file_byte_array.setSize( (u32)file_size + 2 );

					u8 * ptr = file_byte_array.data();
					
					file->read( ptr, file_size );
					ptr[ (u32)file_size ] = ' ';
					ptr[ (u32)file_size+1 ] = 0;

					bool groupBegin = false;
					bool isModel = false;
					bool grpFound = false;
					
					v4f tcoords;
					v4f pos;
					v4f norm;

					kkArray<v4f,kkDefault_allocator_stdlib> position;
					kkArray<v4f,kkDefault_allocator_stdlib> uv; 
					kkArray<v4f,kkDefault_allocator_stdlib> normal; 
					

					u32 numoffaces =0;
					reserveArrays( position, uv, normal, ptr, numoffaces );

					//float progress_val = 0.002f;

					//u32 num_for_progress = position.capacity() + uv.capacity() + normal.capacity() + numoffaces;
					//u32 num_for_progress_div100 = (float)num_for_progress * progress_val;
					//u32 gui_draw_counter = 0.f;

					//printf("num_for_progress_div100 [%u]\n num_for_progress[%u]\n",num_for_progress_div100,num_for_progress);

					kkStringA tmp_word;
					kkStringA curr_word;
					kkStringA prev_word;

					geometry_creator->BeginModel();
					
					face f;
					u32 i2 = 0;
					char s[0xff];

					bool generate_normals = false;

					if( importData->fix_generate_normals )
						generate_normals = true;

					std::unordered_map<std::string,u32> map;
					bool weld = false;


					float progress = 0.0f;

					// считает количество v vt vn текущей суб модели
					// нужно для случая если индексы указаны со знаком минус
					v3i last_counter;

					while( *ptr )
					{
						switch( *ptr )
						{
						case '#':
						case 's':
						case 'l':
						case 'u'://usemtl
						case 'c'://curv
						case 'm'://mtllib
						case 'p'://parm
						case 'd'://deg
						case 'e'://end
							ptr = nextLine( ptr );
							break;
						case 'v':
						{
					//		++gui_draw_counter;

							++ptr;
							if( groupBegin )
								groupBegin = false;
							switch( *ptr )
							{
								case 't':
									ptr = readVec2( ++ptr, tcoords );
									uv.push_back( tcoords );
									++last_counter.y;
									break;
								case 'n':
									ptr = readVec3( ++ptr, norm );
									
									if( importData->fix_z_up )
									{
										auto Z = norm.z;
										norm.z  = norm.y;
										norm.y  = Z;
									}

									normal.push_back( norm );
									++last_counter.z;
									break;
								default:
									ptr = readVec3( ptr, pos );

									if( importData->fix_z_up )
									{
										auto Z = pos.z;
										pos.z  = pos.y;
										pos.y  = Z;
									}

									position.push_back( pos );
									++last_counter.x;
									break;
								}
							}break;
						case 'f':
						{
					//		++gui_draw_counter;
							isModel = true;
							f.reset();
							ptr = readFace( ++ptr, f, s );

							if( !generate_normals )
							{
								if( f.ft == face_type::p || f.ft == face_type::pu )
								{
									generate_normals = true;
								}
							}

							geometry_creator->BeginPolygon();
							for( u32 sz2 = f.p.size(), i2 = 0; i2 < sz2; ++i2 )
							{
								auto index = i2;
								auto pos_index = f.p.data[ index ];

								if( pos_index < 0 )
								{
									// если индекс отрицательный то он указывает на позицию относительно последнего элемента
									// -1 = последний элемент
									pos_index = last_counter.x + pos_index + 1;
								}

								{
									std::string hash;
									hash += pos_index;

									// это я не помню зачем сделал
									// когда дойду до control вершин, станет ясно зачем это здесь
									auto it = map.find( hash );
									if( it == map.end() )
									{
										map[ hash ] = pos_index;
									}
									else
									{
										weld=true;
									}
								}

								auto v = position[ pos_index ];

								geometry_creator->AddPosition( v.x, v.y, v.z );

								if( f.ft == face_type::pu || f.ft == face_type::pun )
								{
									auto uv_index  = f.u.data[ index ];

									if( uv_index < 0 )
									{
										uv_index = last_counter.y + uv_index + 1;
									}

									auto u = uv[ uv_index ];
									geometry_creator->AddUV( u.x, u.y );
								}

								if( f.ft == face_type::pn || f.ft == face_type::pun )
								{
									auto nor_index = f.n.data[ index ];

									if( nor_index < 0 )
									{
										nor_index = last_counter.z + nor_index + 1;
									}

									auto n = normal[ nor_index ];
									geometry_creator->AddNormal( n.x, n.y, n.z );
								}
							}						

							geometry_creator->EndPolygon( weld, importData->option_triangulate, importData->fix_flip_normals );
							weld = false;
						}break;
						case 'o':
						case 'g':
							if( !groupBegin )
							{
								groupBegin = true;
							}
							else
							{
								ptr = nextLine(ptr);
								break;
							}

							ptr = readWord( ++ptr, tmp_word );
							if( tmp_word.size() )
							{
								prev_word = curr_word;
								curr_word = tmp_word;
							}
							
							if( grpFound )
							{
								if( prev_word.size() )
									geometry_creator->SetName( kkString( prev_word.data() ).data() );
								if( generate_normals || importData->fix_generate_flat_normals )
									geometry_creator->GenerateNormals(importData->fix_generate_flat_normals);
								geometry_creator->GenerateBT();
								geometry_creator->EndModel();
								geometry_creator->BeginModel();
							}
							grpFound = true;
							break;

						default:
							++ptr;
							break;
						}
					}

					if( curr_word.size() )
						geometry_creator->SetName( kkString( curr_word.data() ).data() );

					if( generate_normals || importData->fix_generate_flat_normals )
						geometry_creator->GenerateNormals(importData->fix_generate_flat_normals);

					geometry_creator->EndModel();
				}

				cInterface->FSCloseFile(file);
			}
		}

		return result;
	}

	void     reserveArrays( kkArray<v4f,kkDefault_allocator_stdlib>& position, 
							kkArray<v4f,kkDefault_allocator_stdlib>& uv, 
							kkArray<v4f,kkDefault_allocator_stdlib>& normal, u8 * ptr, u32& numoffaces )
	{
		u32 p = 0;
		u32 u = 0;
		u32 n = 0;
		while( *ptr )
		{

			switch( *ptr )
			{
			case 'f': ++numoffaces; ++ptr; break;
			case '#':
			case 's':
			case 'l':
			case 'u':
			case 'c':
			case 'm':
			case 'p':
			case 'd':
			case 'e':
			case 'o':
			case 'g':
				ptr = nextLine( ptr );
				break;
			case 'v':{
				++ptr;
				switch( *ptr ){
					case 't':
						++u;
						break;
					case 'n':
						++n;
						break;
					default:
						++p;
						break;
					}
				}break;
			{

			}break;
			default:
				++ptr;
				break;
			}
		}

		position.reserve( p + 0xff );
		uv.reserve( u + 0xff );
		normal.reserve( n + 0xff );
		position.setAddMemoryValue( 0xffff );
		uv.setAddMemoryValue( 0xffff );
		normal.setAddMemoryValue( 0xffff );
	}

	u8 * nextLine( u8 * ptr )
	{
		while( *ptr )
		{
			if( *ptr == '\n' )
			{
				ptr++;
				return ptr;
			}
			ptr++;
		}
		return ptr;
	}

	u8 * readVec2( u8 * ptr, v4f& vec2 )
	{
		ptr = skipSpaces( ptr );
		f32 x, y;
		if( *ptr == '\n' )
		{
			ptr++;
		}
		else
		{
			ptr = readFloat( ptr, x );
			ptr = skipSpaces( ptr );
			ptr = readFloat( ptr, y );
			ptr = nextLine( ptr );
			vec2.x = x;
			vec2.y = y;
		}
		return ptr;
	}

	u8 * skipSpaces( u8 * ptr )
	{
		while( *ptr )
		{
			if( *ptr != '\t' && *ptr != ' ' )
				break;
			ptr++;
		}
		return ptr;
	}

	u8 * readFloat( u8 * ptr, f32& value )
	{
		char str[ 32u ];
		memset( str, 0, 32 );
		char * p = &str[ 0u ];
		while( *ptr ){
			if( !isdigit(*ptr) && (*ptr != '-')  && (*ptr != '+')
				 && (*ptr != 'e')  && (*ptr != 'E') && (*ptr != '.')  ) break;
			*p = *ptr;
			++p;
			++ptr;
		}
		value = (f32)atof( str );
		return ptr;
	}

	u8 * readVec3( u8 * ptr, v4f& vec3 ){
		ptr = skipSpaces( ptr );
		f32 x, y, z;
		if( *ptr == '\n' ){
			ptr++;
		}else{
			ptr = readFloat( ptr, x );
			ptr = skipSpaces( ptr );
			ptr = readFloat( ptr, y );
			ptr = skipSpaces( ptr );
			ptr = readFloat( ptr, z );
			ptr = nextLine( ptr );
			vec3.x = x;
			vec3.y = y;
			vec3.z = z;
		}
		return ptr;
	}

	u8 * skipSpace( u8 * ptr ){
		while( *ptr ){
			if( *ptr != ' ' && *ptr != '\t' ) break;
			ptr++;
		}
		return ptr;
	}
	
	u8 * getInt( u8 * p, s32& i )
	{
		char str[ 8u ];
		memset( str, 0, 8 );
		char * pi = &str[ 0u ];

		while( *p )
		{
			/*if( *p == '-' )
			{
				++p;
				continue;
			}*/

			if( !isdigit( *p ) && *p != '-' ) break;


			*pi = *p;
			++pi;
			++p;
		}
		i = atoi( str );
		return p;
	}

	u8 * readFace( u8 * ptr, face& f, char * s ){
		ptr = skipSpaces( ptr );
		if( *ptr == '\n' )
		{
			ptr++;
		}
		else
		{
			while(true)
			{				
				s32 p = 1;
				s32 u = 1;
				s32 n = 1;

				ptr = getInt( ptr, p );

				if( *ptr == '/' )
				{
					ptr++;
					if( *ptr == '/' )
					{
						ptr++;
						f.ft = face_type::pn;
						ptr = getInt( ptr, n );
					}
					else
					{
						ptr = getInt( ptr, u );
						if( *ptr == '/' )
						{
							ptr++;
							f.ft = face_type::pun;
							ptr = getInt( ptr, n );
						}
						else
						{
							f.ft = face_type::pu;
						}
					}
				}
				else
				{
					f.ft = face_type::p;
				}
				f.n.push_back( n-1 );
				f.u.push_back( u-1 );
				f.p.push_back( p-1 );
				ptr = skipSpace( ptr );

				if( *ptr == '\r' )
					break;
				else if( *ptr == '\n' )
					break;
			}
		}
		return ptr;
	}

	u8 * readWord( u8 * ptr, kkStringA& str )
	{
		ptr = skipSpaces( ptr );
		str.clear();
		while( *ptr )
		{
			if( isspace(*ptr) )
				break;
			str += (char)*ptr;
			ptr++;
		}
		return ptr;
	}
}