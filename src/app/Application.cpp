// SPDX-License-Identifier: GPL-3.0-only
#define KRGUI_DEFAULT_FONT_FILE_PATH "../res/gui/defaultFont.data"
#include "kkrooo.engine.h"

#include "FileSystem/kkFileSystem.h"
#include "Common/kkUtil.h"

#include "EventConsumer.h"

#include "Scene3D/Scene3D.h"
#include "Scene3D/Scene3DObject.h"
#include "Geometry/GeometryCreator.h"
#include "GraphicsSystem/kkTexture.h"
#include "Application.h"
#include "ShortcutManager.h"
#include "Functions.h"

#include "Common/kkInfoSharedLibrary.h"

#include "Renderer/kkRenderer.h"
#include "RenderManager/RenderManager.h"

#include "Plugins/Plugin.h"
#include "Plugins/PluginGUI.h"
#include "Plugins/PluginGUIWindow.h"
#include "Plugins/PluginObjectCategory.h"


#include "Input/kkInputSystem.h"


#include "Gizmo.h"

#include "Shaders/simple.h"
#include "Shaders/points.h"
#include "Shaders/scene3DobjectDefault.h"
#include "Shaders/linemodel.h"
#include "Shaders/ShaderScene3DObjectDefault_polymodeforlinerender.h"

#include "Materials/MaterialEditor.h"
#include "GUI/GUIResources.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <filesystem>

#ifdef KK_PLATFORM_WINDOWS
#include <Windows.h>
#include <shobjidl.h> 
#pragma comment(lib, "opengl32.lib")
#else
#error IIIIIIMMMPPLLEEEEMEENT!!!
#endif

#include "window_callbacks.h"

using namespace Kr;

template<>
Application* kkSingleton<Application>::s_instance = nullptr;

extern ViewportMouseState g_mouseState;

template<>
PluginCommonInterface* kkSingleton<PluginCommonInterface>::s_instance = nullptr;
struct PluginCommonInterface_t
{
    PluginCommonInterface_t()
    {
    }

    ~PluginCommonInterface_t()
    {
         if( kkSingleton<PluginCommonInterface>::s_instance )
        {
            kkDestroy(kkSingleton<PluginCommonInterface>::s_instance);
            kkSingleton<PluginCommonInterface>::s_instance = nullptr;
        }
    }
    PluginCommonInterface * m_plugin_interface = nullptr;
}g_PluginCommonInterface;




//kkTexture * g_testTexture =nullptr;

Application::Application()
{
	kkSingleton<Application>::s_instance = this;

    m_guiStyle_mainToolbarButtons.buttonTextIdleColor  = Kr::Gui::ColorWhite;
	m_guiStyle_mainToolbarButtons.buttonTextHoverColor = Kr::Gui::ColorRed;
	m_guiStyle_mainToolbarButtons.buttonTextPushColor  = Kr::Gui::ColorDarkRed;
	m_guiStyle_mainToolbarButtons.buttonBackgroundAlpha = 0.f;
}

Application::~Application()
{
    for( auto o : m_objectGeneratorCategories )
	{
		kkDestroy(o);
	}

#ifdef KK_PLATFORM_WINDOWS
    if( m_fileSaveDialog ) m_fileSaveDialog->Release();
    CoUninitialize();
#else
#error IIIIIIMMMPPLLEEEEMEENT!!!
#endif
 //   if( g_testTexture )
  //      kkDestroy(g_testTexture);

    if( m_current_scene3D )
    {
        kkDestroy(m_current_scene3D);
    }

    for( auto * o : m_renderers )
    {
        kkDestroy(o);
    }

    if( m_KrGuiSystem )
        delete m_KrGuiSystem;

    //if( m_editPolygons_paramWindow )
       // kkDestroy(m_editPolygons_paramWindow);

    if( m_pluginGUI )
        kkDestroy(m_pluginGUI);

    if( m_geomCreator )
        kkDestroy(m_geomCreator);

    if( m_main_viewport )
    {
        kkDestroy(m_main_viewport);
    }

    
}

void Application::_init_OS_Windows()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if( FAILED(hr) )
    {
        KK_PRINT_FAILED;
        throw std::runtime_error("Failed CoInitializeEx :(");
    }

    hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&m_fileSaveDialog));
    if( FAILED(hr) )
    {
        KK_PRINT_FAILED;
        throw std::runtime_error("Failed CoCreateInstance - IID_IFileSaveDialog :(");
    }
}

void Application::_init_events()
{
    m_event_consumer  = std::make_unique<EventConsumer>();
    m_event_consumer->m_app = this;
	m_event_system = m_main_system->getEventSystem();
	m_event_system->setEventConsumer( m_event_consumer.get() );
}

void Application::_init_scene()
{
    m_scene_system = kkGetSceneSystem();
}

void Application::_init_shortcuts()
{
    m_shortcutManager = kkCreate<ShortcutManager>();
    if( !m_shortcutManager->init() )
    {
        KK_PRINT_FAILED;
		throw std::runtime_error("Failed to init shortcut manager :(");
    }
    m_shortcutManager->m_app = this;
    m_shortcutManager->m_eventConsumer = m_event_consumer.get();
}

void Application::_init_materialEditor(bool first)
{
    if( first )
    {
        m_materialEditorWindow = m_main_system->createWindow( kk::window::style::resize | kk::window::style::center | kk::window::style::maximize,
            v4i(0,0,1000,600), kk::window::state::hide, m_mainWindow.ptr(), E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW );
	    m_materialEditorWindow->set_onClose(window_onClose);
        m_materialEditorWindow->setWindowText(u"Material editor");
        m_gs->initWindow(m_materialEditorWindow.ptr());
        m_materialEditor = kkCreate<MaterialEditor>();
    }
    else
    {
        if( !m_materialEditor->init(m_gs.ptr(), m_KrGuiSystem, m_materialEditorWindow.ptr() ))
        {
            KK_PRINT_FAILED;
		    throw std::runtime_error("Failed to init materials :(");
        }
    }
}

void Application::_init_plugins()
{
    m_pluginGUI  = kkCreate<PluginGUI>(this);
    kkSingleton<PluginCommonInterface>::s_instance = kkCreate<PluginCommonInterface>();
    m_plugin_interface = kkSingleton<PluginCommonInterface>::s_instance;
    g_PluginCommonInterface.m_plugin_interface = m_plugin_interface;
    m_plugin_interface->m_pluginGUI = m_pluginGUI;
    m_plugin_interface->m_materialEditor = m_materialEditor.ptr();

    if( !m_plugin_manager.init(m_plugin_interface) )
    {
        KK_PRINT_FAILED;
		throw std::runtime_error("Failed to init plugin manager :(");
    }
    
    _setStandartObjectCategory();

    auto plmgr = getPluginManager();
	auto & plugins = plmgr->getImportModelPlugins();
	for( auto p : plugins )	{
		auto & info = p->getInfo();
        std::vector<kkString> extensions_array;
        util::stringGetWords<kkString>( &extensions_array, info.m_extensions );
		for( auto e : extensions_array )		{
			if( e == "obj" )			{
				m_OBJplugin = p;
				return;
			}
		}
	}
}

void Application::_init_mainWindow()
{
	m_mainWindow = m_main_system->createWindow( 
        kk::window::style::resize | kk::window::style::center | kk::window::style::maximize | kk::window::style::size_limit, 
        v4i(0,0,800,600), kk::window::state::hide, 0, E_WINDOW_ID::EWID_MAIN_WINDOW );
	if( !m_mainWindow.ptr() )
	{
		KK_PRINT_FAILED;
		throw std::runtime_error("Failed to create main window :(");
	}
	m_mainWindow->setWindowText( u"Kkrooo" );
	m_mainWindow->set_onMove(window_onMove);
	m_mainWindow->set_onPaint(window_onPaint);
	m_mainWindow->set_onSize(window_onSize);
	m_mainWindow->set_onClose(window_onClose);
}

//void Application::_init_materialEditorWindow()
//{
//    m_materialEditorWindow = m_main_system->createWindow( kk::window::style::resize | kk::window::style::center | kk::window::style::maximize,
//        v4i(0,0,1000,600), kk::window::state::hide, m_mainWindow.ptr(), E_WINDOW_ID::EWID_MATERIALEDITOR_WINDOW );
//	m_materialEditorWindow->set_onClose(window_onClose);
//    m_materialEditorWindow->setWindowText(u"Material editor");
//}

void Application::_init_renderWindow()
{
    m_renderWindow = m_main_system->createWindow(  
        kk::window::style::resize | kk::window::style::center | kk::window::style::maximize, 
        v4i(0,0,1000,600), 
        kk::window::state::hide,
        m_mainWindow.ptr(), 
        E_WINDOW_ID::EWID_RENDER_WINDOW );
	m_renderWindow->set_onClose(window_onClose);
    m_renderWindow->setWindowText(u"Render");
    m_gs->initWindow(m_renderWindow.ptr());
}

void Application::_init_gs()
{
    m_gs = m_main_system->createGraphicsSystem( m_mainWindow.ptr(), m_mainWindow->getWindowRect().getWidthAndHeight(), 32 );
	if( !m_gs.ptr() )
	{
		KK_PRINT_FAILED;
		throw std::runtime_error("Failed to create GS :(");
	}
    _createWhiteTexture();
    if( !_createShaders() )
    {
		KK_PRINT_FAILED;
		throw std::runtime_error("Failed to create shaders :(");
	}
    m_gs->setActive(m_mainWindow.ptr());
}

void Application::_init_input()
{
    m_input_system = m_main_system->getInputSystem();
}

void Application::_init_krgui()
{
    m_KrGuiSystem = Gui::CreateSystem(Gui::GraphicsSystemType::OpenGL3, "../res/fonts/noto/", "notosans.txt" );
    if( !m_KrGuiSystem )
    {
        KK_PRINT_FAILED;
		throw std::runtime_error("Failed to create KrGuiSystem:(");
    }

}

void Application::_init_renderManager()
{
    m_renderManager = kkCreate<RenderManager>();
    m_renderManager->init(m_gs.ptr(), m_KrGuiSystem, m_renderWindow.ptr());
}

void Application::init()
{
	m_main_system  = kkGetMainSystem();
    m_programName += u"Kkrooo";

#ifdef KK_PLATFORM_WINDOWS
    _init_OS_Windows();
#else
#error IMPLEMENT!!!!
#endif

    _init_events();
    _init_scene();
    _init_shortcuts();
    _init_mainWindow();
    _init_gs();
    _init_materialEditor(true);
    _init_renderWindow();

    m_importExportWindow = m_main_system->createWindow(  
        kk::window::style::resize | kk::window::style::center | kk::window::style::maximize, 
        v4i(0,0,100,100), 
        kk::window::state::hide,
        m_mainWindow.ptr(), 
        E_WINDOW_ID::EWID_IMPORTEXPORT_WINDOW );
	m_importExportWindow->set_onClose(window_onClose);
    m_importExportWindow->setWindowText(u"...");
    m_gs->initWindow(m_importExportWindow.ptr());

    _init_plugins();
   // _init_materialEditorWindow();
    _init_krgui();

    _init_input();

    m_commonGUIStyle.groupBackgroundAlpha = 0.f;
    m_commonGUIStyle.buttonBackgroundAlpha = 0.f;
    m_commonGUIStyle.buttonIdleColor1 = 0xffFFFFFF;
    m_commonGUIStyle.buttonIdleColor2 = 0xffFFFFFF;

    updateBuffers();
    _updateColorTheme();
    
    _init_renderManager();
    

    // гизмо создаётся перед передачей указателя на него во вьюпорт
    // потом нужно создать сцену, и уже потом инициализировать
	m_gizmo = kkCreate<Gizmo>();

    
    m_geomCreator = kkCreate<GeometryCreator>();
    m_plugin_interface->m_geomCreator = m_geomCreator;

    _init_GUIResources();
    
    _resetViewports();
    resetScene3D();
    
    m_gizmo->setGraphicsSystem( m_gs.ptr() );
    m_gizmo->init();

    updateViewports();
    
    m_editPolygons_paramWindow = m_pluginGUI->createWindow();
    m_editPolygons_paramWindow->SetSize(v2i(200,0));
    m_editPolygons_paramWindow->SetType( kkPluginGUIWindowType::Parameters );
    m_editPolygons_paramWindow->SetName(u"Editable mesh");

    _init_materialEditor(false);
    

    m_guiMainWindow.OSWindow = m_mainWindow->getHandle();
    m_guiMaterialEditorWindow.OSWindow = m_materialEditorWindow->getHandle();
    m_guiRenderWindow.OSWindow = m_renderWindow->getHandle();
    m_guiImportExportWindow.OSWindow = m_importExportWindow->getHandle();

    setActiveRenderer(m_renderers[0]);

    ShowWindow((HWND)m_mainWindow->getHandle(), SW_MAXIMIZE);

    m_gs->useBackFaceCulling(true);
    setNeedToSave(false);

    //SetParent((HWND)m_materialEditorNewMaterialWindow->getHandle(),(HWND)m_mainWindow->getHandle());
    //EnableWindow((HWND)m_mainWindow->getHandle(), FALSE);
    //m_materialEditorNewMaterialWindow->show();

}

const v2i& Application::getWindowClientSize(){	return m_window_client_size;}
const v2i& Application::getWindowSize()      {  return m_window_size;}
kkGraphicsSystem* Application::getGS()       {  return m_gs.ptr();}

void Application::setActiveRenderer(kkRenderer* renderer)
{
    m_activeRenderer = renderer;
}

void Application::setActiveViewport(Viewport* v)
{
    if( m_active_viewport )
    {
        m_active_viewport->setActive(false);
    }

    if( v )
    {
        v->setActive( true );
        m_active_viewport = v;
    }
}


void Application::run()
{
	auto dt = m_main_system->getDeltaTime();

	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

    v2_s16 cp;
    

	while( m_main_system->update() )
	{
        if( isWindowActive(EWID_MAIN_WINDOW) )
        {
            m_cursor_position.set( (s32)Kr::Gui::GuiSystem::m_cursorCoords.x,  (s32)Kr::Gui::GuiSystem::m_cursorCoords.y );

            if( m_cursor_position.x < -4000 ) m_cursor_position.x = 0;
            if( m_cursor_position.y < -4000 ) m_cursor_position.y = 0;
            m_mouseWheel = Kr::Gui::GuiSystem::wheel_delta;
        }

        

        //printf("m_mouseWheel %f\n",m_mouseWheel);

        a = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = a - b;

       /* if(work_time.count() < 10.0)
        {
            std::chrono::duration<double, std::milli> delta_ms(10.0 - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
        }

        b = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> sleep_time = b - a;*/

        if( m_state_app == AppState_main::Gizmo )
        {
        }
        else
        {
            WaitForSingleObject(GetCurrentThread(), 10 );
        }

        if( !m_minimized )
        {
            
            m_shortcutManager->onFrame();
            
            m_cursorInGUI = false;

		    drawAll();

            if( m_event_consumer->m_input_update )
            {
                this->updateInput();
            }

            _updateKeyboard();

            m_active_viewport->onFrame();

            _onEndFrame();
        }
        //kkColor
        //kkString
        //printf("Time: %f \n", (work_time + sleep_time).count());
	}
}

void Application::onWindowMinimize()
{
    m_minimized = true;
}

void Application::onWindowRestore()
{
    m_minimized = false;
    /*if( m_current_scene3D )
        m_current_scene3D->updateObject2DPoints(m_current_scene3D->getObjects());*/
}

void Application::onWindowMaximize()
{
    /*if( m_current_scene3D )
        m_current_scene3D->updateObject2DPoints(m_current_scene3D->getObjects());*/
}

void Application::onWindowActivate()
{
    switch(m_state_keyboard){
    case AppState_keyboard::None:
    case AppState_keyboard::Ctrl:
    case AppState_keyboard::Shift:
    case AppState_keyboard::ShiftCtrl:
    case AppState_keyboard::END:
    default:
        break;
    case AppState_keyboard::Alt:
    case AppState_keyboard::ShiftAlt:
    case AppState_keyboard::ShiftCtrlAlt:
    case AppState_keyboard::CtrlAlt:
    {
        m_state_keyboard = AppState_keyboard::None;
    }
        break;
    }

    m_cursor_position.set(0,0);
    m_currentGizmoEvent.reset();
    _clearAppEvents();
    m_active_viewport->onWindowActivate();
}

void Application::_updateKeyboard()
{
    s32 ctrl_shift_alt = 0;
    
    /// Удержание кнопок
    if( m_event_consumer->isKeyDown(kkKey::K_LALT) || m_event_consumer->isKeyDown(kkKey::K_RALT) )
    {
        ctrl_shift_alt |= 1;
    }
    if( m_event_consumer->isKeyDown(kkKey::K_LSHIFT) || m_event_consumer->isKeyDown(kkKey::K_RSHIFT) )
    {
        ctrl_shift_alt |= 2;
    }
    if( m_event_consumer->isKeyDown(kkKey::K_LCTRL) || m_event_consumer->isKeyDown(kkKey::K_RCTRL) )
    {
        ctrl_shift_alt |= 4;
    }

    switch( ctrl_shift_alt )
    {
    default:
    case 0:  m_state_keyboard = AppState_keyboard::None;          break;
    case 1:  m_state_keyboard = AppState_keyboard::Alt;           break;
    case 2:  m_state_keyboard = AppState_keyboard::Shift;         break;
    case 3:  m_state_keyboard = AppState_keyboard::ShiftAlt;      break;
    case 4:  m_state_keyboard = AppState_keyboard::Ctrl;          break;
    case 5:  m_state_keyboard = AppState_keyboard::CtrlAlt;       break;
    case 6:  m_state_keyboard = AppState_keyboard::ShiftCtrl;     break;
    case 7:  m_state_keyboard = AppState_keyboard::ShiftCtrlAlt;  break;
    }
}

void Application::drawBegin()
{
	m_gs->beginDraw(m_isClearCanvas);
}

void Application::drawEnd()
{
    m_gs->endDraw();
}

void Application::drawViewport()
{
    m_main_viewport->draw();
}

void Application::onWindowSize()
{
}

void Application::drawAll()
{
   // if( m_activeOSWindow == E_WINDOW_ID::EWID_MAIN_WINDOW )
    {
        m_gs->setActive(m_mainWindow.ptr());
        m_gs->update();

        drawBegin();
        drawViewport();

        m_KrGuiSystem->newFrame(&m_guiMainWindow);
        // тут рисование в главное окно
        _drawMainMenuBar();
        _drawMainToolBar();
        _drawLeftToolBar();
        _drawRightToolBar();
       /* if( m_KrGuiSystem->addButton() ) printf("Simple button\n");
		if( m_KrGuiSystem->addButton(0,0,Gui::Vec2f(20,10)) ) printf("With size\n");
		if( m_KrGuiSystem->addButton(u"Button") ) printf("With text\n");
		if( m_KrGuiSystem->addButton(u"B",0,Gui::Vec2f(30,20)) ) printf("Wider than text\n");*/
        m_KrGuiSystem->render();

        drawEnd();
    
        _processMainMenuCommand();
    }
   
    if( m_materialEditorWindow->isVisible() )
    {
        m_gs->setActive(m_materialEditorWindow.ptr());
        m_gs->update();
	    m_gs->beginDraw(m_isClearCanvas);
        m_KrGuiSystem->switchWindow(&m_guiMaterialEditorWindow);
        m_materialEditor->drawWindow();
        m_KrGuiSystem->render();
        m_gs->endDraw();
    }

    if( m_renderWindow->isVisible() )
    {
        m_gs->setActive(m_renderWindow.ptr());
        m_gs->update();
	    m_gs->beginDraw(m_isClearCanvas);
        m_KrGuiSystem->switchWindow(&m_guiRenderWindow);
        m_renderManager->drawWindow();
        m_KrGuiSystem->render();
        m_gs->endDraw();
    }

    if( m_importExportWindow->isVisible() )
    {
        m_gs->setActive(m_importExportWindow.ptr());
        m_gs->update();
	    m_gs->beginDraw(m_isClearCanvas);
        m_KrGuiSystem->switchWindow(&m_guiImportExportWindow);
        m_importExportGUIWindow->draw();
        //m_renderManager->drawWindow();
        m_KrGuiSystem->render();
        m_gs->endDraw();
    }


	m_KrGuiSystem->endFrame();
}

void Application::clearColor(bool v)
{
    m_isClearCanvas = v;
}

void Application::quit()
{
    m_renderManager->stopRender();
    // Спросить о сохранении если нужно
    if( m_need_to_save )
    {
        if( m_materialEditorWindow->isVisible() ) showMaterialEditor(false);
        if( m_renderWindow->isVisible() ) showRenderWindow(false);
        
        auto result = MessageBoxW( (HWND)m_mainWindow->getHandle(), L"Do you want to save current scene?", L"Attention!", MB_YESNOCANCEL | MB_ICONQUESTION );
        if( result == IDNO)
        {
            m_main_system->quit();
        }
        else if( result == IDYES )
        {
            // save and quit
            m_main_system->quit();
        }
    }
    else
    {
        m_main_system->quit();
    }
}

void Application::_updateColorTheme() /// set colors
{
    m_gs->setClearColor( m_current_color_theme.clear_color );
}

void Application::updateBuffers()
{
    auto wndrct = m_mainWindow->getWindowRect();
    m_window_size        = wndrct.getWidthAndHeight();
    m_window_client_rect = m_mainWindow->getClientRect();
    m_window_client_size = m_window_client_rect.getWidthAndHeight();

    //m_gs->get_active_camera()->set_aspect((f32)m_window_client_size.x/(f32)m_window_client_size.y);
    //m_gs->get_active_camera()->update();

    m_gs->update();
}

void Application::updateViewports()
{
    /// m_main_viewport обновит всех дочерних
    m_main_viewport->update();
}

void Application::setStateKeyboard(AppState_keyboard s)
{
    m_state_keyboard = s;
}
AppState_keyboard Application::getStateKeyboard(){    return m_state_keyboard;}

void Application::updateInput()
{
    if( m_state_app != AppState_main::MainMenu )
    {
        // ввод во вьюпорте будет отключен если показано это окно
        if( m_drawShortcutManager || m_drawPreferencesWindow )
        {
            return;
        }

        if( m_state_app == AppState_main::Idle )
        {
            if( m_event_consumer->isLmbUp())
            {
            }


            if( m_state_keyboard == AppState_keyboard::None )
            {
            }
        }

        // ввод для активного вьюпорта
        // там-же _processShortcuts для вьюпорта
        if( !m_cursorInGUI )
        {
            if( isWindowActive(EWID_MAIN_WINDOW) && !this->isGlobalInputBlocked() )
            {
                m_main_viewport->updateInput();
            }
        }
        else
        {
            m_main_viewport->checkMouseEvents();
        }

        // обработка тех горячих клавиш, действие команд которых более шире чем у например узкоспециализированных типа вьюпорта
        // вполне возможно что для редактора текстурных координат (или чего-то ещё) придётся делать свой _processShortcuts
        _processShortcuts();
    }
}

void Application::_deleteSelectedObjects()
{
    m_current_scene3D->deleteSelectedObjects();
    m_active_viewport->onDeleteObjects();
}


void Application::_processShortcuts()
{
    // если активно гизмо то нужно запретить менять режим выбора и т.д.
    if( m_state_app == AppState_main::Gizmo ||  m_enterTextMode )
        return;


	if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::New)){}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::Open)){}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::Save))
    {
        //////if( m_sceneFilePath.size() )
        save();
    }
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::SaveAs))
    {
        save();
    }
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::ShowMaterialEditor))
    {
        showMaterialEditor( m_materialEditorWindow->isVisible() ? false : true );
    }
    if( m_shortcutManager->isShortcutActive(ShortcutCommand_General::ShowRenderWindow))
    {
        showRenderWindow( m_renderWindow->isVisible() ? false : true );
    }

	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::DeselectAll)){ m_current_scene3D->deselectAll(); }
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectAll)){m_current_scene3D->selectAll();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectInvert)){m_current_scene3D->selectInvert();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::Redo)){}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::Undo)){}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectModeJustSelect)){_setSelectMode( SelectMode::JustSelect );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectModeMove)){_setSelectMode( SelectMode::Move);}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectModeRotate)){_setSelectMode( SelectMode::Rotate );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::SelectModeScale)){_setSelectMode( SelectMode::Scale );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::EditModeObject)){_setEditMode( EditMode::Object );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::EditModeVertex)){_setEditMode( EditMode::Vertex );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::EditModeEdge)){_setEditMode( EditMode::Edge );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::EditModePolygon)){_setEditMode( EditMode::Polygon );}
    
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Edit::EnterTransformation)){m_drawTransformWindow = true;}
    
    

    // возможно нужно будет блокировать это когда нужно вводить текст
    // да и вообще любой shortcut
	if( m_event_consumer->isKeyDownOnce( kkKey::K_DELETE ) )
    {
        _deleteSelectedObjects();
    }
}

void Application::showRenderWindow(bool v){ v ? m_renderWindow->show() : m_renderWindow->hide(); }
void Application::showMaterialEditor(bool v){ v ? m_materialEditorWindow->show() : m_materialEditorWindow->hide(); }

void Application::save()
{
    setNeedToSave(false);
}

Scene3D** Application::getScene3D(){    return &m_current_scene3D; }

void Application::updateWindowTitle()
{
    kkString string;
    string += m_programName;
    string += u" - ";
    string += m_sceneName;
    if( m_need_to_save )
        string += u" *";
    m_mainWindow->setWindowText(string.data());
}

void Application::setNeedToSave( bool v )
{
    this->m_need_to_save = v;
    updateWindowTitle();
}

void Application::resetScene3D()
{
    m_renderManager->stopRender();
    if( m_need_to_save )
    {
        auto result = MessageBoxW( (HWND)m_mainWindow->getHandle(), L"Do you want to save current scene?", L"Attention!", MB_YESNOCANCEL | MB_ICONQUESTION );
        if( result == IDNO)
        {
        }
        else if( result == IDYES )
        {
            save();
        }
        else
        {
            return;
        }
    }


    m_sceneName = u"untitled";

    m_main_viewport->onDeleteObjects();


    if( m_current_scene3D )
    {
        kkDestroy(m_current_scene3D);
    }

    m_current_scene3D = kkCreate<Scene3D>();
    m_plugin_interface->m_scene3D = m_current_scene3D;
    m_geomCreator->m_scene3D     = m_current_scene3D;

    m_active_viewport->onNewObject();

    this->_setEditMode(EditMode::Object);
    setNeedToSave(false);
}


bool Application::importModel(Plugin* pl)
{
    IFileOpenDialog *pFileOpen;

    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if( SUCCEEDED(hr) )
    {
        pFileOpen->SetTitle(L"Import...");
        pFileOpen->SetOkButtonLabel(L"Select");

        std::vector<kkString> extensions_array;
        util::stringGetWords<kkString>(&extensions_array,pl->getInfo().m_extensions);

        COMDLG_FILTERSPEC rgSpec;
            
        kkStringW wstr;
            
        for( u32 i = 0, sz = (u32)extensions_array.size(); i < sz; ++i )
        {
            wstr += L"*.";
            wstr += extensions_array[i].data();

            if( i < sz-1 )
            {
                wstr += L";";
            }
        }
            
        rgSpec.pszName = (wchar_t*)pl->getInfo().m_name.data();
        rgSpec.pszSpec = wstr.data();

        pFileOpen->SetFileTypes(1,&rgSpec);
        hr = pFileOpen->Show((HWND)m_mainWindow->getHandle());
        if( SUCCEEDED(hr) )
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if( SUCCEEDED(hr) )
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if( SUCCEEDED(hr) )
                {
                    //pl->call_onImportMesh(&m_plugin_interface, (const char16_t*)pszFilePath);
                    pl->call_onActivate(m_plugin_interface,kkPluginGUIWindowType::Import, (const char16_t*)pszFilePath);
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    m_state_app = AppState_main::Idle;
    m_active_viewport->onNewObject();

    return true;
}

bool Application::exportModel(Plugin* pl)
{
    IFileSaveDialog *pFileSave;

    auto hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
    if( SUCCEEDED(hr) )
    {
        pFileSave->SetTitle(L"Export...");
        pFileSave->SetOkButtonLabel(L"Export");

        std::vector<kkString> extensions_array;
        util::stringGetWords<kkString>(&extensions_array,pl->getInfo().m_extensions);

        COMDLG_FILTERSPEC rgSpec;
            
        kkStringW wstr;
            
        for( u32 i = 0, sz = (u32)extensions_array.size(); i < sz; ++i )
        {
            wstr += L"*.";
            wstr += extensions_array[i].data();

            if( i < sz-1 )
            {
                wstr += L";";
            }
        }
            
        rgSpec.pszName = (wchar_t*)pl->getInfo().m_name.data();
        rgSpec.pszSpec = wstr.data();

        pFileSave->SetFileTypes(1,&rgSpec);
        hr = pFileSave->Show((HWND)m_mainWindow->getHandle());
        if( SUCCEEDED(hr) )
        {
            IShellItem *pItem;
            hr = pFileSave->GetResult(&pItem);
            if( SUCCEEDED(hr) )
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if( SUCCEEDED(hr) )
                {
                    kkString text = (const char16_t*)pszFilePath;


                    if( !kkFileSystem::existFile(text) )
                    {
                        text += u".";
                        text += extensions_array[ 0 ].data();
                    }

                    //  wprintf(L"FILE: %s\n",text.data());
                    pl->call_onActivate(m_plugin_interface,kkPluginGUIWindowType::Export, text.data());
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }

    m_state_app = AppState_main::Idle;
    
    m_active_viewport->onNewObject();

    return true;
}

kkString Application::getOpenFileNameForImages()
{
    kkString result;

    auto plugins = m_plugin_manager.getImportImagePlugins();
    if( !plugins.size() )
        return result;

    IFileOpenDialog *pFileOpen;

    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if( SUCCEEDED(hr) )
    {
        pFileOpen->SetTitle(L"Select image file");
        pFileOpen->SetOkButtonLabel(L"Select");

        std::vector<kkString> extensions_array;
        std::vector<kkString> all_extensions_array;

        COMDLG_FILTERSPEC * rgSpec = new COMDLG_FILTERSPEC[plugins.size()+1];
        kkStringW *         wstr   = new kkStringW[plugins.size()+1];
        //COMDLG_FILTERSPEC * rgSpec_p = rgSpec;

        for( u32 i = 0, sz =  (u32)plugins.size(); i < sz; ++i )
        {
            // std::vector<kkString> extensions_array;
            // util::stringGetWords<kkString>(&extensions_array,arr[i]->getInfo().m_extensions);
            //kkStringW wstr;
            util::stringGetWords<kkString>( &extensions_array, plugins[i]->getInfo().m_extensions );
            util::stringGetWords<kkString>( &all_extensions_array, plugins[i]->getInfo().m_extensions );

            for( u32 o = 0, sz2 = (u32)extensions_array.size(); o < sz2; ++o )
            {
                wstr[i+1] += L"*.";
                wstr[i+1] += extensions_array[o].data();

                if( o < sz2-1 )
                {
                    wstr[i+1] += L";";
                }
            }
                
            // rgSpec_p->pszName = (wchar_t*)arr[i]->getInfo().m_extension_description.data();
            // rgSpec_p->pszSpec = wstr.data();
            rgSpec[i+1].pszName = (wchar_t*)plugins[i]->getInfo().m_name.data();
            rgSpec[i+1].pszSpec = wstr[i+1].data();

            // ++rgSpec_p;
            extensions_array.clear();
        }
            
        for( u32 o = 0, sz2 = (u32)all_extensions_array.size(); o < sz2; ++o )
        {
            wstr[0] += L"*.";
            wstr[0] += all_extensions_array[o].data();

            if( o < sz2-1 )
            {
                wstr[0] += L";";
            }
        }
        rgSpec[0].pszName = L"All supported formats";
        rgSpec[0].pszSpec = wstr[0].data();

        pFileOpen->SetFileTypes((UINT)plugins.size()+1,rgSpec);
        // pFileOpen->SetFileTypes((UINT)arr.size(),rgSpec);

        hr = pFileOpen->Show((HWND)m_mainWindow->getHandle());
        if( SUCCEEDED(hr) )
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if( SUCCEEDED(hr) )
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if( SUCCEEDED(hr) )
                {
                    result = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }

        delete[] wstr;
        delete[] rgSpec;
        pFileOpen->Release();
    }


    return result;
}

kkString Application::getSaveFileNameForImages()
{
    kkString result;

    auto plugins = m_plugin_manager.getExportImagePlugins();
    if( !plugins.size() )
        return result;

    m_fileSaveDialog->SetTitle(L"Save file");
    m_fileSaveDialog->SetOkButtonLabel(L"Save");

    std::vector<kkString> extensions_array;

    COMDLG_FILTERSPEC * rgSpec = new COMDLG_FILTERSPEC[plugins.size()];
    kkStringW *         wstr   = new kkStringW[plugins.size()];

    for( u32 i = 0, sz =  (u32)plugins.size(); i < sz; ++i )
    {
        util::stringGetWords<kkString>( &extensions_array, plugins[i]->getInfo().m_extensions );

        for( u32 o = 0, sz2 = (u32)extensions_array.size(); o < sz2; ++o )
        {
            wstr[i] += L"*.";
            wstr[i] += extensions_array[o].data();

            if( o < sz2-1 )
            {
                wstr[i] += L";";
            }
        }
                
        rgSpec[i].pszName = (wchar_t*)plugins[i]->getInfo().m_name.data();
        rgSpec[i].pszSpec = wstr[i].data();

        extensions_array.clear();
    }
        

    m_fileSaveDialog->SetFileTypes((UINT)plugins.size(),rgSpec);

    auto hr = m_fileSaveDialog->Show((HWND)m_mainWindow->getHandle());
    if( SUCCEEDED(hr) )
    {
        IShellItem *pItem;
        hr = m_fileSaveDialog->GetResult(&pItem);
        if( SUCCEEDED(hr) )
        {
            PWSTR pszFilePath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            if( SUCCEEDED(hr) )
            {
                UINT fileTypeIndex = 0;
                m_fileSaveDialog->GetFileTypeIndex(&fileTypeIndex);
                --fileTypeIndex;

                result = pszFilePath;

                extensions_array.clear();
                util::stringGetWords<kkString>( &extensions_array, 
                    plugins[fileTypeIndex]->getInfo().m_extensions );
                
                auto extension = util::stringGetExtension(kkStringW(pszFilePath), true);
                //auto extension = std::filesystem::extension(pszFilePath);
                if(!extension.size())
                {
                    result.append(u".");
                    result.append(extensions_array[0].data());
                }


                CoTaskMemFree(pszFilePath);
            }
            pItem->Release();
        }
    }

    delete[] wstr;
    delete[] rgSpec;

    return result;
}

kkImage* Application::loadImageFromDialog()
{
    kkString fn = getOpenFileNameForImages();
    if( fn.size() )
    {
        return loadImage(fn.data());
    }

    return nullptr;
}

kkImage* Application::loadImage(const char16_t* str)
{
    kkString fn = str;
    if( fn.size() )
    {
        kkString file_extension = util::stringGetExtension(fn);
        util::stringToLower(file_extension);

        auto arr = m_plugin_manager.getImportImagePlugins();
        
        for( u32 i = 0, sz =  (u32)arr.size(); i < sz; ++i )
        {
            std::vector<kkString> extensions_array;
            util::stringGetWords<kkString>(&extensions_array,arr[i]->getInfo().m_extensions);

            for( auto & o : extensions_array )
            {
                if( kkString(o.data()) == file_extension )
                {
                    return arr[i]->call_onImportImage(m_plugin_interface, fn.data());
                }
            }
        }
    }

    return nullptr;
}


void Application::_init_GUIResources()
{
    m_guiResources = kkCreate<GUIResources>();
    m_guiResources->loadResources(this, m_gs.ptr());

    if( !_initFonts() )
    {
		throw std::runtime_error("Failed to create some font :(");
    }

    m_mainMenuStyle.menuBarBGAlpha = 1.f;
    m_mainMenuStyle.popupAlpha = 0.6f;
    m_mainMenuStyle.iconFont = m_iconsFont;
}

EventConsumer *   Application::getEventConsumer(){    return m_event_consumer.get();}

kkString Application::getOpenFileName(const char16_t* file_desc, const char16_t* ext)
{
    kkString result;

    IFileOpenDialog *pFileOpen;

    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if( SUCCEEDED(hr) )
    {
        pFileOpen->SetTitle(L"Select file");
        pFileOpen->SetOkButtonLabel(L"Select");

        COMDLG_FILTERSPEC rgSpec;
        kkStringW wstr = L"*.";
        wstr += ext;

        rgSpec.pszName = (wchar_t*)file_desc;
        rgSpec.pszSpec = wstr.data();
            

        pFileOpen->SetFileTypes(1,&rgSpec);

        hr = pFileOpen->Show((HWND)m_mainWindow->getHandle());
        if( SUCCEEDED(hr) )
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if( SUCCEEDED(hr) )
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if( SUCCEEDED(hr) )
                {
                    result = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    return result;
}

void Application::_resetViewports()
{
    if( m_main_viewport )
    {
        m_main_viewport->m_mainViewport = nullptr;
        kkDestroy(m_main_viewport);
    }

    ViewportData vd;
    vd.m_current_color_theme = &m_current_color_theme;
    vd.m_gs                  = m_gs.ptr();
    vd.m_window_client_size  = &m_window_client_size;
    vd.m_state_app           = &m_state_app;
    vd.m_state_keyboard      = &m_state_keyboard;
    vd.m_cursor_position     = &m_cursor_position;
    vd.m_mouseWheel          = &m_mouseWheel;
    vd.m_event_consumer      = m_event_consumer.get();
    vd.m_app                 = this;
    vd.m_GUIResources        = m_guiResources.ptr();
    vd.m_shortcutManager     = m_shortcutManager.ptr();
    vd.m_deltaTime           = m_main_system->getDeltaTime();
    vd.m_gizmo               = m_gizmo.ptr();
    
    m_main_viewport          = kkCreate<Viewport>(vd);
    m_main_viewport->initMaximizedViewport();

    setActiveViewport(m_main_viewport);
    m_main_viewport->m_mainViewport = m_main_viewport;

    m_main_viewport->update();
}


void Application::drawToolTip(const char* text)
{
}

void   Application::_drawSelectByNameWindow()
{
    //ImGui::SetNextWindowBgAlpha(0.8f);
    //if(ImGui::Begin("Select by name", &m_drawSelectByNameWindow, 
    //    ImGuiWindowFlags_NoCollapse
    //    | ImGuiWindowFlags_NoSavedSettings 
    //    | ImGuiWindowFlags_MenuBar
    //))
    //{
    //    auto windowPosition = ImGui::GetWindowPos();
    //    auto windowSize     = ImGui::GetWindowSize();
    //    
    //    if( kkrooo::pointInRect( m_cursor_position, v4f(windowPosition.x,windowPosition.y,windowPosition.x+windowSize.x,windowPosition.y+windowSize.y) ) )
    //    {
    //        m_imguiWindowFocus = 1;
    //    }

    //    if (ImGui::BeginMenuBar())
    //    {
    //        if (ImGui::BeginMenu("Help"))
    //        {
    //            if( ImGui::MenuItem("Close") )
    //            {
    //                m_drawSelectByNameWindow = false;
    //            }

    //            ImGui::EndMenu();
    //        }
    //        ImGui::EndMenuBar();
    //    }

    //    auto & objects = m_current_scene3D->getObjects();
    //    static kkStringA stra;

    //    auto wsz = ImGui::GetWindowSize();
    //    if(wsz.x > 250) wsz.x = wsz.x - 250 - 50;
    //    else            wsz.x = 0;

    //    if(wsz.y > 400) wsz.y = wsz.y - 400 - 50;
    //    else            wsz.y = 0;

    //    ImGui::BeginChild("left pane", ImVec2(250 + wsz.x, 400 + wsz.y ), true, ImGuiWindowFlags_HorizontalScrollbar );
    //    for( size_t i = 0, sz = objects.size(); i < sz; ++i )
    //    {
    //        auto obj = objects.at(i);
    //        stra = obj->GetName();
    //        
    //        bool b = obj->m_isSelected;

    //        if( ImGui::Checkbox(stra.c_str(),&b) )
    //        {
    //            if( !b )
    //            {
    //                m_current_scene3D->deselectObject(obj);
    //            }
    //            else
    //            {
    //                m_current_scene3D->selectObject(obj);
    //            }
    //        }
    //        /*ImGui::SameLine();
    //        if(ImGui::SmallButton("Rename"))
    //        {
    //        }*/


    //    }
    //    ImGui::EndChild();

    //    /*if (ImGui::TreeNode("Basic trees"))
    //    {
    //        for (int i = 0; i < 5; i++)
    //        {
    //            if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
    //            {
    //                ImGui::Text("blah blah");
    //                ImGui::SameLine();
    //                if (ImGui::SmallButton("button")) {};
    //                ImGui::TreePop();
    //            }
    //        }
    //        ImGui::TreePop();
    //    }*/



    //    ImGui::End();
    //}
}

void   Application::_drawPreferencesWindow()
{
   /* ImGui::SetNextWindowBgAlpha(0.8f);
    if(ImGui::Begin("Preferences", &m_drawPreferencesWindow, 
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_MenuBar
    ))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Close"))
            {
                m_drawPreferencesWindow = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if( ImGui::IsWindowFocused() )
        {
            m_active_viewport->onLoseFocus();
        }


        ImGui::End();
    }*/
}

void Application::_setSelectMode( SelectMode m )
{
    m_selectMode      = m;
    m_isLocalScale    = false;
    m_isLocalRotation = false;
}

void Application::_setEditMode( EditMode m )
{
    auto old_mode = m_editMode;

    if( m_editMode == EditMode::Object )
    {
        if( m_current_scene3D->getNumOfSelectedObjects() )
        {
            //m_editMode = m;

            /*for( u32 i = 0, sz = m_current_scene3D->getNumOfSelectedObjects(); i < sz; ++i )
            {
                auto object = m_current_scene3D->getSelectedObject(i);
                if( object->GetType() == kkScene3DObjectType::PolygonObject )
                {
                    if( object->GetPluginGUIWindow() != m_editPolygons_paramWindow )
                    {
                        object->SetPluginGUIWindow(this->m_editPolygons_paramWindow);
                        auto ptr = object->DropParametersWindowData();
                        if( ptr )
                            kkMemory::free(ptr);
                    }
                }
            }*/
        }
    }
    else
    {
             if( m_editMode == EditMode::Vertex  && m == EditMode::Vertex )  m_editMode = EditMode::Object;
        else if( m_editMode == EditMode::Edge    && m == EditMode::Edge )    m_editMode = EditMode::Object;
        else if( m_editMode == EditMode::Polygon && m == EditMode::Polygon ) m_editMode = EditMode::Object;
        else
        {
            m_editMode = m;
        }
    }

    for( u32 i = 0, sz = m_current_scene3D->getNumOfSelectedObjects(); i < sz; ++i )
    {
        auto object = m_current_scene3D->getSelectedObject(i);
        if( object->GetType() == kkScene3DObjectType::PolygonObject )
        {
            switch (m_editMode)
            {
            case EditMode::Object:
                ((Scene3DObject*)object)->deleteEdges();
                if( old_mode == EditMode::Edge )
                    ((Scene3DObject*)object)->updateEdgeModel();
                if( old_mode == EditMode::Polygon )
                    ((Scene3DObject*)object)->updatePolygonModel(); // возможно это не нужно
                break;
            case EditMode::Vertex:
                ((Scene3DObject*)object)->deleteEdges();
                if( old_mode == EditMode::Edge )
                    ((Scene3DObject*)object)->updateEdgeModel();
                if( old_mode == EditMode::Polygon )
                    ((Scene3DObject*)object)->updatePolygonModel();
                break;
            case EditMode::Edge:
                ((Scene3DObject*)object)->createEdges();
                ((Scene3DObject*)object)->updateEdgeModel();
                if( old_mode == EditMode::Polygon )
                    ((Scene3DObject*)object)->updatePolygonModel();
                break;
            case EditMode::Polygon:
                ((Scene3DObject*)object)->deleteEdges();
                if( old_mode == EditMode::Edge )
                    ((Scene3DObject*)object)->updateEdgeModel();
                ((Scene3DObject*)object)->updatePolygonModel();
                break;
            default:
                break;
            }
        }
    }

    m_active_viewport->updateObject2DPoints();
    m_current_scene3D->updateSelectionAabb();
}


// Код в этой функции должен быть переосмыслен
// Нужно уметь одинаково производить операции и над объектами в целом, и над вершинами и прочими - одинаково.
void   Application::_drawTransformWindow()
{
    const char * text = "not active";

    switch(m_selectMode)
    {
    case SelectMode::JustSelect:
    default:
        break;
    case SelectMode::Move:
        text = "Move";
        break;
    case SelectMode::Rotate:
        text = "Rotate";
        break;
    case SelectMode::Scale:
        text = "Scale";
        break;
    }

//    ImGui::SetNextWindowBgAlpha(0.8f);
//    if(ImGui::Begin("Enter transformation...", &m_drawTransformWindow, 
//        ImGuiWindowFlags_NoCollapse
//        | ImGuiWindowFlags_NoSavedSettings 
//       // | ImGuiWindowFlags_MenuBar
//        | ImGuiWindowFlags_NoResize
//    ))
//    {
//        auto windowPosition = ImGui::GetWindowPos();
//        auto windowSize     = ImGui::GetWindowSize();
//        
//        if( kkrooo::pointInRect( m_cursor_position, v4f(windowPosition.x,windowPosition.y,windowPosition.x+windowSize.x,windowPosition.y+windowSize.y) ) )
//        {
//            m_imguiWindowFocus = 1;
//        }
//
//        auto & objects = m_current_scene3D->getObjects();
//        static kkStringA stra;
//
//        auto numOfSelectedObjects = m_current_scene3D->getNumOfSelectedObjects();
//
//        ImGui::Text(text);
//        ImGui::Text("Shift - Fast, Alt - Slow");
//        ImGui::BeginChild("left", ImVec2(130, 100 ), true, 0 );
//
//        auto f_drawMove = [=]()
//        {
//            static float x = 0, y = 0, z = 0;
//
//            if( m_event_consumer->isLmbUp() )
//            {
//                x = 0, y = 0, z = 0;
//            }
//
//
//            bool need_update = false;
//
//            if( numOfSelectedObjects > 1 )
//            {
//            }
//            else
//            {
//                auto o = m_current_scene3D->getSelectedObject(0);
//                auto pivot = o->GetPivot();
//                x = pivot.KK_X;
//                y = pivot.KK_Y;
//                z = pivot.KK_Z;
//            }
//
//            ImGui::Text("X: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("X",&x,0.01f,0.f,0.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            ImGui::Text("Y: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("Y",&y,0.01f,0.f,0.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            ImGui::Text("Z: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("Z",&z,0.01f,0.f,0.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            
//            if( need_update )
//            {
//                m_state_app = AppState_main::Gizmo;
//
//                if( numOfSelectedObjects > 1 )
//                {
//                    for( auto i = 0u; i < numOfSelectedObjects; ++i )
//                    {
//                        auto o = m_current_scene3D->getSelectedObject(i);
//                        auto pivotFix = o->GetPivotFixed();
//
//                        auto & pivot = o->GetPivot();
//
//                        pivot = pivotFix + kkVector4(x,y,z);
//
//
////                        o->updateMatrixPosition();
//                        o->UpdateAabb();
//                        m_current_scene3D->updateSceneAabb();
//                        m_current_scene3D->updateSelectionAabb();
//                    }
//                    //x = y = z = 0.f;
//                }
//                else
//                {
//                    auto o = m_current_scene3D->getSelectedObject(0);
//                    auto & pivot = o->GetPivot();
//                    pivot.KK_X = x;
//                    pivot.KK_Y = y;
//                    pivot.KK_Z = z;
//                    o->ApplyPivot();
////                    o->updateMatrixPosition();
//                    o->UpdateAabb();
//                    m_current_scene3D->updateSceneAabb();
//                    m_current_scene3D->updateSelectionAabb();
//
//                   // m_current_scene3D->updateObject2DPoints(o); // нужно обновлять при LMB_UP
//
//                    x = 0, y = 0, z = 0;
//                }
//            }
//
//            if( m_event_consumer->isLmbUp() )
//            {
//                m_state_app = AppState_main::Idle;
//
//                if( numOfSelectedObjects > 1 )
//                {
//                    for( auto i = 0u; i < numOfSelectedObjects; ++i )
//                    {
//                        auto o = m_current_scene3D->getSelectedObject(i);
//                        o->ApplyPivot();
//                       // m_current_scene3D->updateObject2DPoints(o);
//                    }
//                }
//               /* else
//                    m_current_scene3D->updateObject2DPoints(m_current_scene3D->getSelectedObject(0));*/
//            }
//        };
//
//        auto f_drawRotate = [=]()
//        {
//            
//        };
//
//        auto f_drawScale = [=]()
//        {
//            static float x = 1, y = 1, z = 1;
//
//            if( m_event_consumer->isLmbUp() )
//            {
//                x = 1, y = 1, z = 1;
//            }
//
//
//            bool need_update = false;
//
//            if( numOfSelectedObjects > 1 )
//            {
//            }
//            else
//            {
//            }
//
//            ImGui::Text("X: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("X",&x,0.01f,0.f,100.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            ImGui::Text("Y: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("Y",&y,0.01f,0.f,100.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            ImGui::Text("Z: ");
//            ImGui::SameLine();
//            if( ImGui::DragFloatVertical("Z",&z,0.01f,0.f,100.f,"%.4f") )
//            {
//                need_update = true;
//            }
//            
//            if( x <= 0.f ) x = FLT_MIN;
//            if( y <= 0.f ) y = FLT_MIN;
//            if( z <= 0.f ) z = FLT_MIN;
//
//            if( need_update )
//            {
//                m_state_app = AppState_main::Gizmo;
//
//                for( auto i = 0u; i < numOfSelectedObjects; ++i )
//                {
//                    kkMatrix4 S;
//	                math::makeScaleMatrix(kkVector4(x,y,z,1.f),S);
//                    /*
//                    auto R = o->m_matrixFixed;
//			        o->m_matrix = S * R;
//                    */
//
//                    auto o = m_current_scene3D->getSelectedObject(i);
//                    if( o->GetType() == kkScene3DObjectType::PolygonObject )
//                    {
//                        ((Scene3DObject*)o)->m_matrix = S * ((Scene3DObject*)o)->m_matrixFixed;
//                    }
////                    o->updateMatrixPosition();
//                    o->UpdateAabb();
//
//                }
//            }
//
//            if( m_event_consumer->isLmbUp() )
//            {
//                m_state_app = AppState_main::Idle;
//
//                    for( auto i = 0u; i < numOfSelectedObjects; ++i )
//                    {
//                        auto o = m_current_scene3D->getSelectedObject(i);
//                        if( o->GetType() == kkScene3DObjectType::PolygonObject )
//                        {
//				            ((Scene3DObject*)o)->m_matrixFixed = ((Scene3DObject*)o)->m_matrix;
//                        }
//                       // m_current_scene3D->updateObject2DPoints(o);
//                    }
//            }
//        };
//
//        switch(m_selectMode)
//        {
//        case SelectMode::JustSelect:
//        default:
//            break;
//        case SelectMode::Move:
//            if( numOfSelectedObjects )
//                f_drawMove();
//            break;
//        case SelectMode::Rotate:
//            if( numOfSelectedObjects )
//                f_drawRotate();
//            break;
//        case SelectMode::Scale:
//            if( numOfSelectedObjects )
//                f_drawScale();
//            break;
//        }
//
//        ImGui::EndChild();
//        ImGui::SameLine();
//        ImGui::BeginChild("right", ImVec2(130, 100 ), true, 0 );
//        ImGui::EndChild();
//
//        ImGui::End();
//    }
}

bool Application::_createShaders()
{
    m_shaderSimple = kkCreate<ShaderSimple>(m_gs.ptr());
    if( !m_gs->createShader( m_shaderSimple.ptr(), m_shaderSimple->text_v, nullptr, m_shaderSimple->text_f, nullptr ) )
        return false;

    m_shaderPoint = kkCreate<ShaderPoint>(m_gs.ptr());
    if( !m_gs->createShader( m_shaderPoint.ptr(), m_shaderPoint->text_v, nullptr, m_shaderPoint->text_f, nullptr, nullptr, nullptr ) )
        return false;

    m_shader3DObjectDefault = kkCreate<ShaderScene3DObjectDefault>(m_gs.ptr());
    if( !m_gs->createShader( m_shader3DObjectDefault.ptr(), m_shader3DObjectDefault->text_v, nullptr, m_shader3DObjectDefault->text_f, nullptr, nullptr, nullptr ) )
        return false;

    m_shaderLineModel = kkCreate<ShaderLineModel>(m_gs.ptr());
    if( !m_gs->createShader( m_shaderLineModel.ptr(), m_shaderLineModel->text_v, nullptr, m_shaderLineModel->text_f, nullptr, nullptr, nullptr ) )
        return false;

    m_shader3DObjectDefault_polymodeforlinerender = kkCreate<ShaderScene3DObjectDefault_polymodeforlinerender>(m_gs.ptr());
    if( !m_gs->createShader( m_shader3DObjectDefault_polymodeforlinerender.ptr(), m_shader3DObjectDefault_polymodeforlinerender->text_v, nullptr, m_shader3DObjectDefault_polymodeforlinerender->text_f, nullptr, nullptr, nullptr ) )
        return false;

    return true;
}

// надо открыть окно выбора, и чтобы там были все типы которые поддерживают плагины
void Application::_openImportWindowWithAllTypes()
{
    auto plugins = m_plugin_manager.getImportModelPlugins();
    if( !plugins.size() )
        return;

    IFileOpenDialog *pFileOpen;

    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if( SUCCEEDED(hr) )
    {
        pFileOpen->SetTitle(L"Import...");
        pFileOpen->SetOkButtonLabel(L"Select");

        std::vector<kkString> extensions_array;
        std::vector<kkString> all_extensions_array;

            
        COMDLG_FILTERSPEC * rgSpec = new COMDLG_FILTERSPEC[plugins.size()+1];
        kkStringW *         wstr   = new kkStringW[plugins.size()+1];

        for( size_t i = 0, sz = plugins.size(); i < sz; ++i )
        {
            util::stringGetWords<kkString>( &extensions_array, plugins[i]->getInfo().m_extensions );
            util::stringGetWords<kkString>( &all_extensions_array, plugins[i]->getInfo().m_extensions );
            
            for( u32 o = 0, sz2 = (u32)extensions_array.size(); o < sz2; ++o )
            {
                wstr[i+1] += L"*.";
                wstr[i+1] += extensions_array[o].data();

                if( o < sz2-1 )
                {
                    wstr[i+1] += L";";
                }
            }
            rgSpec[i+1].pszName = (wchar_t*)plugins[i]->getInfo().m_name.data();
            rgSpec[i+1].pszSpec = wstr[i+1].data();

            extensions_array.clear();
        }

        for( u32 o = 0, sz2 = (u32)all_extensions_array.size(); o < sz2; ++o )
        {
            wstr[0] += L"*.";
            wstr[0] += all_extensions_array[o].data();

            if( o < sz2-1 )
            {
                wstr[0] += L";";
            }
        }
        rgSpec[0].pszName = L"All supported formats";
        rgSpec[0].pszSpec = wstr[0].data();
            

        pFileOpen->SetFileTypes((UINT)plugins.size()+1,rgSpec);
        hr = pFileOpen->Show((HWND)m_mainWindow->getHandle());
        if( SUCCEEDED(hr) )
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if( SUCCEEDED(hr) )
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if( SUCCEEDED(hr) )
                {
                    UINT fileTypeIndex = 0;
                    hr = pFileOpen->GetFileTypeIndex(&fileTypeIndex);
                    //printf("fileTypeIndex %u\n",fileTypeIndex);

                    if( fileTypeIndex == 1 )
                    {
                        //m_importModelWindow->show();
                        m_drawImportByPluginWindow = true;
                        m_lastFilePath = pszFilePath;
                    }
                    else
                    {
                        plugins[fileTypeIndex-2]->call_onActivate(m_plugin_interface,kkPluginGUIWindowType::Import, (const char16_t*)pszFilePath);
                    }

                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }

        delete[] wstr;
        delete[] rgSpec;

        pFileOpen->Release();
    }
}

void   Application::_drawImportByPluginWindow()
{
    Plugin * plugin_for_import = nullptr;

    /*ImGui::SetNextWindowBgAlpha(0.8f);
    if(ImGui::Begin("Select plugin for import", &m_drawImportByPluginWindow, 
        ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoResize
    ))
    {
        auto windowPosition = ImGui::GetWindowPos();
        auto windowSize     = ImGui::GetWindowSize();
        
        this->setImguiButtonStyle(Application::_imgui_buttonStyle::_imgui_buttonStyle_default);
        
        if( kkrooo::pointInRect( m_cursor_position, v4f(windowPosition.x,windowPosition.y,windowPosition.x+windowSize.x,windowPosition.y+windowSize.y) ) )
        {
            m_imguiWindowFocus = 1;
        }

        kkString ext = util::stringGetExtension(m_lastFilePath).data();
        util::stringToLower(ext);

        ImGui::Text(".%s",ext.to_kkStringA().data());
        ImGui::BeginChild("left", ImVec2(400, 200 ), true, 0 );

        std::vector<Plugin*> plugins_with_ext;
        std::vector<kkString> extensions_array;
        auto plugins = m_plugin_manager.getImportModelPlugins();
        for( size_t i = 0, sz = plugins.size(); i < sz; ++i )
        {
            util::stringGetWords<kkString>( &extensions_array, plugins[i]->getInfo().m_extensions );

            for( auto e : extensions_array )
            {
                if( kkString(e.data()) == ext )
                {
                    plugins_with_ext.push_back(plugins[i]);
                    break;
                }
            }
            extensions_array.clear();
        }

        char buttonname[200];
        for( auto p : plugins_with_ext )
        {
            sprintf(buttonname,"Import %s",p->getInfo().m_name.to_kkStringA().data());
            if( ImGui::Button(buttonname) )
            {
                plugin_for_import = p;
                m_drawImportByPluginWindow = false;
                break;
            }
        }

        ImGui::EndChild();

        ImGui::End();
    }*/


    if( plugin_for_import )
    {
        m_mainMenuCommand.type   = MainMenuCommandType::ImportDirectly;
        m_mainMenuCommand.plugin = plugin_for_import;
    }
}

void   Application::_drawPivotToolWindow()
{
    /*ImGui::SetNextWindowBgAlpha(0.8f);
    if(ImGui::Begin("Pivot tool", &m_drawPivotToolWindow, 
        ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoResize
    ))
    {
        auto windowPosition = ImGui::GetWindowPos();
        auto windowSize     = ImGui::GetWindowSize();
        
        this->setImguiButtonStyle(Application::_imgui_buttonStyle::_imgui_buttonStyle_default);
        
        if( kkrooo::pointInRect( m_cursor_position, v4f(windowPosition.x,windowPosition.y,windowPosition.x+windowSize.x,windowPosition.y+windowSize.y) ) )
        {
            m_imguiWindowFocus = 1;
        }

        float window_width = 200.f;
        ImGui::BeginChild("c", ImVec2(window_width, 200.f ), true, 0 );

        if( ImGui::Button("To Object Center", ImVec2(window_width,20.f) ) )
        {
        }
        if( ImGui::Button("To Space Center", ImVec2(window_width,20.f)) )
        {
        }

        ImGui::EndChild();

        ImGui::End();
    }*/
}

void Application::addAppEvent( const AppEvent& e, AppEventPriority p )
{
    switch(p)
    {
    case AppEventPriority::_high:
        m_appEvents[0].push_back( e );
        break;
    case AppEventPriority::_medium:
        m_appEvents[1].push_back( e );
        break;
    case AppEventPriority::_low:
        m_appEvents[2].push_back( e );
        break;
    }
}

void Application::_clearAppEvents()
{
    m_appEvents[0].clear();
    m_appEvents[1].clear();
    m_appEvents[2].clear();
}

void Application::_onEndFrame()
{
    m_tootipText = nullptr;

    //printf("m_state_app %u\n", (u32)m_state_app);

    //printf("Size = %u %u %u\n", m_appEvents[0].size(), m_appEvents[1].size(), m_appEvents[2].size());
    while( m_appEvents[0].size() )
    {
        auto & e = m_appEvents[0].front();

        if( e.type == AppEventType::_gizmo && m_state_app != AppState_main::Gizmo )
        {
            // Более приоритетная часть гизмо
            // устанавливается новый аппстейн, дальнейшие изменения m_currentGizmoEvent запрещены
            if( m_event_consumer->isLmbDownOnce() )
            {
                e.gizmo.point2D = m_currentGizmoEvent.point2D;
                m_currentGizmoEvent = e.gizmo;
                m_state_app = AppState_main::Gizmo;
            }
        }

        //printf("   %u\n",m_appEvents[0].front().uinteger);
        //m_appEvents[0].pop_front();
        m_appEvents[0].clear();
    }

    while( m_appEvents[1].size() )
    {
        auto & e = m_appEvents[1].front();

        // если тип события = гизмо, и небыло события гизмо в более высоком приоритете
        if( e.type == AppEventType::_gizmo && m_state_app != AppState_main::Gizmo )
        {
            if( m_event_consumer->isLmbDownOnce() )
            {
                e.gizmo.point2D = m_currentGizmoEvent.point2D;
                m_currentGizmoEvent = e.gizmo;
                m_state_app = AppState_main::Gizmo;
                //gizmo_event = e.gizmo;
            }
        }

       // printf("   %u\n",m_appEvents[1].front().uinteger);
        m_appEvents[1].clear();
    }


    if(m_event_consumer->isLmbUp() || m_event_consumer->isRmbUp() || m_event_consumer->isMmbUp())
    {
        //printf("up\n");
        // если перемещали объект то этот вызов должен применить изменения
        if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_move )
        {
            m_current_scene3D->moveSelectedObjects( false, m_currentGizmoEvent, m_event_consumer->isRmbUp(), false );

        }else if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_scale )
        {
            m_current_scene3D->scaleSelectedObjects( false, m_currentGizmoEvent, m_event_consumer->isRmbUp(), false );

        }else if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_rotate )
        {
            m_current_scene3D->rotateSelectedObjects( false, m_currentGizmoEvent, m_event_consumer->isRmbUp(), false );
        }

        if( m_state_app != AppState_main::CancelTransformation )
            m_state_app = AppState_main::Idle;
       // m_active_viewport->onLoseFocus();
        m_currentGizmoEvent.reset();
        // при перемещении гизмо остаётся на месте, и луч остаётся там-же
        // при отжатии, нужно получить новый луч по курсором
        //   убираю видимость активной части гизмо
    }
    
    m_active_viewport->updateCursorRay();

    // попробую здесь настроить перемещение объектов...
    if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_move)
    {
        m_current_scene3D->moveSelectedObjects(true,m_currentGizmoEvent,false,m_event_consumer->isLmbDownOnce());
        setNeedToSave(true);
    }else if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_scale)
    {
        m_current_scene3D->scaleSelectedObjects(true,m_currentGizmoEvent,false,m_event_consumer->isLmbDownOnce());
        setNeedToSave(true);
    
    }else if( m_state_app == AppState_main::Gizmo && m_currentGizmoEvent.type == AppEvent_gizmo::_type::_rotate)
    {
        m_current_scene3D->rotateSelectedObjects(true,m_currentGizmoEvent,false,m_event_consumer->isLmbDownOnce());
        setNeedToSave(true);
    }

    while( m_appEvents[2].size() )
    {
      //  printf("   %u\n",m_appEvents[2].front().uinteger);
      
        m_appEvents[2].clear();
    }

    m_enterTextMode = false;
}

void Application::_drawInfoWindow()
{
    
}

void Application::_applyMatrices()
{
    _setEditMode(EditMode::Object);
    m_current_scene3D->applyMatrices();
}

void Application::_resetMatrices()
{
    _setEditMode(EditMode::Object);
    m_current_scene3D->resetMatrices();
}

void Application::_showRightTab(bool v)
{
    m_showRightTab = v;
}

void Application::_setRightTabMode(RightTabMode m)
{
    auto old = m_rightTabMode;
    m_rightTabMode = m;
    switch(m_rightTabMode)
    {
    case RightTabMode::Create:
        if(old == RightTabMode::Create)
        {
            _showRightTab(m_showRightTab ? false : true);
        }
        else
        {
            _showRightTab(true);
        }
        break;
    case RightTabMode::Edit:
        if(old == RightTabMode::Edit)
        {
            _showRightTab(m_showRightTab ? false : true);
        }
        else
        {
            _showRightTab(true);
        }
        _callOnActivateGUIPlugin();
        break;
    case RightTabMode::UVEdit:
        if(old == RightTabMode::UVEdit)
        {
            _showRightTab(m_showRightTab ? false : true);
        }
        else
        {
            _showRightTab(true);
        }
        break;
    default:
        break;
    }
}

void Application::openImageFilePathDialog(kkString* outFilePath)
{
    *outFilePath = getOpenFileNameForImages();
}

void Application::_createWhiteTexture()
{
    kkImage image;
    image.m_width    = 32;
	image.m_height   = 32;
	image.m_bits     = 32;
	image.m_format   = kkImageFormat::R8G8B8A8;
	image.m_frames   = 1;
	image.m_mipCount = 1;
	image.m_pitch    = image.m_width * 4;
	image.m_dataSize = image.m_height * image.m_pitch;
	image.m_data8    = (u8*)kkMemory::allocate(image.m_dataSize);
    auto data = image.m_data8;
    for( int i = 0; i < 32 * 32 * 4; )
    {
        *data = 255; ++data;
        *data = 255; ++data;
        *data = 255; ++data;
        *data = 255; ++data;

        i += 4;
    }

    m_whiteTexture = m_gs->createTexture(&image);
}

kkTexture* Application::getWhiteTexture()
{
    return m_whiteTexture.ptr();
}

void Application::_setStandartObjectCategory()
{
    u64 index = 0;
    for( u64 i = 0, sz = m_objectGeneratorCategories.size(); i < sz; ++i )
    {
        auto cat = m_objectGeneratorCategories[i];

        auto & plugin_id = cat->GetPluginID();

        if( !plugin_id.m_data )
            continue;

        if( plugin_id.m_dataSize != sizeof(kkPluginIDData) )
            continue;

        kkPluginIDData* id_data = reinterpret_cast<kkPluginIDData*>( plugin_id.m_data );

        if( id_data->returnMagic() != KK_MAKEFOURCC('K','K','R','O') )
            continue;

        if( id_data->m_uniqueNumber != id_data->returnMagic() + KK_MAKEFOURCC('S','T','D','O') )
            continue;

        //m_selectedCategoryIndex = i;
        index = i;

        break;
    }

    if( index )
    {
        auto target_cat = m_objectGeneratorCategories[0];
        m_objectGeneratorCategories[0] = m_objectGeneratorCategories[index];
        m_objectGeneratorCategories[index] = target_cat;
    }
}

kkPluginObjectCategory*  Application::createObjectCategory( const char16_t* name, const kkPluginID& id )
{
    assert(name);
	for( auto c : m_objectGeneratorCategories )
	{
		if( c->m_name == kkString(name) )
			return c;
	}

	PluginObjectCategory * cat = kkCreate<PluginObjectCategory>(name, id);
	m_objectGeneratorCategories.push_back(cat);
	return cat;
}

kkString Application::getSaveFileName(const char16_t* fileTypeName, const char16_t* extension)
{
    kkString result;

#ifdef KK_PLATFORM_WINDOWS
    
    m_fileSaveDialog->SetTitle(L"Save file");
    m_fileSaveDialog->SetOkButtonLabel(L"Save");

    COMDLG_FILTERSPEC rgSpec;
    kkStringW wstr = L"*.";
    wstr += extension;

    rgSpec.pszName = (wchar_t*)fileTypeName;
    rgSpec.pszSpec = wstr.data();
            
    m_fileSaveDialog->SetFileTypes(1,&rgSpec);

    auto hr = m_fileSaveDialog->Show((HWND)m_mainWindow->getHandle());
    if( SUCCEEDED(hr) )
    {
        IShellItem *pItem;
        hr = m_fileSaveDialog->GetResult(&pItem);
        if( SUCCEEDED(hr) )
        {
            PWSTR pszFilePath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            if( SUCCEEDED(hr) )
            {
                result = pszFilePath;
                CoTaskMemFree(pszFilePath);
            }
            pItem->Release();
        }
    }
#else
#error Implement...
#endif
    return result;
}

void Application::saveImageToFile(kkImage* image)
{
	auto file_name = getSaveFileNameForImages();
    if( file_name.size() )
    {
        auto plugins = m_plugin_manager.getExportImagePlugins();
        
        auto file_name_ext = util::stringGetExtension(file_name);
        util::stringToLower(file_name_ext);
        /*boost::filesystem::path boost_path((wchar_t*)file_name.data());

        auto file_name_ext = boost::filesystem::extension(boost_path);*/

        for( u64 i = 0, sz =  plugins.size(); i < sz; ++i )
        {
            std::vector<kkString> extensions_array;
            util::stringGetWords<kkString>( &extensions_array, plugins[i]->getInfo().m_extensions );

            for( auto & e : extensions_array )
            {
                if( e == file_name_ext )
                {
                    printf("Save image [%s]\n", file_name.to_kkStringA().data());
                    plugins[i]->call_onExportImage(m_plugin_interface, file_name.data(), image);
                    return;
                }
            }
        }

    }
}

bool Application::isWindowActive(E_WINDOW_ID id)
{
    return m_activeOSWindow == id;
}

void Application::showImportExportWindow(PluginGUIWindow* w, const v2i& size)
{
    m_importExportWindow->resizeWindow(size);
    m_importExportWindow->moveWindow(v2i(50,50));
    m_importExportWindow->show();
    m_importExportGUIWindow = w;
}
