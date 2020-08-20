// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_IMPL_H__
#define __PLUGIN_IMPL_H__

#include "Plugins/kkPlugin.h"

enum class PluginType
{
	sharedLib,
};

class Plugin
{
	PluginType         m_type = PluginType::sharedLib;
	kkString           m_file_path; // "stdplugins/obj.kkp"
	KK_LIBRARY_HANDLE  m_handle = nullptr;
	kkPluginInfo       m_info;

	kkPluginShutdown_t    m_shutdown   = nullptr;
	kkPlugin_onActivate_t m_onActivate = nullptr;

	friend class Application;
public:
	Plugin();
	virtual ~Plugin();

	bool init();


	const char16_t * getFilePath() const ;
	const kkPluginInfo& getInfo() const  ;

	int  call_onActivate( kkPluginCommonInterface* CI, kkPluginGUIWindowType windowType, const char16_t* file_path );
	void call_onImportMesh(kkPluginCommonInterface *,const char16_t *);
	
	kkImage* call_onImportImage(kkPluginCommonInterface *,const char16_t *);
	void call_onExportImage(kkPluginCommonInterface *,const char16_t *, kkImage*);

	friend class PluginManager;
};

#endif