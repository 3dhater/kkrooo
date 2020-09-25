// SPDX-License-Identifier: GPL-3.0-only
#include "common.h"

template<>
ParametersBox* kkSingleton<ParametersBox>::s_instance = nullptr;

void generateTopPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )  half_width = 0.001f;
	if( half_length == 0.f ) half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_z / (float)box_data->z_segments;
	float segment_size_w = box_data->size_x / (float)box_data->x_segments;

	float uv_segment_size_h = 1.f / (float)box_data->z_segments;
	float uv_segment_size_w = 1.f / (float)box_data->x_segments;


	float begin_x = 0.f;
	float begin_z = 0.f;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;

	for( u32 h_i = 0; h_i < box_data->z_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->x_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			
			gc->AddPosition(begin_x-half_width,   half_height,   begin_z-half_length);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y);
			
			gc->AddPosition(begin_x-half_width,  half_height,   begin_z+segment_size_h-half_length);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(begin_x+segment_size_w-half_width,  half_height,   begin_z+segment_size_h-half_length);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(begin_x+segment_size_w-half_width,  half_height,   begin_z-half_length);
			gc->AddNormal(0.,1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_x += segment_size_w;
			uv_begin_x += uv_segment_size_w;

			if( begin_x > box_data->size_x )
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
}

void generateBottomPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )half_width = 0.001f;
	if( half_length == 0.f )half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_z / (float)box_data->z_segments;
	float segment_size_w = box_data->size_x / (float)box_data->x_segments;

	float uv_segment_size_h = 1.f / (float)box_data->z_segments;
	float uv_segment_size_w = 1.f / (float)box_data->x_segments;


	float begin_x = 0.f;
	float begin_z = 0.f;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;

	for( u32 h_i = 0; h_i < box_data->z_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->x_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			
			gc->AddPosition( -(begin_x-half_width),   -half_height,   begin_z-half_length);
			gc->AddNormal(0.,-1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y);
			
			gc->AddPosition( -(begin_x-half_width),  -half_height,   begin_z+segment_size_h-half_length);
			gc->AddNormal(0.,-1.,0.);
			gc->AddUV(uv_begin_x, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(-(begin_x+segment_size_w-half_width),  -half_height,   begin_z+segment_size_h-half_length);
			gc->AddNormal(0.,-1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(-(begin_x+segment_size_w-half_width),  -half_height,   begin_z-half_length);
			gc->AddNormal(0.,-1.,0.);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_x += segment_size_w;
			uv_begin_x += uv_segment_size_w;

			if( begin_x > box_data->size_x )
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
}

void generateFrontPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )  half_width = 0.001f;
	if( half_length == 0.f ) half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_y / (float)box_data->y_segments;
	float segment_size_w = box_data->size_x / (float)box_data->x_segments;

	float uv_segment_size_h = 1.f / (float)box_data->y_segments;
	float uv_segment_size_w = 1.f / (float)box_data->x_segments;


	float begin_x = 0.f;
	float begin_y = 0.f;

	float uv_begin_x = 1.f;
	float uv_begin_y = 0.f;

	for( u32 h_i = 0; h_i < box_data->y_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->x_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			gc->AddPosition( -(begin_x-half_width),   begin_y-half_height,   half_length);
			gc->AddNormal(0.f,0.f,1.f);
			gc->AddUV(uv_begin_x, uv_begin_y);

			
			gc->AddPosition( -(begin_x-half_width),  begin_y+segment_size_h-half_height,   half_length);
			gc->AddNormal(0.f,0.f,1.f);
			gc->AddUV(uv_begin_x, uv_begin_y+uv_segment_size_h);
			
			gc->AddPosition(-(begin_x+segment_size_w-half_width),  begin_y+segment_size_h-half_height,   half_length);
			gc->AddNormal(0.f,0.f,1.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y+uv_segment_size_h);
			
			gc->AddPosition(-(begin_x+segment_size_w-half_width),  begin_y-half_height,   half_length);
			gc->AddNormal(0.f,0.f,1.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_x += segment_size_w;
			uv_begin_x -= uv_segment_size_w;

			if( begin_x > box_data->size_x )
			{
				begin_x = 0;
				uv_begin_x = 1;
			}
		}

		begin_y += segment_size_h;
		begin_x = 0;

		uv_begin_y += uv_segment_size_h;
		uv_begin_x = 1;
	}
}

void generateBackPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )  half_width = 0.001f;
	if( half_length == 0.f ) half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_y / (float)box_data->y_segments;
	float segment_size_w = box_data->size_x / (float)box_data->x_segments;

	float uv_segment_size_h = 1.f / (float)box_data->y_segments;
	float uv_segment_size_w = 1.f / (float)box_data->x_segments;


	float begin_x = 0.f;
	float begin_y = 0.f;

	float uv_begin_x = 1.f;
	float uv_begin_y = 0.f;

	for( u32 h_i = 0; h_i < box_data->y_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->x_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			gc->AddPosition( begin_x-half_width,   begin_y-half_height,   -half_length);
			gc->AddNormal(0.f,0.f,-1.f);
			gc->AddUV(uv_begin_x, uv_begin_y);
			
			gc->AddPosition( begin_x-half_width,  begin_y+segment_size_h-half_height,   -half_length);
			gc->AddNormal(0.f,0.f,-1.f);
			gc->AddUV(uv_begin_x, uv_begin_y+uv_segment_size_h);

			gc->AddPosition(begin_x+segment_size_w-half_width,  begin_y+segment_size_h-half_height,   -half_length);
			gc->AddNormal(0.f,0.f,-1.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y+uv_segment_size_h);
			
			gc->AddPosition(begin_x+segment_size_w-half_width,  begin_y-half_height,   -half_length);
			gc->AddNormal(0.f,0.f,-1.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_x += segment_size_w;
			uv_begin_x -= uv_segment_size_w;

			if( begin_x > box_data->size_x )
			{
				begin_x = 0;
				uv_begin_x = 1;
			}
		}

		begin_y += segment_size_h;
		begin_x = 0;

		uv_begin_y += uv_segment_size_h;
		uv_begin_x = 1;
	}
}

void generateRightPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )  half_width = 0.001f;
	if( half_length == 0.f ) half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_y / (float)box_data->y_segments;
	float segment_size_w = box_data->size_z / (float)box_data->z_segments;

	float uv_segment_size_h = 1.f / (float)box_data->y_segments;
	float uv_segment_size_w = 1.f / (float)box_data->z_segments;


	float begin_z = 0.f;
	float begin_y = 0.f;

	float uv_begin_x = 1.f;
	float uv_begin_y = 0.f;

	for( u32 h_i = 0; h_i < box_data->y_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->z_segments; ++w_i )
		{
			gc->BeginPolygon();
			

			gc->AddPosition( -half_width,   begin_y-half_height,   -(begin_z-half_length));
			gc->AddNormal(-1.f,0.f,0.f);
			gc->AddUV(uv_begin_x, uv_begin_y);
			
			gc->AddPosition( -half_width,  begin_y+segment_size_h-half_height,   -(begin_z-half_length));
			gc->AddNormal(-1.f,0.f,0.f);
			gc->AddUV(uv_begin_x, uv_begin_y+uv_segment_size_h);
			
			gc->AddPosition(-half_width,  begin_y+segment_size_h-half_height,   -(begin_z+segment_size_w-half_length));
			gc->AddNormal(-1.f,0.f,0.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y+uv_segment_size_h);
			
			gc->AddPosition(-half_width,  begin_y-half_height,   -(begin_z+segment_size_w-half_length));
			gc->AddNormal(-1.f,0.f,0.f);
			gc->AddUV(uv_begin_x-uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_z += segment_size_w;
			uv_begin_x -= uv_segment_size_w;

			if( begin_z > box_data->size_z )
			{
				begin_z = 0;
				uv_begin_x = 1;
			}
		}

		begin_y += segment_size_h;
		begin_z = 0;

		uv_begin_y += uv_segment_size_h;
		uv_begin_x = 1;
	}
}

void generateLeftPlane(ParametersBox * box_data, kkGeometryCreator* gc)
{
	float half_width  = box_data->size_x * 0.5f;
	float half_length = box_data->size_z * 0.5f;
	float half_height = box_data->size_y * 0.5f;

	if( half_width == 0.f )  half_width = 0.001f;
	if( half_length == 0.f ) half_length = 0.001f;
	if( half_height == 0.f ) half_height = 0.001f;

	float segment_size_h = box_data->size_y / (float)box_data->y_segments;
	float segment_size_w = box_data->size_z / (float)box_data->z_segments;

	float uv_segment_size_h = 1.f / (float)box_data->y_segments;
	float uv_segment_size_w = 1.f / (float)box_data->z_segments;


	float begin_z = 0.f;
	float begin_y = 0.f;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;

	for( u32 h_i = 0; h_i < box_data->y_segments; ++h_i )
	{
		for( u32 w_i = 0; w_i < box_data->z_segments; ++w_i )
		{
			gc->BeginPolygon();
			
			gc->AddPosition( half_width,   -(begin_y-half_height),   -(begin_z-half_length));
			gc->AddNormal(1.f,0.f,0.f);
			gc->AddUV(uv_begin_x, uv_begin_y);

			
			gc->AddPosition( half_width,  -(begin_y+segment_size_h-half_height),   -(begin_z-half_length));
			gc->AddNormal(1.f,0.f,0.f);
			gc->AddUV(uv_begin_x, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(half_width,  -(begin_y+segment_size_h-half_height),   -(begin_z+segment_size_w-half_length));
			gc->AddNormal(1.f,0.f,0.f);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y-uv_segment_size_h);
			
			gc->AddPosition(half_width,  -(begin_y-half_height),   -(begin_z+segment_size_w-half_length));
			gc->AddNormal(1.f,0.f,0.f);
			gc->AddUV(uv_begin_x+uv_segment_size_w, uv_begin_y);
			
			gc->EndPolygon(1,0);

			begin_z += segment_size_w;
			uv_begin_x += uv_segment_size_w;

			if( begin_z > box_data->size_z )
			{
				begin_z = 0;
				uv_begin_x = 0;
			}
		}

		begin_y += segment_size_h;
		begin_z = 0;

		uv_begin_y -= uv_segment_size_h;
		uv_begin_x = 0;
	}
}

void boxCallback(s32 id, void * d )
{
	auto staticData  = kkSingleton<kkPluginIDData>::s_instance;
	
	auto boxParams = kkSingleton<ParametersBox>::s_instance;
	boxParams->window->Activate();

	auto scene = staticData->m_ci->GetScene();
	auto gc    = staticData->m_ci->GetGeometryCreator();


	kkString name(u"Box");
	kkVector4  pivot;
	kkMatrix4  matrix;
	kkMaterialImplementation* materialImpl = nullptr;
	kkString materialName;

	ParametersBox * box_data = nullptr;

	auto object = scene->getSelectedObject(0);
	if( object )
	{
		if( object->GetPluginGUIWindow() == boxParams->window )
		{
			name  = object->GetName();
			pivot = object->GetPivot();
			matrix = object->GetMatrix();
			materialImpl = object->GetMaterialImplementation();
			if( object->GetMaterial() )
				materialName = object->GetMaterial();
			object->SaveShaderParameter();

			box_data = (ParametersBox *)object->DropParametersWindowData();

			if(!box_data)
			{
				box_data = (ParametersBox*)object->AllocateParametersWindowData(sizeof(ParametersBox));
				copyParametersBox(boxParams, box_data);
			}
				
			scene->deleteObject(object);
		}
	}

	bool need_new_data = false;
	// если объекта ещё нет (не выделен)
	if( !box_data )
	{
		// то установка параметров по умолчанию
		box_data = boxParams;
		need_new_data = true; // в конце нужно создать свои параметры
	}

	gc->BeginModel(v4f(pivot[0],pivot[1],pivot[2]));
	
	generateTopPlane(box_data, gc);
	generateBottomPlane(box_data, gc);
	generateFrontPlane(box_data, gc);
	generateBackPlane(box_data, gc);
	generateRightPlane(box_data, gc);
	generateLeftPlane(box_data, gc);

	gc->SetName(name.data());
	auto new_object = gc->EndModel(true, 0.00001f);

	new_object->SetPluginGUIWindow(box_data->window);
	scene->deselectAll();
	scene->selectObject(new_object);

	if( need_new_data )
	{
		box_data = (ParametersBox*)new_object->AllocateParametersWindowData(sizeof(ParametersBox));
		copyParametersBox(boxParams, box_data);
	}

	new_object->SetParametersWindowData(box_data);
	new_object->SetMatrix(matrix);
	new_object->UpdateAabb();
	if( materialImpl )
	{
		new_object->SetMaterialImplementation(materialImpl);
		new_object->SetMaterial(materialName.data());
	}
	new_object->RestoreShaderParameter();
}


ParametersBox* getBoxData()
{
	return (ParametersBox*)getSceneObject()->DropParametersWindowData();
}
void updateBox(s32 id, void * d, ParametersBox* box_data, kkScene3DObject* object)
{
	object->SetParametersWindowData(box_data);
	boxCallback(id,d);
}
void onActivateBoxCallback(s32 id, void * d)
{
	kkPluginIDData* staticData = kkSingleton<kkPluginIDData>::s_instance;
	ParametersBox* boxParams = kkSingleton<ParametersBox>::s_instance;
		
	// нужно взять объект
	auto scene = staticData->m_ci->GetScene();
	auto object = scene->getSelectedObject(0);

	// взять данные которые были ранее установлены
	ParametersBox* box_data = (ParametersBox*)object->DropParametersWindowData();

	// или создать и инициализировать значением по умолчанию
	if( !box_data )
	{
		box_data = (ParametersBox*)object->AllocateParametersWindowData(sizeof(ParametersBox));
		copyParametersBox(boxParams, box_data);
	}

	// дать новые значения нужным элементам
	box_data->size_x_element->SetPointerFloat(&box_data->size_x);
	box_data->size_y_element->SetPointerFloat(&box_data->size_y);
	box_data->size_z_element->SetPointerFloat(&box_data->size_z);

	box_data->seg_x_element->SetPointerInt(&box_data->x_segments);
	box_data->seg_y_element->SetPointerInt(&box_data->y_segments);
	box_data->seg_z_element->SetPointerInt(&box_data->z_segments);
	box_data->text_seg_x_element->SetText(u"%i",box_data->x_segments);
	box_data->text_seg_y_element->SetText(u"%i",box_data->y_segments);
	box_data->text_seg_z_element->SetText(u"%i",box_data->z_segments);

	// и установить\вернуть на место данные
	object->SetParametersWindowData(box_data);
}