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

	//bool InViewport = false;
	bool IsMove = false;
	bool IsMove2 = false;
	bool IsSelectByFrame = false;
	bool IsFirstClickLMB = false;
	bool IsFirstClickMMB = false;
	bool IsFirstClickRMB = false;

	/*void reset()
	{
		if( IsSelectByRect && LMB_HOLD )
		{
		}
		else
		{
			IsSelectByRect = false;
		}

		if( !LMB_HOLD ) IsFirstClickLMB = false;
		if( !MMB_HOLD ) IsFirstClickMMB = false;
		if( !RMB_HOLD ) IsFirstClickRMB = false;

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
	}*/
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
	void updateInput(const v2i& windowSize, const v2f& mouseDelta, bool inFocus);
	//void updateInputCamera(const v2f& mouseDelta, bool inFocus);
	void updateCursorRay();
	void resetCamera();
	void processShortcuts();
	void setActiveCamera(ViewportCamera* c);

	ShortcutManager* m_shortcutManager = nullptr;

	CursorRay* m_cursorRay = nullptr;
	kkRay m_rayOnClick;

	std::basic_string<Scene3DObject*> m_objects_inFrustum;
	void _updateObjectsInFrustum();
	// видимые объекты отсортированные по дальности
	std::basic_string<Scene3DObject*> m_drawObjects;
	// объекты из m_drawObjects которые находятся под мышкой
	// так как объекты m_drawObjects отсортированы по дальности
	//   первый объект будет самым дальним
	std::basic_string<Scene3DObject*> m_hoveredObjects;
	std::basic_string<Scene3DObject*> m_hoveredObjects_savedCopy; 
	int m_frame_skip = 0;
	int m_frame_limit = 1; // нужно сделать так чтобы при потере производительности это значение увеличивалось
	v4i m_selectionFrame;
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
	
	Application* m_app = nullptr;

	DrawMode m_draw_mode = DrawMode::Material;
	void setDrawMode( DrawMode );
	void toggleDrawModeMaterial();
	void toggleDrawModeLines();
	void moveToSelection();

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
	void drawSelectedObjectFrame();
	void drawSelectionFrame(bool inFocus);
	void drawName(bool isActive);
	void drawEditMode_hoverMark();
	ViewportCamera * getActiveViewportCamera()
	{
		return m_activeCamera;
	}

	void drawGizmo2D();

	bool m_drawEditMode_hoverMark = false;

	bool updateInputCamera(bool inFocus);

	kkScene3DObject* pickObject();
	kkVertex* pickVertex(kkScene3DObject** object);
	
	bool m_drawPickLine = false;
	v2i  m_drawPickLineP1;
	void setDrawPickLine(bool v);
	void drawPickLine();
};

class Viewport
{
	Application* m_app = nullptr;

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
	void updateInputCamera();

	void init(ViewportType, ViewportLayoutType, const v4f& indent);

	bool cursorInViewport();
};


#endif