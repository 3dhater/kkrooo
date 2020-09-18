#include "kkrooo.engine.h"
#include "Common.h"

#include "Application.h"

void window_onMove(kkWindow* window)
{
	auto id = window->getID();
	switch (id)
	{
	case E_WINDOW_ID::EWID_MAIN_WINDOW:
		//kkSingleton<Application>::s_instance->drawAll();
		break;

	case E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW:
	case E_WINDOW_ID::EWID_RENDER_WINDOW:
		break;
	}
}

void window_onSize(kkWindow* window)
{
	//if( kkSingleton<Application>::s_instance->m_state_app == AppState_main::PluginGUI ) return;

	auto id = window->getID();
	switch (id)
	{
	case E_WINDOW_ID::EWID_MAIN_WINDOW:
		//kkGetMainSystem()->update();
		kkSingleton<Application>::s_instance->updateBuffers();
		kkSingleton<Application>::s_instance->updateViewports();
		kkSingleton<Application>::s_instance->onWindowSize();
		break;

	case E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW:
	case E_WINDOW_ID::EWID_RENDER_WINDOW:
		
		break;
	}

	
}

void window_onPaint(kkWindow* window)
{
	auto id = window->getID();
	switch (id)
	{
	case E_WINDOW_ID::EWID_MAIN_WINDOW:
		break;

	case E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW:
		
		break;
	}
}

void window_onClose(kkWindow* window)
{
	auto id = window->getID();

	switch (id)
	{
	case E_WINDOW_ID::EWID_MAIN_WINDOW:
		kkSingleton<Application>::s_instance->quit();
		break;

	case E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW:
		window->hide();
		kkSingleton<Application>::s_instance->m_mainWindow->setFocus(false);
		break;
	case E_WINDOW_ID::EWID_RENDER_WINDOW:
		window->hide();
		kkSingleton<Application>::s_instance->m_mainWindow->setFocus(false);
		break;
	case E_WINDOW_ID::EWID_IMPORTEXPORT_WINDOW:
		window->hide();
		kkSingleton<Application>::s_instance->m_mainWindow->setFocus(false);
		break;
	}

}