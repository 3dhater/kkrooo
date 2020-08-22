// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"
#include "common.h"

template<>
ImportData* kkSingleton<ImportData>::s_instance = nullptr;

extern "C" 
{

	KK_API int KK_C_DECL kkPluginIsDebug()
	{
#ifdef KK_DEBUG
		return 1;
#else
		return 0;
#endif
	}

	KK_API int KK_C_DECL kkPluginGetPluginInfo( kkPluginInfo* out_info )
	{
		if( !out_info )
		{
			return 0;
		}

		out_info->m_author            = u"Artyom Basov";
		out_info->m_extensions        = u"3ds";
		//out_info->m_extensions        = u"3ds obj"; // плагин может грузить и другие форматы...
		out_info->m_extension_description   
									  = u"3ds (assimp)";
		out_info->m_full_description  = u"Plugin for import .3ds files.";
		out_info->m_short_description = u"3ds loader.";
		out_info->m_name              = u"3ds Import plugin.";
		out_info->m_type              = kkPluginType::mesh_loader;

		out_info->m_majorVersion	  = 1;
		out_info->m_minorVersion	  = 0;

		return 1;
	}

	KK_API int KK_C_DECL kkPlugin_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* filePath )
	{
		kkSingleton<ImportData>::s_instance->file_path = filePath;

		switch (windowType)
		{
		case kkPluginGUIWindowType::Custom:
			break;
		case kkPluginGUIWindowType::Import:
			kkSingleton<ImportData>::s_instance->import_window->Activate();
			break;
		case kkPluginGUIWindowType::Export:
			kkSingleton<ImportData>::s_instance->export_window->Activate();
			break;
		case kkPluginGUIWindowType::Material:
			break;
		case kkPluginGUIWindowType::Parameters:
			break;
		default:
			break;
		}
		return 1;
	}

	void onOK_import(s32 id, void * _data )
	{
		ImportData * data = (ImportData *)_data;
		kkPlugin_onImportMesh( kkSingleton<ImportData>::s_instance->common_interface, kkSingleton<ImportData>::s_instance->file_path.data() );
	}

	void onOK_export(s32 id, void * _data )
	{
		ImportData * data = (ImportData *)_data;
	}

	KK_API int KK_C_DECL kkPluginInit( kkPluginCommonInterface* CI )
	{
		if( kkSingleton<ImportData>::s_instance )
			delete kkSingleton<ImportData>::s_instance;
		kkSingleton<ImportData>::s_instance = new ImportData;

		auto gui           = CI->GetGUI();
		
		kkSingleton<ImportData>::s_instance->common_interface = CI;
		
		kkSingleton<ImportData>::s_instance->import_window = gui->createWindow();
		kkSingleton<ImportData>::s_instance->export_window = gui->createWindow();

		kkSingleton<ImportData>::s_instance->import_window->SetName(u"Import 3ds");
		kkSingleton<ImportData>::s_instance->export_window->SetName(u"Export 3ds");

		kkSingleton<ImportData>::s_instance->import_window->SetSize(v2i(400,300));

		kkSingleton<ImportData>::s_instance->import_window->SetType(kkPluginGUIWindowType::Import);
		kkSingleton<ImportData>::s_instance->export_window->SetType(kkPluginGUIWindowType::Export);

		kkSingleton<ImportData>::s_instance->import_window->SetUserData(kkSingleton<ImportData>::s_instance);
		kkSingleton<ImportData>::s_instance->export_window->SetUserData(kkSingleton<ImportData>::s_instance);

		kkSingleton<ImportData>::s_instance->import_window->SetOnOK(onOK_import);
		kkSingleton<ImportData>::s_instance->export_window->SetOnOK(onOK_export);

		return 1;
	}

	

	KK_API int KK_C_DECL kkPluginShutdown( kkPluginCommonInterface* CI )
	{
		if( kkSingleton<ImportData>::s_instance )
			delete kkSingleton<ImportData>::s_instance;
		kkSingleton<ImportData>::s_instance = nullptr;

		return 1;
	}

}