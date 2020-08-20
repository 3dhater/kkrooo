// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PLUGIN_H__
#define __KK_PLUGIN_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"

#include "kkPluginGUI.h"

#include "kkPluginCommonInterface.h"

// Тип указывается в плагинах
namespace kkPluginType
{
	enum
	{
		mesh_loader    = BIT(0),  
		object_creator = BIT(1),  
		renderer       = BIT(2),  
		image_loader   = BIT(3),  
 	};
}

// Основная информация. Заполняется в плагинах внутри kkPluginGetPluginInfo
struct kkPluginInfo
{
	kkString m_name;
	kkString m_short_description;
	kkString m_full_description;
	kkString m_author;
	kkString m_url;
	kkString m_email;
	kkString m_company;
		
	/// "obj dae fbx"
	kkString m_extensions;
	kkString m_extension_description;

	u32    m_type  = 0;
	u32    m_majorVersion = 0;
	u32    m_minorVersion = 0;

	kkPluginID m_pluginID;
	// пока не используется
	/*void *   m_userData;
	u32    m_userDataSize = 0;*/
};

struct kkImage;

/// Эти функции экспортируют плагины. Значит каждый .cpp файл использующий эти функции должен объявить KK_EXPORTS перед включением "kkrooo.engine.h"

// return: 1 - good, 0 - bad
extern "C" KK_API int KK_C_DECL kkPluginGetPluginInfo(kkPluginInfo* info);
using kkPluginGetPluginInfo_t = int(KK_C_DECL*)(kkPluginInfo* info);

extern "C" KK_API int KK_C_DECL kkPluginIsDebug();
using kkPluginIsDebug_t = int(KK_C_DECL*)();

// return: 1 - good, 0 - bad
extern "C" KK_API int KK_C_DECL kkPlugin_onImportMesh(kkPluginCommonInterface *,const char16_t* file_path);
using kkPlugin_onImportMesh_t = int(KK_C_DECL*)(kkPluginCommonInterface *,const char16_t* file_path);

extern "C" KK_API kkImage* KK_C_DECL kkPlugin_onImportImage(kkPluginCommonInterface *,const char16_t* file_path);
using kkPlugin_onImportImage_t = kkImage*(KK_C_DECL*)(kkPluginCommonInterface *,const char16_t* file_path);

extern "C" KK_API void KK_C_DECL kkPlugin_onExportImage(kkPluginCommonInterface *,const char16_t* file_path, kkImage* image);
using kkPlugin_onExportImage_t = void(KK_C_DECL*)(kkPluginCommonInterface *,const char16_t* file_path, kkImage* image);

// return: 1 - good, 0 - bad
extern "C" KK_API int KK_C_DECL kkPlugin_onExportMesh(kkPluginCommonInterface *,const char16_t* file_path);
using kkPlugin_onExportMesh_t = int(KK_C_DECL*)(kkPluginCommonInterface *,const char16_t* file_path);

// вызывается при запуске программы
extern "C" KK_API int KK_C_DECL kkPluginInit(kkPluginCommonInterface *);
using kkPluginInit_t = int(KK_C_DECL*)(kkPluginCommonInterface *);

// вызывается при завершении программы
extern "C" KK_API int KK_C_DECL kkPluginShutdown(kkPluginCommonInterface *);
using kkPluginShutdown_t = int(KK_C_DECL*)(kkPluginCommonInterface *);

// перед работой плагина будет вызываться эта функция
// file_path может и не использоваться, если для плагина не нужно выбирать файл
extern "C" KK_API int KK_C_DECL kkPlugin_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* file_path );
using kkPlugin_onActivate_t = int(KK_C_DECL*)( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* file_path );

class kkPlugin
{
public:
	kkPlugin(){}
	virtual ~kkPlugin(){}
	
	//virtual 
};

#endif