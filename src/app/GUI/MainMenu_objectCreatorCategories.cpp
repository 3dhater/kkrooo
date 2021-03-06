#include "kkrooo.engine.h"
#include "../Common.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Scene3D/Scene3D.h"

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
                                m_current_scene3D->deselectAll();
                                item.m_callback(item.m_id,item.m_data);
                            }
                        }
                    }
                    if( m_KrGuiSystem->isLastItemCursorHover() || m_KrGuiSystem->isLastItemCursorMove() )
						m_cursorInGUI = true;
		        	m_KrGuiSystem->popupMenuEnd();
                }
            }
            if( m_KrGuiSystem->isLastItemCursorHover() || m_KrGuiSystem->isLastItemCursorMove() )
						m_cursorInGUI = true;
			m_KrGuiSystem->popupMenuEnd();
        }
    }
}

