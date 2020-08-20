// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

//#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Functions.h" 

#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"
#include "../Plugins/PluginGUIWindow.h"
#include "../Plugins/PluginCommonInterface.h"
#include "../Plugins/PluginObjectCategory.h"

//#include "imgui.h"
using namespace Kr;

void Application::_callOnActivateGUIPlugin()
{
    //if( m_current_scene3D->getNumOfSelectedObjects() == 1 )
    //{
    //    auto object = (Scene3DObject *)m_current_scene3D->getSelectedObject(0);
    //    if( object->m_paramsWindow )
    //    {
    //        object->m_paramsWindow->callOnActivate();
    //    }
    //}
}

void Application::_drawRightToolBar_editTab(float x, float y)
{
    if( m_current_scene3D->getNumOfSelectedObjects() == 1 )
    {
        auto object = (Scene3DObject *)m_current_scene3D->getSelectedObject(0);
        if( object->m_paramsWindow )
        {
            object->m_paramsWindow->callOnActivate();
            object->m_paramsWindow->draw();
        }
    }
}

void Application::_drawRightToolBar_createTab(float x, float y)
{
    //m_KrGuiSystem->addText(u"Standart objects");
    //m_KrGuiSystem->newLine();
    auto textFont = m_KrGuiSystem->getCurrentFont();
    f32 button_w = (200 - 10 - 10 - 10) * 0.5f;
    
    Gui::Style expandCollapseButtonStyle;
    expandCollapseButtonStyle.buttonBackgroundAlpha = 0.f;
    expandCollapseButtonStyle.buttonTextIdleColor = Gui::ColorWhite;

    for( auto cat : m_objectGeneratorCategories )
    {
        
        m_KrGuiSystem->setCurrentFont(m_iconsFont);
        if(cat->m_expanded)
        {
            if( m_KrGuiSystem->addButtonSymbol(kkrooo::getIconFontChar(IconFontSymbol::CollapseCategory), &expandCollapseButtonStyle, Gui::Vec2f(10.f,10.f)) )
            {
                cat->m_expanded = false;
            }
        }
        else
        {
            if( m_KrGuiSystem->addButtonSymbol(kkrooo::getIconFontChar(IconFontSymbol::ExpandCategory), &expandCollapseButtonStyle, Gui::Vec2f(10.f,10.f)) )
            {
                cat->m_expanded = true;
            }
        }
        m_KrGuiSystem->setCurrentFont(textFont);
        m_KrGuiSystem->addText(cat->m_name.data());

        m_KrGuiSystem->newLine();

        if( !cat->m_expanded )
            continue;

        for( auto subcat : cat->m_subCategories )
        {
            m_KrGuiSystem->moveLeftRight(10.f);
            m_KrGuiSystem->setCurrentFont(m_iconsFont);
            if(subcat->m_expanded)
            {
                if( m_KrGuiSystem->addButtonSymbol(kkrooo::getIconFontChar(IconFontSymbol::CollapseCategory), &expandCollapseButtonStyle, Gui::Vec2f(10.f,10.f)) )
                {
                    subcat->m_expanded = false;
                }
            }
            else
            {
                if( m_KrGuiSystem->addButtonSymbol(kkrooo::getIconFontChar(IconFontSymbol::ExpandCategory), &expandCollapseButtonStyle, Gui::Vec2f(10.f,10.f)) )
                {
                    subcat->m_expanded = true;
                }
            }
            m_KrGuiSystem->setCurrentFont(textFont);
            m_KrGuiSystem->addText(subcat->m_name.data());
            m_KrGuiSystem->newLine();

            if( !subcat->m_expanded )
                continue;

            //for( auto & item : subcat->m_items )
            for( u64 i = 0, sz = subcat->m_items.size(); i < sz;  )
            {
                m_KrGuiSystem->moveLeftRight(10.f);
                auto & item = subcat->m_items[ i ];
                if( m_KrGuiSystem->addButton(item.m_name.data(), 0, Gui::Vec2f(button_w, 15), m_globalInputBlock ? false : true, true, Gui::Vec4f(3.f, 3.f, 3.f, 3.f) ) )
                {
                    if( item.m_callback )
                    {
                        _setRightTabMode(RightTabMode::Edit);
                        _setEditMode(EditMode::Object);
                        item.m_callback(item.m_id,item.m_data);
                    }
                }
                ++i;
                if(i==sz) 
                    break;
               /* m_KrGuiSystem->moveLeftRight(10.f);
                item = subcat->m_items[ i ];
                if( m_KrGuiSystem->addButton(item.m_name.data(), 0, Gui::Vec2f(button_w, 20)) )
                {
                    if( item.m_callback )
                    {
                        _setRightTabMode(RightTabMode::Edit);
                        _setEditMode(EditMode::Object);
                        item.m_callback(item.m_id,item.m_data);
                    }
                }
                ++i;
                m_KrGuiSystem->newLine(10.f);*/
            }
            m_KrGuiSystem->newLine();
        }
        m_KrGuiSystem->newLine();
    }
}

//void Application::_drawImguiRightPart_drawParams()
//{
//    if( m_editMode != EditMode::Object )
//        return;
//
//    if( m_current_scene3D->getNumOfSelectedObjects() == 1 )
//    {
//        auto object = (Scene3DObject *)m_current_scene3D->getSelectedObject(0);
//        if( object->m_paramsWindow )
//        {
//            object->m_paramsWindow->callOnActivate();
//            object->m_paramsWindow->activate();
//            object->m_paramsWindow->draw();
//        }
//    }
//}

void Application::_drawRightToolBar()
{
	auto oldFont = m_KrGuiSystem->getCurrentFont();
	m_KrGuiSystem->setCurrentFont(m_iconsFont);

    float X = m_window_client_size.x-20;
    m_KrGuiSystem->setDrawPointPosition(X, m_mainMenuHeight + m_mainToolBarHeight);
	auto oldDrawPosition = m_KrGuiSystem->getDrawPointPosition();

    if( m_rightTabMode == RightTabMode::Create )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	    m_KrGuiSystem->setDrawPointPosition(oldDrawPosition.x, oldDrawPosition.y);
	}
    if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::NewObject), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true))
    {
        _setRightTabMode(RightTabMode::Create);
    }
	m_KrGuiSystem->newLine();
    m_KrGuiSystem->moveLeftRight(X);

	oldDrawPosition = m_KrGuiSystem->getDrawPointPosition();
    if( m_rightTabMode == RightTabMode::Edit )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	    m_KrGuiSystem->setDrawPointPosition(oldDrawPosition.x, oldDrawPosition.y);
	}
    if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::EditObject), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true))
    {
        _setRightTabMode(RightTabMode::Edit);
    }
    m_KrGuiSystem->newLine();
    m_KrGuiSystem->moveLeftRight(X);

	oldDrawPosition = m_KrGuiSystem->getDrawPointPosition();
    if( m_rightTabMode == RightTabMode::UVEdit )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	    m_KrGuiSystem->setDrawPointPosition(oldDrawPosition.x, oldDrawPosition.y);
	}
    if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::EditUV), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true))
    {
        _setRightTabMode(RightTabMode::UVEdit);
    }
	m_KrGuiSystem->setCurrentFont(oldFont);

    auto Y = m_mainMenuHeight + m_mainToolBarHeight;
    if(m_showRightTab)
    {
        const f32 W = 200.f;
        m_KrGuiSystem->setDrawPointPosition(X - W, Y);
        Gui::Style style;
        style.rectangleIdleColor1  = 0xFF555555;
        style.rectangleIdleColor2  = style.rectangleIdleColor1;
        style.rectangleHoverColor1 = 0xFF5f5F5F;
        style.rectangleHoverColor2 = style.rectangleHoverColor1;
        auto tabSize = Gui::Vec2f(W, m_window_client_size.y - m_bottomAreaHeight - Y);
        m_KrGuiSystem->addRectangle(&style, tabSize, 0.8f, Gui::Vec4f(5.f, 5.f, 0.f, 0.f));


        m_KrGuiSystem->setDrawPointPosition(X - W, Y);
        Gui::Style groupStyle;
        //groupStyle.groupColor1 = 0xffff0000;
        groupStyle.groupBackgroundAlpha = 0.f;
        groupStyle.groupHoverColor1 = 0x00000000;
        groupStyle.groupHoverColor2 = 0x00000000;
        m_KrGuiSystem->setScrollMultipler(30.f);
        if( m_KrGuiSystem->beginGroup(tabSize, 0, &groupStyle) )
        {
            if( m_KrGuiSystem->isLastItemCursorHover() || m_KrGuiSystem->isLastItemCursorMove() )
            {
                m_cursorInGUI = true;
            }

            switch (m_rightTabMode)
            {
            case RightTabMode::Create:
            default:
                _drawRightToolBar_createTab(X, Y);
                break;
            case RightTabMode::Edit:
                _drawRightToolBar_editTab(X, Y);
                break;
            case RightTabMode::UVEdit:
                break;
            }
            m_KrGuiSystem->endGroup();
        }
    }
}

//void Application::_drawImguiRightPart()
//{
//    auto old_round = m_imguiStyle->WindowRounding;
//    auto old_itemSpacing = m_imguiStyle->ItemSpacing;
//    auto old_windowPadding = m_imguiStyle->WindowPadding;
//
//    m_imguiStyle->WindowRounding = 0.f;
//    
//    m_imguiStyle->ItemSpacing = ImVec2(1,1);
//    m_imguiStyle->WindowPadding= ImVec2(1,1);
//
//    m_rightPartSize.x = 200.f;
//    m_rightPartSize.y = (float)m_window_client_size.y-110.f;
//
//    ImGui::SetNextWindowPos(ImVec2( (float)m_window_client_size.x - 200.f, 50.f ));
//    ImGui::SetNextWindowSize(ImVec2(m_rightPartSize.x,m_rightPartSize.y));
//    ImGui::SetNextWindowBgAlpha(0.0f);
//
//    const float button_size = 30.f;
//
//    this->setImguiButtonStyle(_imgui_buttonStyle_default);
//
//    if( ImGui::Begin("rightpart",0,ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus
//    | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration))
//    {
//
//        if( ImGui::Button("Cr", ImVec2(button_size,button_size)) )
//        {
//            _setRightTabMode(RightTabMode::Create);
//        }
//        ImGui::SameLine();
//        if( ImGui::Button("Ed", ImVec2(button_size,button_size)) )
//        {
//            _setRightTabMode(RightTabMode::Edit);
//        }
//        ImGui::SameLine();
//        if( ImGui::Button("UV", ImVec2(button_size,button_size)) )
//        {
//            _setRightTabMode(RightTabMode::UVEdit);
//        }
//        ImGui::SameLine();
//        if( ImGui::Button(".", ImVec2(button_size,button_size)) )
//        {
//        }
//        ImGui::SameLine();
//        if( ImGui::Button("..", ImVec2(button_size,button_size)) )
//        {
//        }
//        ImGui::SameLine();
//        if( ImGui::Button("...", ImVec2(button_size,button_size)) )
//        {
//        }
//        
//        switch(m_rightTabMode)
//        {
//        default:
//        case RightTabMode::Create:
//        {
//            ImGui::Dummy(ImVec2());
//            ImGui::SameLine(20.f);
//            ImGui::Text("Create new object");
//
//            if( m_objectGeneratorCategories.size() )
//            {
//                
//                static u64 selected_subcategory_index = 0;
//
//                ImGui::Separator();
//                ImGui::SetNextItemWidth(200);
//                if( ImGui::BeginCombo(".createObject1", m_objectGeneratorCategories[m_selectedCategoryIndex]->m_nameA.data(), 0 ) )
//                {
//                    for( u64 i = 0, sz = m_objectGeneratorCategories.size(); i < sz; ++i )
//                    {
//                        bool is_selected = i == m_selectedCategoryIndex;
//                        if( ImGui::Selectable( m_objectGeneratorCategories[i]->m_nameA.data(), is_selected))
//                        {
//                            selected_subcategory_index = 0;
//                            m_selectedCategoryIndex = i;
//                        }
//                        if(is_selected)
//                            ImGui::SetItemDefaultFocus();
//                    }
//                    ImGui::EndCombo();
//                }
//
//                ImGui::Separator();
//                ImGui::SetNextItemWidth(200);
//                if( ImGui::BeginCombo(".createObject2", 
//                    m_objectGeneratorCategories[m_selectedCategoryIndex]->
//                    m_subCategories[selected_subcategory_index]->m_nameA.data(), 0 ) )
//                {
//                    for( u64 i = 0, sz = m_objectGeneratorCategories[m_selectedCategoryIndex]->m_subCategories.size(); i < sz; ++i )
//                    {
//                        bool is_selected = i == selected_subcategory_index;
//                        if( ImGui::Selectable( m_objectGeneratorCategories[m_selectedCategoryIndex]->m_subCategories[i]->m_nameA.data(), is_selected))
//                        {
//                            selected_subcategory_index = i;
//                        }
//                        if(is_selected)
//                            ImGui::SetItemDefaultFocus();
//                    }
//                    ImGui::EndCombo();
//                }
//                ImGui::Dummy(ImVec2(1,2));
//                ImGui::Separator();
//
//                for( u64 i = 0, sz = m_objectGeneratorCategories[m_selectedCategoryIndex]->m_subCategories[selected_subcategory_index]->m_items.size();
//                    i < sz;  )
//                {
//                    ImGui::Dummy(ImVec2());
//
//                    ImGui::SameLine(4.f);
//                    auto item = m_objectGeneratorCategories[m_selectedCategoryIndex]->m_subCategories[selected_subcategory_index]->m_items[ i ];
//                    if( ImGui::Button(item.m_nameA.data(), ImVec2(100,20)) )
//                    {
//                        if( item.m_callback )
//                        {
//                            _setRightTabMode(RightTabMode::Edit);
//                            _setEditMode(EditMode::Object);
//                            item.m_callback(item.m_id,item.m_data);
//                        }
//                    }
//                    ++i;
//                    if( i == sz )
//                        break;
//                    ImGui::SameLine();
//                    item = m_objectGeneratorCategories[m_selectedCategoryIndex]->m_subCategories[selected_subcategory_index]->m_items[ i ];
//                    if( ImGui::Button(item.m_nameA.data(), ImVec2(100,20)) )
//                    {
//                        if( item.m_callback )
//                        {
//                            _setRightTabMode(RightTabMode::Edit);
//                            _setEditMode(EditMode::Object);
//                            item.m_callback(item.m_id,item.m_data);
//                        }
//                    }
//                    ++i;
//                }
//                ImGui::Separator();
//            }
//
//            //_drawImguiRightPart_drawParams();
//        }break;
//        case RightTabMode::Edit:
//        {
//            ImGui::Dummy(ImVec2());
//            ImGui::SameLine(20.f);
//            ImGui::Text("Edit object");
//            ImGui::Separator();
//            _drawImguiRightPart_drawParams();
//        }break;
//        case RightTabMode::UVEdit:
//        {
//            ImGui::Dummy(ImVec2());
//            ImGui::SameLine(20.f);
//            ImGui::Text("Edit UV");
//        }break;
//        }
//
//
//        ImGui::End();
//    }
//
//
//
//    m_imguiStyle->WindowPadding= old_windowPadding;
//    m_imguiStyle->ItemSpacing = old_itemSpacing;
//    m_imguiStyle->WindowRounding = old_round;
//}