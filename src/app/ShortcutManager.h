// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SHORTCUT_MANAGER_H__
#define __SHORTCUT_MANAGER_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Input/kkInputKey.h"
#include "ApplicationState.h"

// после изменения этого нужно так-же изменить массив со строками для GUI
enum class ShortcutCommandCategory : u32
{
	General,
	Viewport,
	Camera,
	Edit,

	_End
};

enum class ShortcutCommand_General : u32
{
	ShowMaterialEditor,
	ShowRenderWindow,
	New,
	Save,
	SaveAs,
	Open,

	_End
};

enum class ShortcutCommand_Viewport : u32
{
	SetPerspective,
	SetFront,
	SetBack,
	SetTop,
	SetBottom,
	SetLeft,
	SetRight,
	ToggleGrid,
	Maximize,
	DrawModeLines,
	DrawModeMaterial,
	DrawModeMaterialAndLines,
	ToggleDrawModeMaterial,
	ToggleDrawModeLines,

	_End
};


enum class ShortcutCommand_Camera : u32
{
	Reset,
	ToSelection,

	_End
};

enum class ShortcutCommand_Edit : u32
{
	Undo,
	Redo,
	SelectAll,
	DeselectAll,
	SelectInvert,
	SelectModeJustSelect,
	SelectModeMove,
	SelectModeRotate,
	SelectModeScale,
	EnterTransformation,
	EditModeObject,
	EditModeVertex,
	EditModeEdge,
	EditModePolygon,

	_End
};

struct ShortcutCommandDesc
{
	ShortcutCommandCategory  category   = ShortcutCommandCategory::_End;
	u32 id = 0;
	const char * name = "";
};

struct ShortcutCommandNode
{
	AppState_keyboard  keyboardState = AppState_keyboard::None;
	kkKey              key           = kkKey::K_NONE;
	s32               onRelease     = 0;
	kkString          title;

	// не идёт на запись
	// каждая итерация должна проверять нажатия для всех типов команд
	// если условия соблюдены (удержания определённых кнопок), то истина, иначе ложь
	bool isReady = false;
	bool isUsed  = false; //чтобы не было повтора из за удержания
};

class Application;
class kkGraphicsSystem;
class EventConsumer;
struct kkXMLNode;

class ShortcutManager
{
	Application   * m_app = nullptr;
	EventConsumer * m_eventConsumer = nullptr;

	ShortcutCommandDesc m_cammandDesc_General[(u32)ShortcutCommand_General::_End];
	ShortcutCommandDesc m_cammandDesc_Viewport[(u32)ShortcutCommand_Viewport::_End];
	ShortcutCommandDesc m_cammandDesc_Camera[(u32)ShortcutCommand_Camera::_End];
	ShortcutCommandDesc m_cammandDesc_Edit[(u32)ShortcutCommand_Edit::_End];
	const char * m_imguiComboText_General[(u32)ShortcutCommand_General::_End];
	const char * m_imguiComboText_Viewport[(u32)ShortcutCommand_Viewport::_End];
	const char * m_imguiComboText_Camera[(u32)ShortcutCommand_Camera::_End];
	const char * m_imguiComboText_Edit[(u32)ShortcutCommand_Edit::_End];

	kkArray<ShortcutCommandNode> m_cammandNodes_General;
	kkArray<ShortcutCommandNode> m_cammandNodes_Viewport;
	kkArray<ShortcutCommandNode> m_cammandNodes_Camera;
	kkArray<ShortcutCommandNode> m_cammandNodes_Edit;

	friend class Application;

	kkString m_inputBuffer;
	const char16_t* _getKeyString( kkKey key );

	ShortcutCommandNode * _getNode(u32 i1, u32 i2);
	void _save();
	void _load(const char16_t*);
	void _load();

	void _setUpXMLNode(kkArray<ShortcutCommandNode>& , const char16_t* , kkXMLNode*);
	void _readXMLNodes(const kkArray<kkXMLNode*>& xmlnodes, kkArray<ShortcutCommandNode>& );


public:
	ShortcutManager();
	~ShortcutManager();

	bool init();
	void onFrame();

	void draw(bool*);

	const char16_t* getShortcutText( ShortcutCommand_General );
	const char16_t* getShortcutText( ShortcutCommand_Viewport );
	const char16_t* getShortcutText( ShortcutCommand_Camera );
	const char16_t* getShortcutText( ShortcutCommand_Edit );
	bool        isShortcutActive( ShortcutCommand_General/*, bool down_once = false*/ );
	bool        isShortcutActive( ShortcutCommand_Viewport/*, bool down_once  = false*/ );
	bool        isShortcutActive( ShortcutCommand_Camera/*, bool down_once  = false*/ );
	bool        isShortcutActive( ShortcutCommand_Edit/*, bool down_once  = false*/ );
};


#endif