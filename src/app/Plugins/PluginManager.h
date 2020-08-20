// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

#include "PluginCommonInterface.h"
#include "Classes/Strings/kkString.h"

//#define PY_SSIZE_T_CLEAN
//#include <Python.h>

#include <vector>

class Plugin;
class PluginManager
{
	// доступные для загрузки плагины. ожидается что все функции на месте
	std::vector<kkString> m_availablePlugins;
	
	std::vector<Plugin*> m_loadedPlugins;

	// массивы для быстрого доступа к загруженным плагинам
	// должны быть read-only
	std::vector<Plugin*> m_loadedPlugins_importMesh;
	std::vector<Plugin*> m_loadedPlugins_exportMesh;
	std::vector<Plugin*> m_loadedPlugins_importImage;
	std::vector<Plugin*> m_loadedPlugins_exportImage;
	std::vector<Plugin*> m_loadedPlugins_renderer;
	std::vector<Plugin*> m_loadedPlugins_objectCreator;

	//void _readScripts( std::vector<kkString>& );
	void _readPlugins( const std::vector<kkString>& );

	//void _initPython();

	PluginCommonInterface * m_commonInterface = nullptr;
public:
	PluginManager();
	virtual ~PluginManager();

	bool init(PluginCommonInterface*);

	const std::vector<Plugin*>& getExportModelPlugins();
	const std::vector<Plugin*>& getImportModelPlugins();
	const std::vector<Plugin*>& getImportImagePlugins();
	const std::vector<Plugin*>& getExportImagePlugins();
	const std::vector<Plugin*>& getObjectCreatorPlugins();
};

#endif