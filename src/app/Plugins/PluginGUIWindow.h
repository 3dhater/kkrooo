// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_GUI_WINDOWIMPL_H__
#define __PLUGIN_GUI_WINDOWIMPL_H__

#include "Plugins/kkPluginGUI.h"

#include "Classes/Containers/kkArray.h"

enum class PluginGUIWindowElementType
{
	Default,
	Button,
	NewLine,
	Text,
	MoveLeftRight,
	RangeSlider,
	RangeSliderInteger,
	CheckBox,
	ValueSelectorFloat,
	TextInput,
};

struct PluginGUIWindowElementGroup
{
	kkString m_text;
	bool m_expanded = true;
};

class PluginGUIWindowElement : public kkPluginGUIWindowElement
{
public:

	PluginGUIWindowElement()
	{
	}

	virtual ~PluginGUIWindowElement()
	{
	}
	
	virtual void SetText( const char16_t* fmt, ... );
	virtual void SetPointerFloat( float * float_ptr );
	virtual void SetPointerInt( int * int_ptr );


	v2f      m_size;
	kkString m_text;
	PluginGUIWindowElementType m_type = PluginGUIWindowElementType::Default;
	kkPluginGUIParameterType m_paramType = kkPluginGUIParameterType::Object;

	kkPluginGUICallback       m_callback = nullptr;
	bool(*m_textInputFilter)(char16_t) = nullptr;
	void(*m_textInputResult)(const char16_t*) = nullptr;
	s32  m_id  = -1;

	bool m_horizontal = true;
	f32 * m_float_ptr = nullptr;
	s32 * m_int_ptr = nullptr;
	f32 m_minimum = 0.f;
	f32 m_maximum = 0.f;
	s32 m_minimum_int = 0;
	s32 m_maximum_int = 0;
	f32 m_offset1 = 0.f;
	f32 m_speed = 0.f;
	Kr::Gui::Vec4f m_gui_color;

	bool * m_checkbox_ptr = nullptr;

	PluginGUIWindowElementGroup* m_group_ptr = nullptr;
};

class Application;
class PluginGUIWindow : public kkPluginGUIWindow
{
	kkPluginGUIWindowType m_type = kkPluginGUIWindowType::Custom;

	Application * m_app = nullptr;

	kkArray<PluginGUIWindowElement*> m_guiElements;
	kkString m_name;
	
	v2i m_size;
	v2i m_position;
	
	//PyObject* m_py_onOK = nullptr;

	// становится активным только если нажали на импорт\экспорт и т.д.
	bool m_isActive = false;

	kkPluginGUICallback m_onOK = nullptr;
	kkPluginGUICallback m_onActivate = nullptr;

	void * m_userData = nullptr;
	
	PluginGUIWindowElementGroup* m_currentGroup = nullptr;
	std::vector<PluginGUIWindowElementGroup*> m_groupsForDelete;

	static int s_windowCounter;

	friend class PluginGUI;
	friend class PluginCommonInterface;

public:
	PluginGUIWindow();
	virtual ~PluginGUIWindow();

	void SetType( kkPluginGUIWindowType type );
	void SetName( const char16_t* name );
	const char16_t* GetName();
	void SetSize( const v2i& s );
	void Reset();
	void Activate();
	void Deactivate();
	void SetUserData( void * );
	
	void SetOnOK( kkPluginGUICallback );
	void SetOnActivate( kkPluginGUICallback );
	

	kkPluginGUIWindowElement* AddButton( const char16_t* text, const v2f& size, kkPluginGUICallback, s32 id, kkPluginGUIParameterType pt );
	kkPluginGUIWindowElement* AddNewLine( f32 Y_offset, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddText( const char16_t* text, u32 color_argb, f32 text_Y_offset, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddMoveLeftRight( f32 value, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddRangeSliderFloat( f32 minimum, f32 maximum, f32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddRangeSliderInt( s32 minimum, s32 maximum, s32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddCheckBox( const char16_t* text, bool* ptr, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddValueSelectorFloat( f32 * ptr, f32 speed, bool horizontal, const v2f& size, kkPluginGUICallback cb, kkPluginGUIParameterType pt);
	kkPluginGUIWindowElement* AddTextInput( const char16_t* text, const v2f& size, bool(*filter)(char16_t), void(*textInputResult)(const char16_t*), kkPluginGUIParameterType pt);

	void BeginGroup(const char16_t* text, bool expanded);
	void EndGroup();

	void draw();
	
	void callOnActivate();
};

#endif
