// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"


#include "../Application.h"

#include  "../Functions.h"

#include "PluginGUI.h"
#include "PluginGUIWindow.h"

PluginGUI::PluginGUI(Application * a)
    :
    m_app(a)
{
}

PluginGUI::~PluginGUI()
{
    for( u64 i = 0, sz = m_windows.size(); i < sz; ++i )
    {
        kkDestroy(m_windows[i]);
    }
}

kkPluginGUIWindow* PluginGUI::createWindow()
{
    PluginGUIWindow * window = kkCreate<PluginGUIWindow>();
    window->Reset();

    m_windows.push_back(window);
    return window;
}





void PluginGUI::drawNonChildWindow(u32& inFocus, v2i& cp)
{
    for( u64 i = 0, sz = m_windows.size(); i < sz; ++i )
    {
        auto w = m_windows[i];
        
        if( !w->m_isActive )
            continue;

        switch(w->m_type)
        {
        case kkPluginGUIWindowType::Custom:
        case kkPluginGUIWindowType::Export:
        case kkPluginGUIWindowType::Import:
        {
            if( kkrooo::pointInRect( cp, v4f((float)w->m_position.x,(float)w->m_position.y,(float)w->m_position.x+(float)w->m_size.x,(float)w->m_position.y+(float)w->m_size.y) ) )
            {
                inFocus = 1;
            }
            w->draw();
        }
            break;
        default:
            break;
        }
    }
}