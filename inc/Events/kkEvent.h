// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_EVENT_H__
#define __KK_EVENT_H__

#include "Input/kkInputKey.h"

class kkWindow;

enum class kkEventSystemAction : u32
{
	None,
	Timer,
	Quit,
	BeginEventLoop,
	EndEventLoop,
};

enum class kkEventWindowAction : u32
{
	None,
	Sizing,
	Restore,
	Maximize,
	Minimize,
	Move,
	Paint,
	Size,
	Activate,
	Show,
	FocusSet
};
	
enum class kkEventType : u32 
{
	None,		
	Keyboard,
	Mouse,
	MouseWheel,
	Joystick,
	GUI,
	Window,
	System
};

enum class kkEventGUIAction : u32
{
	None,
	MouseEnter,
	MouseMove,
	MouseHover,
	MouseLeave,
	MouseLeftButtonDown,
	MouseLeftButtonUp,
	MouseLeftButtonDouble,
	MouseRightButtonDown,
	MouseRightButtonUp,
	MouseRightButtonDouble,
	MouseMiddleButtonDown,
	MouseMiddleButtonUp,
	MouseMiddleButtonDouble,
	MenuShow,
	MenuHide,
	MenuActive,
	MenuActivate //on click
};

struct kkEventGUI
{
	u32              id      = 0u;
	kkEventGUIAction    action  = kkEventGUIAction::None;
	//kkGUIObject*     object  = nullptr;
};

struct kkEventSystemEvent
{
	kkEventSystemAction action = kkEventSystemAction::None;
};
	
struct kkEventJoystick
{
	u32 joystickID           = 0u;
	u32 joystickEventID      = 0u;
	//kkInputDevice * joystick = nullptr;
};

struct kkEventWindow
{
	kkWindow *          window  = nullptr;
	kkEventWindowAction	eventID = kkEventWindowAction::None;
};


struct kkEventMouse
{
	
	enum kkEventMouseState
	{
		MS_LMB_DOWN   = BIT(0),
		MS_LMB_UP     = BIT(1),
		MS_LMB_DOUBLE = BIT(2),
		MS_RMB_DOWN   = BIT(3),
		MS_RMB_UP     = BIT(4),
		MS_RMB_DOUBLE = BIT(5),
		MS_MMB_DOWN   = BIT(6),
		MS_MMB_UP     = BIT(7),
		MS_MMB_DOUBLE = BIT(8),
		MS_X1MB_DOWN  = BIT(9),
		MS_X1MB_UP    = BIT(10),
		MS_X1MB_DOUBLE= BIT(11),
		MS_X2MB_DOWN  = BIT(12),
		MS_X2MB_UP    = BIT(13),
		MS_X2MB_DOUBLE= BIT(14)
	};

	kkEventMouse():state(0u){};

	u32 state;

	f32	x;	//< Cursor position on X
	f32 y;	//< Cursor position on Y

	int   wheel;	//< Wheel

	bool	isLeftButtonDown()    const { return (state&MS_LMB_DOWN);  }
	bool	isLeftButtonDouble()  const { return (state&MS_LMB_DOUBLE);}
	bool	isLeftButtonUp()	  const { return (state&MS_LMB_UP);    }
		
	bool	isRightButtonDown()   const { return (state&MS_RMB_DOWN);  }
	bool	isRightButtonDouble() const { return (state&MS_RMB_DOUBLE);}
	bool	isRightButtonUp()	  const { return (state&MS_RMB_UP);	 }
		
	bool	isMiddleButtonDown()  const { return (state&MS_MMB_DOWN);  }
	bool	isMiddleButtonDouble()const { return (state&MS_MMB_DOUBLE);}
	bool	isMiddleButtonUp()    const { return (state&MS_MMB_UP);	 }
		
	bool	isExt1ButtonDown()  const { return (state&MS_X1MB_DOWN); }
	bool	isExt1ButtonDouble()const { return (state&MS_X1MB_DOUBLE);}
	bool	isExt1ButtonUp()    const { return (state&MS_X1MB_UP);	 }
		
	bool	isExt2ButtonDown()  const { return (state&MS_X2MB_DOWN); }
	bool	isExt2ButtonDouble()const { return (state&MS_X2MB_DOUBLE);}
	bool	isExt2ButtonUp()    const { return (state&MS_X2MB_UP);	 }
		
};

struct kkEventKeyboard
{
	kkKey key = kkKey::K_NONE;
	char32_t character = 0;
		
	s8 state_is_pressed = 0;
	s8 state_is_relesed = 0;
	s8 state_is_ctrl  = 0;
	s8 state_is_shift = 0;
	s8 state_is_alt   = 0;

	bool isPressed( kkKey Key ) const {	return ((Key == key) && state_is_pressed == 1 );	}
	bool isReleased( kkKey Key )const {	return ((Key == key) && state_is_relesed == 1 );	}

	bool isCtrl()  {	return state_is_ctrl  == 1;					}
	bool isShift() {	return state_is_shift == 1;					}
	bool isAlt()   {	return state_is_alt   == 1;					}

private:
	kkEventKeyboard(){}
};

		
struct kkEvent
{

	kkEvent(){};

	union
	{
		kkEventGUI		GUIEvent;
		kkEventKeyboard	keyboardEvent;
		kkEventMouse	mouseEvent;
		kkEventWindow	windowEvent;
		kkEventJoystick joystickEvent;
		kkEventSystemEvent systemEvent;
	};

	kkEventType type = kkEventType::None;
		
	u32 dataSize	= 0u;
	void* data		= nullptr;

};
	
	// like event receiver in irrlicht
class kkEventConsumer
{
public:			
	virtual void processEvent( const kkEvent& ev ) = 0;
};

#endif