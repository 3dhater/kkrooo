// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_GUIIMPL_H__
#define __PLUGIN_GUIIMPL_H__

#include "Classes/Containers/kkArray.h"

#include "Plugins/kkPluginGUI.h"

struct ColorTheme;
class PluginGUIWindow;
class Application;

class PluginGUI : public kkPluginGUI
{
	bool m_isWindowBegin = false;
	
	ColorTheme * m_currentColorTheme = nullptr;

	kkArray<PluginGUIWindow*> m_windows;

	Application * m_app = nullptr;
	friend class Application;
public:
	PluginGUI(Application *);
	virtual ~PluginGUI();
	
	kkPluginGUIWindow* createWindow();


	void drawNonChildWindow(u32& inFocus, v2i& cp);
};



#endif