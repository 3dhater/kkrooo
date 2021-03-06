#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <exception>
#include <stdexcept>
#include <memory>

// need
#include "window_callbacks.h"

#include "KrGui.h"
#include "ApplicationState.h"
#include "AppEvent.h"
#include "GUI/MainMenuCommands.h"
#include "Plugins/PluginManager.h"
#include "Classes/Common/kkPtr.h"
#include "MainSystem/kkMainSystem.h"
#include "SceneSystem/kkSceneSystem.h"
#include "Events/kkEventSystem.h"
#include "Input/kkInputSystem.h"
#include "SceneSystem/kkSceneSystem.h"
#include "Window/kkWindow.h"
#include "GraphicsSystem/kkGraphicsSystem.h"
#include "GUI/ColorTheme.h"
//#include "Viewport/Viewport.h"
#include "GUI/GUIResources.h"


#define KKROOO kkSingleton<Application>::s_instance

using AppCallback = void(*)(s32 id, void* data);

enum class EditMode
{
	Object,
	Vertex,
	Edge,
	Polygon
};

enum class SelectMode
{
	JustSelect,
	Move,
	Rotate,
	Scale
};

enum class RightTabMode
{
	Create,
	Edit,
	UVEdit
};

class Application
{
	kkCreator               m_engine_creator; // инициализирует kkrooo.engine.dll
	kkPtr<kkWindow>         m_mainWindow;
	kkPtr<kkWindow>         m_materialEditorWindow; //bool m_showMaterialEditorWindow = false;
	kkPtr<kkWindow>         m_renderWindow;
	kkPtr<kkWindow>         m_importExportWindow;
	kkPtr<kkWindow>         m_shortcutEditorWindow;

	PluginGUIWindow*     m_importExportGUIWindow = nullptr;

	Kr::Gui::Style m_commonGUIStyle;
	Kr::Gui::Style m_mainMenuStyle;


	kkPtr<kkGraphicsSystem> m_gs;
	kkPtr<ShortcutManager>  m_shortcutManager;
	kkPtr<Gizmo>            m_gizmo;

	PluginManager           m_plugin_manager;

	kkPtr<MaterialEditor>   m_materialEditor;
	kkPtr<RenderManager>    m_renderManager;

	bool m_globalInputBlock = false;

	kkPtr<GUIResources>     m_guiResources ; // это стоит ниже m_gs значит уничтожится первее чем m_gs

	kkPtr<Viewport> m_mainViewport;

	void _init_OS_Windows();
	void _init_events();
	void _init_scene();
	void _init_shortcuts();
	void _init_materialEditor(bool);
	void _init_plugins();
	void _init_mainWindow();
	//void _init_materialEditorWindow();
	void _init_renderWindow();
	void _init_gs();
	void _init_input();
	void _init_GUIResources();
	void _init_krgui();
	void _init_renderManager();
	void _init_viewports(ViewportLayoutType);
	void _init_shortcutEditor();

	void _draw_shortcutEditor();

	Kr::Gui::GuiSystem * m_KrGuiSystem = nullptr;
	Kr::Gui::Font * m_smallFont = nullptr;
	Kr::Gui::Font * m_microFont = nullptr;
	Kr::Gui::Font * m_iconsFont = nullptr;
	Kr::Gui::Window m_guiMainWindow;
	Kr::Gui::Window m_guiMaterialEditorWindow;
	Kr::Gui::Window m_guiRenderWindow;
	Kr::Gui::Window m_guiImportExportWindow;
	Kr::Gui::Window m_guiShortcutWindow;

	bool _initFonts();
    
	// индекс категории
	// должен быть установлен по умолчанию стандартный плагин
	// ...та часть где идёт выбор какой объект создать...
	u64 m_selectedCategoryIndex = 0;
	void _setStandartObjectCategory();
	kkArray<PluginObjectCategory *> m_objectGeneratorCategories;


	std::unique_ptr<EventConsumer>   m_event_consumer;

	kkPluginGUIWindow * m_edit_params_window = nullptr;
	void _initEditParamsWindow();


	void _showRightTab(bool);
	bool m_showRightTab = false;
	RightTabMode m_rightTabMode = RightTabMode::Create;
	void _setRightTabMode(RightTabMode);

	kkMainSystem  * m_main_system  = nullptr;
	kkEventSystem * m_event_system = nullptr;
	kkSceneSystem * m_scene_system = nullptr;
	kkInputSystem * m_input_system = nullptr;

	E_WINDOW_ID m_activeOSWindow = E_WINDOW_ID::EWID_MAIN_WINDOW;

	ColorTheme m_current_color_theme;

	kkPtr<kkTexture> m_whiteTexture;
	void _createWhiteTexture();

	// когда курсор вне вьюпорта, или внутри но на GUI элементе (который поверх вьюпорта)
	bool m_cursorInGUI = false;

	f32 m_mainMenuHeight = 20.f;
	f32 m_mainToolBarHeight = 22.f;
	f32 m_leftToolBarWidth = 22.f;
	f32 m_bottomAreaHeight = 22.f;
	Kr::Gui::Style m_guiStyle_mainToolbarButtons;
	void _drawMainToolBar();
	void _drawLeftToolBar();
	void _drawRightToolBar();
	void _drawRightToolBar_createTab(float x, float y);
	void _drawRightToolBar_editTab(float x, float y);
	void _drawMainMenuBar();
	void _updateColorTheme(); /// set colors

	void _callOnActivateGUIPlugin();

	void _deleteSelectedObjects();

	kkArray<kkRenderer*> m_renderers;
	kkRenderer* m_activeRenderer = nullptr;

	v4i m_window_client_rect;
	v2i m_window_client_size;
	v2i m_window_size;

	AppState_main       m_state_app       = AppState_main::Idle;
	AppState_keyboard   m_state_keyboard  = AppState_keyboard::None;
	EditMode            m_editMode        = EditMode::Object;
	SelectMode          m_selectMode      = SelectMode::JustSelect;

	float m_mouseWheel = 0.f;
	v2i m_cursor_position;
	v2i m_cursor_position_firstClick; // сохраняется при LMB down once
	

	void _updateKeyboard();

	GeometryCreator*      m_geomCreator = nullptr;
	PluginGUI*            m_pluginGUI   = nullptr;

	Scene3D*              m_current_scene3D = nullptr;
	bool m_need_to_save = false;

	PluginCommonInterface * m_plugin_interface = nullptr;
	
	bool m_drawAllEvent = false;
	kkList<AppEvent> m_appEvents[3];

	// Для того чтобы рисовать ГУИ из плагинов нужно выйти из рисования менюшки.
	// Главное меню активно в момент рисования, диалог импорта\экспорта работает тоже в этот момент
	// Эти команды выполняются вне зоны рисования!!!
	MainMenuCommandInfo   m_mainMenuCommand;
	void                  _processMainMenuCommand();
	void                  _processMainMenuCommand_exportModel();
	void                  _processMainMenuCommand_importModel();
	void                  _processMainMenuCommand_importModelDirectly();
	void                  _processMainMenuCommand_importModelWithFileName();
	void                  _processMainMenuCommand_newScene();
	

	bool      m_isClearCanvas = false;

	bool   m_debug_drawSelectedObjectsAabb  = false;
	bool   m_debug_drawSceneAabb  = false;
	bool   m_debug_drawObjectAabb = false;
	bool   m_debug_drawObjectObb  = false;
	bool   m_debug_draw2DPoints   = false;

	bool   m_isDrawViewportBorders = true;
    
	bool m_useBackFaceCulling = false;

	bool m_isLocalRotation = false;
	bool m_isLocalScale    = false;

	bool   m_drawSelectByNameWindow = false;
    void   _drawSelectByNameWindow();
	
	bool   m_drawPreferencesWindow = false;
    void   _drawPreferencesWindow();

	bool   m_drawImportByPluginWindow = false;
    void   _drawImportByPluginWindow();

	bool   m_drawPivotToolWindow = false;
    void   _drawPivotToolWindow();

	//void _resetViewports();
	void _processShortcuts();

	bool m_minimized = false;


	void _openImportWindowWithAllTypes();

	kkStringW m_lastFilePath;

	void _onEndFrame();

	// когда берётся элемент гизмо, состояние сохраняется сюда
	// используется в трансформациях
	AppEvent_gizmo m_currentGizmoEvent;

    kkString m_programName;
    kkString m_sceneName;
    kkString m_sceneFilePath;

	f32 * m_deltaTime = nullptr;

	friend class EventConsumer;
	friend class ViewportObject;
	friend class Viewport;
	friend class Scene3D;
	friend class Gizmo;
	friend class PluginGUI;
	friend class PluginGUIWindow;
	friend class MaterialEditor;
	friend class RenderManager;

	friend void window_onSize(kkWindow* window);
	friend void window_onMove(kkWindow* window);
	friend void window_onPaint(kkWindow* window);
	friend void window_onClose(kkWindow* window);
	friend void Application_editParamsWindow_onActivate(s32 id, void* data);
	

	const char * m_tootipText = nullptr;

	bool m_drawInfoWindow = false;
	void _drawInfoWindow();
	void _applyMatrices();
	void _resetMatrices();
	void _clearAppEvents();

	void _drawMainMenu_ObjectCreatorCategories();

	v2f m_rightPartSize;

	bool m_enterTextMode = false;

	Plugin * m_OBJplugin = nullptr;

#ifdef KK_PLATFORM_WINDOWS
    IFileSaveDialog * m_fileSaveDialog = nullptr;
#endif

	bool m_objectPickMode = false;
	kkScene3DObject* m_objectPicked = nullptr;
	void (*m_objectPickCallback)(s32 id, void* data) = nullptr;

	bool m_vertexPickMode = false;
	kkVertex* m_vertexPicked = nullptr;
	void (*m_vertexPickCallback)(s32 id, void* data) = nullptr;

	//bool m_drawPickLine = false;
	ViewportObject* m_activeViewport = nullptr;
public:

	Application();
	~Application();
	
	void blockGlobalInput(bool v){m_globalInputBlock=v;}
	bool isGlobalInputBlocked(){return m_globalInputBlock;}
	void setActiveRenderer(kkRenderer*);
	void addRenderer(kkRenderer* renderer){m_renderers.push_back(renderer);}
	kkRenderer* getActiveRenderer(){return m_activeRenderer;}
	void addAppEvent( const AppEvent&, AppEventPriority );
	void init();
	void run();
	void drawBegin();
	void drawEnd();
	void drawViewport();
	void clearColor(bool);

	void save();

	const v2f& getRightPartSize()
	{
		return m_rightPartSize;
	}
	
	void onWindowActivate();
	void onWindowMinimize();
	void onWindowRestore();
	void onWindowMaximize();

	/*Viewport * getActiveViewport()
	{
		return m_active_viewport;
	}*/

	void drawAll(bool force);
	void onWindowSize();
	void drawToolTip(const char*);
	void setSelectMode( SelectMode );
	void setEditMode( EditMode );
	EditMode getEditMode(){ return m_editMode; }
	SelectMode getSelectMode(){ return m_selectMode; }

	PluginManager* getPluginManager()
	{
		return &m_plugin_manager;
	}

	PluginCommonInterface* getPluginCommonInterface()
	{
		return m_plugin_interface;
	}

	void quit();
	void updateBuffers();
	void updateInput();
	void updateViewports();
	void setActiveViewport(Viewport* v);
	void setStateKeyboard(AppState_keyboard s);
	AppState_keyboard getStateKeyboard();
	void resetScene3D();
	Scene3D** getScene3D();
	void setNeedToSave( bool v );
	void updateWindowTitle();
	bool importModel(Plugin* pl);
	bool exportModel(Plugin* pl);
	kkGraphicsSystem* getGS();
	EventConsumer *   getEventConsumer();

	kkString getOpenFileNameForImages();
	kkString getSaveFileNameForImages();
	kkString getSaveFileName(const char16_t* file_desc, const char16_t* ext);
	kkString getOpenFileName(const char16_t* file_desc, const char16_t* ext);
	kkImage* loadImageFromDialog();
	kkImage* loadImage(const char16_t*);

	const v2i& getWindowClientSize();
	const v2i& getWindowSize();

	bool debugIsDrawSelectedObjectsAabb()  { return m_debug_drawSelectedObjectsAabb;  }
	bool debugIsDrawSceneAabb()  { return m_debug_drawSceneAabb;  }
	bool debugIsDrawObjectAabb() { return m_debug_drawObjectAabb; }
	bool debugIsDrawObjectObb()  { return m_debug_drawObjectObb;  }

	bool isDrawViewportBorders() { return m_isDrawViewportBorders; }

	kkPtr<ShaderSimple>     m_shaderSimple;
	kkPtr<ShaderPoint>      m_shaderPoint;
	kkPtr<ShaderLineModel>  m_shaderLineModel;
	//kkPtr<ShaderScene3DObjectSilhouette>   m_shader3DObjectSilhouette;
	kkPtr<ShaderScene3DObjectDefault>      m_shader3DObjectDefault;
	kkPtr<ShaderScene3DObjectDefault_polymodeforlinerender>      m_shader3DObjectDefault_polymodeforlinerender;
	bool _createShaders();

	void showMaterialEditor(bool);
	void showRenderWindow(bool);
	void openImageFilePathDialog(kkString* outFilePath);

	kkPluginObjectCategory*  createObjectCategory( const char16_t* name, const kkPluginID& id );

	kkTexture* getProjTexture();
	kkTexture* getWhiteTexture();
	void saveImageToFile(kkImage*);

	bool isWindowActive(E_WINDOW_ID);
	
	void showImportExportWindow(PluginGUIWindow*, const v2i& size, const char16_t* title);
	void convertSelectedObjectToPolygonalObject();
	bool isSelectedObjectNeedConvert();

	void setObjectPickMode(void(*callback)(s32 id, void* data));
	kkScene3DObject* getPickedObject();
	
	void setVertexPickMode(void(*callback)(s32 id, void* data));
	kkVertex* getPickedVertex();

	void EnableVertexTool(AppCallback onSelect, AppCallback onCancel, AppCallback onAccept);
	AppCallback m_vertexToolCallback_onSelect = nullptr;
	AppCallback m_vertexToolCallback_onCancel = nullptr;
	AppCallback m_vertexToolCallback_onAccept = nullptr;
	bool m_vertexTool = false;

	void setDrawPickLine(bool);
	void GSSetDepth(bool v);
	void GSSetViewport(s32 x, s32 y, s32 z, s32 w);
	bool IsLmbDownOnce();
	bool IsLmbDown();
	bool IsLmbUp();
	bool IsRmbDownOnce();
	bool IsRmbDown();
	bool IsRmbUp();
	bool IsMmbDownOnce();
	bool IsMmbDown();
	bool IsMmbUp();
	void DrawAllEvent();
	v2i* GetCursorPosition();
	AppState_keyboard* GetAppState_keyboard();
	AppState_main* GetAppState_main();
	bool * GetGlobalInputBlock();
	void* GetGUI();
	bool IsKeyDown(kkKey k);
	ShortcutManager* GetShortcutManager();
	void GSDrawModel(kkMesh* mesh, const kkMatrix4& mat, const kkColor& difCol, kkImageContainerNode* m_diffTex, bool isSelected);
	void GSDrawModelLineModePolyEdit(kkMesh* mesh,const kkMatrix4& mat);
	void GSDrawModelEdge(kkMesh*,const kkMatrix4&, const kkColor& edgeCol);
	void GSDrawObb( const kkObb& obb, const kkColor& color);
	void GSDrawAabb( const kkAabb& aabb, const kkColor& color);
	void GSDrawModelSilhouette(kkMesh* mesh,const kkMatrix4& mat, const kkColor& difCol);
	void GSSetTarget(kkTexture* fbo);
	void GSDrawRectangle(const v2i& LT, const v2i& RB, const kkColor& color, kkTexture* t );
	void SetAppStateMain(AppState_main s);
	void GSSetScissor(bool, const v4i&);
	void GSDrawLine2D(const v2i& p1, const v2i& p2, const kkColor& color);
	ViewportObject* GetActiveViewport();
	void SetActiveViewport(ViewportObject* v);
	void GSDrawModelPoint(kkMesh* mesh,const kkMatrix4& mat);
	EditMode* GetEditMode();
	SelectMode* GetSelectMode();
	EventConsumer* GetEventConsumer();
};


#endif