#include "kkrooo.engine.h"
#include "../Common.h"

#include "Common/kkInfoSharedLibrary.h"

#include "Plugins/kkPlugin.h"

#include "PluginManager.h"
#include "FileSystem/kkFileSystem.h"
#include "Common/kkUtil.h"

#include "Plugin.h"

Plugin::Plugin()
{
}

Plugin::~Plugin()
{
}

bool Plugin::init()
{
	return true;
}

const char16_t * Plugin::getFilePath() const 
{
	return m_file_path.c_str();
}

const kkPluginInfo& Plugin::getInfo() const
{
	return m_info;
}

int  Plugin::call_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* file_path )
{
	kkSingleton<PluginCommonInterface>::s_instance->m_filePath = file_path;

	if( m_type == PluginType::sharedLib )
	{
		if( m_onActivate )
		{
			m_onActivate(CI,windowType, file_path);
		}
	}
	else
	{
		u32 wt = 0;
		switch (windowType)
		{
		case kkPluginGUIWindowType::Import:
			wt = 1;
			break;
		case kkPluginGUIWindowType::Export:
			wt = 2;
			break;
		case kkPluginGUIWindowType::Material:
		case kkPluginGUIWindowType::Parameters:
		case kkPluginGUIWindowType::Custom:
		default:
			break;
		}

	}
	return 1;
}

void Plugin::call_onImportMesh(kkPluginCommonInterface * ci,const char16_t * path)
{
	kkSingleton<PluginCommonInterface>::s_instance->m_filePath = path;

	if( m_type == PluginType::sharedLib )
	{
		kkPlugin_onImportMesh_t Plugin_onImportMesh = (kkPlugin_onImportMesh_t)KK_LOAD_FUNCTION(m_handle,"kkPlugin_onImportMesh");
		Plugin_onImportMesh(ci,path);
	}
	else
	{
		
	}
}

kkImage* Plugin::call_onImportImage(kkPluginCommonInterface * ci,const char16_t * path)
{
	kkSingleton<PluginCommonInterface>::s_instance->m_filePath = path;
	kkPlugin_onImportImage_t Plugin_onImportImage = (kkPlugin_onImportImage_t)KK_LOAD_FUNCTION(m_handle,"kkPlugin_onImportImage");
	return Plugin_onImportImage(ci, path);
}

void Plugin::call_onExportImage(kkPluginCommonInterface * ci, const char16_t * path, kkImage* image)
{
	kkSingleton<PluginCommonInterface>::s_instance->m_filePath = path;
	kkPlugin_onExportImage_t Plugin_onExportImage = (kkPlugin_onExportImage_t)KK_LOAD_FUNCTION(m_handle,"kkPlugin_onExportImage");
	Plugin_onExportImage(ci, path, image);
}