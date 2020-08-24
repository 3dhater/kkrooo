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
                        setEditMode(EditMode::Object);
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
