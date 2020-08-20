// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_MAINMENUKOMMANDOS_H__
#define __KK_MAINMENUKOMMANDOS_H__

enum class MainMenuCommandType : u32
{
	None,
	Import,
	Export,
	NewScene,
	ApplyMatrices,
	ImportWithFileName,
	ImportDirectly,
	ShowMaterialEditor,
	ShowRenderWindow,
};

class Plugin;
struct MainMenuCommandInfo
{
	MainMenuCommandType type   = MainMenuCommandType::None;
	Plugin *            plugin = nullptr;                    // for Import
};


#endif