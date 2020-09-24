#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

enum class DrawMode : u32
{
	Edge,
	Material,
	EdgesAndMaterial
};

struct ViewportMouseState
{
	bool LMB_UP   = false;
	bool LMB_DOWN = false;
	bool LMB_HOLD = false;

	bool RMB_UP   = false;
	bool RMB_DOWN = false;
	bool RMB_HOLD = false;

	bool MMB_UP   = false;
	bool MMB_DOWN = false;
	bool MMB_HOLD = false;

	bool InViewport = false;
	bool IsMove = false;
	bool IsSelectByRect = false;
	bool IsFirstClick = false;

	void reset()
	{
		if( IsSelectByRect && LMB_HOLD )
		{
		}
		else
		{
			IsSelectByRect = false;
		}

		if( LMB_HOLD || RMB_HOLD || MMB_HOLD )
		{
		}else
			IsFirstClick = false;
		IsMove = false;
		InViewport = false;
		LMB_UP   = false;
		LMB_DOWN = false;
		LMB_HOLD = false;
		RMB_UP   = false;
		RMB_DOWN = false;
		RMB_HOLD = false;
		MMB_UP   = false;
		MMB_DOWN = false;
		MMB_HOLD = false;
	}
};

enum class ViewportLayoutType
{
	Single,
	ParallelHor,
	ParallelVer,
};

enum class ViewportType
{
	Main
};

// не хочу изобретать велосипеды, по этому всё будет просто
enum class ViewportUID
{
	Single,
	ParallelHorUp,
	ParallelHorDown,
	ParallelVerLeft,
	ParallelVerRight,
};

class ViewportObject
{
	void _panMove();
	void _rotate();
	void _update_frame(const v2f& mouseDelta);
	void _drawGrid_persp(ColorTheme* colorTheme);
	void _drawGridXZ(f32 limits, f32 step, ColorTheme* colorTheme);
	void _drawGridXY(f32 limits, f32 step, ColorTheme* colorTheme);
	void _drawGridZY(f32 limits, f32 step, ColorTheme* colorTheme);
public:
	ViewportObject();
	~ViewportObject();
	
	void init(const v4f& indent, ViewportLayoutType lt);
	void update(const v2i& windowSize);
	bool updateInput(const v2i& windowSize, const v2f& mouseDelta, bool inFocus);
	void updateInputCamera(const v2f& mouseDelta, bool inFocus);
	void updateCursorRay();
	void resetCamera();
	void processShortcuts();
	void setActiveCamera(ViewportCamera* c);

	ShortcutManager* m_shortcutManager = nullptr;

	bool * m_inputBlock = nullptr;
	CursorRay* m_cursorRay = nullptr;
	kkRay m_rayOnClick;
	v2i* m_windowSize = nullptr;

	//kkMesh* m_gridMesh = nullptr;
	//kkTexture* m_silhouetteFBO = nullptr;

	ViewportCamera* m_activeCamera = nullptr;
	kkPtr<ViewportCamera> m_cameraPersp ;
	kkPtr<ViewportCamera> m_cameraFront ;
	kkPtr<ViewportCamera> m_cameraBack  ;
	kkPtr<ViewportCamera> m_cameraTop   ;
	kkPtr<ViewportCamera> m_cameraBottom;
	kkPtr<ViewportCamera> m_cameraLeft  ;
	kkPtr<ViewportCamera> m_cameraRight ;

	ViewportLayoutType m_layoutType = ViewportLayoutType::Single;

	ViewportObject* m_left  = nullptr;
	ViewportObject* m_right = nullptr;
	
	v4f m_viewport_area;
	f32 m_gridStep = 0.f;
	bool m_isDrawGrid = true;
	
	DrawMode m_draw_mode = DrawMode::Material;
	void setDrawMode( DrawMode );
	void toggleDrawModeMaterial();
	void toggleDrawModeLines();
	void moveToSelection();


	v2i* m_cursor_position = nullptr;
	AppState_keyboard* m_appState_key = nullptr;

	v2i  m_mouse_first_click_coords;
	// изменение положения рамки
	// не всё так просто, нужно полностью изменять прямоугольные области каждого ViewportObject
	// универсально делать лень, по этому для каждого ViewportLayoutType нужно делать по своему
	// по сути m_framePosition это отступ
	v4f m_framePosition;

	// вычисляется и используется для рисования. реагирует на изменение размера окна
	v4f m_rect_modified;
	v4f m_gs_viewport;

	// origin означает что значения не изменятся при изменении размеров окна. они изменяются только при создании\уничтожении вьюпорта
	v4f m_rect_origin; /// left top right bottom /// 0;0 - левый ВЕРХНИЙ угол
	v2f m_window_size_origin;
	v4f m_orig_indent; // начальный отступ
	v2f m_resize_window_coef; // для увеличения или уменьшения нужно умножать значения
	v4f m_viewport_area_origin; /// зона вьюпорта. меньше чем окно. используется для вычисления m_resize_window_coef

	bool m_cursorInRect = false;

	ViewportUID m_uid = ViewportUID::Single;

	void beginDraw();
	//void drawSilhouette(const v2i& windowSize);
	void drawBG(const v2i& windowSize, ColorTheme* colorTheme);
	void drawGrid(ColorTheme* colorTheme);
	void drawScene(bool inFocus);
	void drawObjectPivot(bool inFocus);
	void drawSelectionRectangle(bool inFocus);
	void drawName(bool isActive);
	ViewportCamera * getActiveViewportCamera()
	{
		return m_activeCamera;
	}

	void drawGizmo3D();
	void drawGizmo2D();
};

class Viewport
{
	ViewportLayoutType m_layoutType = ViewportLayoutType::Single;
	ViewportType m_type = ViewportType::Main;

	ViewportObject* m_viewports = nullptr;
	ViewportObject* m_viewportInFocus = nullptr;
	void _init_single(const v4f& indent, ViewportLayoutType lt);
	void _init_parallel_h(const v4f& indent, ViewportLayoutType lt);
	void _init_parallel_v(const v4f& indent, ViewportLayoutType lt);

	kkWindow* m_window = nullptr;
	v2i m_windowSize;
public:
	Viewport(kkWindow*);
	~Viewport();

	void draw(ColorTheme* colorTheme);
	void update();
	void updateInput(const v2f& mouseDelta);
	void updateInputCamera(const v2f& mouseDelta);

	void init(ViewportType, ViewportLayoutType, const v4f& indent);
};

//#include <string>
//#include "Classes/Strings/kkString.h"
//
//#include "Classes/Math/kkRay.h"
//#include "Classes/Math/kkVector4.h"
//#include "../ApplicationState.h"
//
//class EventConsumer;
//class kkGraphicsSystem;
//class Application;
//struct ColorTheme;
//struct GUIResources;
//class ShortcutManager;
//class Viewport;
//

//
//enum class ViewportCommandType : u32
//{
//	None,
//	AddViewPort_Vert,
//	AddViewPort_Hor,
//	RemoveViewPort_Vert,
//	RemoveViewPort_Hor
//};
//
//struct ViewportCommand
//{
//	Viewport* m_viewport = nullptr; // parent
//	ViewportCommandType m_type = ViewportCommandType::None;
//};
//
//class kkColor;
//class kkAabb;
//class kkObb;
//class Scene3D;
//class Scene3DObject;
//class ViewportCamera;
//struct CursorRay;
//class Viewport
//{
//	Scene3D ** m_scene3D_ptr  = nullptr;
//	// видимые объекты для рисования
//	//kkArray<Scene3DObject*> m_objects_inFrustum;
//	std::basic_string<Scene3DObject*> m_objects_inFrustum;
//	void _updateObjectsInFrustum();
//
//	// видимые объекты отсортированные по дальности
//	std::basic_string<Scene3DObject*> m_drawObjects;
//
//	// объекты из m_drawObjects которые находятся под мышкой
//	// так как объекты m_drawObjects отсортированы по дальности
//	//   первый объект будет самым дальним
//	std::basic_string<Scene3DObject*> m_hoveredObjects;
//	std::basic_string<Scene3DObject*> m_hoveredObjects_savedCopy; //при ПКМ нужно запомнить объекты и использовать в попап меню
//
//	// нужен текст для подсказки, и чтобы память постоянно не выделялась
//	kkStringA m_toolTipText;
//
//	// получить объекты которые пересекаются лучом
//	void _getObjectsOnRay();
//
//
//	std::vector<Viewport*> m_children;
//	std::vector<ViewportCommand> m_viewport_commands;
//
//	char m_name[256];
//	char m_typeName[7][256];
//	char m_nameViewportMenuBar[256];
//	char m_nameViewportTypeText[256];
//
//	void _updateRecursively(Viewport*);
//
//	void _drawRecursivelyBorders(Viewport*);
//	void _drawRecursively(Viewport*);
//	
//	void _drawGizmo2DPart( const v2i& );
//
//	void _setGLViewport();
//
//	void _destroyViewportsRecursively();
//	void _destroyViewports(Viewport*);
//	
//	void _drawSelectedObjectFrame();
//	void _drawSelectedObjectPivot();
//
//	//void _drawDebugPoints();
//
//	Viewport* m_parent = nullptr;
//	bool m_is_horizontal = true; /// при создании новых нужно сохранить каким образом он должен создаваться
//	
//	// активный вьюпорт имеет ввод с клавиатуры, и имеет дополнительную подсветку
//	bool m_is_active = false;
//	
//	bool m_isContextHover = false;
//
//	int m_frame_skip = 0;
//	int m_frame_limit = 1; // нужно сделать так чтобы при потере производительности это значение увеличивалось
//
//
//	// когда нажали на активный вьюпорт (и удерживаем кнопку) то должно быть true
//	bool m_is_mouse_focus = false;
//
//	bool m_ignoreInput = false;
//
//	// проблема. клик и удержание MMB вне вьюпорта всё равно включит перемещение камеры когда курсор попадёт во вьюпорт
////	bool m_is_mouse_first_click = false;
//	v2i  m_mouse_first_click_coords;
//	//bool m_drawMouseSelectionRectangle = false;
//	void _drawSelectionRectangle();
//
//	ViewportData m_vd;
//	
//	//bool      m_isRotated   = false;
//	bool      m_isMaximized = false;
//	Viewport* m_maximizedViewport = nullptr; //создаётся только главным вьюпортом
//	void      _setMaximizedViewportPointers(Viewport*);
//
//	static bool m_isNotHideViewportGUI;
//
//	bool m_isDrawGrid = true;
//	enum _draw_mode
//	{
//		_draw_mode_lines,
//		_draw_mode_material,
//		_draw_mode_lines_and_material
//	};
//	_draw_mode m_draw_mode = _draw_mode::_draw_mode_material; // так-же нужно передавать во m_maximizedViewport
//	void _set_draw_mode( _draw_mode );
//	void _toggleDrawModeMaterial();
//	void _toggleDrawModeLines();
//
//	// зачем так много bool ? 
//	bool m_isActivePerspective = true;
//	bool m_isActiveFront       = false;
//	bool m_isActiveBack        = false;
//	bool m_isActiveTop         = false;
//	bool m_isActiveBottom      = false;
//	bool m_isActiveLeft        = false;
//	bool m_isActiveRight       = false;
//
//	ViewportCamera * m_activeCamera = nullptr;
//	ViewportCamera * m_cameraPersp  = nullptr;
//	ViewportCamera * m_cameraFront  = nullptr;
//	ViewportCamera * m_cameraBack   = nullptr;
//	ViewportCamera * m_cameraTop    = nullptr;
//	ViewportCamera * m_cameraBottom = nullptr;
//	ViewportCamera * m_cameraLeft   = nullptr;
//	ViewportCamera * m_cameraRight  = nullptr;
//
//	// если крутанули камеру в ортографическом вьюпорте то это должно быть true
//	// используется:
//	//   - рисование сетки для орто. вьюпорта по всем 3м осям.
//	//bool m_isOrthoUserview = false;
//
//	// когда нажимаем на меню во вьюпорте, нужно сделать так, чтобы вьюпорт не реагировал.
//	// например, при клике на пункте меню не происходило переключение на другой вьюпорт, если он под курсором
//	static bool m_isMainMenuActive;
//	
//	// origin означает что значения не изменятся при изменении размеров окна. они изменяются только при создании\уничтожении вьюпорта
//	v4f m_rect_origin; /// left top right bottom /// 0;0 - левый ВЕРХНИЙ угол
//	v2f m_window_size_origin;
//
//	//v4i m_viewport_origin; /// left top right bottom /// начало ХУ, и развер Ширина Высота
//
//	// вычисляется и используется для рисования. реагирует на изменение размера окна
//	v4f m_rect_modified;
//	v4f m_viewport_to_gl_funk; // эти значения идут в glViewport. Эти значения должны обновляться при изменении размеров окна.
//
//	v4f m_orig_indent; // начальный отступ
//	v2f m_resize_window_coef; // для увеличения или уменьшения нужно умножать значения
//	v4f m_viewport_area_origin; /// зона вьюпорта. меньше чем окно. используется для вычисления m_resize_window_coef
//
//
//	f32 m_gridStep = 0.f;
//
//	// луч который всегда создаётся когда двигается курсор мышки
//	//kkRay m_cursorRay;
//	CursorRay* m_cursorRay = nullptr;
//
//#ifdef KK_DEBUG
//	s32 m_debug_color_id = 0;
//#endif
//
//	void _drawBackground();
//	void _drawGrid();
//	void _drawGridPerspective();
//	void _drawGridXZ(f32 limits, f32 step);
//	void _drawGridXY(f32 limits, f32 step);
//	void _drawGridZY(f32 limits, f32 step);
//	void _drawScene3D();
//	void _drawAabb( const kkAabb&, const kkColor& );
//	void _drawObb( const kkObb&, const kkColor& );
//
//	void _processCommands();
//	void _addViewport(bool hor);
//
//	void _rotate();
//	void _panMove();
//	
//	void _setActiveCamera(ViewportCamera*);
//
//	void _drawViewportTypeName(Viewport* v);
//	void _drawAxis(Viewport* v);
//
//	void _toggleFullscreen();
//	void _onDeleteObjects(Viewport* v);
//
//	v4i m_selectionFrame;
//
//	bool m_drawPickLine = false;
//	v2i  m_drawPickLineP1;
//
//	// меню которое откроется правым кликом
//	// сброс должен быть просто на нажатии какой либо кнопки мышки
//	bool m_drawContextMenu = false;
//	bool m_contextMenuHovered = false;
//	void _drawPopupContextMenu( bool over_window );
//
//	bool m_drawEditMode_hoverMark = false;
//	void _drawEditMode_hoverMark();
//
//	friend class Application;
//
//public:
//
//	Viewport(ViewportData);
//	~Viewport();
//
//	const v4f& getIndentOrigin()
//	{
//		return m_orig_indent;
//	}
//
//	bool isPerspective()
//	{
//		return m_isActivePerspective;
//	}
//
//	void destroyViewport();
//	void draw();
//	//void drawImgui();
//	void update();
//	void updateInput();
//	void updateInputCamera();
//	void checkMouseEvents();
//	void resetCamera();
//
//	void setActive( bool );
//
//	kkCamera * getCamera();
//	ViewportCamera * getActiveViewportCamera()
//	{
//		return m_activeCamera;
//	}
//
//	const v4f& getRect();
//	//void setRect( /*bool Main,*/ const v4i& r );
//
//	//void rotateActiveCameraX(float);
//	//void rotateActiveCameraY(float);
//	
//	static Viewport* m_mainViewport;
//	static Viewport* m_activeViewport;
//	
//	// вызывается только главным вьюпортом
//	void initMaximizedViewport();
//
//	void moveToSelection();
//
//	// предполагается что если нажали куда-то вне вьюпорта, то нужно снять всё что влияло на ввод в этот вьюпорт
//	//  напр: 
//	//        - при фокусе на imgui окно (первый клик по окну) вьюпорт успевает поставить m_is_mouse_first_click = true.
//	// этот метод нужно уметь вызывать только один раз при потере фокуса.
//	void onLoseFocus();
//
//	// когда объекты удалены, нужно очистить список для объектов под курсором
//	void onDeleteObjects();
//	
//	// вызывается каждый кадр
//	// активный вьюпорт
//	void onFrame();
//	
//	// вызывается когда появляется новый объект на сцене
//	// активный вьюпорт
//	void onNewObject();
//	
//	void onWindowActivate();
//
//	void updateCursorRay();
//
//	kkScene3DObject* pickObject();
//	kkVertex* pickVertex(Scene3DObject*);
//	void setDrawPickLine(bool v);
//	void processShortcuts();
//};

#endif