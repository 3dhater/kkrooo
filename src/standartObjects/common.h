// SPDX-License-Identifier: GPL-3.0-only
#ifndef __COMMON_H__
#define __COMMON_H__

#include "PluginSDK.h"

kkScene3DObject* getSceneObject();

void planeCallback(s32 id, void * d );
//void subPlaneHCallback(s32 id, void * d);
//void subPlaneWCallback(s32 id, void * d);
//void addPlaneHCallback(s32 id, void * d);
//void addPlaneWCallback(s32 id, void * d);
void onActivatePlaneCallback(s32 id, void * d);

void boxCallback(s32 id, void * d );
void onActivateBoxCallback(s32 id, void * d);

struct ParametersPlane
{
	// окно с GUI элементами
	kkPluginGUIWindow* window;         
	
	// те элементы, значения которых нужно будет изменить
	kkPluginGUIWindowElement * height_element;
	kkPluginGUIWindowElement * width_element;

	kkPluginGUIWindowElement * height_segments_element;
	kkPluginGUIWindowElement * width_segments_element;
	kkPluginGUIWindowElement * height_segments_text_element;
	kkPluginGUIWindowElement * width_segments_text_element;

	// чтобы выводить текст в GUI элементы
	//std::u16string str_wid

	// сами параметры
	int                height_segments = 1;
	int                width_segments = 1;
	float              height = 1.f;
	float              width  = 1.f;
};

inline void copyParametersPlane( ParametersPlane* src, ParametersPlane* dst )
{
	dst->window = src->window;
	dst->height = src->height;
	dst->width  = src->width;
	dst->height_segments = src->height_segments;
	dst->width_segments = src->width_segments;

	dst->height_element = src->height_element;
	dst->width_element = src->width_element;
	dst->height_segments_element = src->height_segments_element;
	dst->width_segments_element = src->width_segments_element;
	
	dst->height_segments_text_element = src->height_segments_text_element;
	dst->width_segments_text_element = src->width_segments_text_element;
}

struct ParametersBox
{
		// окно с GUI элементами
	kkPluginGUIWindow* window = nullptr;         
	
	// те элементы, значения которых нужно будет изменить
	kkPluginGUIWindowElement * size_x_element = nullptr;
	kkPluginGUIWindowElement * size_y_element = nullptr;
	kkPluginGUIWindowElement * size_z_element = nullptr;
	kkPluginGUIWindowElement * seg_x_element = nullptr;
	kkPluginGUIWindowElement * seg_y_element = nullptr;
	kkPluginGUIWindowElement * seg_z_element = nullptr;

	kkPluginGUIWindowElement * text_seg_x_element = nullptr;
	kkPluginGUIWindowElement * text_seg_y_element = nullptr;
	kkPluginGUIWindowElement * text_seg_z_element = nullptr;

	// сами параметры
	float              size_x = 1.f;
	float              size_y = 1.f;
	float              size_z = 1.f;
	int                x_segments = 1;
	int                y_segments = 1;
	int                z_segments = 1;
};

inline void copyParametersBox( ParametersBox* src, ParametersBox* dst )
{
	dst->x_segments  = src->x_segments;
	dst->y_segments  = src->y_segments;
	dst->z_segments  = src->z_segments;
	dst->size_x  = src->size_x;
	dst->size_y  = src->size_y;
	dst->size_z  = src->size_z;
	dst->window  = src->window;
	dst->size_x_element = src->size_x_element;
	dst->size_y_element = src->size_y_element;
	dst->size_z_element = src->size_z_element;
	dst->seg_x_element = src->seg_x_element;
	dst->seg_y_element = src->seg_y_element;
	dst->seg_z_element = src->seg_z_element;

	dst->text_seg_x_element = src->text_seg_x_element;
	dst->text_seg_y_element = src->text_seg_y_element;
	dst->text_seg_z_element = src->text_seg_z_element;
}

#endif