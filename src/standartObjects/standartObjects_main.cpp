// SPDX-License-Identifier: GPL-3.0-only
#include "common.h"

template<>
kkPluginIDData* kkSingleton<kkPluginIDData>::s_instance = nullptr;

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
		out_info->m_full_description  = u"Standart objects.";
		out_info->m_short_description = u"Standart objects.";
		out_info->m_name              = u"Standart objects generator.";
		out_info->m_type              = kkPluginType::object_creator;

		out_info->m_majorVersion	  = 1;
		out_info->m_minorVersion	  = 0;


		out_info->m_pluginID.m_data     = kkSingleton<kkPluginIDData>::s_instance;
		out_info->m_pluginID.m_dataSize = sizeof(kkPluginIDData);

		return 1;
	}


	KK_API int KK_C_DECL kkPlugin_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* filePath )
	{

		return 1;
	}


	KK_API int KK_C_DECL kkPluginInit( kkPluginCommonInterface* CI )
	{
		if( kkSingleton<kkPluginIDData>::s_instance ) delete kkSingleton<kkPluginIDData>::s_instance;
		kkSingleton<kkPluginIDData>::s_instance = new kkPluginIDData;
		auto staticData = kkSingleton<kkPluginIDData>::s_instance;

		if( kkSingleton<ParametersPlane>::s_instance ) delete kkSingleton<ParametersPlane>::s_instance;
		kkSingleton<ParametersPlane>::s_instance = new ParametersPlane;
		auto planeParams = kkSingleton<ParametersPlane>::s_instance;

		if( kkSingleton<ParametersBox>::s_instance ) delete kkSingleton<ParametersBox>::s_instance;
		kkSingleton<ParametersBox>::s_instance = new ParametersBox;
		auto boxParams = kkSingleton<ParametersBox>::s_instance;

		staticData->m_ci = CI;
		staticData->m_uniqueNumber = staticData->returnMagic() + KK_MAKEFOURCC('S','T','D','O');
		
		kkPluginID plugin_id;
		plugin_id.m_data = staticData;
		plugin_id.m_dataSize = sizeof(kkPluginIDData);

		auto gui      = CI->GetGUI();
		auto main_cat = CI->CreateObjectCategory(u"Default objects", plugin_id);


		auto poly_cat = main_cat->AddSubCategory(u"Polygonal objects");

		poly_cat->AddItem( u"Plane", planeCallback, 0 );

		planeParams->window = gui->createWindow();
		planeParams->window->SetName(u"Plane parameters");
		planeParams->window->SetType(kkPluginGUIWindowType::Parameters);
		
		planeParams->window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"Size:", 0xffffffff, 0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"X:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		// надо сохранить элемент, чтобы потом изменить значение - поменять указатель
		planeParams->width_element = planeParams->window->AddValueSelectorFloat(&planeParams->width, 0.1f, true, v2f(130.f, 20.f), planeCallback, kkPluginGUIParameterType::Object);
		// почти то же самое, только надо будет поменять текст
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"Y:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		planeParams->height_element = planeParams->window->AddValueSelectorFloat(&planeParams->height, 0.1f, true, v2f(130.f, 20.f), planeCallback, kkPluginGUIParameterType::Object);
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"Segments:", 0xffffffff, 0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"X:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		planeParams->width_segments_element = planeParams->window->AddRangeSliderInt(1, 100, &planeParams->width_segments, 1.f, true, v2f(130.f, 20.f), planeCallback, kkPluginGUIParameterType::Object);
		planeParams->width_segments_text_element = planeParams->window->AddText(u"1", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddText(u"Y:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		planeParams->height_segments_element = planeParams->window->AddRangeSliderInt(1, 100, &planeParams->height_segments, 1.f, true, v2f(130.f, 20.f), planeCallback, kkPluginGUIParameterType::Object);
		planeParams->height_segments_text_element = planeParams->window->AddText(u"1", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		planeParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		// пока надо создавать элементы используя kkPluginGUIParameterType::Object
		// то что ниже - на будущее
		/*auto bc_vertex = [](s32 id, void * d){printf("On button - vertex\n");};
		planeParams->window->AddButton(u"Vertex", v2f(80.f, 20.f), bc_vertex, -1, kkPluginGUIParameterType::Vertex);*/
		planeParams->window->SetOnActivate(onActivatePlaneCallback);

		poly_cat->AddItem( u"Box", boxCallback, 0 );

		boxParams->window = gui->createWindow();
		boxParams->window->SetName(u"Box parameters");
		boxParams->window->SetType(kkPluginGUIWindowType::Parameters);
		boxParams->window->SetOnActivate(onActivateBoxCallback);
		boxParams->window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Size:", 0xffffffff, 0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"X:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->size_x_element = boxParams->window->AddValueSelectorFloat(&boxParams->size_x, 0.1f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Y:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->size_y_element = boxParams->window->AddValueSelectorFloat(&boxParams->size_y, 0.1f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Z:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->size_z_element = boxParams->window->AddValueSelectorFloat( &boxParams->size_z, 0.1f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Segments:", 0xffffffff, 0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"X:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->seg_x_element = boxParams->window->AddRangeSliderInt(1, 200, &boxParams->x_segments, 1.f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->text_seg_x_element = boxParams->window->AddText(u"1", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Y:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->seg_y_element = boxParams->window->AddRangeSliderInt(1, 200, &boxParams->y_segments, 1.f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->text_seg_y_element = boxParams->window->AddText(u"1", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
		boxParams->window->AddText(u"Z:", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);
		boxParams->seg_z_element = boxParams->window->AddRangeSliderInt(1, 200, &boxParams->z_segments, 1.f, true, v2f(130.f, 20.f), boxCallback, kkPluginGUIParameterType::Object);
		boxParams->text_seg_z_element = boxParams->window->AddText(u"1", 0xffffffff, 3.f, kkPluginGUIParameterType::Object);

		return 1;
	}

	KK_API int KK_C_DECL kkPluginShutdown( kkPluginCommonInterface* CI )
	{
		if( kkSingleton<kkPluginIDData>::s_instance )
		{
			delete kkSingleton<kkPluginIDData>::s_instance;
			kkSingleton<kkPluginIDData>::s_instance = nullptr;
		}
		if( kkSingleton<ParametersPlane>::s_instance )
		{
			delete kkSingleton<ParametersPlane>::s_instance;
			kkSingleton<ParametersPlane>::s_instance = nullptr;
		}
		if( kkSingleton<ParametersBox>::s_instance )
		{
			delete kkSingleton<ParametersBox>::s_instance;
			kkSingleton<ParametersBox>::s_instance = nullptr;
		}
		return 1;
	}
}