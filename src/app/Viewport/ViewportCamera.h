#ifndef __VIEWPORT_CAMERA_H__
#define __VIEWPORT_CAMERA_H__

#include "../ApplicationState.h"
#include "../Functions.h"

enum class ViewportCameraType
{
	Perspective,
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom
};

class ViewportCamera
{
	friend class ViewportObject;

	bool   m_active = false;
	void * m_owner = nullptr;

	bool   m_isRotated   = false;

	ViewportCameraType m_type = ViewportCameraType::Perspective;

	kkCamera * m_kk_camera = nullptr;

	// камера которая нужна для рисования линий в углу вьюпорта
	// тоже-самое что основная камера, только она не двигается, а только вращается
	//kkCamera * m_kk_axisCamera = nullptr;

	enum _Camera
	{
		Base_     = 0,
		RotY_     = 1,
		RotX_     = 2,
		CamPos_   = 3,
		CamPos_Ax = 4, // для axis камеры. она должна оставаться на одной высоте - CamPos_ двигается вверх вниз для зума

		BaseMoveXP= 5, // эти объекты прикреплены к RotX_ .  При pan move, Base_ объект устанавливается на позицию нужного объекта
		BaseMoveXN= 6, // XY - оси, P - позитив, N - негатив.
		BaseMoveYP= 7, // избавимся от преобразования координаты курсора в точку в пространстве.
		BaseMoveYN= 8, // так-же будет проще настраивать скорость перемещения

		ObjectRotation = 9, // вращается по Y и постоянно повёрнут к камере. Чтобы крутиться 

		_number
	};

	kkDummy * m_camera_parts[_Camera::_number];

	/// всё вертится через set_orientation - поворот кватерниона...без сохранения углов
	/// Эта переменная будет хранить углы
	v4f m_all_rotation;
	f32 m_fov;

	void _destroy();

	// перемещение ортогональной камеры
	kkVector4   m_cameraPos_ort;
	f32  m_zoomOrt = 1.f;

	void _updatePanMoveSpeed(AppState_keyboard key);
	f32 m_panMoveSpeed_base = 0.001f;

public:

	ViewportCamera(ViewportCameraType type,void *);
	~ViewportCamera();
	void init();

	void reset();

	void update();
	kkCamera * getCamera();
	
	void setPositionBase(const kkVector4&);
	
	const kkVector4& getPositionBase();
	const kkVector4& getPositionCamera();

	/// получить значение которое изменяется зумированием (колесом мышки)
	f32 getCameraHeight();
	void setCameraHeight(f32);
	
	void rotateX( f32 x );
	void rotateY( f32 y );

	void zoomIn( AppState_keyboard key, s32 wheel_delta );
	void zoomOut( AppState_keyboard key, s32 wheel_delta );

	void setOwner( ViewportObject* );
	ViewportObject* getOwner();

	void setObjectRotationAngle(f32);
	kkMatrix4 getObjectRotationMatrix();

	ViewportCameraType getType()
	{
		return m_type;
	}

	const v4f& getAllRotation()
	{
		return m_all_rotation;
	}

	const kkVector4& getCameraPosition_ort(){return m_cameraPos_ort;}
	void setCameraPosition_ort(const kkVector4& p){m_cameraPos_ort= p;}

	/*kkCamera * getAxisCamera()
	{
		return m_kk_axisCamera;
	}*/

	f32 getZoomOrt()
	{
		return m_zoomOrt;
	}

	void movePan( AppState_keyboard key, f32, f32 );

	void centerToAabb( const kkAabb& aabb );
};

#endif