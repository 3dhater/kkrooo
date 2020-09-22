#ifndef __APPLICATION_STATE_H__
#define __APPLICATION_STATE_H__

/*
	Состояния программы. Каждое действие должно изменять состояние. 
*/
enum class AppState_main : u32
{
	Idle,
	MainMenu,
	CameraTransformation,

	//когда нажимается гизмо, включается это состояние
	Gizmo, 

	SelectRectangle,

	// когда нажали правую кнопку мыши в моменте когда объект трансформируется с помощью гизмо
	CancelTransformation, /// вроде не нужно...

};

/// Зажали
enum class AppState_keyboard : u32
{
	None,
	Ctrl,
	Alt,
	Shift,
	ShiftAlt,
	ShiftCtrl,
	ShiftCtrlAlt,
	CtrlAlt,

	END
};


#endif