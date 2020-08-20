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
	_high,
	_medium,
	_low
};

enum class AppEventType : u32
{
	_default,
	_gizmo,
};


struct AppEvent_gizmo
{

	enum class _type : u32
	{
		_default,
		_move,
		_rotate,
		_scale
	};

	enum class _part : u32
	{
		_default,
		_x,
		_y,
		_z,
		_xz_plane,
		_xy_plane,
		_zy_plane,
		_screen_plane
	};

	_type type = _type::_default;
	_part part = _part::_default;

	v2i point2D; // должна вычисляться 2д точка 
	//kkVector4 point3D;

	void reset()
	{
		type = _type::_default;
		part = _part::_default;
	}
};

struct AppEvent
{
	AppEvent(){}
	AppEvent(const AppEvent & e)
	{
		type = e.type;

		switch(type)
		{
		case AppEventType::_default:
			break;
		case AppEventType::_gizmo:
			gizmo = e.gizmo;
			break;
		default:
			break;
		}
	}

	~AppEvent(){}

	union
	{
		AppEvent_gizmo gizmo;
	};

	AppEventType type = AppEventType::_default;

	AppEvent& operator=(const AppEvent & e )
	{
		type = e.type;

		switch(type)
		{
		case AppEventType::_default:
			break;
		case AppEventType::_gizmo:
			gizmo = e.gizmo;
			break;
		default:
			break;
		}

		return *this;
	}
};


#endif