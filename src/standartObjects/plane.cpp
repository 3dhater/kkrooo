// SPDX-License-Identifier: GPL-3.0-only
#include "common.h"

template<>
ParametersPlane* kkSingleton<ParametersPlane>::s_instance = nullptr;

void planeCallback(s32 id, void * d )
{
	auto staticData  = kkSingleton<kkPluginIDData>::s_instance;
	auto planeParams = kkSingleton<ParametersPlane>::s_instance;
	planeParams->window->Activate();
		
	auto scene = staticData->m_ci->GetScene();
	auto gc    = staticData->m_ci->GetGeometryCreator();


	kkString name(u"Plane");
	kkVector4  pivot;
	kkMatrix4  matrix;
	kkMaterialImplementation* materialImpl = nullptr;
	kkString materialName;

	ParametersPlane * plane_data = nullptr;

	auto object = scene->getSelectedObject(0);
	if( object )
	{
		if( object->GetPluginGUIWindow() == planeParams->window )
		{
			name  = object->GetName();
			pivot = object->GetPivot();
			matrix = object->GetMatrix();
			materialImpl = object->GetMaterialImplementation();
			if( object->GetMaterial() )
				materialName = object->GetMaterial();
			object->SaveShaderParameter();

			// при изменении параметров старый объект уничтожается и создаётся новый
			// но нужно где-то хранить параметры объекта
			// по этому, если они уже есть, берём их используя DropParametersWindowData
			// удаляем объект, создаём новый, и возвращаем plane_data
			// если их нет, то создаются новые параметры на основе базовых planeParams 
			// (planeParams нужен так как при создании, объектов вообще нет...возможно нужно дать правильное имя, defaultPrarams например)
			plane_data = (ParametersPlane *)object->DropParametersWindowData();

			if(!plane_data)
			{
				plane_data = (ParametersPlane*)object->AllocateParametersWindowData(sizeof(ParametersPlane));
				copyParametersPlane(planeParams, plane_data);
			}
				
			scene->deleteObject(object);
		}
	}

	bool need_new_data = false;
	// если объекта ещё нет (не выделен)
	if( !plane_data )
	{
		// то установка параметров по умолчанию
		plane_data = planeParams;
		need_new_data = true; // в конце нужно создать свои параметры
	}

	float half_width  = plane_data->width * 0.5f;
	float half_height = plane_data->height * 0.5f;

	if( half_width == 0.f )
		half_width = 0.001f;

	if( half_height == 0.f )
		half_height = 0.001f;

	float segment_size_h = plane_data->height / (float)plane_data->height_segments;
	float segment_size_w = plane_data->width / (float)plane_data->width_segments;

	float uv_segment_size_h = 1.f / (float)plane_data->height_segments;
	float uv_segment_size_w = 1.f / (float)plane_data->width_segments;

	gc->BeginModel(v4f(pivot[0],pivot[1],pivot[2]));

	float begin_x = 0.f;
	float begin_z = 0.f;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;

	for( u32 h_i = 0; h_i < plane_data->height_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < plane_data->width_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			gc->AddPosition(begin_x-half_width,   0,   begin_z-half_height);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y);
			
			gc->AddPosition(begin_x-half_width,  0,   begin_z+segment_size_h-half_height);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(begin_x+segment_size_w-half_width,  0,   begin_z+segment_size_h-half_height);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(begin_x+segment_size_w-half_width,  0,   begin_z-half_height);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0,0);

			begin_x += segment_size_w;
			uv_begin_x += uv_segment_size_w;

			if( begin_x > plane_data->width )
			{
				begin_x = 0;
				uv_begin_x = 0;
			}
		}

		begin_z += segment_size_h;
		begin_x = 0;

		uv_begin_y -= uv_segment_size_h;
		uv_begin_x = 0;
	}


	gc->SetName(name.data());
	auto new_object = gc->EndModel();

	new_object->SetPluginGUIWindow(plane_data->window);
	scene->deselectAll();
	scene->selectObject(new_object);

	if( need_new_data )
	{
		plane_data = (ParametersPlane*)new_object->AllocateParametersWindowData(sizeof(ParametersPlane));
		copyParametersPlane(planeParams, plane_data);
	}

	new_object->SetParametersWindowData(plane_data);
	new_object->SetMatrix(matrix);
	new_object->UpdateAabb();
	if( materialImpl )
	{
		new_object->SetMaterialImplementation(materialImpl);
		new_object->SetMaterial(materialName.data());
	}
	new_object->RestoreShaderParameter();
}

kkScene3DObject* getSceneObject()
{
	return kkSingleton<kkPluginIDData>::s_instance->m_ci->GetScene()->getSelectedObject(0);
}
ParametersPlane* getPlaneData()
{
	return (ParametersPlane*)getSceneObject()->DropParametersWindowData();
}
void updatePlane(s32 id, void * d, ParametersPlane* plane_data, kkScene3DObject* object)
{
	object->SetParametersWindowData(plane_data);
	planeCallback(id,d);
}

void onActivatePlaneCallback(s32 id, void * d)
{
	kkPluginIDData* staticData = kkSingleton<kkPluginIDData>::s_instance;
	ParametersPlane* planeParams = kkSingleton<ParametersPlane>::s_instance;
		
	// нужно взять объект
	auto scene = staticData->m_ci->GetScene();
	auto object = scene->getSelectedObject(0);

	// взять данные которые были ранее установлены
	ParametersPlane* plane_data = (ParametersPlane*)object->DropParametersWindowData();

	// или создать и инициализировать значением по умолчанию
	if( !plane_data )
	{
		plane_data = (ParametersPlane*)object->AllocateParametersWindowData(sizeof(ParametersPlane));
		copyParametersPlane(planeParams, plane_data);
	}

	
	// дать новые значения нужным элементам
	plane_data->width_element->SetPointerFloat(&plane_data->width);

	plane_data->height_element->SetPointerFloat(&plane_data->height);

	plane_data->width_segments_element->SetPointerInt(&plane_data->width_segments);
	plane_data->width_segments_text_element->SetText(u"%i", plane_data->width_segments);

	plane_data->height_segments_element->SetPointerInt(&plane_data->height_segments);
	plane_data->height_segments_text_element->SetText(u"%i", plane_data->height_segments);

	// и установить\вернуть на место данные
	object->SetParametersWindowData(plane_data);
}