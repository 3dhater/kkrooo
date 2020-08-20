// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Common/kkInfoSharedLibrary.h"
#include "MainMenuCommands.h"
#include "../Plugins/Plugin.h"
#include "../Application.h"
#include "../Geometry/GeometryCreator.h"
#include "../Scene3D/Scene3D.h"


void Application::_processMainMenuCommand()
{
	switch( m_mainMenuCommand.type )
	{
	case MainMenuCommandType::None:
	default:
		return;
	case MainMenuCommandType::Export:
		_processMainMenuCommand_exportModel();
		break;
	case MainMenuCommandType::Import:
		_processMainMenuCommand_importModel();
		break;
	case MainMenuCommandType::ImportWithFileName:
		_processMainMenuCommand_importModelWithFileName();
		break;
	case MainMenuCommandType::NewScene:
		_processMainMenuCommand_newScene();
		break;
	case MainMenuCommandType::ImportDirectly:
		_processMainMenuCommand_importModelDirectly();
		break;
	case MainMenuCommandType::ApplyMatrices:
		_applyMatrices();
		break;
	case MainMenuCommandType::ShowMaterialEditor:
		showMaterialEditor(true);
		break;
	case MainMenuCommandType::ShowRenderWindow:
		showRenderWindow(true);
		break;
	}

	m_mainMenuCommand.type = MainMenuCommandType::None;
}

void Application::_processMainMenuCommand_exportModel()
{
	exportModel(m_mainMenuCommand.plugin);
	m_current_scene3D->test();
}

void Application::_processMainMenuCommand_importModel()
{
	importModel(m_mainMenuCommand.plugin);
	m_geomCreator->reset();
}

void Application::_processMainMenuCommand_newScene()
{
	this->resetScene3D();
}

void Application::_processMainMenuCommand_importModelWithFileName()
{
	//m_state_app = AppState_main::PluginGUI;
	_openImportWindowWithAllTypes();
	//importModel(m_mainMenuCommand.plugin);
    m_state_app = AppState_main::Idle;
    m_active_viewport->onNewObject();
	m_geomCreator->reset();
}

void              Application::_processMainMenuCommand_importModelDirectly()
{
    m_mainMenuCommand.plugin->call_onActivate(m_plugin_interface,kkPluginGUIWindowType::Import, (const char16_t*)m_lastFilePath.data());

    m_state_app = AppState_main::Idle;
    m_active_viewport->onNewObject();
	m_geomCreator->reset();
}
