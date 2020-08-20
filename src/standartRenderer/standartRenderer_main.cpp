// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include "Common.h"

#include "Renderer.h"
#include "MaterialDefault.h"
#include "ParameterColor.h"
#include "ParameterBitmap.h"



template<>
StaticData* kkSingleton<StaticData>::s_instance = nullptr;

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
		out_info->m_extensions        = u"";
		out_info->m_extension_description = u"";
		out_info->m_full_description  = u"Standart renderer.";
		out_info->m_short_description = u"Standart renderer.";
		out_info->m_name              = u"Default";
		out_info->m_type              = kkPluginType::renderer;

		out_info->m_majorVersion	  = 1;
		out_info->m_minorVersion	  = 0;

		return 1;
	}

	KK_API int KK_C_DECL kkPlugin_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* file_path )
	{

		return 1;
	}

	KK_API int KK_C_DECL kkPluginInit( kkPluginCommonInterface* CI )
	{

		kkSingleton<StaticData>::s_instance = new StaticData;
		kkSingleton<StaticData>::s_instance->m_ci = CI;
		auto staticData = kkSingleton<StaticData>::s_instance;

		auto renderer = kkCreate<Renderer>();
		CI->AddRenderer(renderer);

		CI->AddMaterialType(u"Default", renderer, CreateMaterial_Default);
		CI->AddMaterialType(u"Toon", renderer, CreateMaterial_Default);
		CI->AddMaterialType(u"Shadow", renderer, CreateMaterial_Default);
		CI->AddMaterialParameter(u"Color", renderer, CreateParameter_Color);
		CI->AddMaterialParameter(u"Bitmap", renderer, CreateParameter_Bitmap);

		return 1;
	}

	KK_API int KK_C_DECL kkPluginShutdown( kkPluginCommonInterface* CI )
	{
		//printf("Destroy plugin;\n");
		if( kkSingleton<StaticData>::s_instance )
		{
			delete kkSingleton<StaticData>::s_instance;
			kkSingleton<StaticData>::s_instance = nullptr;
		}
		return 1;
	}

}