#include "kkrooo.engine.h"
#include "../Common.h"

#include "KrGui.h"

#include "Common/kkInfoSharedLibrary.h"
#include "Plugins/kkPlugin.h"
#include "Common/kkUtil.h"

#include "../Geometry/GeometryCreator.h"


#include "PluginGUIWindow.h"
#include "PluginGUI.h"
#include "PluginManager.h"
#include "Plugin.h"
#include "FileSystem/kkFileSystem.h"
#include "Common/kkUtil.h"


PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	for( auto p : m_loadedPlugins )
	{
		/*if( p->m_type == PluginType::pythonScript )
		{
		}
		else*/
		{
			if( p->m_handle )
			{
				p->m_shutdown(m_commonInterface);

				KK_FREE_LIBRARY(p->m_handle);
			}
		}
	}
}

bool PluginManager::init(PluginCommonInterface* ci)
{
	m_commonInterface = ci;

	if( !kkFileSystem::existDir(u"stdplugins") )
	{
		fprintf(stderr,"dir `stdplugins` not found :(\n");
		return false;
	}

	std::vector<kkString> plugin_files;
	util::getFilesFromDir(&plugin_files,u"stdplugins",true);

	if( !plugin_files.size() )
	{
		fprintf(stderr,"dir `stdplugins` is empty :(\n");
		return false;
	}

	_readPlugins(plugin_files);


	return true;
}


void PluginManager::_readPlugins( const std::vector<kkString>& arr )
{
	m_availablePlugins.clear();

	m_loadedPlugins_importMesh.clear();
	m_loadedPlugins_exportMesh.clear();
	m_loadedPlugins_importImage.clear();
	m_loadedPlugins_objectCreator.clear();

	kkString kkp(u"kkp");

	printf("Check %llu files\n", arr.size());

	for(auto & o : arr)
	{
		auto extension = util::stringGetExtension(o);

		if( extension == kkp )
		{
			printf("Load plugin [%s]...",o.to_kkStringA().data());

			/// нужно проверить, библиотека ли, есть ли нужные функции
			///   и дебаг ли версия этот плагин
			KK_LIBRARY_HANDLE handle = KK_LOAD_LIBRARY((wchar_t*)o.data());
			if( !handle )
			{
				printf("FAIL [dll is not load]:(\n");
				continue;
			}

			
			kkPluginIsDebug_t PluginIsDebug = (kkPluginIsDebug_t)KK_LOAD_FUNCTION(handle, "kkPluginIsDebug");
			if(!PluginIsDebug)
			{
				printf("FAIL [no PluginIsDebug]:(\n");
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			if( PluginIsDebug() )
			{
#ifndef KK_DEBUG
				printf("..DEBUG..");
				KK_FREE_LIBRARY(handle);
				continue;
#endif
			}

			kkPluginGetPluginInfo_t PluginGetPluginInfo = (kkPluginGetPluginInfo_t)KK_LOAD_FUNCTION(handle, "kkPluginGetPluginInfo");
			if(!PluginGetPluginInfo)
			{
				printf("FAIL [no PluginGetPluginInfo function] :(\n");
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			kkPluginInit_t PluginInit = (kkPluginInit_t)KK_LOAD_FUNCTION(handle, "kkPluginInit");
			if(!PluginInit)
			{
				printf("FAIL [no PluginInit] :(\n");
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			kkPluginShutdown_t PluginShutdown = (kkPluginShutdown_t)KK_LOAD_FUNCTION(handle, "kkPluginShutdown");
			if(!PluginShutdown)
			{
				printf("FAIL [no PluginShutdown]:(\n");
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			if( PluginInit(m_commonInterface) == 0 )
			{
				printf("FAIL [bad init] :(\n");
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			kkPluginInfo info;
			if( !PluginGetPluginInfo(&info))
			{
				printf("FAIL [can't get info] :(\n");
				PluginShutdown(m_commonInterface);
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			if(info.m_type == 0)
			{
				printf("FAIL [info.m_type == 0] :(\n");
				PluginShutdown(m_commonInterface);
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			kkPlugin_onActivate_t Plugin_onActivate = (kkPlugin_onActivate_t)KK_LOAD_FUNCTION(handle, "kkPlugin_onActivate");
			if(!Plugin_onActivate)
			{
				printf("FAIL [no onActivate]:(\n");
				PluginShutdown(m_commonInterface);
				KK_PRINT_FAILED;
				KK_FREE_LIBRARY(handle);
				continue;
			}

			bool isImport = false;
			bool isExport = false;
			
			if(info.m_type & kkPluginType::mesh_loader)
			{
				kkPlugin_onImportMesh_t Plugin_onImportMesh = nullptr;
				kkPlugin_onExportMesh_t Plugin_onExportMesh = nullptr;

				Plugin_onImportMesh = (kkPlugin_onImportMesh_t)KK_LOAD_FUNCTION(handle, "kkPlugin_onImportMesh");
				Plugin_onExportMesh = (kkPlugin_onExportMesh_t)KK_LOAD_FUNCTION(handle, "kkPlugin_onExportMesh");
				
				bool good = false;

				if(Plugin_onImportMesh)
				{
					good = true;
					isImport = true;
				}

				if(Plugin_onExportMesh)
				{
					good = true;
					isExport = true;
				}

				if( !good )
				{
					printf("FAIL [no import/export functions] :(\n");
					PluginShutdown(m_commonInterface);
					KK_PRINT_FAILED;
					KK_FREE_LIBRARY(handle);
					continue;
				}

				std::vector<kkString> extensions_array;
				util::stringGetWords<kkString>(&extensions_array,info.m_extensions);
				if( !extensions_array.size() )
				{
					printf("FAIL [no extensions] :(\n");
					PluginShutdown(m_commonInterface);
					KK_PRINT_FAILED;
					KK_FREE_LIBRARY(handle);
					continue;
				}
			}

			bool image_export = false;

			if(info.m_type & kkPluginType::image_loader)
			{
				kkPlugin_onImportImage_t Plugin_onImportImage = nullptr;
				Plugin_onImportImage = (kkPlugin_onImportImage_t)KK_LOAD_FUNCTION(handle, "kkPlugin_onImportImage");
				if(!Plugin_onImportImage)
				{
					printf("FAIL [no import function] :(\n");
					PluginShutdown(m_commonInterface);
					KK_PRINT_FAILED;
					KK_FREE_LIBRARY(handle);
					continue;
				}

				kkPlugin_onExportImage_t Plugin_onExportImage = nullptr;
				Plugin_onExportImage = (kkPlugin_onExportImage_t)KK_LOAD_FUNCTION(handle, "kkPlugin_onExportImage");
				if(Plugin_onExportImage)
				{
					image_export = true;
				}

				std::vector<kkString> extensions_array;
				util::stringGetWords<kkString>(&extensions_array,info.m_extensions);
				if( !extensions_array.size() )
				{
					printf("FAIL [no extensions]:(\n");
					PluginShutdown(m_commonInterface);
					KK_PRINT_FAILED;
					KK_FREE_LIBRARY(handle);
					continue;
				}
			}

			m_availablePlugins.push_back(o);

			Plugin * plugin = kkCreate<Plugin>();
			plugin->m_file_path = o;
			plugin->m_handle    = handle;
			plugin->m_info      = info;
			plugin->m_shutdown  = PluginShutdown;
			plugin->m_onActivate= Plugin_onActivate;

			m_loadedPlugins.push_back(plugin);

			if( info.m_type & kkPluginType::mesh_loader )
			{
				if( isImport )
					m_loadedPlugins_importMesh.push_back(plugin);
				
				if( isExport )
					m_loadedPlugins_exportMesh.push_back(plugin);
			}

			if( info.m_type & kkPluginType::image_loader )
			{
				m_loadedPlugins_importImage.push_back(plugin);
			}

			if( info.m_type & kkPluginType::renderer )
			{
				m_loadedPlugins_renderer.push_back(plugin);
			}

			if( info.m_type & kkPluginType::object_creator )
			{
				m_loadedPlugins_objectCreator.push_back(plugin);
			}

			if( image_export )
			{
				m_loadedPlugins_exportImage.push_back(plugin);
			}

			printf("OK\n");
		}
	}
}

const std::vector<Plugin*>& PluginManager::getExportModelPlugins() { return m_loadedPlugins_exportMesh;  }
const std::vector<Plugin*>& PluginManager::getImportModelPlugins() { return m_loadedPlugins_importMesh;  }
const std::vector<Plugin*>& PluginManager::getImportImagePlugins() { return m_loadedPlugins_importImage; }
const std::vector<Plugin*>& PluginManager::getExportImagePlugins() { return m_loadedPlugins_exportImage; }
const std::vector<Plugin*>& PluginManager::getObjectCreatorPlugins(){ return m_loadedPlugins_objectCreator; }
