// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "../Application.h"

#include "PluginGUIWindow.h"

//#include "imgui.h"
#include <sstream>

int PluginGUIWindow::s_windowCounter = 0;

void deformat( const char16_t* fmt,	va_list& args, kkString& message )
{
	u32 len = 0U;
	const char16_t* p = fmt;
	do	{		++len;	} while(*p++);
	--len;

	va_list list = (va_list)args;

	bool S = false;
	for( u32 i(0); i < len; ++i )
    {
		std::wostringstream ss;
		if( S )
        {
			if( fmt[ i ] == u'f' )
            {
				ss << va_arg( list, f64 );
				message += (char16_t*)ss.str().c_str();
				continue;
			}
            else if( fmt[ i ] == u'i' )
            {
				ss << va_arg( list, s32 );
				message += (char16_t*)ss.str().c_str();
				continue;
			}
            else if( fmt[ i ] == u'u' )
            {
				ss << va_arg( list, u32 );
				message += (char16_t*)ss.str().c_str();
				continue;
			}
            else if( fmt[ i ] == u'c' )
            {
				message += va_arg( list, /*char16_t*/int );
				continue;
			}
            else if( fmt[ i ] == u's' )
            {
				char16_t * p2 = va_arg( list, char16_t* );
				u32 len2( 0U );
				do{ ++len2; } while(*p2++);
				p2 -= len2;
				for( u32 o(0U); o < len2-1; ++o )
					message += p2[ o ];
				continue;
			}
		}

		if( fmt[ i ] == u'%' )
        {
			if( !S ) S = true;
			else S = false;
		}
        else S = false;

		if( !S )
			message += fmt[ i ];
	}
}

void PluginGUIWindowElement::SetText( const char16_t* fmt, ... )
{
    m_text.clear();
    va_list args;
    va_start(args, fmt);
    deformat(fmt, args, m_text);
    va_end(args);
}

void PluginGUIWindowElement::SetPointerFloat( float * float_ptr )
{
    m_float_ptr = float_ptr;
}

void PluginGUIWindowElement::SetPointerInt( int * int_ptr )
{
    m_int_ptr = int_ptr;
}

PluginGUIWindow::PluginGUIWindow()
{
    m_app = kkSingleton<Application>::s_instance;
	m_name.append("window_");
	m_name.append(s_windowCounter++);
}

PluginGUIWindow::~PluginGUIWindow()
{
    Reset();
}

void PluginGUIWindow::Activate()
{
    m_isActive = true;
    if( m_type == kkPluginGUIWindowType::Export || m_type == kkPluginGUIWindowType::Import )
    {
        m_app->showImportExportWindow(this);
    }
}

void PluginGUIWindow::Deactivate()
{
    m_isActive = false;
}

void PluginGUIWindow::SetType( kkPluginGUIWindowType type )
{
	m_type = type;
}

void PluginGUIWindow::SetUserData( void * data )
{
    m_userData = data;
}

void PluginGUIWindow::SetOnOK( kkPluginGUICallback f )
{
    m_onOK = f;
}

void PluginGUIWindow::SetOnActivate( kkPluginGUICallback f )
{
    m_onActivate = f;
}

void PluginGUIWindow::callOnActivate()
{
    if( m_onActivate )
        m_onActivate(-1,m_userData);
}

void PluginGUIWindow::Reset()
{
    m_size.set( 300, 200 );
    for( u64 i = 0, sz = m_guiElements.size(); i < sz; ++i )
    {
        kkDestroy(m_guiElements[i]);
    }
    m_guiElements.clear();
}

void PluginGUIWindow::SetName( const char16_t* name )
{
    m_name = name;
}

const char16_t* PluginGUIWindow::GetName()
{
    return m_name.data();
}

void PluginGUIWindow::SetSize( const v2i& s )
{
    m_size = s;
}


kkPluginGUIWindowElement* PluginGUIWindow::AddButton( const char16_t* text, const v2f& size, kkPluginGUICallback f, s32 id, kkPluginGUIParameterType pt )
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::Button;
    e->m_paramType = pt;
    e->m_text = text;
    e->m_callback = f;
    e->m_id = id;
    e->m_size = size;
    m_guiElements.push_back( e );
    return e;
}

kkPluginGUIWindowElement* PluginGUIWindow::AddNewLine( f32 Y_offset, kkPluginGUIParameterType pt)
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::NewLine;
    e->m_offset1 = Y_offset;
    e->m_paramType = pt;
    m_guiElements.push_back( e );
    return e;
}

kkPluginGUIWindowElement* PluginGUIWindow::AddText( const char16_t* text, u32 color_argb, f32 text_Y_offset, kkPluginGUIParameterType pt )
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::Text;
    e->m_text = text;
    e->m_gui_color.makeColor(color_argb);
    e->m_paramType = pt;
    e->m_offset1 = text_Y_offset;
    m_guiElements.push_back( e );
    return e;
}

kkPluginGUIWindowElement* PluginGUIWindow::AddMoveLeftRight( f32 value, kkPluginGUIParameterType pt)
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::MoveLeftRight;
    e->m_paramType = pt;
    e->m_offset1  = value;
    m_guiElements.push_back( e );
    return e;
}

kkPluginGUIWindowElement* PluginGUIWindow::AddRangeSliderFloat( f32 minimum, f32 maximum, f32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt)
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::RangeSlider;
    e->m_paramType = pt;
    e->m_minimum  = minimum;
    e->m_maximum  = maximum;
    e->m_float_ptr  = ptr;
    e->m_size = size;
    e->m_horizontal = horizontal;
    e->m_speed = speed;
    e->m_callback = cb;
    m_guiElements.push_back( e );
    return e;
}

kkPluginGUIWindowElement* PluginGUIWindow::AddRangeSliderInt( s32 minimum, s32 maximum, s32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt)
{
    PluginGUIWindowElement * e = kkCreate<PluginGUIWindowElement>();
    e->m_type     = PluginGUIWindowElementType::RangeSliderInteger;
    e->m_paramType = pt;
    e->m_minimum_int  = minimum;
    e->m_maximum_int  = maximum;
    e->m_int_ptr  = ptr;
    e->m_size = size;
    e->m_horizontal = horizontal;
    e->m_speed = speed;
    e->m_callback = cb;
    m_guiElements.push_back( e );
    return e;
}

void PluginGUIWindow::draw()
{
    if(!m_isActive)
        return;

    if( m_type != kkPluginGUIWindowType::Import && m_type != kkPluginGUIWindowType::Export )
    {
        m_app->m_KrGuiSystem->addText(m_name.data());
        m_app->m_KrGuiSystem->newLine();
    }

    for( auto item : m_guiElements )
    {
        switch (m_app->m_editMode)
        {
        case EditMode::Object:
        default:
            switch(item->m_paramType)
            {
            case kkPluginGUIParameterType::Object: break;
            default: continue;
            }
        break;
        case EditMode::Vertex:
            switch(item->m_paramType)
            {
            case kkPluginGUIParameterType::Vertex: break;
            default: continue;
            }
        break;
        case EditMode::Edge:
            switch(item->m_paramType)
            {
            case kkPluginGUIParameterType::Edge: break;
            default: continue;
            }
        break;
        case EditMode::Polygon:
            switch(item->m_paramType)
            {
            case kkPluginGUIParameterType::Polygon: break;
            default: continue;
            }
        break;
        }

        switch( item->m_type )
        {
            case PluginGUIWindowElementType::Button:
            default:{
                if( m_app->m_KrGuiSystem->addButton(item->m_text.data(), 0, Kr::Gui::Vec2f(item->m_size.x,item->m_size.y), 
                    m_app->m_globalInputBlock ? false : true, true, Kr::Gui::Vec4f(3.f,3.f,3.f,3.f) ) )
                {
                    if(item->m_callback)
                    {
                        item->m_callback(item->m_id, m_userData);
                    }
                }
            }break;
            case PluginGUIWindowElementType::NewLine:
            {
                m_app->m_KrGuiSystem->newLine(item->m_offset1);
            }break;
            case PluginGUIWindowElementType::Text:
            {
                Kr::Gui::Style style;
                style.commonTextColor = item->m_gui_color;
                style.commonTextYOffset = item->m_offset1;
                m_app->m_KrGuiSystem->addText(item->m_text.data(), &style);
            }break;
            case PluginGUIWindowElementType::MoveLeftRight:
            {
                m_app->m_KrGuiSystem->moveLeftRight(item->m_offset1);
            }break;
            case PluginGUIWindowElementType::RangeSlider:
            {
                if(item->m_float_ptr)
                {
                    if( m_app->m_KrGuiSystem->addRangeSlider( item->m_minimum, item->m_maximum, item->m_float_ptr, Kr::Gui::Vec2f(item->m_size.x,item->m_size.y), 
                        item->m_horizontal, item->m_speed, 0, Kr::Gui::Vec4f(3.f, 3.f, 3.f, 3.f) ) )
                    {
                        if(item->m_callback)
                        {
                            item->m_callback(item->m_id, m_userData);
                        }
                    }
                }
            }break;
            case PluginGUIWindowElementType::RangeSliderInteger:
            {
                if(item->m_int_ptr)
                {
                    if( m_app->m_KrGuiSystem->addRangeSliderInt( item->m_minimum_int, item->m_maximum_int, item->m_int_ptr, Kr::Gui::Vec2f(item->m_size.x,item->m_size.y), 
                        item->m_horizontal, item->m_speed, 0, Kr::Gui::Vec4f(3.f, 3.f, 3.f, 3.f) ) )
                    {
                        if(item->m_callback)
                        {
                            item->m_callback(item->m_id, m_userData);
                        }
                    }
                }
            }break;
        }
    }

    //if( is_windowBegin || is_params )
    //{
    //    ImGui::BeginChild("main", ImVec2((float)m_size.x,(float)m_size.y - y_indent), true, 0 );

    //    static bool is_tree_node_begin = false;
    //    static bool is_tree_node       = false;

    //    static bool is_tabbar_begin = false;
    //    static bool is_tabbar       = false;

    //    if( is_params )
    //    {
    //       // ImGui::Text(m_name.data());
    //    }

    //    for( auto item : m_guiElements )
    //    {
    //        switch( item->type )
    //        {
    //        case PluginGUIWindowElementType::_button:
    //        default:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            if( ImGui::Button(item->label) )
    //            {
    //                if(item->callback)
    //                {
    //                    m_app->setNeedToSave(true);
    //                    item->callback(item->id,m_userData);
    //                }
    //                else
    //                {
    //                    fprintf(stderr,"PluginGUIWindow - Button [%s] without callback\n", item->label);
    //                }
    //            }
    //            break;
    //        case PluginGUIWindowElementType::_smallButton:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            if( ImGui::SmallButton(item->label) )
    //            {
    //                if(item->callback)
    //                {
    //                    m_app->setNeedToSave(true);
    //                    item->callback(item->id,m_userData);
    //                }
    //                else
    //                {
    //                    fprintf(stderr,"PluginGUIWindow - Button [%s] without callback\n", item->label);
    //                }
    //            }
    //            break;
    //        case PluginGUIWindowElementType::_arrowButton:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            if( ImGui::ArrowButton(item->label,item->direction) )
    //            {
    //                if(item->callback)
    //                {
    //                    m_app->setNeedToSave(true);
    //                    item->callback(item->id,m_userData);
    //                }
    //                else
    //                {
    //                    fprintf(stderr,"PluginGUIWindow - Button [%s] without callback\n", item->label);
    //                }
    //            }
    //            break;
    //            case PluginGUIWindowElementType::_checkBox:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::Checkbox(item->label,item->check);
    //            break;
    //        /*case PluginGUIWindowElementType::_checkBox_Py:
    //        {
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }

    //            if( ImGui::Checkbox(item->label,&item->py_check) )
    //            {
    //            }
    //            if(ImGui::IsItemClicked())
    //            {
    //                auto obj = PyObject_CallObject(item->py_object, 0);
			 //       if(!obj)
			 //       {
				//        PyErr_Print();
			 //       }
			 //       else
			 //       {
				//        Py_DECREF(obj);
			 //       }
    //            }
    //        }break;*/
    //        case PluginGUIWindowElementType::_dragFloat:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::SetNextItemWidth(item->offset);
    //            ImGui::DragFloat(item->label,item->drag_f,item->drag_speed, item->drag_min, item->drag_max);
    //            if( item->callback )
    //            {
    //                if(ImGui::IsItemEdited())
    //                {
    //                    m_app->setNeedToSave(true);
    //                    item->callback(item->id,m_userData);
    //                }
    //            }
    //            if( ImGui::IsItemHovered() ) m_app->m_enterTextMode = true;
    //            break;
    //        case PluginGUIWindowElementType::_dragInt:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::SetNextItemWidth(item->offset);
    //            ImGui::DragInt(item->label,item->drag_i,item->drag_speed, item->drag_min_i, item->drag_max_i);
    //            if( item->callback )
    //            {
    //                if(ImGui::IsItemEdited())
    //                {
    //                    m_app->setNeedToSave(true);
    //                    item->callback(item->id,m_userData);
    //                }
    //            }
    //            if( ImGui::IsItemHovered() ) m_app->m_enterTextMode = true;
    //            break;
    //        case PluginGUIWindowElementType::_radioButton:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::RadioButton(item->label,item->radio_v,item->v_button);
    //            break;
    //        case PluginGUIWindowElementType::_sameline:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::SameLine(item->offset);
    //            break;
    //        case PluginGUIWindowElementType::_separator:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::Separator();
    //            break;
    //        case PluginGUIWindowElementType::_dummy:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::Dummy(ImVec2(item->size.x, item->size.y));
    //            break;
    //        case PluginGUIWindowElementType::_text:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::Text(item->label);
    //            break;
    //        case PluginGUIWindowElementType::_treeBegin:
    //        {
    //            is_tree_node = true;
    //            if( item->open )
    //            {
    //                ImGui::SetNextTreeNodeOpen(item->open);
    //                item->open = false;
    //            }
    //            if( ImGui::TreeNode(item->label) )
    //            {
    //                is_tree_node_begin = true;
    //            }
    //        }
    //            break;
    //        case PluginGUIWindowElementType::_treeEnd:
    //            is_tree_node = false;
    //            if( is_tree_node_begin )
    //            {
    //                ImGui::TreePop();
    //                is_tree_node_begin = false;
    //            }
    //            break;
    //        case PluginGUIWindowElementType::_progressbar:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::ProgressBar(*item->progress);
    //            break;
    //        case PluginGUIWindowElementType::_childBegin:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::BeginChild( item->label, ImVec2(item->size.x,item->size.y), item->borders, 0 );
    //            break;
    //        case PluginGUIWindowElementType::_childEnd:
    //            if( is_tree_node ) { if( !is_tree_node_begin ) break; }
    //            if( is_tabbar ) { if( !is_tabbar_begin ) break; }
    //            ImGui::EndChild();
    //            break;
    //        case PluginGUIWindowElementType::_tabbarBegin:
    //            ImGui::BeginTabBar(item->label);
    //            break;
    //        case PluginGUIWindowElementType::_tabbarEnd:
    //            ImGui::EndTabBar();
    //            break;
    //        case PluginGUIWindowElementType::_tabbarItemBegin:
    //            is_tabbar = true;
    //            if( ImGui::BeginTabItem(item->label) )
    //            {
    //                is_tabbar_begin = true;
    //            }
    //            break;
    //        case PluginGUIWindowElementType::_tabbarItemEnd:
    //            is_tabbar = false;
    //            if( is_tabbar_begin )
    //            {
    //                is_tabbar_begin = false;
    //                ImGui::EndTabItem();
    //            }
    //            break;
    //        }
    //    }

    //    ImGui::EndChild();

    //    if(!is_params)
    //    {
    //        ImGui::BeginChild("buttons", ImVec2((float)m_size.x - 20.f,35.f), true, 0 );
    //        const char * label = "OK";
    //        if( m_type == kkPluginGUIWindowType::_export ){ label = "Export"; }
    //        else if( m_type == kkPluginGUIWindowType::_import ){ label = "Import"; }
    //        if( ImGui::Button(label) )
    //        {
    //            if(m_onOK)
    //            {
    //                m_onOK(-1, m_userData);
    //                m_isActive = false;
    //            }
    //           /* else if( m_py_onOK )
    //            {
    //                PyObject_CallObject(m_py_onOK, 0);

    //                if (PyErr_Occurred())
				//	    PyErr_Print();

    //                m_isActive = false;
    //            }*/
    //            else
    //            {
    //                fprintf(stderr,"PluginGUIWindow - Button [%s] without callback\n", label);
    //            }
    //        }
    //        ImGui::SameLine((float)m_size.x - 80.f);
    //        if( ImGui::Button("Cancel") )
    //        {
    //            m_isActive = false;
    //        }
    //        ImGui::EndChild();
    //    }
    //}


    //if(!is_params && is_windowBegin)
    //{
    //    ImGui::End();
    //}
}