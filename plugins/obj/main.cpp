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
		out_info->m_extensions        = u"obj";
		out_info->m_extension_description   
									  = u"Wavefront OBJ";
		out_info->m_full_description  = u"Plugin for import and export .obj files.";
		out_info->m_short_description = u"OBJ loader.";
		out_info->m_name              = u"OBJ Import/Export plugin.";
		out_info->m_type              = kkPluginType::mesh_loader;

		out_info->m_majorVersion	  = 1;
		out_info->m_minorVersion	  = 0;

		return 1;
	}

	// общая функция для всех типов плагинов
	// вызывается при импорте\экспорте (потом надо дополнить этот список)
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

	void on_import( s32 id, void * _data )
	{
		ImportData * data = (ImportData *)_data;
		kkPlugin_onImportMesh( kkSingleton<ImportData>::s_instance->common_interface, kkSingleton<ImportData>::s_instance->file_path.data() );
	}

	void on_export( s32 id, void * _data )
	{
		ImportData * data = (ImportData *)_data;
		kkPlugin_onExportMesh( kkSingleton<ImportData>::s_instance->common_interface, kkSingleton<ImportData>::s_instance->file_path.data() );
	}

	KK_API int KK_C_DECL kkPluginInit( kkPluginCommonInterface* CI )
	{
		if( kkSingleton<ImportData>::s_instance )
			delete kkSingleton<ImportData>::s_instance;
		kkSingleton<ImportData>::s_instance = new ImportData;

		ImportData * importData = kkSingleton<ImportData>::s_instance;

		auto gui           = CI->GetGUI();

		importData->common_interface = CI;

		// создать окна для импорта и для экспорта
		importData->import_window = gui->createWindow();
		importData->export_window = gui->createWindow();

		// установить заголовок
		importData->import_window->SetName(u"Import OBJ");
		importData->export_window->SetName(u"Export OBJ");

		// установить тип
		// по умолчанию стоит kkPluginGUIWindowType::_custom
		// изменение типа для обычных окон просто изменит текст на кнопке ("OK", "Import", "Export").
		importData->import_window->SetType(kkPluginGUIWindowType::Import);
		importData->export_window->SetType(kkPluginGUIWindowType::Export);

		// можно изменить размер
		importData->import_window->SetSize(v2i(300,300));
		importData->export_window->SetSize(v2i(300,300));

		// передать данные для того чтобы потом был к ним доступ из коллбэк функций
		importData->import_window->SetUserData(importData);
		importData->export_window->SetUserData(importData);

		importData->import_window->SetOnOK(on_import);
		importData->export_window->SetOnOK(on_export);


		// заполнение окон
		
		
		importData->import_window->BeginGroup(u"Common", true);
		importData->import_window->AddNewLine(5.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Import materials",&importData->option_import_materials, kkPluginGUIParameterType::Object);
		importData->import_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Triangulate",&importData->option_triangulate, kkPluginGUIParameterType::Object);
		importData->import_window->EndGroup();

		importData->import_window->BeginGroup(u"Fix", true);
		importData->import_window->AddNewLine(5.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Flip normals",&importData->fix_flip_normals, kkPluginGUIParameterType::Object);
		importData->import_window->AddNewLine(5.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Generate normals",&importData->fix_generate_normals, kkPluginGUIParameterType::Object);
		importData->import_window->AddNewLine(5.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Generate flat normals",&importData->fix_generate_flat_normals, kkPluginGUIParameterType::Object);
		importData->import_window->AddNewLine(5.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		importData->import_window->AddCheckBox(u"Z Up",&importData->fix_z_up, kkPluginGUIParameterType::Object);
		importData->import_window->EndGroup();


		/*importData->import_window->treeBegin("Common",true);
		importData->import_window->addCheckbox("Import materials",&importData->option_import_materials);
		importData->import_window->addCheckbox("Triangulate",&importData->option_triangulate);
		importData->import_window->treeEnd();
		importData->import_window->treeBegin("Fix",true);
		importData->import_window->addCheckbox("Flip normals",&importData->fix_flip_normals);
		importData->import_window->addCheckbox("Generate normals",&importData->fix_generate_normals);
		importData->import_window->addCheckbox("Generate flat normals",&importData->fix_generate_flat_normals);
		importData->import_window->addCheckbox("Z Up",&importData->fix_z_up);
		importData->import_window->treeEnd();


		importData->export_window->treeBegin("Common",true);
		importData->export_window->addCheckbox("Selected only",&importData->selected_only);
		importData->export_window->addSeparator();
		importData->export_window->addCheckbox("Write materials",&importData->option_write_materials);
		importData->export_window->addCheckbox("Write normals",&importData->option_write_normals);
		importData->export_window->addCheckbox("Write UVs",&importData->option_write_UVs);
		importData->export_window->addSeparator();
		importData->export_window->addCheckbox("Triangulate",&importData->option_triangulate_export);
		importData->export_window->treeEnd();*/

		/*importData->export_window->treeBegin("Optimize",true);
		importData->export_window->addCheckbox("Optimize positions",&importData->optimize_positions);
		importData->export_window->addCheckbox("Optimize normals",&importData->optimize_normals);
		importData->export_window->addCheckbox("Optimize UVs",&importData->optimize_UVs);
		importData->export_window->treeEnd();*/


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