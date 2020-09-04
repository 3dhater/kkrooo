// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "Scene3D/Scene3D.h"
#include "Scene3D/Scene3DObject.h"
#include "Common/kkInfoSharedLibrary.h"
#include "Classes/Math/kkMath.h"
#include "Plugins/Plugin.h"
#include "Plugins/PluginGUI.h"

#include "Application.h"

struct EditPolyObjectsGUIElements
{
    kkPluginGUIWindowElement * m_object_position_x_element = nullptr;
    kkPluginGUIWindowElement * m_object_position_y_element = nullptr;
    kkPluginGUIWindowElement * m_object_position_z_element = nullptr;
    f32 * m_position_x = nullptr;
    f32 * m_position_y = nullptr;
    f32 * m_position_z = nullptr;
    
    kkPluginGUIWindowElement * m_object_pitch_element = nullptr;
    kkPluginGUIWindowElement * m_object_yaw_element = nullptr;
    kkPluginGUIWindowElement * m_object_roll_element = nullptr;
    
    kkPluginGUIWindowElement * m_object_rotX_element = nullptr;
    kkPluginGUIWindowElement * m_object_rotY_element = nullptr;
    kkPluginGUIWindowElement * m_object_rotZ_element = nullptr;

    kkPluginGUIWindowElement * m_object_scaleX_element = nullptr;
    kkPluginGUIWindowElement * m_object_scaleY_element = nullptr;
    kkPluginGUIWindowElement * m_object_scaleZ_element = nullptr;
    
    kkPluginGUIWindowElement * m_pivot_positionX_element = nullptr;
    kkPluginGUIWindowElement * m_pivot_positionY_element = nullptr;
    kkPluginGUIWindowElement * m_pivot_positionZ_element = nullptr;
    
    kkPluginGUIWindowElement * m_weld_len_element = nullptr;
    kkPluginGUIWindowElement * m_chamfer_vertex_len_element = nullptr;
    f32 m_weld_len = 0.01f;
    f32 m_chamfer_vertex_len = 0.01f;
    
    kkPluginGUIWindowElement * m_object_name_element = nullptr;
}g_EditPolyObjectsGUIElements;

kkControlVertex* g_pickedVertex1 = nullptr;
kkControlVertex* g_pickedVertex2 = nullptr;

Scene3DObject* GetSelectedObject()
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    if( scene->getNumOfSelectedObjects() == 1 )
    {
        auto object = (Scene3DObject *)scene->getSelectedObject(0);
        if(object->GetType() == kkScene3DObjectType::PolygonObject)
        {
            return object;
        }
    }
    return nullptr;
}

void attach_pickObject_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    auto pickedObject = app->getPickedObject();
    if(object && pickedObject)
    {
        object->AttachObject(pickedObject);
        scene->deleteObject(pickedObject);
        scene->selectObject(object);
    }
}
void attach_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    app->setObjectPickMode(attach_pickObject_callback);
}

void change_pivot_position_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->ChangePivotPosition(object->GetPivot());
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void change_pivot_position_callback_toObjectCenter(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        kkVector4 center;
        object->Aabb().center(center);
        object->ChangePivotPosition(center);
        object->setPosition(center);
        object->ApplyPivot();
        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void change_pivot_position_callback_toSceneCenter(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->ChangePivotPosition(kkVector4());
        object->setPosition(kkVector4());
        object->ApplyPivot();
        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void reset_matrix_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->SetMatrix(kkMatrix4());
        auto & s = object->GetScale();
        s.set(1.f,1.f,1.f,1.f);
        auto & r = object->GetRotationAngles();
        r.set(0.f,0.f,0.f,0.f);
        r = object->GetRotationPitchYawRoll();
        r.set(0.f,0.f,0.f,0.f);
        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void apply_matrix_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->applyMatrices();
        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void change_scale_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto & scale = object->GetScale();
        auto M = object->GetMatrix();
            
        M[ 0u ].KK_X = scale.KK_X;
		M[ 1u ].KK_Y = scale.KK_Y;
		M[ 2u ].KK_Z = scale.KK_Z;

        object->SetMatrix(M);
        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void change_rotation_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto & rotation = object->GetRotationAngles();

        kkMatrix4 MP, MY, MR, W;
          
        auto & scale = object->GetScale();
        W[ 0u ].KK_X = scale.KK_X;
		W[ 1u ].KK_Y = scale.KK_Y;
		W[ 2u ].KK_Z = scale.KK_Z;

        kkQuaternion qX(kkVector4(math::degToRad(rotation.getX()), 0.f, 0.f));
        kkQuaternion qY(kkVector4(0.f, math::degToRad(rotation.getY()), 0.f));
        kkQuaternion qZ(kkVector4(0.f, 0.f, math::degToRad(rotation.getZ()) ));
            
        math::makeRotationMatrix(MP,qX);
        math::makeRotationMatrix(MY,qY);
        math::makeRotationMatrix(MR,qZ);

        W = MP * W;
        W = MY * W;
        W = MR * W;
            
            
        object->SetMatrix(W);

        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void change_pitch_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto & rotation = object->GetRotationPitchYawRoll();
        kkMatrix4 MP, MY, MR;

        kkQuaternion qX(kkVector4(math::degToRad(rotation.getX()), 0.f, 0.f));
        kkQuaternion qY(kkVector4(0.f, math::degToRad(rotation.getY()), 0.f));
        kkQuaternion qZ(kkVector4(0.f, 0.f, math::degToRad(rotation.getZ()) ));
            
        math::makeRotationMatrix(MP,qX);
        math::makeRotationMatrix(MY,qY);
        math::makeRotationMatrix(MR,qZ);

        auto W = object->GetMatrix();

        W = W*MP;
        W = W*MY;
        W = W*MR;

        object->SetMatrix(W);

        object->UpdateWorldMatrix();
        object->UpdateAabb();
        object->ApplyFixedMatrix();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();

        rotation.set(0.f, 0.f, 0.f, 0.f);
    }
}

void change_position_x_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto pivot = object->GetPivot();
        object->setPosition( kkVector4(*g_EditPolyObjectsGUIElements.m_position_x,pivot._f32[1],pivot._f32[2],pivot._f32[3]) );
        object->ApplyPivot();
        object->UpdateAabb();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}
void change_position_y_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto pivot = object->GetPivot();
        object->setPosition( kkVector4(pivot._f32[0],*g_EditPolyObjectsGUIElements.m_position_y,pivot._f32[2],pivot._f32[3]) );
        object->ApplyPivot();
        object->UpdateAabb();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}
void change_position_z_callback(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        auto pivot = object->GetPivot();
        object->setPosition( kkVector4(pivot._f32[0],pivot._f32[1],*g_EditPolyObjectsGUIElements.m_position_z,pivot._f32[3]) );
        object->ApplyPivot();
        object->UpdateAabb();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void pick_1_vertex(s32 id, void* data);
void pick_2_vertex_weld(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;

    g_pickedVertex2 = app->getPickedVertex();
    app->setDrawPickLine(false);

    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();

    if(g_pickedVertex1 && g_pickedVertex2)
    {
        if(object)
        {
            object->Weld(g_pickedVertex1, g_pickedVertex2);
            app->setVertexPickMode(pick_1_vertex);
            object->UpdateAabb();
	        scene->updateObjectVertexSelectList();
            scene->updateSceneAabb();
            scene->updateSelectionAabb();
        }
    }

}
void pick_1_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;

    g_pickedVertex1 = app->getPickedVertex();

    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        app->setVertexPickMode(pick_2_vertex_weld);
        app->setDrawPickLine(true);
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}
void target_weld_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        scene->deselectAll();
        app->setVertexPickMode(pick_1_vertex);
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}
void weld_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->WeldSelectedVerts(g_EditPolyObjectsGUIElements.m_weld_len);
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void break_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->BreakVerts();
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void chamfer_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        //object->ChamferVerts(g_EditPolyObjectsGUIElements.m_chamfer_vertex_len, true);
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void connect_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->ConnectVerts();
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void select_add_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->SelecVertsByAdd();
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void select_sub_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        object->SelecVertsBySub();
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
    }
}

void select_all_vertex(s32 id, void* data)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    auto object = GetSelectedObject();
    if(object)
    {
        scene->selectAll();
        //object->SelecVertsBySub();
        object->UpdateAabb();
	    scene->updateObjectVertexSelectList();
        scene->updateSceneAabb();
        scene->updateSelectionAabb();
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
            g_EditPolyObjectsGUIElements.m_object_position_y_element->SetPointerFloat(&data[1]);
            g_EditPolyObjectsGUIElements.m_object_position_z_element->SetPointerFloat(&data[2]);
            g_EditPolyObjectsGUIElements.m_position_x = &data[0];
            g_EditPolyObjectsGUIElements.m_position_y = &data[1];
            g_EditPolyObjectsGUIElements.m_position_z = &data[2];
            
            g_EditPolyObjectsGUIElements.m_object_name_element->SetText(object->GetName());

            auto data_rotationPYR = object->GetRotationPitchYawRoll().data();
            g_EditPolyObjectsGUIElements.m_object_pitch_element->SetPointerFloat(&data_rotationPYR[0]);
            g_EditPolyObjectsGUIElements.m_object_yaw_element->SetPointerFloat(&data_rotationPYR[1]);
            g_EditPolyObjectsGUIElements.m_object_roll_element->SetPointerFloat(&data_rotationPYR[2]);


            auto data_rotationAngles = object->GetRotationAngles().data();
            g_EditPolyObjectsGUIElements.m_object_rotX_element->SetPointerFloat(&data_rotationAngles[0]);
            g_EditPolyObjectsGUIElements.m_object_rotY_element->SetPointerFloat(&data_rotationAngles[1]);
            g_EditPolyObjectsGUIElements.m_object_rotZ_element->SetPointerFloat(&data_rotationAngles[2]);
            
            auto data_scale = object->GetScale().data();
            g_EditPolyObjectsGUIElements.m_object_scaleX_element->SetPointerFloat(&data_scale[0]);
            g_EditPolyObjectsGUIElements.m_object_scaleY_element->SetPointerFloat(&data_scale[1]);
            g_EditPolyObjectsGUIElements.m_object_scaleZ_element->SetPointerFloat(&data_scale[2]);

            g_EditPolyObjectsGUIElements.m_pivot_positionX_element->SetPointerFloat(&data[0]);
            g_EditPolyObjectsGUIElements.m_pivot_positionY_element->SetPointerFloat(&data[1]);
            g_EditPolyObjectsGUIElements.m_pivot_positionZ_element->SetPointerFloat(&data[2]);
        }
    }
};
bool nameInputFilter(char16_t ch)
{
    return true;
}
void textInputResult(const char16_t* text)
{
    auto app = kkSingleton<Application>::s_instance;
    Scene3D* scene = *app->getScene3D();
    if( scene->getNumOfSelectedObjects() == 1 )
    {
        auto object = scene->getSelectedObject(0);
        kkString currentName = object->GetName();
        kkString newName = text;

        if(!newName.size())
            newName = currentName;

        if( currentName != newName )
        {
            scene->renameObject(scene->getSelectedObject(0), text);
        }
    }
}
void Application::_initEditParamsWindow()
{
    m_edit_params_window = m_pluginGUI->createWindow();
    m_edit_params_window->SetSize(v2i(200,0));
    m_edit_params_window->SetType( kkPluginGUIWindowType::Parameters );
    m_edit_params_window->SetName(u"Editable object");

    m_edit_params_window->SetOnActivate(Application_editParamsWindow_onActivate);
    m_edit_params_window->SetUserData(m_edit_params_window);

    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Name", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_name_element = m_edit_params_window->AddTextInput(u"hello", v2f(130.f, 20.f), nameInputFilter, textInputResult, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Position", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"X:", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_position_x_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_position_x_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Y:", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_position_y_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_position_x_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Z:", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_position_z_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_position_x_callback, kkPluginGUIParameterType::Object);
 
    m_edit_params_window->BeginGroup(u"Matrix settings", false);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddMoveLeftRight(15.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Rotation:", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_pitch_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pitch_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Pitch", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_yaw_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pitch_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Yaw", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_roll_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pitch_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Roll", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddMoveLeftRight(15.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Rotation by angles:", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_rotX_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_rotation_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"X", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_rotY_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_rotation_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Y", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_rotZ_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_rotation_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Z", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddMoveLeftRight(15.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Scale:", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_scaleX_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_scale_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"X", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_scaleY_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_scale_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Y", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_object_scaleZ_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_scale_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Z", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddButton(u"Reset", v2f(80.f, 20.f), reset_matrix_callback,0, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddButton(u"Apply", v2f(80.f, 20.f), apply_matrix_callback,0, kkPluginGUIParameterType::Object);
    m_edit_params_window->EndGroup();

    m_edit_params_window->BeginGroup(u"Pivot", false);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddMoveLeftRight(15.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Change position:", 0xFFFFFFFF, 0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_pivot_positionX_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pivot_position_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"X", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_pivot_positionY_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pivot_position_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Y", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    g_EditPolyObjectsGUIElements.m_pivot_positionZ_element = m_edit_params_window->AddValueSelectorFloat(0, 0.01f, true, v2f(130.f, 20.f), change_pivot_position_callback, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddText(u"Z", 0xFFFFFFFF, 3.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddButton(u"To object center", v2f(120.f, 20.f), change_pivot_position_callback_toObjectCenter,0, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddButton(u"To scene center", v2f(120.f, 20.f), change_pivot_position_callback_toSceneCenter,0, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->EndGroup();

    m_edit_params_window->BeginGroup(u"Other", false);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Object);
    m_edit_params_window->AddButton(u"Attach", v2f(80.f, 20.f), attach_callback,0, kkPluginGUIParameterType::Object);
    m_edit_params_window->EndGroup();

    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->BeginGroup(u"Selection", true);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(20.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"+", v2f(20.f, 20.f), select_add_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(20.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"-", v2f(20.f, 20.f), select_sub_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(20.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"All", v2f(20.f, 20.f), select_all_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->EndGroup();
    m_edit_params_window->BeginGroup(u"Geometry edit", true);
    m_edit_params_window->AddNewLine(0.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"Break", v2f(60.f, 20.f), break_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"Target weld", v2f(110.f, 20.f), target_weld_vertex, 0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddNewLine(7.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"Weld", v2f(60.f, 20.f), weld_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    g_EditPolyObjectsGUIElements.m_weld_len_element = m_edit_params_window->AddValueSelectorFloatLimit(0.f, 99999999.f, &g_EditPolyObjectsGUIElements.m_weld_len, 0.01f, true, v2f(110.f, 20.f), 0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddNewLine(7.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"Connect", v2f(60.f, 20.f), connect_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddNewLine(7.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddButton(u"Chamfer", v2f(60.f, 20.f), chamfer_vertex,0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->AddMoveLeftRight(10.f, kkPluginGUIParameterType::Vertex);
    g_EditPolyObjectsGUIElements.m_chamfer_vertex_len_element = m_edit_params_window->AddValueSelectorFloatLimit(0.0001f, 99999999.f, &g_EditPolyObjectsGUIElements.m_chamfer_vertex_len, 0.01f, true, v2f(110.f, 20.f), 0, kkPluginGUIParameterType::Vertex);
    m_edit_params_window->EndGroup();
}