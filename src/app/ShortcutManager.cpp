// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include <stdexcept>

#include "MainSystem/kkMainSystem.h"

#include "EventConsumer.h"

#include "Application.h"
#include "Functions.h"
#include "ShortcutManager.h"
#include "Common/kkUtil.h"
#include "FileSystem/kkFileSystem.h"


ShortcutManager::ShortcutManager()
{
}

ShortcutManager::~ShortcutManager()
{
}

void ShortcutManager::onFrame()
{
    auto ks = m_app->getStateKeyboard();

    auto f = []( kkArray<ShortcutCommandNode>& arr, EventConsumer* eventConsumer, AppState_keyboard ks )
    {
        u64 i = 0;
        u64 sz = arr.size();

        for(; i < sz; ++i )
        {
            arr[ i ].isReady = false;

            if( arr[ i ].keyboardState != ks )
            {
                continue;
            }


            if( arr[ i ].onRelease == 0 )
            {
                if( eventConsumer->m_keys_down[(u32)arr[ i ].key] != 0 )
                {
                    if( arr[ i ].isUsed == false )
                    {
                        arr[ i ].isUsed  = true;
                        arr[ i ].isReady = true;
                    }
                }

                if( eventConsumer->m_keys_up[(u32)arr[ i ].key] != 0 )
                    arr[ i ].isUsed = false;
            }
            else if( eventConsumer->m_keys_up[(u32)arr[ i ].key] != 0 )
            {
                arr[ i ].isReady = true;
            }
        }
    };

    f(m_cammandNodes_General, m_eventConsumer, ks);
    f(m_cammandNodes_Edit, m_eventConsumer, ks);
    f(m_cammandNodes_Camera, m_eventConsumer, ks);
    f(m_cammandNodes_Viewport, m_eventConsumer, ks);
}


bool ShortcutManager::init()
{
	int command_end = (int)ShortcutCommand_General::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_General.push_back(ShortcutCommandNode()); 
        m_cammandDesc_General[i].category = ShortcutCommandCategory::General;

        switch((ShortcutCommand_General)i)
        {
        case ShortcutCommand_General::ShowMaterialEditor: m_cammandDesc_General[i].name = "ShowMaterialEditor"; break;
        case ShortcutCommand_General::ShowRenderWindow:   m_cammandDesc_General[i].name = "ShowRenderWindow";   break;
        case ShortcutCommand_General::New:    m_cammandDesc_General[i].name = "New";    break;
        case ShortcutCommand_General::Save:   m_cammandDesc_General[i].name = "Save";   break;
        case ShortcutCommand_General::SaveAs: m_cammandDesc_General[i].name = "SaveAs"; break;
        case ShortcutCommand_General::Open:   m_cammandDesc_General[i].name = "Open";   break;
        case ShortcutCommand_General::_End:                                             break;
        default:                              fprintf(stdout, "Need to implement....\n");      return false;
        }

        m_imguiComboText_General[i] = m_cammandDesc_General[i].name;
    }

    command_end = (int)ShortcutCommand_Viewport::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Viewport.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Viewport[i].category = ShortcutCommandCategory::Viewport;

        switch((ShortcutCommand_Viewport)i)
        {
        case ShortcutCommand_Viewport::SetPerspective:  m_cammandDesc_Viewport[i].name = "SetPerspective";  break;
        case ShortcutCommand_Viewport::SetFront:        m_cammandDesc_Viewport[i].name = "SetFront";        break;
        case ShortcutCommand_Viewport::SetBack:         m_cammandDesc_Viewport[i].name = "SetBack";         break;
        case ShortcutCommand_Viewport::SetTop:          m_cammandDesc_Viewport[i].name = "SetTop";          break;
        case ShortcutCommand_Viewport::SetBottom:       m_cammandDesc_Viewport[i].name = "SetBottom";       break;
        case ShortcutCommand_Viewport::SetLeft:         m_cammandDesc_Viewport[i].name = "SetLeft";         break;
        case ShortcutCommand_Viewport::SetRight:        m_cammandDesc_Viewport[i].name = "SetRight";        break;
        case ShortcutCommand_Viewport::ToggleGrid:      m_cammandDesc_Viewport[i].name = "ToggleGrid";      break;
        case ShortcutCommand_Viewport::Maximize:        m_cammandDesc_Viewport[i].name = "Maximize";        break;
        case ShortcutCommand_Viewport::DrawModeLines:   m_cammandDesc_Viewport[i].name = "DrawModeLines";   break;
        case ShortcutCommand_Viewport::DrawModeMaterial:m_cammandDesc_Viewport[i].name = "DrawModeMaterial";break;
        case ShortcutCommand_Viewport::DrawModeMaterialAndLines:m_cammandDesc_Viewport[i].name = "DrawModeMaterialAndLines";   break;
        case ShortcutCommand_Viewport::ToggleDrawModeLines:m_cammandDesc_Viewport[i].name = "ToggleDrawModeLines";   break;
        case ShortcutCommand_Viewport::ToggleDrawModeMaterial:m_cammandDesc_Viewport[i].name = "ToggleDrawModeMaterial";   break;
        case ShortcutCommand_Viewport::_End:                                                                                break;
        default:                                                fprintf(stdout, "Need to implement....\n");                 return false;
        }


        m_imguiComboText_Viewport[i] = m_cammandDesc_Viewport[i].name;
    }

    command_end = (int)ShortcutCommand_Camera::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Camera.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Camera[i].category = ShortcutCommandCategory::Camera;

        switch((ShortcutCommand_Camera)i)
        {
        case ShortcutCommand_Camera::Reset:             m_cammandDesc_Camera[i].name = "Reset";            break;
        case ShortcutCommand_Camera::ToSelection:       m_cammandDesc_Camera[i].name = "ToSelection";      break;
        case ShortcutCommand_Camera::_End:                                                                             break;
        default:                                              fprintf(stdout, "Need to implement....\n");              return false;
        }

        m_imguiComboText_Camera[i] = m_cammandDesc_Camera[i].name;
    }

    command_end = (int)ShortcutCommand_Edit::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Edit.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Edit[i].category = ShortcutCommandCategory::Edit;

        switch((ShortcutCommand_Edit)i)
        {
        case ShortcutCommand_Edit::DeselectAll:             m_cammandDesc_Edit[i].name = "DeselectAll";             break;
        case ShortcutCommand_Edit::Redo:                    m_cammandDesc_Edit[i].name = "Redo";                    break;
        case ShortcutCommand_Edit::SelectAll:               m_cammandDesc_Edit[i].name = "SelectAll";               break;
        case ShortcutCommand_Edit::SelectInvert:            m_cammandDesc_Edit[i].name = "SelectInvert";            break;
        case ShortcutCommand_Edit::Undo:                    m_cammandDesc_Edit[i].name = "Undo";                    break;
        case ShortcutCommand_Edit::SelectModeJustSelect:    m_cammandDesc_Edit[i].name = "SelectModeJustSelect";    break;
        case ShortcutCommand_Edit::SelectModeMove:          m_cammandDesc_Edit[i].name = "SelectModeMove";          break;
        case ShortcutCommand_Edit::SelectModeRotate:        m_cammandDesc_Edit[i].name = "SelectModeRotate";        break;
        case ShortcutCommand_Edit::SelectModeScale:         m_cammandDesc_Edit[i].name = "SelectModeScale";         break;
        case ShortcutCommand_Edit::EnterTransformation:     m_cammandDesc_Edit[i].name = "EnterTransformation";     break;
        case ShortcutCommand_Edit::EditModeObject:          m_cammandDesc_Edit[i].name = "EditModeObject";          break;
        case ShortcutCommand_Edit::EditModeVertex:          m_cammandDesc_Edit[i].name = "EditModeVertex";          break;
        case ShortcutCommand_Edit::EditModeEdge:            m_cammandDesc_Edit[i].name = "EditModeEdge";            break;
        case ShortcutCommand_Edit::EditModePolygon:         m_cammandDesc_Edit[i].name = "EditModePolygon";         break;
        case ShortcutCommand_Edit::_End:                                                                            break;
        default:                                            fprintf(stdout, "Need to implement....\n");             return false;
        }

        m_imguiComboText_Edit[i] = m_cammandDesc_Edit[i].name;
    }


    if( !kkFileSystem::existFile(u"../res/keys/default.xml") )
    {
        fprintf(stderr,"Warning! ../res/keys/default.xml not found!\n");
        return true;
    }

    _load(u"../res/keys/default.xml");

	return true;
}

void ShortcutManager::_load(const char16_t* fn)
{

    kkString filename(fn);

    if( !kkFileSystem::existFile(fn) )
    {
        fprintf(stderr,"Warning! %s not found!\n",filename.to_kkStringA().data());
        return;
    }

    auto xml = kkCreatePtr( kkFileSystem::XMLRead(filename) );
    if(!xml.ptr())
    {
        fprintf(stderr,"Warning! Failed to read %s\n",filename.to_kkStringA().data());
        return;
    }

    _readXMLNodes(xml->selectNodes( u"/kkrooo_shortcuts/general" ), m_cammandNodes_General);
    _readXMLNodes(xml->selectNodes( u"/kkrooo_shortcuts/viewport" ), m_cammandNodes_Viewport);
    _readXMLNodes(xml->selectNodes( u"/kkrooo_shortcuts/camera" ), m_cammandNodes_Camera);
    _readXMLNodes(xml->selectNodes( u"/kkrooo_shortcuts/edit" ), m_cammandNodes_Edit);
}

void ShortcutManager::_load()
{
    auto fn = m_app->getOpenFileName(u"Shortcuts", u"xml");
    _load(fn.data());
}


const char16_t* ShortcutManager::_getKeyString( kkKey key )
{
    switch(key)
    {
    case kkKey::K_F1:  return u"F1";
    case kkKey::K_F2:  return u"F2";
    case kkKey::K_F3:  return u"F3";
    case kkKey::K_F4:  return u"F4";
    case kkKey::K_F5:  return u"F5";
    case kkKey::K_F6:  return u"F6";
    case kkKey::K_F7:  return u"F7";
    case kkKey::K_F8:  return u"F8";
    case kkKey::K_F9:  return u"F9";
    case kkKey::K_F10: return u"F10";
    case kkKey::K_F11: return u"F11";
    case kkKey::K_F12: return u"F12";
    case kkKey::K_0: return u"0";
    case kkKey::K_1: return u"1";
    case kkKey::K_2: return u"2";
    case kkKey::K_3: return u"3";
    case kkKey::K_4: return u"4";
    case kkKey::K_5: return u"5";
    case kkKey::K_6: return u"6";
    case kkKey::K_7: return u"7";
    case kkKey::K_8: return u"8";
    case kkKey::K_9: return u"9";
    case kkKey::K_A: return u"A";
    case kkKey::K_B: return u"B";
    case kkKey::K_C: return u"C";
    case kkKey::K_D: return u"D";
    case kkKey::K_E: return u"E";
    case kkKey::K_F: return u"F";
    case kkKey::K_G: return u"G";
    case kkKey::K_H: return u"H";
    case kkKey::K_I: return u"I";
    case kkKey::K_J: return u"J";
    case kkKey::K_K: return u"K";
    case kkKey::K_L: return u"L";
    case kkKey::K_M: return u"M";
    case kkKey::K_N: return u"N";
    case kkKey::K_O: return u"O";
    case kkKey::K_P: return u"P";
    case kkKey::K_Q: return u"Q";
    case kkKey::K_R: return u"R";
    case kkKey::K_S: return u"S";
    case kkKey::K_T: return u"T";
    case kkKey::K_U: return u"U";
    case kkKey::K_V: return u"V";
    case kkKey::K_W: return u"W";
    case kkKey::K_X: return u"X";
    case kkKey::K_Y: return u"Y";
    case kkKey::K_Z: return u"Z";
    case kkKey::K_ADD: return u"+";
    case kkKey::K_BACKSLASH: return u"\\";
    case kkKey::K_COLON: return u";";
    case kkKey::K_COMMA: return u",";
    case kkKey::K_DOT: return u".";
    case kkKey::K_FIGURE_CLOSE: return u"}";
    case kkKey::K_FIGURE_OPEN: return u"{";
    case kkKey::K_QUOTE: return u"'";
    case kkKey::K_SLASH: return u"/";
    case kkKey::K_SUB: return u"-";
    case kkKey::K_TILDE: return u"~";
    }
    return nullptr;
}

void ShortcutManager::draw(bool * drawShortcutManager)
{
    m_inputBuffer.clear();

    auto window_size = m_app->getWindowSize();

    static int combo_item_current = 0;
    static const char * active_label   = "...";

    static const char * label_general  = "General";
    static const char * label_viewport = "Viewport";
    static const char * label_camera   = "Camera";
    static const char * label_edit     = "Edit";


    constexpr u32 item_count_general  = (u32)ShortcutCommand_General::_End;
    constexpr u32 item_count_viewport = (u32)ShortcutCommand_Viewport::_End;
    constexpr u32 item_count_camera   = (u32)ShortcutCommand_Camera::_End;
    constexpr u32 item_count_edit     = (u32)ShortcutCommand_Edit::_End;
    int item_count                      = item_count_general;


    const char** current_items = m_imguiComboText_General;

    static int selected_category = 0;
    switch (selected_category)
    {
    case 0:
        active_label=label_general;
        break;
    case 1:
        active_label=label_viewport;
        current_items = m_imguiComboText_Viewport;
        item_count = item_count_viewport;
        break;
    case 2:
        active_label=label_camera;
        current_items = m_imguiComboText_Camera;
        item_count = item_count_camera;
        break;
    case 3:
        active_label=label_edit;
        current_items = m_imguiComboText_Edit;
        item_count = item_count_edit;
        break;
    }
    

   /* ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2((float)window_size.x, (float)window_size.y), ImGuiCond_FirstUseEver);
    if(ImGui::Begin("Shortcut_manager_bg", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs ))
    {*/
        
     //   ImGui::SetNextWindowBgAlpha(1.f);
	    //ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
     //   if(ImGui::Begin("Shortcut Manager", drawShortcutManager, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize))
     //   {
     //       if (ImGui::BeginMenuBar())
     //       {
     //           if (ImGui::BeginMenu("Close"))
     //           {
     //               *drawShortcutManager = false;
     //               ImGui::EndMenu();
     //           }
     //           ImGui::EndMenuBar();
     //       }

     //       ImGui::BeginChild("left pane", ImVec2(150, 0), true);
     //       if( ImGui::Selectable(label_general, selected_category == 0 ) )  { selected_category = 0; combo_item_current = 0; }
     //       if( ImGui::Selectable(label_viewport, selected_category == 1 ) ) { selected_category = 1; combo_item_current = 0; }
     //       if( ImGui::Selectable(label_camera,   selected_category == 2 ) ) { selected_category = 2; combo_item_current = 0; }
     //       if( ImGui::Selectable(label_edit,     selected_category == 3 ) ) { selected_category = 3; combo_item_current = 0; }
     //       ImGui::EndChild();

     //       ImGui::SameLine();
     //       
     //       ImGui::BeginGroup();
     //       ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
     //       ImGui::Text(active_label);
     //       ImGui::Separator();


     //       ImGui::Combo("action", &combo_item_current, current_items, item_count );

     //       auto node = _getNode(selected_category, combo_item_current);


     //      // ImGui::InputText("current", node->title.data(), node->title.size(), ImGuiInputTextFlags_ReadOnly);

     //        switch(m_app->getStateKeyboard())
     //       {
     //       case AppState_keyboard::Alt:          m_inputBuffer += "Alt+";            break;
     //       case AppState_keyboard::Ctrl:         m_inputBuffer += "Ctrl+";           break;
     //       case AppState_keyboard::Shift:        m_inputBuffer += "Shift+";          break;
     //       case AppState_keyboard::CtrlAlt:      m_inputBuffer += "Ctrl+Alt+";       break;
     //       case AppState_keyboard::ShiftAlt:     m_inputBuffer += "Shift+Alt+";      break;
     //       case AppState_keyboard::ShiftCtrl:    m_inputBuffer += "Ctrl+Shift+";     break;
     //       case AppState_keyboard::ShiftCtrlAlt: m_inputBuffer += "Ctrl+Shift+Alt+"; break;
     //       }

     //       kkKey key = kkKey::K_NONE;
     //       for( u32 i = 0; i < 222; ++i )
     //       {
     //           switch((kkKey)i)
     //           {
     //           case kkKey::K_CTRL:
     //           case kkKey::K_RCTRL:
     //           case kkKey::K_LCTRL:
     //           case kkKey::K_SHIFT:
     //           case kkKey::K_RSHIFT:
     //           case kkKey::K_LSHIFT:
     //           case kkKey::K_ALT:
     //           case kkKey::K_RALT:
     //           case kkKey::K_LALT:
     //               continue;
     //           }
     //           if( m_eventConsumer->isKeyDown((kkKey)i) )
     //           {
     //               key = (kkKey)i;
     //           }
     //       }

     //       const char16_t* key_str = _getKeyString(key);

     //       bool can_assign = false;
     //       if( key_str )
     //       {
     //           m_inputBuffer += key_str;
     //           can_assign = true;
     //       }

     //       //ImGui::InputText("hold key", m_inputBuffer.data(), m_inputBuffer.size(), ImGuiInputTextFlags_ReadOnly);
     //       
     //       static bool onRelease = false;

     //       if( node->onRelease )
     //           onRelease = true;
     //       else
     //           onRelease = false;

     //       ImGui::Checkbox( "on release", &onRelease );

     //       onRelease ? node->onRelease = 1 : node->onRelease = 0;

     //       bool do_assign = false;
     //       if( ImGui::Button("Assign (Enter)") )
     //       {
     //           if( can_assign )
     //           {
     //               do_assign = true;
     //           }
     //       }

     //       if( can_assign )
     //       {
     //           if( m_eventConsumer->isKeyDown(kkKey::K_ENTER))
     //           {
     //               do_assign = true;
     //           }
     //       }

     //       if( do_assign )
     //       {
     //           node->title         = m_inputBuffer;
     //           node->keyboardState = m_app->getStateKeyboard();
     //           node->key           = key;
     //       }

     //       ImGui::SameLine();
     //       if( ImGui::Button("Clear") )
     //       {
     //           node->title.clear();
     //           node->keyboardState = AppState_keyboard::None;
     //           node->key           = kkKey::K_NONE;
     //       }

     //       ImGui::SameLine();
     //       if(ImGui::Button("Save"))
     //       {
     //           _save();
     //       }
     //       ImGui::SameLine();
     //       if(ImGui::Button("Load"))
     //       {
     //           _load();
     //       }


     //       ImGui::EndChild();
     //       ImGui::EndGroup();
     //   }
    /*    ImGui::End();
    }*/

}

ShortcutCommandNode * ShortcutManager::_getNode(u32 i1, u32 i2)
{
    switch(i1)
    {
    case 0:  return &m_cammandNodes_General[i2];
    case 1:  return &m_cammandNodes_Viewport[i2];
    case 2:  return &m_cammandNodes_Camera[i2];
    case 3:  return &m_cammandNodes_Edit[i2];
    default:
        fprintf(stderr,"implement please!!!!\n");
        KK_PRINT_FAILED;
        return nullptr;
    }


    return nullptr;
}

const char16_t* ShortcutManager::getShortcutText( ShortcutCommand_General c ){ return m_cammandNodes_General[(u32)c].title.c_str(); }
const char16_t* ShortcutManager::getShortcutText( ShortcutCommand_Viewport c ){ return m_cammandNodes_Viewport[(u32)c].title.c_str(); }
const char16_t* ShortcutManager::getShortcutText( ShortcutCommand_Camera c ){ return m_cammandNodes_Camera[(u32)c].title.c_str(); }
const char16_t* ShortcutManager::getShortcutText( ShortcutCommand_Edit c ){ return m_cammandNodes_Edit[(u32)c].title.c_str(); }

bool ShortcutManager::isShortcutActive( ShortcutCommand_General c )
{
    return m_cammandNodes_General[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Viewport c )
{
    return m_cammandNodes_Viewport[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Camera c )
{
    return m_cammandNodes_Camera[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Edit c )
{
    return m_cammandNodes_Edit[(u32)c].isReady; 
}

void ShortcutManager::_save()
{
    auto fn = m_app->getSaveFileName(u"Shortcuts", u"xml");
    auto extension = util::stringGetExtension(fn);
    if( extension != u"xml" )
        fn += u".xml";

    auto root = kkCreatePtr(kkCreate<kkXMLNode>());

    if( !root.ptr() )
        throw std::runtime_error(KK_FUNCTION);

    root->name = u"kkrooo_shortcuts";

    
    int command_end = (int)ShortcutCommandCategory::_End;
	for( int i = 0; i < command_end; ++i )
    {
        switch((ShortcutCommandCategory)i)
        {
        case ShortcutCommandCategory::General:  _setUpXMLNode(m_cammandNodes_General,u"general",root.ptr());   break;
        case ShortcutCommandCategory::Viewport: _setUpXMLNode(m_cammandNodes_Viewport,u"viewport",root.ptr()); break;
        case ShortcutCommandCategory::Camera:   _setUpXMLNode(m_cammandNodes_Camera,u"camera",root.ptr());     break;
        case ShortcutCommandCategory::Edit:     _setUpXMLNode(m_cammandNodes_Edit,u"edit",root.ptr());         break;
        case ShortcutCommandCategory::_End:                                                                    break;
        default:                                fprintf(stdout, "Need to implement....\n");                    return;
        }
    }


    kkFileSystem::XMLWrite(fn,root.ptr());
}

void ShortcutManager::_setUpXMLNode(kkArray<ShortcutCommandNode>& command_nodes, const char16_t*  nade_title, kkXMLNode* root)
{
    kkString str;
    for( u32 i = 0, sz = (u32)command_nodes.size(); i < sz; ++i )
    {
        if( command_nodes[ i ].key != kkKey::K_NONE )
        {
            kkXMLNode * node = kkCreate<kkXMLNode>();
            node->name = nade_title;

            str.clear();
            str += i;
            node->addAttribute( u"command_id", str );

            str.clear();

            str += (u32)command_nodes[ i ].keyboardState;
            node->addAttribute( u"keyaboard_state", str );

            str.clear();

            str += (u32)command_nodes[ i ].key;
            node->addAttribute( u"key", str );

            str.clear();

            str += command_nodes[ i ].onRelease;
            node->addAttribute( u"on_release", str );

            node->text = command_nodes[i].title.data();

            root->addNode(node);
        }
    }
}

void ShortcutManager::_readXMLNodes(const std::vector<kkXMLNode*>& xmlnodes, kkArray<ShortcutCommandNode>& command_nodes)
{
    kkString attribute_command_id        = "command_id";
    kkString attribute_keyaboard_state   = "keyaboard_state";
    kkString attribute_key               = "key";
    kkString attribute_on_release        = "on_release";

    int command_index = -1;
	for( u32 i = 0, sz = (u32)xmlnodes.size(); i < sz; ++i )
    {
        for( u32 o = 0, asz = (u32)xmlnodes[i]->attributeList.size(); o < asz; ++o )
        {
            if( xmlnodes[i]->attributeList[o]->name == attribute_command_id )
            {
                command_index = atoi(xmlnodes[i]->attributeList[o]->value.to_kkStringA().data());
                if( command_index < (s32)command_nodes.size() )
                    command_nodes[ command_index ].title = xmlnodes[i]->text;
            }
        }
        for( u32 o = 0, asz = (u32)xmlnodes[i]->attributeList.size(); o < asz; ++o )
        {
            if( xmlnodes[i]->attributeList[o]->name == attribute_keyaboard_state )
            {
                int ks = atoi(xmlnodes[i]->attributeList[o]->value.to_kkStringA().data());
                if( ks > 0 && ks < (int)AppState_keyboard::END )
                {
                    command_nodes[ command_index ].keyboardState = (AppState_keyboard)ks;
                }
            }
            if( xmlnodes[i]->attributeList[o]->name == attribute_key )
            {
                int k = atoi(xmlnodes[i]->attributeList[o]->value.to_kkStringA().data());
                if( k > 0 && k < (int)kkKey::END )
                {
                    command_nodes[ command_index ].key = (kkKey)k;
                }
            }
             if( xmlnodes[i]->attributeList[o]->name == attribute_on_release )
            {
                int k = atoi(xmlnodes[i]->attributeList[o]->value.to_kkStringA().data());
                if( k != 0 )
                    command_nodes[ command_index ].onRelease = 1;
                else
                    command_nodes[ command_index ].onRelease = 0;
            }
        }
    }
}