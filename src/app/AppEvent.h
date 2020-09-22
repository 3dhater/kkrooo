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
	GS,
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
		default:
			break;
		}
	}

	~AppEvent(){}

	union
	{
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
		default:
			break;
		}

		return *this;
	}
};


#endif