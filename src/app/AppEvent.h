// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_APP_EVENT_H__
#define __KK_APP_EVENT_H__

/*
Во время работы программы, будут появляться события.
Их нужно сохранять и потом (в конце фрейма или в начале) обрабатывать.
*/

#include "Classes/Math/kkVector4.h"

enum class AppEventPriority : u32
{
	High,
	Medium,
	Low
};

enum class AppEventType : u32
{
	Default,
	Gizmo,
	GS,
};


struct AppEvent_gizmo
{
	enum class _type : u32
	{
		Default,
		Move,
		Rotate,
		Scale
	};

	enum class _part : u32
	{
		Default,
		X,
		Y,
		Z,
		XZ_plane,
		XY_plane,
		ZY_plane,
		Screen_plane
	};

	_type type = _type::Default;
	_part part = _part::Default;

	v2i point2D; // должна вычисляться 2д точка 
	//kkVector4 point3D;

	void reset()
	{
		type = _type::Default;
		part = _part::Default;
	}
};
struct AppEvent_GS
{
	enum class _type : u32
	{
		DrawAll
	};
	_type type = _type::DrawAll;
};
struct AppEvent
{
	AppEvent(){}
	AppEvent(const AppEvent & e)
	{
		type = e.type;

		switch(type)
		{
		case AppEventType::Default:
			break;
		case AppEventType::Gizmo:
			gizmo = e.gizmo;
			break;
		case AppEventType::GS:
			GS = e.GS;
			break;
		default:
			break;
		}
	}

	~AppEvent(){}

	union
	{
		AppEvent_gizmo gizmo;
		AppEvent_GS GS;
	};

	AppEventType type = AppEventType::Default;

	AppEvent& operator=(const AppEvent & e )
	{
		type = e.type;

		switch(type)
		{
		case AppEventType::Default:
			break;
		case AppEventType::GS:
			GS = e.GS;
			break;
		case AppEventType::Gizmo:
			gizmo = e.gizmo;
			break;
		default:
			break;
		}

		return *this;
	}
};


#endif