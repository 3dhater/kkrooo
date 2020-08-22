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
		importData->import_window->SetSize(v2i(400,300));
		importData->export_window->SetSize(v2i(400,300));

		// передать данные для того чтобы потом был к ним доступ из коллбэк функций
		importData->import_window->SetUserData(importData);
		importData->export_window->SetUserData(importData);

		importData->import_window->SetOnOK(on_import);
		importData->export_window->SetOnOK(on_export);


		// заполнение окон
		// для демонстрации
		// лучше не делать вложенные tabbar и treenode
		/*importData->import_window->tabbarBegin("tabbar");
			importData->import_window->tabbarItemBegin("First");
				importData->import_window->addButton("button1",0);
				importData->import_window->addButton("button2",0);
				importData->import_window->addButton("button3",0);
			importData->import_window->tabbarItemEnd();
			importData->import_window->tabbarItemBegin("Second");
				importData->import_window->addButton("Same line 1",0);
				importData->import_window->sameLine(0.f);
				importData->import_window->addSmallButton("Same line 2",0);
				importData->import_window->addButton("With offset",0);
				importData->import_window->sameLine(140.f);
				importData->import_window->addButton("this",0);
			importData->import_window->tabbarItemEnd();
		importData->import_window->tabbarEnd();
		importData->import_window->addSeparator(); // простая линия
		importData->import_window->treeBegin("opened tree",true);
			enum arrowButtonDir
			{
				arrowButtonDir_None    = -1,
				arrowButtonDir_Left    = 0,
				arrowButtonDir_Right   = 1,
				arrowButtonDir_Up      = 2,
				arrowButtonDir_Down    = 3,
			};
			importData->import_window->addArrowButton("ab1",arrowButtonDir::arrowButtonDir_None,0);
			importData->import_window->sameLine(0.f);
			importData->import_window->addArrowButton("ab2",arrowButtonDir::arrowButtonDir_Left,0);
			importData->import_window->sameLine(0.f);
			importData->import_window->addArrowButton("ab3",arrowButtonDir::arrowButtonDir_Right,0);
			importData->import_window->sameLine(0.f);
			importData->import_window->addArrowButton("ab4",arrowButtonDir::arrowButtonDir_Up,0);
			importData->import_window->sameLine(0.f);
			importData->import_window->addArrowButton("ab5",arrowButtonDir::arrowButtonDir_Down,0);
		importData->import_window->treeEnd();
		importData->import_window->treeBegin("closed tree",false);
			static bool check_box_1 = true;
			static bool check_box_2 = false;
			importData->import_window->addCheckbox("Checkbox1",&check_box_1);
			importData->import_window->addCheckbox("Checkbox2",&check_box_2);
			static int radio_0_0 = 0;
			importData->import_window->addRadioButton("R1,",&radio_0_0, 0);
			importData->import_window->addRadioButton("R2,",&radio_0_0, 1);
			importData->import_window->addRadioButton("R3,",&radio_0_0, 2);
			importData->import_window->addSeparator();
			static int radio_0_1 = 0;
			importData->import_window->addRadioButton("R11,",&radio_0_1, 0);
			importData->import_window->addRadioButton("R12,",&radio_0_1, 1);
			importData->import_window->addRadioButton("R13,",&radio_0_1, 2);
			importData->import_window->addText("text");
		importData->import_window->treeEnd();
		importData->import_window->childBegin("child1",v2f(150,50),true);
			importData->import_window->addText("Child window");
		importData->import_window->childEnd();
		importData->import_window->sameLine();
		importData->import_window->childBegin("child2",v2f(150,50),true);
			static float progress = 0.5f;
			importData->import_window->addProgressbar(&progress);
		importData->import_window->childEnd();*/

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