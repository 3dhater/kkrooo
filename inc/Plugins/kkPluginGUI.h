// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PL_GUI_H__
#define __KK_PL_GUI_H__

#include "Classes/Math/kkVector4.h"

// тип окна
enum class kkPluginGUIWindowType
{
	// Не дочерние окна.
	Custom,   // без готовых параметров. Default
	Import,   // для импорта модели. Окно будет иметь кнопку с надписью Import
	Export,   // для экспорта модели. Окно будет иметь кнопку с надписью Export

	// Дочерниче окна
	Material,   // для материалов
	Parameters  // для параметров 
};

// при заполнении окна типа _parameters
// надо указывать тот режим редактирования, при котором этот элемент будет видимым.
enum class kkPluginGUIParameterType
{
	Object,
	Vertex,
	Edge,
	Polygon
};

using kkPluginGUICallback = void(*)(s32 id, void* data);

class kkPluginGUIWindowElement
{
public:
	kkPluginGUIWindowElement(){}
	virtual ~kkPluginGUIWindowElement(){}

	virtual void SetText( const char16_t* fmt, ... ) = 0;
	virtual void SetPointerFloat( float * ) = 0;
	virtual void SetPointerInt( int * ) = 0;
};

class kkPluginGUIWindow
{
public:
	kkPluginGUIWindow(){}
	virtual ~kkPluginGUIWindow(){}

	// установка типа окна
	virtual void SetType( kkPluginGUIWindowType type ) = 0;

	// удалит все контролы и прочее
	virtual void Reset() = 0;

	// установит имя и title окна.
	virtual void SetName( const char16_t* name ) = 0;
	virtual const char16_t* GetName() = 0;

	// размер
	virtual void SetSize( const v2i& s ) = 0;

	// показать окно
	virtual void Activate() = 0;

	// скрыть окно
	virtual void Deactivate() = 0;

	// установить данные чтобы потом был доступ в коллбэках
	virtual void SetUserData( void * ) = 0;

	// установить callback функции
	// в параметрах коллбэк функций должны стоять пользовательские данные установленные ранее вызовом setUserData
	virtual void SetOnOK( kkPluginGUICallback ) = 0;                       // вызывается при импорте/экспорте/нажатии на OK

	// в различных случаях будет необходимо вызвать 1 раз функцию чтобы произвести
	// некоторые установки внутри плагина
	// например, это вызывается когда выделен объект и открывается панель
	// с параметрами объекта, тогда, в окне должны быть установлены 
	// настоящие параметры текущего объекта.
	virtual void SetOnActivate( kkPluginGUICallback ) = 0;

	// добавить кнопку. коллбэк подобный как в setOnOK - вызывается при нажатии
	virtual kkPluginGUIWindowElement* AddButton( const char16_t* text, const v2f& size, kkPluginGUICallback, s32 id, kkPluginGUIParameterType pt ) = 0;
	// перейти на новую строку. Y_offset - отступ, перейти выше или ниже
	virtual kkPluginGUIWindowElement* AddNewLine( f32 Y_offset, kkPluginGUIParameterType pt ) = 0;
	virtual kkPluginGUIWindowElement* AddText( const char16_t* text, u32 color_argb, f32 text_Y_offset, kkPluginGUIParameterType pt ) = 0;
	// переместить текущую позицию рисование влево или вправо.
	virtual kkPluginGUIWindowElement* AddMoveLeftRight( f32 value, kkPluginGUIParameterType pt ) = 0;
	virtual kkPluginGUIWindowElement* AddRangeSliderFloat( f32 minimum, f32 maximum, f32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt) = 0;
	virtual kkPluginGUIWindowElement* AddRangeSliderInt( s32 minimum, s32 maximum, s32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt) = 0;
	virtual kkPluginGUIWindowElement* AddCheckBox( const char16_t* text, bool* ptr, kkPluginGUIParameterType pt) = 0;

	// начать группу. элементы добавленные между BeginGroup и EndGroup
	//   будут сгруппированы. Можно скрывать элементы и показывать опять
	virtual void BeginGroup(const char16_t* text, bool expanded) = 0;
	virtual void EndGroup() = 0;

	// грубо говоря это обёртка для imgui
	//virtual kkPluginGUIWindowElement* addDummy(float x, float y) = 0;
	//virtual kkPluginGUIWindowElement* addSmallButton( const char* label, kkPluginGUICallback, s32 id ) = 0;
	//virtual kkPluginGUIWindowElement* addArrowButton( const char* str_id, int dir/*0123*/, kkPluginGUICallback, s32 id ) = 0;
	//virtual kkPluginGUIWindowElement* addCheckbox(const char* label, bool* v) = 0;
	//virtual kkPluginGUIWindowElement* addRadioButton(const char* label, int* v, int v_button) = 0;
	//virtual kkPluginGUIWindowElement* addSeparator() = 0;
	//virtual kkPluginGUIWindowElement* addText(const char* text) = 0;
	//virtual kkPluginGUIWindowElement* sameLine( float offset = 0.f ) = 0; //расположить следующий элемент на той-же линии
	//virtual kkPluginGUIWindowElement* treeBegin(const char* name, bool open) = 0;
	//virtual kkPluginGUIWindowElement* treeEnd() = 0;
	//virtual kkPluginGUIWindowElement* addProgressbar(float* v) = 0;
	//virtual kkPluginGUIWindowElement* childBegin(const char* name, const v2f& size, bool borders ) = 0;
	//virtual kkPluginGUIWindowElement* childEnd() = 0;
	//virtual kkPluginGUIWindowElement* tabbarBegin(const char* name) = 0;
	//virtual kkPluginGUIWindowElement* tabbarEnd() = 0;
	//virtual kkPluginGUIWindowElement* tabbarItemBegin(const char* name) = 0;
	//virtual kkPluginGUIWindowElement* tabbarItemEnd() = 0;
	//virtual kkPluginGUIWindowElement* addDragFloat( float width, const char* label, float* v, kkPluginGUICallback, float v_speed = 1.0f, float v_min = 0.f, float v_max = 0.f ) = 0;
	//virtual kkPluginGUIWindowElement* addDragInt( float width, const char* label, int* v, kkPluginGUICallback, float v_speed = 1.0f, int v_min = 0, int v_max = 0 ) = 0;
};

class kkPluginGUI
{
public:
	kkPluginGUI(){}
	virtual ~kkPluginGUI(){}

	// просто создаст окно
	// настроить окно нужно через созданный объект
	virtual kkPluginGUIWindow* createWindow() = 0;
	
};

#endif