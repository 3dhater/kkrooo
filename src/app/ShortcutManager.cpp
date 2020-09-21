#include "kkrooo.engine.h"
#include "Common.h"

#include <stdexcept>

#include "MainSystem/kkMainSystem.h"

#include "EventConsumer.h"

#include "Application.h"
#include "Functions.h"
#include "ShortcutManager.h"
#include "Common/kkUtil.h"
#include "FileSystem/kkFileSystem.h"

using namespace Kr;

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
    m_commandNames.push_back(u"General");
    m_commandNames.push_back(u"Viewport");
    m_commandNames.push_back(u"Camera");
    m_commandNames.push_back(u"Edit");

	int command_end = (int)ShortcutCommand_General::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_General.push_back(ShortcutCommandNode()); 
        m_cammandDesc_General[i].category = ShortcutCommandCategory::General;

        switch((ShortcutCommand_General)i)
        {
        case ShortcutCommand_General::ShowMaterialEditor: m_cammandDesc_General[i].name = u"ShowMaterialEditor"; break;
        case ShortcutCommand_General::ShowRenderWindow:   m_cammandDesc_General[i].name = u"ShowRenderWindow";   break;
        case ShortcutCommand_General::New:    m_cammandDesc_General[i].name = u"New";    break;
        case ShortcutCommand_General::Save:   m_cammandDesc_General[i].name = u"Save";   break;
        case ShortcutCommand_General::SaveAs: m_cammandDesc_General[i].name = u"SaveAs"; break;
        case ShortcutCommand_General::Open:   m_cammandDesc_General[i].name = u"Open";   break;
        case ShortcutCommand_General::_End:                                             break;
        default:                              fprintf(stdout, "Need to implement....\n");      return false;
        }
    }

    command_end = (int)ShortcutCommand_Viewport::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Viewport.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Viewport[i].category = ShortcutCommandCategory::Viewport;

        switch((ShortcutCommand_Viewport)i)
        {
        case ShortcutCommand_Viewport::SetPerspective:  m_cammandDesc_Viewport[i].name = u"SetPerspective";  break;
        case ShortcutCommand_Viewport::SetFront:        m_cammandDesc_Viewport[i].name = u"SetFront";        break;
        case ShortcutCommand_Viewport::SetBack:         m_cammandDesc_Viewport[i].name = u"SetBack";         break;
        case ShortcutCommand_Viewport::SetTop:          m_cammandDesc_Viewport[i].name = u"SetTop";          break;
        case ShortcutCommand_Viewport::SetBottom:       m_cammandDesc_Viewport[i].name = u"SetBottom";       break;
        case ShortcutCommand_Viewport::SetLeft:         m_cammandDesc_Viewport[i].name = u"SetLeft";         break;
        case ShortcutCommand_Viewport::SetRight:        m_cammandDesc_Viewport[i].name = u"SetRight";        break;
        case ShortcutCommand_Viewport::ToggleGrid:      m_cammandDesc_Viewport[i].name = u"ToggleGrid";      break;
        case ShortcutCommand_Viewport::DrawModeLines:   m_cammandDesc_Viewport[i].name = u"DrawModeLines";   break;
        case ShortcutCommand_Viewport::DrawModeMaterial:m_cammandDesc_Viewport[i].name = u"DrawModeMaterial";break;
        case ShortcutCommand_Viewport::DrawModeMaterialAndLines:m_cammandDesc_Viewport[i].name = u"DrawModeMaterialAndLines";   break;
        case ShortcutCommand_Viewport::ToggleDrawModeLines:m_cammandDesc_Viewport[i].name = u"ToggleDrawModeLines";   break;
        case ShortcutCommand_Viewport::ToggleDrawModeMaterial:m_cammandDesc_Viewport[i].name = u"ToggleDrawModeMaterial";   break;
        case ShortcutCommand_Viewport::_End:                                                                                break;
        default:                                                fprintf(stdout, "Need to implement....\n");                 return false;
        }
    }

    command_end = (int)ShortcutCommand_Camera::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Camera.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Camera[i].category = ShortcutCommandCategory::Camera;

        switch((ShortcutCommand_Camera)i)
        {
        case ShortcutCommand_Camera::Reset:             m_cammandDesc_Camera[i].name = u"Reset";            break;
        case ShortcutCommand_Camera::ToSelection:       m_cammandDesc_Camera[i].name = u"ToSelection";      break;
        case ShortcutCommand_Camera::_End:                                                                             break;
        default:                                              fprintf(stdout, "Need to implement....\n");              return false;
        }
    }

    command_end = (int)ShortcutCommand_Edit::_End;
	for( int i = 0; i < command_end; ++i )
    {
        m_cammandNodes_Edit.push_back(ShortcutCommandNode()); 
        m_cammandDesc_Edit[i].category = ShortcutCommandCategory::Edit;

        switch((ShortcutCommand_Edit)i)
        {
        case ShortcutCommand_Edit::DeselectAll:             m_cammandDesc_Edit[i].name = u"DeselectAll";             break;
        case ShortcutCommand_Edit::Redo:                    m_cammandDesc_Edit[i].name = u"Redo";                    break;
        case ShortcutCommand_Edit::SelectAll:               m_cammandDesc_Edit[i].name = u"SelectAll";               break;
        case ShortcutCommand_Edit::SelectInvert:            m_cammandDesc_Edit[i].name = u"SelectInvert";            break;
        case ShortcutCommand_Edit::Undo:                    m_cammandDesc_Edit[i].name = u"Undo";                    break;
        case ShortcutCommand_Edit::SelectModeJustSelect:    m_cammandDesc_Edit[i].name = u"SelectModeJustSelect";    break;
        case ShortcutCommand_Edit::SelectModeMove:          m_cammandDesc_Edit[i].name = u"SelectModeMove";          break;
        case ShortcutCommand_Edit::SelectModeRotate:        m_cammandDesc_Edit[i].name = u"SelectModeRotate";        break;
        case ShortcutCommand_Edit::SelectModeScale:         m_cammandDesc_Edit[i].name = u"SelectModeScale";         break;
        case ShortcutCommand_Edit::EnterTransformation:     m_cammandDesc_Edit[i].name = u"EnterTransformation";     break;
        case ShortcutCommand_Edit::EditModeObject:          m_cammandDesc_Edit[i].name = u"EditModeObject";          break;
        case ShortcutCommand_Edit::EditModeVertex:          m_cammandDesc_Edit[i].name = u"EditModeVertex";          break;
        case ShortcutCommand_Edit::EditModeEdge:            m_cammandDesc_Edit[i].name = u"EditModeEdge";            break;
        case ShortcutCommand_Edit::EditModePolygon:         m_cammandDesc_Edit[i].name = u"EditModePolygon";         break;
        case ShortcutCommand_Edit::_End:                                                                            break;
        default:                                            fprintf(stdout, "Need to implement....\n");             return false;
        }
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

const char16_t* ShortcutManager::_getKeyboardStateString( AppState_keyboard state )
{
    switch (state)
    {
    case AppState_keyboard::Ctrl: return u"Ctrl";
    case AppState_keyboard::Alt: return u"Alt";
    case AppState_keyboard::Shift: return u"Shift";
    case AppState_keyboard::ShiftAlt: return u"Shift+Alt";
    case AppState_keyboard::ShiftCtrl: return u"Shift+Ctrl";
    case AppState_keyboard::ShiftCtrlAlt: return u"Shift+Ctrl+Alt";
    case AppState_keyboard::CtrlAlt: return u"Ctrl + Alt";
        break;
    default:
        break;
    }
    return nullptr;
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

void ShortcutManager::draw()
{
    m_combinationStr_new.clear();
    m_combinationStr.clear();
    static ShortcutCommand_General active_command_General  = ShortcutCommand_General::ShowMaterialEditor;
    static ShortcutCommand_Viewport active_command_Viewport = ShortcutCommand_Viewport::SetPerspective;
    static ShortcutCommand_Camera active_command_Camera = ShortcutCommand_Camera::Reset;
    static ShortcutCommand_Edit active_command_Edit = ShortcutCommand_Edit::Undo;
    static ShortcutCommandCategory active_cat = ShortcutCommandCategory::General;
    static u32 active_command_u32 = 0;
    static Gui::Style active_cat_style;
    active_cat_style.buttonIdleColor2 = active_cat_style.buttonIdleColor1;

    auto gui = (Kr::Gui::GuiSystem*)kkGetGUI();
    gui->setDrawPosition(0.f,0.f);
    Gui::Style group_style;
    group_style.groupColor2 = group_style.groupColor1;
    group_style.groupHoverColor1 = group_style.groupColor1;
    group_style.groupHoverColor2 = group_style.groupColor1;
    f32 group_height = 180.f;
    if( gui->beginGroup(Gui::Vec2f(80.f,group_height), nullptr, &group_style) )
    {
        for(u32 i = 0; i < (u32)ShortcutCommandCategory::_End; ++i)
        {
            if(gui->addButton(m_commandNames[i].c_str(), active_cat == (ShortcutCommandCategory)i ? &active_cat_style : nullptr, Gui::Vec2f(80.f,20.f) ))
            {
                active_cat = (ShortcutCommandCategory)i;
                active_command_u32 = 0;
            }
        }
        gui->endGroup();
    }
    gui->setDrawPosition(80.f,0.f);
    gui->setScrollMultipler(32.f);
    gui->setCurrentFont(nullptr);
    if( gui->beginGroup(Gui::Vec2f(320.f,group_height), nullptr, &group_style) )
    {
        switch (active_cat)
        {
        case ShortcutCommandCategory::General:
        {
            for(u32 i = 0; i < (u32)ShortcutCommand_General::_End; ++i)
            {
                if(gui->addButton(m_cammandDesc_General[i].name, active_command_General == (ShortcutCommand_General)i ? &active_cat_style : nullptr, Gui::Vec2f(320.f,18.f) ))
                {
                    active_command_General = (ShortcutCommand_General)i;
                    active_command_u32 = i;
                }
            }
        }break;
        case ShortcutCommandCategory::Viewport:
        {
            for(u32 i = 0; i < (u32)ShortcutCommand_Viewport::_End; ++i)
            {
                if(gui->addButton(m_cammandDesc_Viewport[i].name, active_command_Viewport == (ShortcutCommand_Viewport)i ? &active_cat_style : nullptr, Gui::Vec2f(320.f,18.f) ))
                {
                    active_command_Viewport = (ShortcutCommand_Viewport)i;
                    active_command_u32 = i;
                }
            }
        }break;
        case ShortcutCommandCategory::Camera:
        {
            for(u32 i = 0; i < (u32)ShortcutCommand_Camera::_End; ++i)
            {
                if(gui->addButton(m_cammandDesc_Camera[i].name, active_command_Camera == (ShortcutCommand_Camera)i ? &active_cat_style : nullptr, Gui::Vec2f(320.f,18.f) ))
                {
                    active_command_Camera = (ShortcutCommand_Camera)i;
                    active_command_u32 = i;
                }
            }
        }break;
        case ShortcutCommandCategory::Edit:
        {
            for(u32 i = 0; i < (u32)ShortcutCommand_Edit::_End; ++i)
            {
                if(gui->addButton(m_cammandDesc_Edit[i].name, active_command_Edit == (ShortcutCommand_Edit)i ? &active_cat_style : nullptr, Gui::Vec2f(320.f,18.f) ))
                {
                    active_command_Edit = (ShortcutCommand_Edit)i;
                    active_command_u32 = i;
                }
            }
        }break;
        case ShortcutCommandCategory::_End:
        {
        }break;
        default:
            break;
        }
        gui->endGroup();
    }
    gui->setDrawPosition(0.f,group_height + 5.f);
    auto node = _getNode((u32)active_cat, active_command_u32);
    if(!node)
        return;
    
    auto key_state_str = _getKeyboardStateString( node->keyboardState );
    if(key_state_str)
        m_combinationStr += key_state_str;

    auto key_str = _getKeyString( node->key );
    if(key_str)
    {
        if(key_state_str)
        {
            m_combinationStr += u"+";
        }
        m_combinationStr += key_str;
    }
    auto window_w = 400.f;
    auto old_position = gui->getDrawPosition();
    gui->addText(nullptr, u"Combination: %s", m_combinationStr.c_str());
    gui->setDrawPosition(window_w - 110.f, old_position.y);
    if( gui->addButton(u"Clear", nullptr, Gui::Vec2f(100.f,18.f), true, true, Gui::Vec4f(5.f,5.f,5.f,5.f)) )
    {
        node->key = kkKey::K_NONE;
        node->keyboardState = AppState_keyboard::None;
        node->title.clear();
    }
    gui->newLine(10.f);
    old_position = gui->getDrawPosition();

    auto new_comb_appKey_state = m_app->getStateKeyboard();
    key_state_str = _getKeyboardStateString( new_comb_appKey_state );
    if(key_state_str)
    {
        m_combinationStr_new += key_state_str;
    }
    key_str = nullptr;
    kkKey new_key = kkKey::K_NONE;
    for(u32 i = 0; i < (u32)kkKey::END; ++i)
    {
        if( m_eventConsumer->isKeyDown((kkKey)i) )
        {
            key_str = _getKeyString((kkKey)i);
            if(key_str)
            {
                new_key = (kkKey)i;
                if(key_state_str)
                {
                    m_combinationStr_new += u"+";
                }
                m_combinationStr_new += key_str;
                break;
            }
        }
    }
    gui->addText(nullptr, u"New combination: %s", m_combinationStr_new.c_str());
    gui->setDrawPosition(window_w - 110.f, old_position.y);
    if( gui->addButton(u"Assign", nullptr, Gui::Vec2f(100.f,18.f), true, true, Gui::Vec4f(5.f,5.f,5.f,5.f)) )
    {
        if(new_key != kkKey::K_NONE)
        {
            node->key = new_key;
            node->keyboardState = new_comb_appKey_state;
            node->title = m_combinationStr_new;
        }
    }
    f32 window_h = 280.f;
    gui->setDrawPosition(window_w - 38.f, window_h-15.f);
    if( gui->addButton(u"Reset", nullptr, Gui::Vec2f(38.f,13.f), true, true, Gui::Vec4f(5.f,5.f,5.f,5.f)) )
    {
        reset();
    }
    gui->setDrawPosition(0.f, window_h-15.f);
    if( gui->addButton(u"Save", nullptr, Gui::Vec2f(38.f,13.f), true, true, Gui::Vec4f(5.f,5.f,5.f,5.f)) )
    {
        this->_save();
    }
    gui->moveLeftRight(10.f);
    if( gui->addButton(u"Load", nullptr, Gui::Vec2f(38.f,13.f), true, true, Gui::Vec4f(5.f,5.f,5.f,5.f)) )
    {
        this->_load();
    }
}

void ShortcutManager::reset()
{
    for(u64 i = 0, sz = m_cammandNodes_General.size(); i < sz; ++i)
    {
        auto & node = m_cammandNodes_General[i];
        switch((ShortcutCommand_General)i)
        {
        case ShortcutCommand_General::ShowMaterialEditor:{
            node.key = kkKey::K_M;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::ShowRenderWindow:{
            node.key = kkKey::K_F10;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::New:{
            node.key = kkKey::K_N;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::Save:{
            node.key = kkKey::K_S;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::SaveAs:{
            node.key = kkKey::K_N;
            node.keyboardState = AppState_keyboard::ShiftCtrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::Open:{
            node.key = kkKey::K_O;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_General::_End:break;
        default:fprintf(stdout, "Need to implement....\n");
        }
    }
    for(u64 i = 0, sz = m_cammandNodes_Camera.size(); i < sz; ++i)
    {
        auto & node = m_cammandNodes_Camera[i];
        switch((ShortcutCommand_Camera)i)
        {
        case ShortcutCommand_Camera::Reset:{
        }break;
        case ShortcutCommand_Camera::ToSelection:{
            node.key = kkKey::K_Z;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Camera::_End:break;
        default:fprintf(stdout, "Need to implement....\n");
        }
    }
    for(u64 i = 0, sz = m_cammandNodes_Edit.size(); i < sz; ++i)
    {
        auto & node = m_cammandNodes_Edit[i];
        switch((ShortcutCommand_Edit)i)
        {
        case ShortcutCommand_Edit::DeselectAll:{
            node.key = kkKey::K_D;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::EditModeEdge:{
            node.key = kkKey::K_2;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::EditModeObject:{
        }break;
        case ShortcutCommand_Edit::EditModePolygon:{
            node.key = kkKey::K_3;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::EditModeVertex:{
            node.key = kkKey::K_1;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::Redo:{
            node.key = kkKey::K_Y;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::Undo:{
            node.key = kkKey::K_Z;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectAll:{
            node.key = kkKey::K_A;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectInvert:{
            node.key = kkKey::K_I;
            node.keyboardState = AppState_keyboard::Ctrl;
            node.title = _getKeyboardStateString(node.keyboardState);
            node.title += u"+";
            node.title += _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectModeJustSelect:{
            node.key = kkKey::K_Q;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectModeMove:{
            node.key = kkKey::K_W;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectModeScale:{
            node.key = kkKey::K_E;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::SelectModeRotate:{
            node.key = kkKey::K_R;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Edit::_End:break;
        default:fprintf(stdout, "Need to implement....\n");
        }
    }
    for(u64 i = 0, sz = m_cammandNodes_Viewport.size(); i < sz; ++i)
    {
        auto & node = m_cammandNodes_Viewport[i];
        switch((ShortcutCommand_Viewport)i)
        {
        case ShortcutCommand_Viewport::DrawModeLines:break;
        case ShortcutCommand_Viewport::DrawModeMaterial:break;
        case ShortcutCommand_Viewport::DrawModeMaterialAndLines:break;
        case ShortcutCommand_Viewport::SetBack:{
            node.key = kkKey::K_B;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::SetBottom:{
        }break;
        case ShortcutCommand_Viewport::SetFront:{
            node.key = kkKey::K_F;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::SetLeft:{
            node.key = kkKey::K_L;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::SetPerspective:{
            node.key = kkKey::K_P;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::SetRight:{
        }break;
        case ShortcutCommand_Viewport::SetTop:{
            node.key = kkKey::K_T;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::ToggleDrawModeLines:{
            node.key = kkKey::K_F2;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::ToggleDrawModeMaterial:{
            node.key = kkKey::K_F3;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::ToggleGrid:{
            node.key = kkKey::K_G;
            node.title = _getKeyString(node.key);
        }break;
        case ShortcutCommand_Viewport::_End:break;
        default:fprintf(stdout, "Need to implement....\n");
        }
    }
}

ShortcutCommandNode * ShortcutManager::_getNode(u32 i1, u32 i2)
{
    switch(i1)
    {
    case 0: if( i2 < m_cammandNodes_General.size() ) return &m_cammandNodes_General[i2];
    case 1: if( i2 < m_cammandNodes_Viewport.size() ) return &m_cammandNodes_Viewport[i2];
    case 2: if( i2 < m_cammandNodes_Camera.size() ) return &m_cammandNodes_Camera[i2];
    case 3: if( i2 < m_cammandNodes_Edit.size() ) return &m_cammandNodes_Edit[i2];
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
    return m_disabled ? false : m_cammandNodes_General[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Viewport c )
{
    return m_disabled ? false : m_cammandNodes_Viewport[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Camera c )
{
    return m_disabled ? false : m_cammandNodes_Camera[(u32)c].isReady; 
}
bool ShortcutManager::isShortcutActive( ShortcutCommand_Edit c )
{
    return m_disabled ? false : m_cammandNodes_Edit[(u32)c].isReady; 
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

void ShortcutManager::_readXMLNodes(const kkArray<kkXMLNode*>& xmlnodes, kkArray<ShortcutCommandNode>& command_nodes)
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