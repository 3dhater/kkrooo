// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "Scene3D/Scene3D.h"
#include "Scene3D/Scene3DObject.h"
#include "Common/kkInfoSharedLibrary.h"
#include "Plugins/Plugin.h"
#include "Plugins/PluginGUI.h"


#include "Application.h"

struct EditPolyObjectsGUIElements
{
    kkPluginGUIWindowElement * m_object_position_x_element = nullptr;
    f32 * m_position_x = nullptr;
}g_EditPolyObjectsGUIElements;

void change_position_x_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    if( scene->getNumOfSelectedObjects() == 1 )
    {
        auto object = (Scene3DObject *)scene->getSelectedObject(0);
        if(object->GetType() == kkScene3DObjectType::PolygonObject)
        {
            auto pivot = object->GetPivot();

            object->setPosition( kkVector4(*g_EditPolyObjectsGUIElements.m_position_x,pivot._f32[1],pivot._f32[2],pivot._f32[3]) );
            object->ApplyPivot();
            object->UpdateAabb();
            scene->updateSceneAabb();
            scene->updateSelectionAabb();
        }
    }
}
// установка параметров окна в соответствии с параметрами текущего объекта должны быть установлены здесь
void Application_editParamsWindow_onActivate(s32 id, void* data)
{
    ((kkPluginGUIWindow *)data)->Activate();

    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    if( scene->getNumOfSelectedObjects() == 1 )
    {
        auto object = (Scene3DObject *)scene->getSelectedObject(0);
        if(object->GetType() == kkScene3DObjectType::PolygonObject)
        {
            auto data = object->GetPivot().data();

            g_EditPolyObjectsGUIElements.m_object_position_x_element->SetPointerFloat(&data[0]);
            g_EditPolyObjectsGUIElements.m_position_x = &data[0];
        }
    }
};
void Application::_initEditParamsWindow()
{
    m_edit_params_window = m_pluginGUI->createWindow();
    m_edit_params_window->SetSize(v2i(200,0));
    m_edit_params_window->SetType( kkPluginGUIWindowType::Parameters );
    m_edit_params_window->SetName(u"Editable object");

    m_edit_params_window->SetOnActivate(Application_editParamsWindow_onActivate);
    m_edit_params_window->SetUserData(m_edit_params_window);

    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Position", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"X:", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    
    g_EditPolyObjectsGUIElements.m_object_position_x_element = 
        m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(100.f, 20.f), change_position_x_callback, kkPluginGUIParameterType::Object);
}