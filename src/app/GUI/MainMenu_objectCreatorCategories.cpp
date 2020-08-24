// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"

#include "../Plugins/PluginObjectCategory.h"
#include "../Functions.h"

void Application::_drawMainMenu_ObjectCreatorCategories()
{
    for( auto c : m_objectGeneratorCategories )
    {
		if(m_KrGuiSystem->beginMenu(c->m_name.data(), 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
        {
            for( auto sub_c : c->m_subCategories )
            {
		        if(m_KrGuiSystem->beginMenu(sub_c->m_name.data(), 0, kkrooo::getIconFontChar(IconFontSymbol::NextMenu)))
                {
                    for( auto & item : sub_c->m_items )
                    {
                        if(m_KrGuiSystem->addMenuItem( item.m_name.data(), 0, 0 ))
                        {
                            if( item.m_callback )
                            {
                                _setRightTabMode(RightTabMode::Edit);
                                setEditMode(EditMode::Object);
                                item.m_callback(item.m_id,item.m_data);
                            }
                        }
                    }

		        	m_KrGuiSystem->popupMenuEnd();
                }
            }

			m_KrGuiSystem->popupMenuEnd();
        }
    }
}

