// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include <cstdlib>
#include <cctype>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>


#include "common.h"

extern "C" 
{
	bool   writeModelToFile( kkFile* file, ImportData * id, std::vector<kkScene3DObject*>& objects );
	
	KK_API int KK_C_DECL kkPlugin_onExportMesh(kkPluginCommonInterface* cInterface, const char16_t* file_path )
	{
		auto scene = cInterface->GetScene();

		int result = 1; //good
		ImportData * exportData = kkSingleton<ImportData>::s_instance;

		std::vector<kkScene3DObject*> objects;

		auto file = cInterface->FSCreateFile( file_path );
		if( file )
		{
			u32 num_of_objects = scene->getNumOfObjectsOnScene();
			if( num_of_objects )
			{
				u32 num_of_selected_objects = scene->getNumOfSelectedObjects();
				
				if( exportData->selected_only )
				{
					for( u32 i = 0; i < num_of_selected_objects; ++i )
					{
						auto object = scene->getSelectedObject( i );
						if( object->GetType() == kkScene3DObjectType::PolygonObject )
						{
							objects.push_back( object );
						}
					}
				}

				// если экспортируется вся сцена или если нет выбранных объектов то тоже экспортируется вся сцена
				if( !exportData->selected_only || objects.size() == 0 )
				{
					for( u32 i = 0; i < num_of_objects; ++i )
					{
						auto object = scene->getObjectOnScene( i );
						if( object->GetType() == kkScene3DObjectType::PolygonObject )
						{
							objects.push_back( object );
						}
					}
				}

				if( objects.size() )
				{
					if( !writeModelToFile( file, exportData, objects ))
					{
						result = 0;
						printf( "Something wrong.\n" );
					}
				}

			}else{
				result = 0;
				printf( "Scene is empty.\n" );
			}

			cInterface->FSCloseFile(file);
		}
		else
		{
			result = 0;
			printf( "Unable to create file %s.\n", kkString(file_path).to_kkStringA().data() );
		}

		return result;
	}

	struct my_hash_cv_ind
	{
		std::size_t operator () (u32 const &v) const
		{
			return (std::size_t)v;
		}
	};

	struct my_hash_cv
	{
		std::size_t operator () (kkControlVertex* const &v) const
		{
			return (std::size_t)v;
		}
	};


	bool writeModelToFile( kkFile* file, ImportData * id, std::vector<kkScene3DObject*>& objects )
	{
		kkStringA line = "# Kkrooo Wavefront OBJ Exporter v1.0\r\n\r\n";
		file->write( (u8*)line.data(), line.size() );
		
		// нужно запомнить количество индексов текущей модели чтобы прибавить это число к следующей
		// так как в файле на все o\g одна группа индексов (от 1 и выше)
		u32 position_index_counter = 0;
		u32 uv_index_counter = 0;

		for( auto object : objects )
		{
			struct faces_t
			{
				std::vector<face> f;
			}
			faces;

		
			auto & polygons = object->GetPolygonArray();
			auto & verts=object->GetVertexArray();   
			auto & cverts   = object->GetControlVertexArray();
			auto matrix     = object->GetMatrix();
			auto pivot      = object->GetPivot();
			auto poly_count = polygons.size();

			for( u32 i = 0; i < poly_count; ++i )
			{
				auto polygon = polygons[i];
				
				face current_face;

				// позиция и нормаль
				// главное оптимизировать позицию
				auto & cvert_inds = polygon->GetControlVertInds();
				for( u64 i2 = 0, vcount = cvert_inds.size(); i2 < vcount; ++i2 )
				{
					current_face.p.push_back(cvert_inds[i2]); // значения одинаковые поэтому всё суётся в 1 массив

				}
				
				// cvert_inds.size()
				// и
				// vert_inds.size()
				// должны иметь одинаковый размер
				// сverts - контрольные вершины
				// verts  - конкретные вершины входящие в полигон (они могут быть и не контрольными)

				// текстурные координаты
				// они не оптимизированы поэтому кидаю всё подряд
				auto & vert_inds = polygon->GetVertInds();
				for( u64 i2 = 0, vcount = vert_inds.size(); i2 < vcount; ++i2 )
				{
					current_face.u.push_back(vert_inds[i2]); // индекс для обычных вершин
				}

				faces.f.push_back(current_face);
			}

			line = "\r\no ";
			line += object->GetName();
			line += "\r\n";
			file->write( (u8*)line.data(), line.size() );

			u32 count  = 0;
			for( auto cvertex : cverts )
			{
				++count;
				auto & cvind = cvertex->getVertInds();
				auto vertex  = verts[cvind[0]];
				auto position = math::mul( vertex->getPosition(), matrix ) + pivot;

				line = "v ";
				line += position.KK_X;
				line += " ";
				line += position.KK_Y;
				line += " ";
				line += position.KK_Z;
				line += "\r\n";
				file->write( (u8*)line.data(), line.size() );
			}
			line = "# ";
			line += count;
			line += " positions";
			line += "\r\n\r\n";
			file->write( (u8*)line.data(), line.size() );
			count  = 0;
			
			if(id->option_write_normals)
			{
				for( auto cvertex : cverts )
				{
					++count;
					auto & cvind = cvertex->getVertInds();
					auto vertex  = verts[cvind[0]];
					auto normal  = vertex->getNormal();

					line = "vn ";
					line += normal.KK_X;
					line += " ";
					line += normal.KK_Y;
					line += " ";
					line += normal.KK_Z;
					line += "\r\n";
					file->write( (u8*)line.data(), line.size() );
				}
				line = "# ";
				line += count;
				line += " normals";
				line += "\r\n\r\n";
				file->write( (u8*)line.data(), line.size() );
				count  = 0;
			}
			
			if(id->option_write_UVs)
			{
				for( auto vertex : verts )
				{
					++count;
					auto normal  = vertex->getUV();

					line = "vt ";
					line += normal.KK_X;
					line += " ";
					line += normal.KK_Y;
					line += "\r\n";
					file->write( (u8*)line.data(), line.size() );
				}
				line = "# ";
				line += count;
				line += " UVs";
				line += "\r\n\r\n";
				file->write( (u8*)line.data(), line.size() );
				count  = 0;
			}

			u32 position_index_max = 0;
			u32 uv_index_max = 0;

			for( u64 i = 0, sz = faces.f.size(); i < sz; ++i )
			{
				auto f = faces.f[ i ];
				++count;

				line = "f ";

				///for( u32 i2 = 0; i2 < f.p.sz; ++i2 )
				u32 I = 0;
				for( u32 i2 = 0, vcount = f.p.sz, _3_count = 0, last = vcount - 1;
					i2 < vcount; ++i2 )
				{
					I = i2;

					if( id->option_triangulate_export )
					{
						if( _3_count == 0 )
						{
							I = 0;
						}
					}

					u32 index = f.p.data[I];
					line += index + position_index_counter + 1;

					if( index > position_index_max )
						position_index_max = index;

					if( id->option_write_UVs )
					{
						line += "/";

						auto uv_index = f.u.data[ I ];
						line += uv_index + uv_index_counter + 1;

						if( uv_index > uv_index_max )
							uv_index_max = uv_index;
					}
					if( id->option_write_normals )
					{
						line += "/";
						if( !id->option_write_UVs )
							line += "/";
						line += f.p.data[ I ] + position_index_counter + 1;
					}
					line += " ";

					if( id->option_triangulate_export )
					{
						if( ++_3_count == 3 )
						{
							_3_count = 0;
							if( i2 != last )
							{
								line += " \r\n";
								if( line.size() > 2 )
									file->write( (u8*)line.data(), line.size() );
								line = "f ";
							}else{
								break;
							}
							i2 -= 2;
						}
					}
				}

				line += " \r\n";
				file->write( (u8*)line.data(), line.size() );
			}
			
			position_index_counter += position_index_max + 1;
			uv_index_counter += uv_index_max + 1;


			line = "# ";
			line += count;
			line += " faces";
			line += "\r\n\r\n";
			file->write( (u8*)line.data(), line.size() );
		}

		// без оптимизации
		/*std::map<kkControlVertex*,u32> PosVerts;
		std::map<kkControlVertex*,u32> UVverts;
		std::map<kkControlVertex*,u32> NormalVerts;
		u32 position_index = 0;
		u32 UV_index = 0;
		u32 normal_index = 0;

		for( auto object : objects )
		{
			line = "\r\no ";
			line += object->getName();
			line += "\r\n";
			file->write( (u8*)line.data(), line.size() );

			auto & polygons = object->getPolygonArray();
			auto & verts    = object->getVertexArray();
			auto & сverts   = object->getControlVertexArray();
			
			auto poly_count = polygons.size();
			auto matrix = object->getMatrix();
			auto pivot  = object->getPivot();

			u32 count = 0;
			for( u32 i = 0; i < poly_count; ++i )
			{
				auto polygon = polygons[i];
				auto & cvert_inds = polygon->getControlVertInds();
				for( u32 i2 = 0, vcount = cvert_inds.size(); i2 < vcount; ++i2 )
				{
					auto cvertex = сverts[cvert_inds[i2]];
					auto & cvind = cvertex->getVertInds();
					auto vertex  = verts[cvind[0]];
					auto position = math::mul( vertex->getPosition(), matrix ) + pivot;

					++count;

					line = "v ";
					line += position.KK_X;
					line += " ";
					line += position.KK_Y;
					line += " ";
					line += position.KK_Z;
					line += "\r\n";
					file->write( (u8*)line.data(), line.size() );

					PosVerts[cvertex] = ++position_index;
				}
			}
			
			line = "# ";
			line += count;
			line += " positions";
			line += "\r\n\r\n";
			file->write( (u8*)line.data(), line.size() );

			if( id->option_write_UVs )
			{
				count = 0;
				for( u32 i = 0; i < poly_count; ++i )
				{
					auto polygon = polygons[i];
					auto & cvert_inds = polygon->getControlVertInds();
					for( u32 i2 = 0, vcount = cvert_inds.size(); i2 < vcount; ++i2 )
					{
						auto cvertex = сverts[cvert_inds[i2]];
						auto & cvind = cvertex->getVertInds();
						auto vertex  = verts[cvind[0]];
						auto UV      = vertex->getUV();

						++count;

						line = "vt ";
						line += UV.KK_X;
						line += " ";
						line += UV.KK_Y;
						line += "\r\n";
						file->write( (u8*)line.data(), line.size() );

						UVverts[cvertex] = ++UV_index;
					}
				}
			
				line = "# ";
				line += count;
				line += " UVs";
				line += "\r\n\r\n";
				file->write( (u8*)line.data(), line.size() );
			}

			if( id->option_write_normals )
			{
				count = 0;
				for( u32 i = 0; i < poly_count; ++i )
				{
					auto polygon = polygons[i];
					auto & cvert_inds = polygon->getControlVertInds();
					for( u32 i2 = 0, vcount = cvert_inds.size(); i2 < vcount; ++i2 )
					{
						auto cvertex = сverts[cvert_inds[i2]];
						auto & cvind = cvertex->getVertInds();
						auto vertex  = verts[cvind[0]];
						auto normal  = vertex->getNormal();

						++count;

						line = "vn ";
						line += normal.KK_X;
						line += " ";
						line += normal.KK_Y;
						line += " ";
						line += normal.KK_Z;
						line += "\r\n";
						file->write( (u8*)line.data(), line.size() );

						NormalVerts[cvertex] = ++normal_index;
					}
				}
			
				line = "# ";
				line += count;
				line += " normals";
				line += "\r\n\r\n";
				file->write( (u8*)line.data(), line.size() );
			}

			count = 0;
			for( u32 i = 0; i < poly_count; ++i )
			{
				auto polygon = polygons[i];
				auto & cvert_inds = polygon->getControlVertInds();
				line = "f ";

				kkControlVertex* first_vertex = сverts[cvert_inds[0]];

				for( u32 i2 = 0, vcount = cvert_inds.size(), _3_count = 0, last = vcount - 1; 
					i2 < vcount; ++i2 )
				{
					kkControlVertex* vertex = сverts[cvert_inds[i2]];

					if( id->option_triangulate_export )
					{
						if( _3_count == 0 )
						{
							vertex = first_vertex;
						}
					}

					line += PosVerts[ vertex ];
					if( id->option_write_UVs )
					{
						line += "/";
						line += UVverts[ vertex ];
					}
					if( id->option_write_normals )
					{
						line += "/";
						if( !id->option_write_UVs )
							line += "/";
						line += NormalVerts[ vertex ];
					}
					line += " ";

					if( id->option_triangulate_export )
					{
						if( ++_3_count == 3 )
						{
							_3_count = 0;
							if( i2 != last )
							{
								line += " \r\n";
								if( line.size() > 2 )
									file->write( (u8*)line.data(), line.size() );
								line = "f ";
							}else{
								break;
							}
							i2 -= 2;
						}
					}
				}
				line += " \r\n";
				file->write( (u8*)line.data(), line.size() );
			}

			line = "# ";
			line += count;
			line += " faces";
			line += "\r\n\r\n";
			file->write( (u8*)line.data(), line.size() );

		}*/

		return true;
	}
}