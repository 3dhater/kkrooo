// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "FileSystem/kkFileSystem.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Plugins/Plugin.h"
#include "GraphicsSystem/kkTexture.h"
#include "../Scene3D/Scene3D.h"
#include "../EventConsumer.h"
#include "../ShortcutManager.h"
#include "../Functions.h"

//#include "imgui.h"

using namespace Kr;


void Application::_drawMainMenuBar()
{
    if( m_KrGuiSystem->menuBarBegin() )
	{
		if(m_KrGuiSystem->menuBarMenu(u"File"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->addMenuItem(u"New scene", m_shortcutManager->getShortcutText(ShortcutCommand_General::New), 0 ))
				{
                    m_mainMenuCommand.type   = MainMenuCommandType::NewScene;
				}
				if(m_KrGuiSystem->addMenuItem(u"Open", m_shortcutManager->getShortcutText(ShortcutCommand_General::Open), 0 ))
				{
				}
				if(m_KrGuiSystem->addMenuItem(u"Test image import", 0, 0 ))
				{
					auto i = loadImageFromDialog();
                    if( i )
                        kkDestroy(i);
				}
				if(m_KrGuiSystem->beginMenu(u"Open Recent", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu) ))
				{
					/*if(m_KrGuiSystem->addMenuItem(u"Project/Solution…", u"Ctrl+Shift+O", 0 )){ printf("Open Project\n"); }
					if(m_KrGuiSystem->addMenuItem(u"Folder…", 0, 0 )){ printf("Open Folder\n"); }
					if(m_KrGuiSystem->addMenuItem(u"File…", 0, 0 )){ printf("Open File\n"); }*/
					m_KrGuiSystem->endMenu();
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Save", m_shortcutManager->getShortcutText(ShortcutCommand_General::Save), 0 ))
				{ 
				}
				if(m_KrGuiSystem->addMenuItem(u"Save As..", m_shortcutManager->getShortcutText(ShortcutCommand_General::SaveAs), 0 ))
				{ 
				}
				if(m_KrGuiSystem->addMenuItem(u"File Properties", 0, 0 ))
				{ 
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);

				if(m_KrGuiSystem->beginMenu(u"Import", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
					if(m_KrGuiSystem->addMenuItem(u"Import...", 0, 0 ))
					{
                        m_mainMenuCommand.type   = MainMenuCommandType::ImportWithFileName;
					}
					m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
					auto arr = m_plugin_manager.getImportModelPlugins();
                    for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
                    {
                        auto info = arr[i]->getInfo();
                        if(m_KrGuiSystem->addMenuItem(info.m_extension_description.data(), 0, 0))
                        {
                            m_mainMenuCommand.type   = MainMenuCommandType::Import;
                            m_mainMenuCommand.plugin = arr[i];
                            break;
                        }
                    }
					m_KrGuiSystem->endMenu();
				}
				if(m_KrGuiSystem->beginMenu(u"Export", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    auto arr = m_plugin_manager.getExportModelPlugins();
                    for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
                    {
                        auto info = arr[i]->getInfo();
                        if(m_KrGuiSystem->addMenuItem(info.m_extension_description.data(), 0, 0))
                        {
                            m_mainMenuCommand.type   = MainMenuCommandType::Export;
                            m_mainMenuCommand.plugin = arr[i];
                            break;
                        }
                    }
					m_KrGuiSystem->endMenu();
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Exit", u"Alt+F4", 0 ))
				{
					quit();
				}
				m_KrGuiSystem->popupMenuEnd();
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
						_setEditMode( EditMode::Object );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Vertex", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeVertex), 0))
                    {
						_setEditMode( EditMode::Vertex );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Edge", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeEdge), 0))
                    {
						_setEditMode( EditMode::Edge );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Polygon", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModePolygon), 0))
                    {
						_setEditMode( EditMode::Polygon );
                    }
					m_KrGuiSystem->endMenu();
				}
				if(m_KrGuiSystem->beginMenu(u"Select mode...", 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
				{
                    if(m_KrGuiSystem->addMenuItem(u"Just select", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeJustSelect), 0))
                    {
						_setSelectMode( SelectMode::JustSelect );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Move", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeMove), 0))
                    {
						_setSelectMode( SelectMode::Move );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Rotate", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeRotate), 0))
                    {
						_setSelectMode( SelectMode::Rotate );
                    }
					if(m_KrGuiSystem->addMenuItem(u"Scale", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeScale), 0))
                    {
						_setSelectMode( SelectMode::Scale );
                    }
					m_KrGuiSystem->endMenu();
				}

				if(m_KrGuiSystem->addMenuItem(u"Select by name",0))
				{
					this->m_drawSelectByNameWindow = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Enter transformation",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EnterTransformation)))
				{
					m_drawTransformWindow = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Pivot tool",0))
				{
					m_drawPivotToolWindow = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Undo",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Undo)))
				{
				}
				if(m_KrGuiSystem->addMenuItem(u"Redo",m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Redo)))
				{
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Delete",u"Delete"))
				{
					_deleteSelectedObjects();
				}
				if(m_KrGuiSystem->addMenuItem(u"Clone",0))
				{
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Rotate local",0))
				{
					_setSelectMode( SelectMode::Rotate);
					m_isLocalRotation = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Scale local",0))
				{
					_setSelectMode( SelectMode::Scale);
					m_isLocalScale = true;
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Apply matrices",0))
				{
					m_mainMenuCommand.type   = MainMenuCommandType::ApplyMatrices;
				}
				if(m_KrGuiSystem->addMenuItem(u"Reset matrices",0))
				{
					_resetMatrices();
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Select all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectAll)))
				{
					m_current_scene3D->selectAll();
				}
				if(m_KrGuiSystem->addMenuItem(u"Deselect all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::DeselectAll)))
				{
					m_current_scene3D->deselectAll();
				}
				if(m_KrGuiSystem->addMenuItem(u"Select invert", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectInvert)))
				{
					m_current_scene3D->selectInvert();
				}
				m_KrGuiSystem->popupMenuEnd();
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Tools"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				if(m_KrGuiSystem->addMenuItem(u"Array",0))
				{
				}
				m_KrGuiSystem->addSeparator(&m_mainMenuStyle);
				if(m_KrGuiSystem->addMenuItem(u"Shortcut Manager",0))
				{
					m_drawShortcutManager = true;
				}
				if(m_KrGuiSystem->addMenuItem(u"Preferences",0))
				{
					m_drawPreferencesWindow = true;
				}
				m_KrGuiSystem->popupMenuEnd();
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
						_resetViewports();
					}
					m_KrGuiSystem->endMenu();
				}
                if( m_KrGuiSystem->addMenuItem(u"Cull back faces", 0, 0 /*,&m_backfaceCull*/ ))
				{
					m_backfaceCull = m_backfaceCull ? false : true;
				}
				m_KrGuiSystem->popupMenuEnd();
			}
		}
		if(m_KrGuiSystem->menuBarMenu(u"Create"))
		{
			bool showMenu = true;
			if(m_KrGuiSystem->popupMenuBegin( &showMenu, &m_mainMenuStyle ))
			{
				_drawMainMenu_ObjectCreatorCategories();
				m_KrGuiSystem->popupMenuEnd();
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
				}
				if( m_KrGuiSystem->addMenuItem(u"UV editor", 0, 0 ))
				{
					//m_mainMenuCommand.type   = MainMenuCommandType::ShowMaterialEditor;
				}
				m_KrGuiSystem->popupMenuEnd();
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
				}
				m_KrGuiSystem->popupMenuEnd();
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
		m_KrGuiSystem->menuBarEnd(&m_mainMenuHeight);

		//m_mainToolBarHeight = m_mainMenuHeight;
	}
}

void Application::_drawImguiMainMenuBar()
{
 //   bool menu_opened = false; /// to determine when the menu was closed
 //   
	//if (ImGui::BeginMainMenuBar())
 //   {
 //       if (ImGui::BeginMenu("File",m_imgui_active))
 //       {
 //          // if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;

 //               m_state_app = AppState_main::MainMenu;
 //           
 //               if (ImGui::MenuItem("New scene", m_shortcutManager->getShortcutText(ShortcutCommand_General::New) ))
 //               {
 //                   m_mainMenuCommand.type   = MainMenuCommandType::NewScene;
 //               }
 //               
 //               if (ImGui::MenuItem("Open", m_shortcutManager->getShortcutText(ShortcutCommand_General::Open) ))
 //               {
 //               }

 //               if (ImGui::MenuItem("Test image import" ))
 //               {
 //                   auto i = loadImageFromDialog();
 //                   if( i )
 //                       kkDestroy(i);
 //               }

 //               if (ImGui::BeginMenu("Open Recent"))
 //               {
 //                   //ImGui::MenuItem("fish_hat.c");
 //                   /*if (ImGui::BeginMenu("More.."))
 //                   {
 //                       ImGui::MenuItem("Hello");
 //                       ImGui::MenuItem("Sailor");
 //                       if (ImGui::BeginMenu("Recurse.."))
 //                       {
 //                           ImGui::EndMenu();
 //                       }
 //                       ImGui::EndMenu();
 //                   }*/
 //                   ImGui::EndMenu();
 //               }
 //               ImGui::Separator();

 //               if(ImGui::MenuItem("Save", m_shortcutManager->getShortcutText(ShortcutCommand_General::Save)))
 //               {
 //               }

 //               if(ImGui::MenuItem("Save As..", m_shortcutManager->getShortcutText(ShortcutCommand_General::SaveAs)))
 //               {
 //               }
 //               if(ImGui::MenuItem("File Properties"))
 //               {
 //               }
 //               
 //               ImGui::Separator();

 //               if(ImGui::BeginMenu("Import"))
 //               {
 //                   if(ImGui::MenuItem("Import..."))
 //                   {
 //                       m_mainMenuCommand.type   = MainMenuCommandType::ImportWithFileName;
 //                   }
 //                   ImGui::Separator();

 //                   auto arr = m_plugin_manager.getImportModelPlugins();
 //                   for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
 //                   {
 //                       auto info = arr[i]->getInfo();
 //                       if(ImGui::MenuItem(info.m_extension_description.to_kkStringA().data()))
 //                       {
 //                           m_mainMenuCommand.type   = MainMenuCommandType::Import;
 //                           m_mainMenuCommand.plugin = arr[i];
 //                           break;
 //                       }
 //                   }
 //                   ImGui::EndMenu();
 //               }

 //               if(ImGui::BeginMenu("Export"))
 //               {
 //                   auto arr = m_plugin_manager.getExportModelPlugins();
 //                   for( u32 i = 0, sz = (u32)arr.size(); i < sz; ++i )
 //                   {
 //                       auto info = arr[i]->getInfo();
 //                       if(ImGui::MenuItem(info.m_extension_description.to_kkStringA().data()))
 //                       {
 //                           m_mainMenuCommand.type   = MainMenuCommandType::Export;
 //                           m_mainMenuCommand.plugin = arr[i];
 //                           break;
 //                       }
 //                   }
 //                   ImGui::EndMenu();
 //               }


 //               //if(ImGui::BeginMenu("Options"))
 //              // {
 //                   
 //                   
 //                   /*static bool enabled = true;
 //                   ImGui::MenuItem("Enabled", "", &enabled);
 //                   ImGui::BeginChild("child", ImVec2(0, 60), true);
 //                   for (int i = 0; i < 10; i++)
 //                       ImGui::Text("Scrolling Text %d", i);
 //                   ImGui::EndChild();
 //                   static float f = 0.5f;
 //                   static int n = 0;
 //                   static bool b = true;
 //                   ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
 //                   ImGui::InputFloat("Input", &f, 0.1f);
 //                   ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
 //                   ImGui::Checkbox("Check", &b);*/
 //             //      ImGui::EndMenu();
 //             //  }
 //              /* if (ImGui::BeginMenu("Colors"))
 //               {
 //                   float sz = ImGui::GetTextLineHeight();
 //                   for (int i = 0; i < ImGuiCol_COUNT; i++)
 //                   {
 //                       const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
 //                       ImVec2 p = ImGui::GetCursorScreenPos();
 //                       ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x+sz, p.y+sz), ImGui::GetColorU32((ImGuiCol)i));
 //                       ImGui::Dummy(ImVec2(sz, sz));
 //                       ImGui::SameLine();
 //                       ImGui::MenuItem(name);
 //                   }
 //                   ImGui::EndMenu();
 //               }*/
 //               //if (ImGui::BeginMenu("Disabled", false)) // Disabled
 //               //{
 //               //    IM_ASSERT(0);
 //               //}
 //               /*if (ImGui::MenuItem("Checked", NULL, true))
 //               {
 //               }*/

 //               ImGui::Separator();
 //               if( ImGui::MenuItem("Quit", "Alt+F4") )
 //               {
 //                   quit();
 //               }
 //           }

 //           ImGui::EndMenu();
 //       }

 //       if (ImGui::BeginMenu("Edit",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;
 //               if( ImGui::BeginMenu("Edit mode...") )
 //               {
 //                   if(ImGui::MenuItem("Object", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeObject) ))
 //                   {
 //                       _setEditMode( EditMode::Object );
 //                   }
 //                   if(ImGui::MenuItem("Vertex", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeVertex) ))
 //                   {
 //                       _setEditMode( EditMode::Vertex );
 //                   }
 //                   if(ImGui::MenuItem("Edge", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModeEdge) ))
 //                   {
 //                       _setEditMode( EditMode::Edge );
 //                   }
 //                   if(ImGui::MenuItem("Polygon", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EditModePolygon) ))
 //                   {
 //                       _setEditMode( EditMode::Polygon );
 //                   }
 //                   ImGui::EndMenu();
 //               }
 //               if( ImGui::BeginMenu("Select mode...") )
 //               {
 //                   if(ImGui::MenuItem("Just select", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeJustSelect) ))
 //                   {
 //                       _setSelectMode( SelectMode::JustSelect );
 //                   }
 //                   if(ImGui::MenuItem("Move", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeMove) ))
 //                   {
 //                       _setSelectMode( SelectMode::Move);
 //                   }
 //                   if(ImGui::MenuItem("Rotate", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeRotate) ))
 //                   {
 //                       _setSelectMode( SelectMode::Rotate);
 //                   }
 //                   if(ImGui::MenuItem("Scale", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectModeScale) ))
 //                   {
 //                       _setSelectMode( SelectMode::Scale);
 //                   }
 //                   ImGui::EndMenu();
 //               }
 //               if(ImGui::MenuItem("Select by name"))
 //               {
 //                   this->m_drawSelectByNameWindow = true;
 //               }

 //               if(ImGui::MenuItem("Enter transformation", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::EnterTransformation) ))
 //               {
 //                   m_drawTransformWindow = true;
 //               }
 //               if(ImGui::MenuItem("Pivot tool"))
 //               {
 //                   m_drawPivotToolWindow = true;
 //               }
 //               
 //               ImGui::Separator();

 //               if (ImGui::MenuItem("Undo", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Undo) )) {}
 //               if (ImGui::MenuItem("Redo", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::Redo) )) {}
 //               ImGui::Separator();
 //               if(ImGui::MenuItem("Delete", "Delete"))
 //               {
 //                   _deleteSelectedObjects();
 //               }
 //               if(ImGui::MenuItem("Clone"))
 //               {
 //               }
 //               ImGui::Separator();
 //               if(ImGui::MenuItem("Rotate local"))
 //               {
 //                   _setSelectMode( SelectMode::Rotate);
 //                   m_isLocalRotation = true;
 //               }
 //               if(ImGui::MenuItem("Scale local"))
 //               {
 //                   _setSelectMode( SelectMode::Scale);
 //                   m_isLocalScale = true;
 //               }
 //               ImGui::Separator();
 //               if(ImGui::MenuItem("Apply matrices"))
 //               {
 //                   m_mainMenuCommand.type   = MainMenuCommandType::ApplyMatrices;

 //                   //_applyMatrices();
 //               }
 //               if(ImGui::MenuItem("Reset matrices"))
 //               {
 //                   _resetMatrices();
 //               }
 //               ImGui::Separator();
 //               if(ImGui::MenuItem("Select all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectAll)))
 //               {
 //                   m_current_scene3D->selectAll();
 //               }
 //               if(ImGui::MenuItem("Deselect all", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::DeselectAll)))
 //               {
 //                   m_current_scene3D->deselectAll();
 //               }
 //               if(ImGui::MenuItem("Select invert", m_shortcutManager->getShortcutText(ShortcutCommand_Edit::SelectInvert)))
 //               {
 //                   m_current_scene3D->selectInvert();
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //       if (ImGui::BeginMenu("Tools",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;

 //               if(ImGui::MenuItem("Array"))
 //               {
 //               }

 //               

 //               ImGui::Separator();
 //               if(ImGui::MenuItem("Shortcut Manager"))
 //               {
 //                   m_drawShortcutManager = true;
 //               }
 //               if(ImGui::MenuItem("Preferences" ))
 //               {
 //                   m_drawPreferencesWindow = true;
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //       if (ImGui::BeginMenu("View",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;
 //               if(ImGui::BeginMenu("Viewport"))
 //               {
 //                   ImGui::MenuItem("Draw borders",0,&m_isDrawViewportBorders);
 //                   ImGui::Separator();
 //                   if(ImGui::MenuItem("Reset"))
 //                   {
 //                       _resetViewports();
 //                   }
 //                   
 //                   ImGui::EndMenu();
 //               }
 //               if(ImGui::MenuItem("Cull back faces", 0, &m_backfaceCull))
 //               {
 //                   //m_backfaceCull = m_backfaceCull ? false : true;
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //       if (ImGui::BeginMenu("Create",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;

 //               _drawMainMenu_ObjectCreatorCategories();
 //               
 //           }
 //           ImGui::EndMenu();
 //       }
 //       if(ImGui::BeginMenu("Materials",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;

 //               if(ImGui::MenuItem("Material editor", m_shortcutManager->getShortcutText(ShortcutCommand_General::ShowMaterialEditor) ))
 //               {
 //                   //m_testWindow->show();
 //                   m_mainMenuCommand.type   = MainMenuCommandType::ShowMaterialEditor;
 //               }
 //               if(ImGui::MenuItem("UV editor"))
 //               {
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //       if(ImGui::BeginMenu("Rendering",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;

 //               if(ImGui::MenuItem("Render", m_shortcutManager->getShortcutText(ShortcutCommand_General::ShowRenderWindow)))
 //               {
 //                   m_mainMenuCommand.type   = MainMenuCommandType::ShowRenderWindow;
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //        if(ImGui::BeginMenu("Debug",m_imgui_active))
 //       {
 //           //if( m_state_app != AppState_main::PluginGUI )
 //           {
 //               menu_opened = true;
 //               m_state_app = AppState_main::MainMenu;

 //               ImGui::MenuItem("Draw scene AABB" ,0,&m_debug_drawSceneAabb);
 //               ImGui::MenuItem("Draw object AABB",0,&m_debug_drawObjectAabb);
 //               ImGui::MenuItem("Draw object OBB" ,0,&m_debug_drawObjectObb);
 //               ImGui::MenuItem("Draw selected objects AABB" ,0,&m_debug_drawSelectedObjectsAabb);
 //               ImGui::MenuItem("Draw 2D points" ,0,&m_debug_draw2DPoints);
 //               ImGui::MenuItem("Draw info window" ,0,&m_drawInfoWindow);
 //               if(ImGui::MenuItem("ImageToBin"))
 //               {
 //                   auto i = loadImageFromDialog();
 //                   if( i )
 //                   {
 //                       /*auto file = util::createFileForWriteBinShared(u"output_debug_ImageToBin.txt");
 //                       if( file )
 //                       {
 //                           for( int k = 0; k < i->m_dataSize; ++k )
 //                           {
 //                               char str[32];
 //                               sprintf(str,"%02x", (int)i->m_data8[k] );
 //                               file->write( (u8*)"0x", 2 );
 //                               file->write( (u8*)str, 2 );
 //                               file->write( (u8*)", ", 2 );
 //                           }
 //                       }*/
 //                       auto file = util::createFileForWriteBinShared(u"output_debug_ImageToBin.bin");
 //                       if( file )
 //                       {
 //                           file->write( i->m_data8, i->m_dataSize );
 //                       }
 //                       kkDestroy(i);
 //                   }
 //               }
 //           }
 //           ImGui::EndMenu();
 //       }
 //       ImGui::EndMainMenuBar();
 //   }

 //   if( !menu_opened && m_state_app == AppState_main::MainMenu ) /// ok, menu is closed, now change app state
 //   {
 //       m_state_app = AppState_main::Idle;
 //   }
}

