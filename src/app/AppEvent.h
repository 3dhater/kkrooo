#ifndef __KK_APP_EVENT_H__
#define __KK_APP_EVENT_H__

/*
Во время работы программы, будут появляться события.
Их нужно сохранять и потом (в конце фрейма или в начале) обрабатывать.
*/

#include "Classes/Math/kkVector4.h"

enum class GizmoPart : u32
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

enum class AppEventPriority : u32
{
	High,
	Medium,
	Low
};

enum class AppEventType : u32
{
	Default,
	GS,
	Gizmo
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

	_type type = _type::_default;
	GizmoPart part = GizmoPart::Default;

	v2i point2D; // должна вычисляться 2д точка 
	//kkVector4 point3D;

	void reset()
	{
		type = _type::_default;
		part = GizmoPart::Default;
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
		case AppEventType::GS:
			GS = e.GS;
			break;
		case AppEventType::Gizmo:
			gizmo = e.gizmo;
			break;
		default:
			break;
		}
	}

	~AppEvent(){}

	union
	{
		AppEvent_GS GS;
		AppEvent_gizmo gizmo;
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