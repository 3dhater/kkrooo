// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

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
		out_info->m_extensions        = u"dds";
		out_info->m_extension_description   
									  = u"DDS";
		out_info->m_full_description  = u"Plugin for import .dds files.";
		out_info->m_short_description = u"DDS loader.";
		out_info->m_name              = u"DDS import plugin.";
		out_info->m_type              = kkPluginType::image_loader;

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
		return 1;
	}

	KK_API int KK_C_DECL kkPluginShutdown( kkPluginCommonInterface* CI )
	{
		return 1;
	}
}