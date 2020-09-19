#include "kkrooo.engine.h"
#include "../Common.h"

#include "FileSystem/kkFileSystem.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Plugins/Plugin.h"
#include "GraphicsSystem/kkTexture.h"
#include "../Scene3D/Scene3D.h"
#include "../EventConsumer.h"
#include "../ShortcutManager.h"
#include "../Functions.h"

using namespace Kr;


void Application::_drawMainMenuBar()
{
	bool redraw = false;
    if( m_KrGuiSystem->menuBarBegin() )
	{
		if(m_KrGuiSystem->menuBarMenu(u"File"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->addMenuItem(u"New scene", m_shortcutManager->getShortcutText(ShortcutCommand_General::New), 0 ))
				{
					redraw = true;
                    m_mainMenuCommand.type   = MainMenuCommandType::NewScene;
				}
				if(m_KrGuiSystem->addMenuItem(u"Open", m_shortcutManager->getShortcutText(ShortcutCommand_General::Open), 0 ))
				{
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Test image import", 0, 0 ))
				{
					redraw = true;
					auto i = loadImageFromDialog();
                    if( i )
                        kkDestroy(i);
				}
				if(m_KrGuiSystem->beginMenu(u"Open Recent", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu) ))
				{
					redraw = true;
					/*if(m_KrGuiSystem->addMenuItem(u"Project/Solution…", u"Ctrl+Shift+O", 0 )){ printf("Open Project\n"); }
					if(m_KrGuiSystem->addMenuItem(u"Folder…", 0, 0 )){ printf("Open Folder\n"); }
					if(m_KrGuiSystem->addMenuItem(u"File…", 0, 0 )){ printf("Open File\n"); }*/
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Save", m_shortcutManager->getShortcutText(ShortcutCommand_General::Save), 0 ))
				{ 
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Save As..", m_shortcutManager->getShortcutText(ShortcutCommand_General::SaveAs), 0 ))
				{ 
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"File Properties", 0, 0 ))
				{ 
					redraw = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);

				if(m_KrGuiSystem->beginMenu(u"Import", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
					if(m_KrGuiSystem->addMenuItem(u"Import...", 0, 0 ))
					{
						redraw = true;
                        m_mainMenuCommand.type   = MainMenuCommandType::ImportWithFileName;
					}
					m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
					auto arr = m_plugin_manager.getImportModelPlugins();
                    for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
                    {
                        auto info = arr[i]->getInfo();
                        if(m_KrGuiSystem->addMenuItem(info.m_extension_description.data(), 0, 0))
                        {
							redraw = true;
                            m_mainMenuCommand.type   = MainMenuCommandType::Import;
                            m_mainMenuCommand.plugin = arr[i];
                            break;
                        }
                    }
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}
				if(m_KrGuiSystem->beginMenu(u"Export", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    auto arr = m_plugin_manager.getExportModelPlugins();
                    for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
                    {
                        auto info = arr[i]->getInfo();
                        if(m_KrGuiSystem->addMenuItem(info.m_extension_description.data(), 0, 0))
                        {
							redraw = true;
                            m_mainMenuCommand.type   = MainMenuCommandType::Export;
                            m_mainMenuCommand.plugin = arr[i];
                            break;
                        }
                    }
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Exit", u"Alt+F4", 0 ))
				{
					quit();
				}

				if( m_KrGuiSystem->isLastItemCursorHover() || m_KrGuiSystem->isLastItemCursorMove() )
					m_cursorInGUI = true;

				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Edit"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->beginMenu(u"Edit mode...", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    if(m_KrGuiSystem->addMenuItem(u"Object", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeObject), 0))
                    {
						setEditMode( EditMode::Object );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Vertex", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeVertex), 0))
                    {
						setEditMode( EditMode::Vertex );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Edge", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeEdge), 0))
                    {
						setEditMode( EditMode::Edge );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Polygon", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModePolygon), 0))
                    {
						setEditMode( EditMode::Polygon );
                    }
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}
				if(m_KrGuiSystem->beginMenu(u"Select mode...", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    if(m_KrGuiSystem->addMenuItem(u"Just select", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeJustSelect), 0))
                    {
						setSelectMode( SelectMode::JustSelect );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Move", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeMove), 0))
                    {
						setSelectMode( SelectMode::Move );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Rotate", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeRotate), 0))
                    {
						setSelectMode( SelectMode::Rotate );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Scale", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeScale), 0))
                    {
						setSelectMode( SelectMode::Scale );
                    }
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}

				if(m_KrGuiSystem->addMenuItem(u"Select by name",0))
				{
					redraw = true;
					this->m_drawSelectByNameWindow = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Enter transformation",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EnterTransformation)))
				{
					redraw = true;
					m_drawTransformWindow = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Pivot tool",0))
				{
					redraw = true;
					m_drawPivotToolWindow = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Undo",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Undo)))
				{
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Redo",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Redo)))
				{
					redraw = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Delete",u"Delete"))
				{
					_deleteSelectedObjects();
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Clone",0))
				{
					redraw = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Rotate local",0))
				{
					setSelectMode( SelectMode::Rotate);
					m_isLocalRotation = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Scale local",0))
				{
					setSelectMode( SelectMode::Scale);
					m_isLocalScale = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Apply matrices",0))
				{
					redraw = true;
					m_mainMenuCommand.type   = MainMenuCommandType::ApplyMatrices;
				}
				if(m_KrGuiSystem->addMenuItem(u"Reset matrices",0))
				{
					_resetMatrices();
					redraw = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Select all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectAll)))
				{
					m_current_scene3D->selectAll();
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Deselect all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::DeselectAll)))
				{
					m_current_scene3D->deselectAll();
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Select invert", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectInvert)))
				{
					m_current_scene3D->selectInvert();
					redraw = true;
				}
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Tools"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->addMenuItem(u"Array",0))
				{
					redraw = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Shortcut Manager",0))
				{
					m_drawShortcutManager = true;
					redraw = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Preferences",0))
				{
					m_drawPreferencesWindow = true;
					redraw = true;
				}
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"View"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->beginMenu(u"Viewport", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    m_KrGuiSystem->addMenuItem(u"Draw borders", 0, 0 /*,&m_isDrawViewportBorders*/);
					m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
                    if( m_KrGuiSystem->addMenuItem(u"Reset", 0, 0 ))
					{
//						_resetViewports();
					redraw = true;
					}
					if( m_KrGuiSystem->endMenu() )
						m_cursorInGUI = true;
				}
                if( m_KrGuiSystem->addMenuItem(u"Cull back faces", 0, 0 /*,&m_backfaceCull*/ ))
				{
					m_backfaceCull = m_backfaceCull ? false : true;
					redraw = true;
				}
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Create"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				_drawMainMenu_ObjectCreatorCategories();
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}	
		if(m_KrGuiSystem->menuBarMenu(u"Materials"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
                if( m_KrGuiSystem->addMenuItem(u"Material editor", 0, 0 ))
				{
					m_mainMenuCommand.type   = MainMenuCommandType::ShowMaterialEditor;
					redraw = true;
				}
				if( m_KrGuiSystem->addMenuItem(u"UV editor", 0, 0 ))
				{
					//m_mainMenuCommand.type   = MainMenuCommandType::ShowMaterialEditor;
					redraw = true;
				}
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Rendering"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
                if( m_KrGuiSystem->addMenuItem(u"Render", 0, 0 ))
				{
					m_mainMenuCommand.type   = MainMenuCommandType::ShowRenderWindow;
					redraw = true;
				}
				if( m_KrGuiSystem->popupMenuEnd() )
					m_cursorInGUI = true;
			}
		}
		/*if(m_KrGuiSystem->menuBarMenu(u"Debug"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
                if( m_KrGuiSystem->addMenuItem(u"Render", 0, 0 ))
				{
					m_mainMenuCommand.type   = MainMenuCommandType::ShowRenderWindow;
				}
				m_KrGuiSystem->popupMenuEnd();
			}
		}*/
		if( m_KrGuiSystem->menuBarEnd(&m_mainMenuHeight) )
		{
			m_cursorInGUI = true;
		}
	}
	if(redraw)
		kkDrawAll();
}

