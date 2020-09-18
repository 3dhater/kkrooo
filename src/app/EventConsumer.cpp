#include "kkrooo.engine.h"
#include "Common.h"

#include "EventConsumer.h"
#include "Viewport/Viewport.h"
#include "Application.h"

EventConsumer::EventConsumer()
{
	memset(m_keys_down,0,256);
	memset(m_keys_down_once,0,256);
	memset(m_keys_down_once_state,0,256);
	memset(m_keys_up,0,256);
}

EventConsumer::~EventConsumer()
{
}


void EventConsumer::_reset()
{
	m_isLastKeyDownOnce = false;
	//m_input_update = false;
	m_lmb_up = false;
	m_rmb_up = false;
	m_mmb_up = false;
	m_lmb_once = false;
	m_rmb_once = false;
	m_mmb_once = false;
	memset(m_keys_up,0,256);
}

bool EventConsumer::isLmbDown()      {   return m_lmb_down;                                 }
bool EventConsumer::isLmbDownOnce() {   auto b = m_lmb_once; return b; }
bool EventConsumer::isLmbUp()        {   return m_lmb_up;                                   }
bool EventConsumer::isRmbDown()      {   return m_rmb_down;                                 }
bool EventConsumer::isRmbDownOnce() {   auto b = m_rmb_once; return b; }
bool EventConsumer::isRmbUp()        {   return m_rmb_up;                                   }
bool EventConsumer::isMmbDown()      {   return m_mmb_down;                                 }
bool EventConsumer::isMmbDownOnce() {   auto b = m_mmb_once; return b; }
bool EventConsumer::isMmbUp()        {   return m_mmb_up;                                   }


bool EventConsumer::isKeyDown( kkKey k )
{
	return m_keys_down[(s32)k]==1 ? true : false;
}

bool EventConsumer::isKeyDownOnce( kkKey k )
{
	bool r = m_keys_down_once[(s32)k]==1 ? true : false;
	if( r )
	{
		m_isLastKeyDownOnce = true;
		m_keys_down_once[(s32)k] = 0;
	}
	return r;
}

void EventConsumer::restoreLastKeyDownOnce()
{
	if( m_isLastKeyDownOnce )
	{
		m_keys_down_once[(s32)m_lastKeyDownOnce] = 1;
		m_isLastKeyDownOnce = false;
	}
}


bool EventConsumer::isKeyUp( kkKey k )
{
	bool r = m_keys_up[(s32)k]==1 ? true : false;
	m_keys_up[(s32)k] = 0;
	return r;
}

void EventConsumer::processEvent( const kkEvent& ev )
{
	switch(ev.type)
	{
	case kkEventType::System:
	{
		switch (ev.systemEvent.action)
		{
		case kkEventSystemAction::Timer:
			break;
		case kkEventSystemAction::BeginEventLoop:
			_reset();
			break;
		case kkEventSystemAction::EndEventLoop:
			break;
		}

	}break;
	case kkEventType::Window:
	{
		if( ev.windowEvent.eventID == kkEventWindowAction::FocusSet )
		{
			if( ev.windowEvent.window == m_app->m_mainWindow.ptr() )
			{
				m_app->m_activeOSWindow = E_WINDOW_ID::EWID_MAIN_WINDOW;
				//printf("focus EWID_MAIN_WINDOW\n");
			}
			else if( ev.windowEvent.window == m_app->m_materialEditorWindow.ptr() )
			{
				m_app->m_activeOSWindow = E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW;
				//printf("focus EWID_MATERIALEDITOR_WINDOW\n");
			}
			else if( ev.windowEvent.window == m_app->m_renderWindow.ptr() )
			{
				m_app->m_activeOSWindow = E_WINDOW_ID::EWID_RENDER_WINDOW;
			}
			else if( ev.windowEvent.window == m_app->m_importExportWindow.ptr() )
			{
				m_app->m_activeOSWindow = E_WINDOW_ID::EWID_IMPORTEXPORT_WINDOW;
			}
			else
			{
				KK_PRINT_FAILED;
			}

			// это здесь потому что возникает проблема когда фокус находится не у главного окна,
			// и при переходе в главное окно, если кликаем во вьюпорт, то активно гизмо (даже и 
			//  без клика это видно), и модель улетает в даль если это например перемещение
			if( m_app ) m_app->onWindowActivate();
		}

		if( ev.windowEvent.eventID == kkEventWindowAction::Activate )
		{
			memset(m_keys_down,0,256);
			if( m_app )
			{
				m_app->onWindowActivate();
			}
		}

		if( ev.windowEvent.eventID == kkEventWindowAction::Minimize )
		{
			if( m_app ) m_app->onWindowMinimize();
		}

		if( ev.windowEvent.eventID == kkEventWindowAction::Restore )
		{
			if( m_app ) m_app->onWindowRestore();
		}

		if( ev.windowEvent.eventID == kkEventWindowAction::Maximize )
		{
			if( m_app ) m_app->onWindowRestore();
			if( m_app ) m_app->onWindowMaximize();
		}

	}break;
	case kkEventType::Keyboard:
	{
		if( ev.keyboardEvent.state_is_pressed == 1 )
		{
			if( ev.keyboardEvent.character )
			{
				Kr::Gui::GuiSystem::m_character = (char16_t)ev.keyboardEvent.character;
			}

			switch (ev.keyboardEvent.key)
			{
			case kkKey::K_A:
				Kr::Gui::GuiSystem::m_IsA = true;
				break;
			case kkKey::K_X:
				Kr::Gui::GuiSystem::m_IsX = true;
				break;
			case kkKey::K_C:
				Kr::Gui::GuiSystem::m_IsC = true;
				break;
			case kkKey::K_V:
				Kr::Gui::GuiSystem::m_IsV = true;
				break;
			case kkKey::K_LEFT:
				Kr::Gui::GuiSystem::m_IsLeft = true;
				break;
			case kkKey::K_RIGHT:
				Kr::Gui::GuiSystem::m_IsRight = true;
				break;
			case kkKey::K_DELETE:
				Kr::Gui::GuiSystem::m_IsDelete = true;
				break;
			case kkKey::K_BACKSPACE:
				Kr::Gui::GuiSystem::m_IsBackspace = true;
				break;
			case kkKey::K_HOME:
				Kr::Gui::GuiSystem::m_IsHome = true;
				break;
			case kkKey::K_END:
				Kr::Gui::GuiSystem::m_IsEnd = true;
				break;
			default:
				break;
			}
		}
		if( ev.keyboardEvent.state_is_pressed == 1 )
		{
			m_keys_down[(s32)ev.keyboardEvent.key] = 1;

			if( m_keys_down_once_state[(s32)ev.keyboardEvent.key] == 0 )
			{
				m_keys_down_once[(s32)ev.keyboardEvent.key] = 1;
				m_keys_down_once_state[(s32)ev.keyboardEvent.key] = 1;
			}
		}
		
		if( ev.keyboardEvent.state_is_relesed == 1)
		{
			m_keys_down[(s32)ev.keyboardEvent.key] = 0;
			m_keys_down_once[(s32)ev.keyboardEvent.key] = 0;
			m_keys_down_once_state[(s32)ev.keyboardEvent.key] = 0;

			m_keys_up[(s32)ev.keyboardEvent.key] = 1;
		}


		m_input_update = true;

	}break;
	case kkEventType::MouseWheel:
	{
		Kr::Gui::GuiSystem::m_wheel_delta = ev.mouseEvent.wheel;
	//	printf("%i\n",ev.mouseEvent.wheel);

		if( ev.mouseEvent.wheel != 0 )
		{
			m_input_update = true;
		}
	}break;
	case kkEventType::Mouse:
	{
		if( ev.mouseEvent.x != m_cursor_coords_old.x )
		{
			m_input_update = true;
		}

		if( ev.mouseEvent.y != m_cursor_coords_old.y )
		{
			m_input_update = true;
		}

		m_cursor_coords_old.x = ev.mouseEvent.x;
		m_cursor_coords_old.y = ev.mouseEvent.y;

		if( ev.mouseEvent.isLeftButtonDouble() )
		{
			Kr::Gui::GuiSystem::m_IsLMBDouble = true;
		}
		if( ev.mouseEvent.isLeftButtonDown() )  { m_lmb_down = true;  m_input_update = true; if( !m_lmb_once_state ){m_lmb_once = true;m_lmb_once_state = true;}}
		if( ev.mouseEvent.isRightButtonDown() ) { m_rmb_down = true;  m_input_update = true; if( !m_rmb_once_state ){m_rmb_once = true;m_rmb_once_state = true;}}
		if( ev.mouseEvent.isMiddleButtonDown() ){ m_mmb_down = true;  m_input_update = true; if( !m_mmb_once_state ){m_mmb_once = true;m_mmb_once_state = true;}}
		if( ev.mouseEvent.isLeftButtonUp()   )  { m_lmb_down = false; m_input_update = true;  m_lmb_up = true; m_lmb_once_state = false; }
		if( ev.mouseEvent.isRightButtonUp()   ) { m_rmb_down = false; m_input_update = true;  m_rmb_up = true; m_rmb_once_state = false; }
		if( ev.mouseEvent.isMiddleButtonUp()   ){ m_mmb_down = false; m_input_update = true;  m_mmb_up = true; m_mmb_once_state = false; }

	}break;
	default:
		break;
	}
}