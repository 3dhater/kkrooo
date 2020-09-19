#include "kkrooo.engine.h"
#include "KrGui.h"
#include "../Common.h"
#include "Viewport.h"
#include "ViewportCamera.h"
#include "GraphicsSystem/kkGraphicsSystem.h"
#include "../GUI/ColorTheme.h"
#include "../ApplicationState.h"
#include "../CursorRay.h"
#include "../ShortcutManager.h"
#include "../Scene3D/Scene3D.h"

ViewportMouseState g_mouseState;
kkGraphicsSystem* g_GS = nullptr;
Kr::Gui::GuiSystem* g_GUI = nullptr;

ViewportObject::ViewportObject(){}
ViewportObject::~ViewportObject()
{
	delete m_cursorRay;
}

void ViewportObject::setActiveCamera(ViewportCamera* c)
{
	auto old_active_camera = m_activeCamera;

	if( m_activeCamera )
		m_activeCamera->m_active = false;
	m_activeCamera  = c;
	m_activeCamera->m_active = true;
	if( old_active_camera == m_activeCamera )
	{
		resetCamera();
	}
	this->update(*m_windowSize);
	kkDrawAll();
}
void ViewportObject::drawName(bool isActive)
{
	bool isOrtho = true;
	if( m_activeCamera == m_cameraPersp.ptr() )
		isOrtho = false;
	const char16_t* name = u"Perspective";
	if(m_activeCamera == m_cameraBack.ptr()) name = u"Back";
	else if(m_activeCamera == m_cameraFront.ptr()) name = u"Front";
	else if(m_activeCamera == m_cameraLeft.ptr()) name = u"Left";
	else if(m_activeCamera == m_cameraRight.ptr()) name = u"Right";
	else if(m_activeCamera == m_cameraTop.ptr()) name = u"Top";
	else if(m_activeCamera == m_cameraBottom.ptr()) name = u"Bottom";
	if( m_activeCamera->m_isRotated && m_activeCamera == m_cameraPersp.ptr() )
	{
		name = u"Orthographic";
	}

	g_GUI->setDrawPosition(m_rect_modified.x+5, m_rect_modified.w-15);
	g_GUI->addText(name);
	if(isActive)
		g_GUI->addText(u"   (active)");
	if(isOrtho)
	{
		g_GUI->addText(nullptr, u"  [%f]", m_gridStep);
	}
}
void ViewportObject::init(const v4f& indent, ViewportLayoutType lt)
{
	m_cursorRay = new  CursorRay;
	m_cursor_position = kkGetCursorPosition();
	m_appState_key = kkGetAppState_keyboard();
	m_inputBlock = kkGetGlobalInputBlock();
	g_GUI = (Kr::Gui::GuiSystem*)kkGetGUI();
	m_shortcutManager = kkGetShortcutManager();

	m_layoutType = lt;
	m_cameraPersp   = kkCreate<ViewportCamera>(ViewportCameraType::Perspective,this);    
	m_cameraBack    = kkCreate<ViewportCamera>(ViewportCameraType::Back,this);
	m_cameraFront   = kkCreate<ViewportCamera>(ViewportCameraType::Front,this);
	m_cameraTop     = kkCreate<ViewportCamera>(ViewportCameraType::Top,this);
	m_cameraBottom  = kkCreate<ViewportCamera>(ViewportCameraType::Bottom,this);
	m_cameraLeft    = kkCreate<ViewportCamera>(ViewportCameraType::Left,this);
	m_cameraRight   = kkCreate<ViewportCamera>(ViewportCameraType::Right,this);
	m_cameraPersp->init();
	m_cameraBack->init();
	m_cameraFront->init();
	m_cameraTop->init();
	m_cameraBottom->init();
	m_cameraLeft->init();
	m_cameraRight->init();

	m_orig_indent.x = indent.x;
	m_orig_indent.y = indent.y;
	m_orig_indent.z = indent.z;
	m_orig_indent.w = indent.w;

	auto m_window_client_size = kkGetWindowClientSize();
	/// 0;0 - left top
	m_rect_origin.x = m_orig_indent.x;
	m_rect_origin.y = m_orig_indent.y;
	m_rect_origin.z = (f32)m_window_client_size.x-m_orig_indent.z;
	m_rect_origin.w = (f32)m_window_client_size.y-m_orig_indent.w;

	m_rect_modified = m_rect_origin;

	m_window_size_origin.x = (float)m_window_client_size.x;
	m_window_size_origin.y = (float)m_window_client_size.y;

	m_viewport_area_origin.x = m_orig_indent.x;
	m_viewport_area_origin.y = m_orig_indent.y;
	m_viewport_area_origin.z = m_window_size_origin.x - m_orig_indent.z;
	m_viewport_area_origin.w = m_window_size_origin.y - m_orig_indent.w;

}
void ViewportObject::processShortcuts()
{
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::ToggleGrid) )
	{
		if( m_isDrawGrid )
			m_isDrawGrid = false;	
		else
			m_isDrawGrid = true;	
	}

	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetPerspective) ){setActiveCamera( m_cameraPersp.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetFront) ){setActiveCamera( m_cameraFront.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetBack) ){setActiveCamera( m_cameraBack.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetRight) ){setActiveCamera( m_cameraRight.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetLeft) ){setActiveCamera( m_cameraLeft.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetTop) ){setActiveCamera( m_cameraTop.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetBottom) ){setActiveCamera( m_cameraBottom.ptr() );}

	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeMaterial) ){setDrawMode( _draw_mode::_draw_mode_material );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeLines) ){setDrawMode( _draw_mode::_draw_mode_lines );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeMaterialAndLines) ){setDrawMode( _draw_mode::_draw_mode_lines_and_material );}
	
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::ToggleDrawModeMaterial) ){toggleDrawModeMaterial();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::ToggleDrawModeLines) ){toggleDrawModeLines();}


	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Camera::Reset) ){resetCamera();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Camera::ToSelection) ){moveToSelection();}
}
void ViewportObject::moveToSelection()
{
	auto scene3D_ptr = *kkGetScene3D();
	if( !scene3D_ptr->getNumOfObjectsOnScene() )
	{
		resetCamera();
	}
	else
	{
		kkAabb aabb;
		if( scene3D_ptr->getNumOfSelectedObjects() )
		{
			// установить камеру на центр aabb выбранных объектов
			aabb = scene3D_ptr->getSelectionAabb();
		}
		else
		{
			aabb = scene3D_ptr->getSceneAabb();
		}
		m_activeCamera->centerToAabb(aabb);
		kkDrawAll();
	}
}
void ViewportObject::toggleDrawModeMaterial()
{
	static bool is_materail_mode = false;

	if( m_draw_mode == _draw_mode::_draw_mode_lines )
	{
		if( is_materail_mode )
		{
			m_draw_mode = _draw_mode::_draw_mode_material;
			is_materail_mode = false;
		}
		else
			m_draw_mode = _draw_mode::_draw_mode_lines_and_material;
	}
	else if( m_draw_mode == _draw_mode::_draw_mode_lines_and_material )
		m_draw_mode = _draw_mode::_draw_mode_lines;
	else if( m_draw_mode == _draw_mode::_draw_mode_material )
	{
		m_draw_mode = _draw_mode::_draw_mode_lines;
		is_materail_mode = true;
	}
	kkDrawAll();
}

void ViewportObject::toggleDrawModeLines()
{
	if( m_draw_mode == _draw_mode::_draw_mode_material )
		m_draw_mode = _draw_mode::_draw_mode_lines_and_material;
	else if( m_draw_mode == _draw_mode::_draw_mode_lines_and_material )
		m_draw_mode = _draw_mode::_draw_mode_material;
	kkDrawAll();
}
void ViewportObject::setDrawMode( _draw_mode m )
{
	m_draw_mode = m;
	kkDrawAll();
}
void ViewportObject::resetCamera()
{
	m_activeCamera->reset();
	m_activeCamera->m_isRotated   = false;
	this->update(*m_windowSize);
	kkDrawAll();
}
void ViewportObject::updateCursorRay()
{
	if( *m_inputBlock )
		return;
	v2i cursorPosition = *m_cursor_position;
	kkrooo::getRay(
		m_cursorRay->m_center, 
		cursorPosition, 
		m_rect_modified, 
		m_rect_modified.getWidthAndHeight(),
		m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_center.update();
	
	const s32 rayOffset = 10;
	kkrooo::getRay(m_cursorRay->m_N, cursorPosition + v2i(0, -rayOffset), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_N.update();
	kkrooo::getRay(m_cursorRay->m_S, cursorPosition + v2i(0, rayOffset), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_S.update();
	kkrooo::getRay(m_cursorRay->m_W, cursorPosition + v2i(-rayOffset, 0), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_W.update();
	kkrooo::getRay(m_cursorRay->m_E, cursorPosition + v2i(rayOffset, 0), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_E.update();
}
void ViewportObject::_panMove()
{
	m_activeCamera->movePan( *m_appState_key, Kr::Gui::GuiSystem::m_mouseDelta.x, Kr::Gui::GuiSystem::m_mouseDelta.y );
}
void ViewportObject::_rotate()
{
	if( Kr::Gui::GuiSystem::m_mouseDelta.x != 0 )
	{
		m_activeCamera->rotateY( -Kr::Gui::GuiSystem::m_mouseDelta.x * 0.25f );
	}

	if( Kr::Gui::GuiSystem::m_mouseDelta.y != 0 )
	{
		m_activeCamera->rotateX( Kr::Gui::GuiSystem::m_mouseDelta.y * 0.25f );
	}

	if( m_activeCamera != m_cameraPersp.ptr() )
		m_activeCamera->m_isRotated = true;
}
void ViewportObject::updateInputCamera(const v2f& mouseDelta, bool inFocus)
{
	if( kkrooo::pointInRect( *m_cursor_position, m_rect_modified  ) )
	{
		g_mouseState.InViewport = true;
		//printf("%i\n",Kr::Gui::GuiSystem::m_wheel_delta);
		if( Kr::Gui::GuiSystem::m_wheel_delta > 0 )
		{
			m_activeCamera->zoomIn( *m_appState_key, Kr::Gui::GuiSystem::m_wheel_delta );
			updateCursorRay();
			kkDrawAll();
		}
		else if( Kr::Gui::GuiSystem::m_wheel_delta < 0 )
		{
			m_activeCamera->zoomOut( *m_appState_key, Kr::Gui::GuiSystem::m_wheel_delta );
			updateCursorRay();
			kkDrawAll();
		}
	}
	if(inFocus)
	{
		if( (g_mouseState.MMB_HOLD && g_mouseState.IsFirstClick) || kkIsKeyDown( kkKey::K_SPACE )  )
		{
			if( *m_appState_key == AppState_keyboard::Alt )
			{
				_rotate();
			}
			else
			{
				_panMove();
			}
		}
	}
}
bool ViewportObject::updateInput(const v2i& windowSize, const v2f& mouseDelta, bool inFocus)
{
	bool res = false;
	_update_frame(mouseDelta);
	update(windowSize);

	bool inRect = false;
	if( kkrooo::pointInRect( *m_cursor_position, m_rect_modified ) )
	{
		kkCursorInViewport(true);
		inRect = true;
		// определяю если курсор двигается
		if( Kr::Gui::GuiSystem::m_mouseDelta.x != 0.f || Kr::Gui::GuiSystem::m_mouseDelta.y != 0.f )
		{
			g_mouseState.IsMove = true;
		}

		if(g_mouseState.LMB_DOWN || g_mouseState.MMB_DOWN)
		{
			res = true;
		}
	}
	if( (g_mouseState.LMB_DOWN || g_mouseState.RMB_DOWN || g_mouseState.MMB_DOWN) && inRect )
	{
		if( !g_mouseState.IsFirstClick )
		{
			g_mouseState.IsFirstClick = true;
			m_mouse_first_click_coords = *m_cursor_position;
		}
	}
	if(inFocus)
	{
		this->processShortcuts();
	}
	return res;
}
void ViewportObject::update(const v2i& windowSize)
{
	/// чтобы вычислить m_resize_window_coef нужно взять текущий размер окна, вычислить область вьюпорта,
	m_viewport_area.x = m_orig_indent.x;
	m_viewport_area.y = m_orig_indent.y;
	m_viewport_area.z = windowSize.x - m_orig_indent.z;
	m_viewport_area.w = windowSize.y - m_orig_indent.w;

	auto viewport_area_sz = m_viewport_area.getWidthAndHeight();
	auto viewport_area_origin_sz = m_viewport_area_origin.getWidthAndHeight();

	m_resize_window_coef.x = viewport_area_sz.x / viewport_area_origin_sz.x;
	m_resize_window_coef.y = viewport_area_sz.y / viewport_area_origin_sz.y;
	/// обновляем основную рамку
	m_rect_modified.x = m_rect_origin.x - m_orig_indent.x;
	m_rect_modified.z = m_rect_origin.z - m_orig_indent.x;
	m_rect_modified.x *= m_resize_window_coef.x;
	m_rect_modified.z *= m_resize_window_coef.x;
	m_rect_modified.x += m_orig_indent.x;
	m_rect_modified.z += m_orig_indent.x;

	m_rect_modified.y = m_rect_origin.y - m_orig_indent.y;
	m_rect_modified.w = m_rect_origin.w - m_orig_indent.y;
	m_rect_modified.y *= m_resize_window_coef.y;
	m_rect_modified.w *= m_resize_window_coef.y;
	m_rect_modified.y += m_orig_indent.y;
	m_rect_modified.w += m_orig_indent.y;

	m_rect_modified.x += m_framePosition.x;
	m_rect_modified.y += m_framePosition.y;
	m_rect_modified.z += m_framePosition.z;
	m_rect_modified.w += m_framePosition.w;

	/// теперь можно легко настроить m_viewport_to_gl_funk - xy=левая нижняя точка для вьюпорта  zw=ширина и высота
	m_gs_viewport.x = m_rect_modified.x;
	/// если в m_rect_modified.y указано малое значение (верх экрана), то в OGL он переместится вниз. надо исправлять
	m_gs_viewport.y = windowSize.y - m_rect_modified.y; 
	/// ширина и высота
	m_gs_viewport.z = m_rect_modified.z - m_rect_modified.x;
	m_gs_viewport.w = m_rect_modified.w - m_rect_modified.y;
	m_gs_viewport.y -= m_gs_viewport.w;

	m_activeCamera->getCamera()->setAspect((f32)m_gs_viewport.z/(f32)m_gs_viewport.w);
	m_activeCamera->getAxisCamera()->setAspect((f32)m_gs_viewport.z/(f32)m_gs_viewport.w);
}
void ViewportObject::beginDraw()
{
	g_GS->setActiveCamera(m_activeCamera->getCamera());
	m_activeCamera->update();
}
void ViewportObject::_update_frame(const v2f& mouseDelta)
{
	auto viewport_area_sz = m_viewport_area.getWidthAndHeight();
	auto half_size = viewport_area_sz * 0.5f;
	switch(m_layoutType)
	{
	default:
		break;
	case ViewportLayoutType::ParallelHor:
	{
		switch(m_uid)
		{
		case ViewportUID::ParallelHorUp:
		{
			if(g_mouseState.RMB_HOLD && g_mouseState.IsFirstClick)
			{
				m_framePosition.w += mouseDelta.y;
				if(m_framePosition.w + half_size.y <= 30.f)
					m_framePosition.w -= mouseDelta.y;
				if(m_framePosition.w + half_size.y >= viewport_area_sz.y - 30.f)
					m_framePosition.w -= mouseDelta.y;
				//printf("%f [%f]\n",m_framePosition.w + half_size.y, viewport_area_sz.y);
			}
			if(m_framePosition.w + half_size.y < 0.f || m_framePosition.w + half_size.y > viewport_area_sz.y)
				m_framePosition.w = 0.f;
		}break;
		case ViewportUID::ParallelHorDown:
		{
			if(g_mouseState.RMB_HOLD && g_mouseState.IsFirstClick)
			{
				m_framePosition.y += mouseDelta.y;
				if(m_framePosition.y + half_size.y <= 30.f)
					m_framePosition.y -= mouseDelta.y;
				if(m_framePosition.y + half_size.y >= viewport_area_sz.y - 30.f)
					m_framePosition.y -= mouseDelta.y;
			}
			if(m_framePosition.y + half_size.y < 0.f || m_framePosition.y + half_size.y > viewport_area_sz.y)
				m_framePosition.y = 0.f;
		}break;
		}
	}break;
	case ViewportLayoutType::ParallelVer:
	{
		switch(m_uid)
		{
		case ViewportUID::ParallelVerLeft:
		{
			if(g_mouseState.RMB_HOLD && g_mouseState.IsFirstClick)
			{
				m_framePosition.z += mouseDelta.x;
				if(m_framePosition.z + half_size.x <= 30.f)
					m_framePosition.z -= mouseDelta.x;
				if(m_framePosition.z + half_size.x >= viewport_area_sz.x - 30.f)
					m_framePosition.z -= mouseDelta.x;
				//printf("%f [%f]\n",m_framePosition.w + half_size.y, viewport_area_sz.y);
			}
			if(m_framePosition.z + half_size.x < 0.f || m_framePosition.z + half_size.x > viewport_area_sz.x)
				m_framePosition.z = 0.f;
		}break;
		case ViewportUID::ParallelVerRight:
		{
			if(g_mouseState.RMB_HOLD && g_mouseState.IsFirstClick)
			{
				m_framePosition.x += mouseDelta.x;
				if(m_framePosition.x + half_size.x <= 30.f)
					m_framePosition.x -= mouseDelta.x;
				if(m_framePosition.x + half_size.x >= viewport_area_sz.x - 30.f)
					m_framePosition.x -= mouseDelta.x;
			}
			if(m_framePosition.x + half_size.x < 0.f || m_framePosition.x + half_size.x > viewport_area_sz.x)
				m_framePosition.x = 0.f;
		}break;
		}
	}break;
	}
}

void ViewportObject::drawBG(const v2i& windowSize, ColorTheme* colorTheme)
{
	//colorTheme->viewport_backgroung_color1.set(1.f,0.f,0.f,1.f);
    g_GS->drawRectangle(
		v2i(0,0),
		v2i(windowSize.x,windowSize.y),
		colorTheme->viewport_backgroung_color1,
		colorTheme->viewport_backgroung_color2 );
}
void ViewportObject::_drawGridXZ(f32 limits, f32 step, ColorTheme* colorTheme)
{
	for( f32 i = step; i < limits; i += step )
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f+i,1.f),kkVector4(limits,0.f,0.f+i,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f-i,1.f),kkVector4(limits,0.f,0.f-i,1.f),colorTheme->viewport_grid_color4);
        
		g_GS->drawLine3D(kkVector4(0.f+i,0.f,-limits,1.f),kkVector4(0.f+i,0.f,limits,1.f),colorTheme->viewport_grid_color3);
		g_GS->drawLine3D(kkVector4(0.f-i,0.f,-limits,1.f),kkVector4(0.f-i,0.f,limits,1.f),colorTheme->viewport_grid_color3);
	}

	if( m_activeCamera == m_cameraBottom.ptr() )
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color4);
	}
	else
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color1);
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color2);
	}
}
void ViewportObject::_drawGridXY(f32 limits, f32 step, ColorTheme* colorTheme)
{
	for( f32 i = step; i < limits; i += step )
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f+i,0.f,1.f),kkVector4(limits,0.f+i,0.f,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(-limits,0.f-i,0.f,1.f),kkVector4(limits,0.f-i,0.f,1.f),colorTheme->viewport_grid_color4);
        
		g_GS->drawLine3D(kkVector4(0.f+i,-limits,0.f,1.f),kkVector4(0.f+i,limits,0.f,1.f),colorTheme->viewport_grid_color3);
		g_GS->drawLine3D(kkVector4(0.f-i,-limits,0.f,1.f),kkVector4(0.f-i,limits,0.f,1.f),colorTheme->viewport_grid_color3);
	}

	if( m_activeCamera == m_cameraBottom.ptr() )
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f,1.f),kkVector4(0.f,limits,0.f,1.f),colorTheme->viewport_grid_color4);
	}
	else
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color1);
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f,1.f),kkVector4(0.f,limits,0.f,1.f),colorTheme->viewport_grid_color5);
	}
}
void ViewportObject::_drawGridZY(f32 limits, f32 step, ColorTheme* colorTheme)
{
	for( f32 i = step; i < limits; i += step )
	{
		g_GS->drawLine3D(kkVector4(0.f,0.f+i,-limits,1.f),kkVector4(0.f,0.f+i,limits,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(0.f,0.f-i,-limits,1.f),kkVector4(0.f,0.f-i,limits,1.f),colorTheme->viewport_grid_color4);
        
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f+i,1.f),kkVector4(0.f,limits,0.f+i,1.f),colorTheme->viewport_grid_color3);
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f-i,1.f),kkVector4(0.f,limits,0.f-i,1.f),colorTheme->viewport_grid_color3);
	}

	if( m_activeCamera == m_cameraBottom.ptr() )
	{
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f,1.f),kkVector4(0.f,limits,0.f,1.f),colorTheme->viewport_grid_color4);
	}
	else
	{
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color2);
		g_GS->drawLine3D(kkVector4(0.f,-limits,0.f,1.f),kkVector4(0.f,limits,0.f,1.f),colorTheme->viewport_grid_color5);
	}
}
void ViewportObject::_drawGrid_persp(ColorTheme* colorTheme)
{
	f32 limits = 7.f;
	for( s32 i = 1; i < 8; ++i )
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f+i,1.f),kkVector4(limits,0.f,0.f+i,1.f),colorTheme->viewport_grid_color3);
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f-i,1.f),kkVector4(limits,0.f,0.f-i,1.f),colorTheme->viewport_grid_color3);
        
		g_GS->drawLine3D(kkVector4(0.f+i,0.f,-limits,1.f),kkVector4(0.f+i,0.f,limits,1.f),colorTheme->viewport_grid_color3);
		g_GS->drawLine3D(kkVector4(0.f-i,0.f,-limits,1.f),kkVector4(0.f-i,0.f,limits,1.f),colorTheme->viewport_grid_color3);
	}

	if(m_activeCamera->getPositionCamera()._f32[1]<0.f)
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color4);
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color4);
	}else
	{
		g_GS->drawLine3D(kkVector4(-limits,0.f,0.f,1.f),kkVector4(limits,0.f,0.f,1.f),colorTheme->viewport_grid_color1);
		g_GS->drawLine3D(kkVector4(0.f,0.f,-limits,1.f),kkVector4(0.f,0.f,limits,1.f),colorTheme->viewport_grid_color2);
	}
}
void ViewportObject::drawGrid(ColorTheme* colorTheme)
{
	if(!m_isDrawGrid)
		return;
	auto cameraType = m_activeCamera->getType();
	switch (cameraType)
	{
	case ViewportCameraType::Perspective:
		_drawGrid_persp(colorTheme);
		break;
	case ViewportCameraType::Front:
	case ViewportCameraType::Back:
	case ViewportCameraType::Left:
	case ViewportCameraType::Right:
	case ViewportCameraType::Top:
	case ViewportCameraType::Bottom:
	{
		f32 limit = 500.f;
		f32 step  = 1.f;

		auto zoomOrt = m_activeCamera->getZoomOrt();

		if( zoomOrt < 0.15f )    { step = 10.f;    limit = 5000.f;   }
		if( zoomOrt < 0.0125f )  { step = 100.f;   limit = 50000.f;  }
		if( zoomOrt < 0.001125f ){ step = 1000.f;  limit = 500000.f; }
		if( zoomOrt < 0.000075f ){ step = 10000.f; limit = 5000000.f; }

		m_gridStep = step;

		if( m_activeCamera == m_cameraTop.ptr() || m_activeCamera == m_cameraBottom.ptr() )
		{
			_drawGridXZ(limit, step, colorTheme);
		}
		else if( m_activeCamera == m_cameraLeft.ptr() || m_activeCamera == m_cameraRight.ptr() )
		{
			_drawGridZY(limit, step, colorTheme);
		}
		else if( m_activeCamera == m_cameraFront.ptr() || m_activeCamera == m_cameraBack.ptr() )
		{
			_drawGridXY(limit, step, colorTheme);
		}
	}break;
	}
}

Viewport::Viewport(kkWindow* window)
	:
	m_window(window)
{
	g_GS = kkGetGS();
}

Viewport::~Viewport()
{
	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			auto next = vp->m_right;
			kkDestroy(vp);
			if(vp == end)
				break;
			vp = next;
		}
	}
}
void Viewport::_init_parallel_v(const v4f& indent, ViewportLayoutType lt)
{
	ViewportObject * VO1 = kkCreate<ViewportObject>();
	VO1->init(indent,lt);
	VO1->m_uid = ViewportUID::ParallelVerLeft;
	ViewportObject * VO2 = kkCreate<ViewportObject>();
	VO2->init(indent,lt);
	VO2->m_uid = ViewportUID::ParallelVerRight;
	m_viewports = VO1;
	m_viewports->m_right = VO2;
	m_viewports->m_left  = VO2;
	VO2->m_right = m_viewports;
	VO2->m_left  = m_viewports;

	VO1->m_windowSize = &m_windowSize;
	VO2->m_windowSize = &m_windowSize;

	auto half = (VO1->m_rect_origin.z - VO1->m_rect_origin.x) / 2;
	VO1->m_rect_origin.z = VO1->m_rect_origin.z - half; /// теперь ГУИ кнопки родителя встанут на место

	VO2->m_rect_origin.x = VO1->m_rect_origin.z;

	VO1->m_rect_modified = VO1->m_rect_origin;
	VO2->m_rect_modified = VO2->m_rect_origin;

	VO1->m_activeCamera = VO1->m_cameraTop.ptr();
	VO2->m_activeCamera = VO2->m_cameraPersp.ptr();
	m_viewportInFocus = VO2;
}
void Viewport::_init_parallel_h(const v4f& indent, ViewportLayoutType lt)
{
	ViewportObject * VO1 = kkCreate<ViewportObject>();
	VO1->init(indent,lt);
	VO1->m_uid = ViewportUID::ParallelHorUp;
	ViewportObject * VO2 = kkCreate<ViewportObject>();
	VO2->init(indent,lt);
	VO2->m_uid = ViewportUID::ParallelHorDown;
	m_viewports = VO1;
	m_viewports->m_right = VO2;
	m_viewports->m_left  = VO2;
	VO2->m_right = m_viewports;
	VO2->m_left  = m_viewports;

	VO1->m_windowSize = &m_windowSize;
	VO2->m_windowSize = &m_windowSize;

	auto half = (VO1->m_rect_origin.w - VO1->m_rect_origin.y) / 2;
	VO1->m_rect_origin.w = VO1->m_rect_origin.w - half; /// теперь ГУИ кнопки родителя встанут на место

	/// Далее изменяется m_rect нового вьюпорта
	/// нужно менять значение w, точку нужно поднимать
	/// вообще-то, эта точка должна вставать туда-же куда встала отодвинутая точка родителя
	VO2->m_rect_origin.y = VO1->m_rect_origin.w;

	VO1->m_rect_modified = VO1->m_rect_origin;
	VO2->m_rect_modified = VO2->m_rect_origin;

	VO1->m_activeCamera = VO1->m_cameraTop.ptr();
	VO2->m_activeCamera = VO2->m_cameraPersp.ptr();
	m_viewportInFocus = VO2;
}
void Viewport::_init_single(const v4f& indent, ViewportLayoutType lt)
{
	ViewportObject * VO = kkCreate<ViewportObject>();
	VO->init(indent,lt);
	m_viewports = VO;
	VO->m_left  = VO;
	VO->m_right = VO;
	VO->m_activeCamera = VO->m_cameraPersp.ptr();
	m_viewportInFocus = VO;
	VO->m_windowSize = &m_windowSize;
}
void Viewport::init(ViewportType type, ViewportLayoutType l_type, const v4f& indent)
{
	m_type = type;
	m_layoutType = l_type;
	switch(m_layoutType)
	{
	default:
	case ViewportLayoutType::Single:
		_init_single(indent, l_type);
		break;
	case ViewportLayoutType::ParallelHor:
		_init_parallel_h(indent, l_type);
		break;
	case ViewportLayoutType::ParallelVer:
		_init_parallel_v(indent, l_type);
		break;
	}
}

void Viewport::updateInputCamera(const v2f& mouseDelta)
{
	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			auto next = vp->m_right;
			vp->updateInputCamera(mouseDelta, m_viewportInFocus == vp);
			if(vp == end)
				break;
			vp = next;
		}
	}
}
void Viewport::updateInput(const v2f& mouseDelta)
{
	g_mouseState.reset();
	g_mouseState.LMB_DOWN = kkIsLmbDownOnce();
	g_mouseState.LMB_HOLD = kkIsLmbDown();
	g_mouseState.LMB_UP   = kkIsLmbUp();
	g_mouseState.RMB_DOWN = kkIsRmbDownOnce();
	g_mouseState.RMB_HOLD = kkIsRmbDown();
	g_mouseState.RMB_UP   = kkIsRmbUp();
	g_mouseState.MMB_DOWN = kkIsMmbDownOnce();
	g_mouseState.MMB_HOLD = kkIsMmbDown();
	g_mouseState.MMB_UP   = kkIsMmbUp();
	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			auto next = vp->m_right;
			if( vp->updateInput(m_windowSize, mouseDelta, m_viewportInFocus == vp) )
			{
				m_viewportInFocus = vp;
			}
			if(vp == end)
				break;
			vp = next;
		}
	}
}

void Viewport::update()
{
	m_windowSize = m_window->getClientRect().getWidthAndHeight();

	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			auto next = vp->m_right;
			vp->update(m_windowSize);
			if(vp == end)
				break;
			vp = next;
		}
	}
}

void Viewport::draw(ColorTheme* colorTheme)
{
	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			if( kkrooo::pointInRect( *vp->m_cursor_position, vp->m_rect_modified ))
			{
				kkCursorInViewport(true);
			}

			auto next = vp->m_right;
			kkGSSetDepth(false);
			kkGSSetViewport((s32)vp->m_gs_viewport.x, (s32)vp->m_gs_viewport.y, (s32)vp->m_gs_viewport.z, (s32)vp->m_gs_viewport.w);
			vp->drawBG(m_windowSize, colorTheme);
			kkGSSetDepth(true);

			// draw 3d
			vp->beginDraw();
			vp->drawGrid(colorTheme);

			// draw 2d top layer
			kkGSSetDepth(false);
			kkGSSetViewport(0,0, m_windowSize.x, m_windowSize.y);
			vp->drawName(m_viewportInFocus == vp);
			kkGSSetDepth(true);

			if(vp == end)
				break;
			vp = next;
		}
	}
}


//
//
//void Viewport::_getObjectsOnRay()
//{
//
//}
//
//void Viewport::_updateObjectsInFrustum()
//{
//	m_objects_inFrustum.clear();
//	m_drawObjects.clear();
//
//	// пока просто добавлю все объекты
//	auto objects = (*m_scene3D_ptr)->getObjects();
//	
//	auto frust = m_activeCamera->getCamera()->getFrustum();
//
//	auto camera_position = m_activeCamera->getPositionCamera();
//
//	kkAabb    aabb;
//	kkObb     obb;
//	kkVector4 center;
//	kkVector4 extent;
//	u32     object_index = 0;
//
//	for( size_t i = 0, sz = objects.size(); i < sz; ++i )
//	{
//		auto object = objects[i];
//
//
//		obb  = object->Obb();
//		aabb = object->Aabb();
//		aabb.center(center);
//
//		object->m_distanceToCamera = camera_position.distance(center);
//
//		//if(sphereInFrustum(m_activeCamera->getCamera()->getFrustum(), aabb.m_max.distance(center),object->getPivot()))
//		if( OBBInFrustum( obb, frust ) )
//		{
//			++object_index;
//
//			m_objects_inFrustum.push_back(object);
//			m_drawObjects.push_back(object);
//		}
//	}
//
//	// нужно оптимизировать. не сортировать зря.
//	// по сути можно добавить bool, и проверять выше.
//	// если изменили позицию камеры или повернули или добавили объект то ИСТИНА, надо обновить
//	//    возможно перед вызовом _updateObjectsInFrustum()
//	sortObjectsInFrustum( m_drawObjects );
//	
//	//printf("%u\n",m_objects_inFrustum.size());
//}
//
//void Viewport::setActive( bool v )
//{
//	if( !v )
//	{
//		// сброс чего-то, при переключении на другой вьюпорт
//
////		m_is_mouse_first_click = false;
////		m_drawMouseSelectionRectangle = false; // если есть рамка выделения, то её нужно убрать
//	}
//
//	if( m_activeViewport )
//	{
//		if( m_activeViewport != this )
//		{
//			m_activeViewport->m_drawContextMenu = false;
//		}
//	}
//
//	m_is_active = v;
//	m_activeViewport = this;
//}
//
//void Viewport::destroyViewport()
//{
//	_destroyViewportsRecursively();
//}
//
//void Viewport::_destroyViewportsRecursively()
//{
//	for( size_t i = 0, sz = m_children.size(); i < sz; ++i )
//	{
//		m_children[i]->_destroyViewportsRecursively();
//		_destroyViewports(m_children[i]);
//	}
//	m_children.clear();
//}
//
//void Viewport::_destroyViewports(Viewport* v)
//{
//	kkDestroy(v);
//}
//
//void Viewport::_addViewport(bool hor)
//{
//	Viewport * new_viewport = kkCreate<Viewport>(m_vd);
//
//	new_viewport->m_parent = this;
//	
//	/// новый вьюпорт изначально имеет размер родителя
//	/// остаётся изменить размеры сначала родителя, потом нового вьюпорта
//	new_viewport->m_rect_origin     = m_rect_origin; 
//
//	new_viewport->m_maximizedViewport = m_maximizedViewport;
//
//	if( hor )
//	{
//		/// Изменяю размеры родителя
//		/// Сначала m_rect - значения в экранных координатах (0;0 - left top)
//		/// Родитель уходит вниз, значит нужно отредактировать y координату
//		/// Сначала берётся общая высота m_rect, делим на 2 получая половину, и из значания w вычитаем половинку...будет середина
//		auto half = (m_rect_origin.w - m_rect_origin.y) / 2;
//		m_rect_origin.y = m_rect_origin.w - half; /// теперь ГУИ кнопки родителя встанут на место
//
//		/// Далее изменяется m_rect нового вьюпорта
//		/// нужно менять значение w, точку нужно поднимать
//		/// вообще-то, эта точка должна вставать туда-же куда встала отодвинутая точка родителя
//		new_viewport->m_rect_origin.w = m_rect_origin.y;
//	}
//	else
//	{
//		/// Изменяю размеры родителя
//		/// Сначала m_rect - значения в экранных координатах (0;0 - left top)
//		/// Родитель уходит влево, значит нужно отредактировать z координату
//		/// Сначала берётся общая ширина m_rect, делим на 2 получая половину, и из значания z вычитаем половинку...будет середина
//		auto half = (m_rect_origin.z - m_rect_origin.x) / 2;
//		m_rect_origin.z = m_rect_origin.z - half; /// теперь ГУИ кнопки родителя встанут на место
//
//		/// Далее изменяется m_rect нового вьюпорта
//		/// нужно менять значение x, точку нужно двигать вправо
//		/// вообще-то, эта точка должна вставать туда-же куда встала отодвинутая точка родителя
//		new_viewport->m_rect_origin.x = m_rect_origin.z;
//	}
//
//	m_rect_modified = m_rect_origin;
//	new_viewport->m_rect_modified = new_viewport->m_rect_origin;
//	new_viewport->m_is_horizontal = hor;
//
//	update();
//	
//	m_children.push_back( new_viewport );
//}
//
//void Viewport::_setGLViewport()
//{
//	m_vd.m_gs->setViewport(
//		(s32)m_viewport_to_gl_funk.x,
//		(s32)m_viewport_to_gl_funk.y,
//		(s32)m_viewport_to_gl_funk.z,
//		(s32)m_viewport_to_gl_funk.w 
//	);
//}
//
//void Viewport::_drawRecursively(Viewport* v)
//{
//	v->m_vd.m_gs->setActiveCamera(v->m_activeCamera->getCamera());
//	v->m_activeCamera->update();
//
//    v->m_vd.m_gs->useDepth(false);
//
//	v->_setGLViewport();
//
//	v->_drawBackground();
//    v->m_vd.m_gs->useDepth(true);
//	
//	if( v->m_isDrawGrid )
//		v->_drawGrid();
//
//	v->_drawScene3D();
//	v->_drawSelectedObjectFrame();
//	v->_drawSelectedObjectPivot();
//
//	v->m_vd.m_gs->useDepth(false);
//	v->_drawAxis(v);
//	
//	//v->_drawDebugPoints();
//
//	if( g_mouseState.IsSelectByRect && v == m_activeViewport )
//	{
//		v->_drawSelectionRectangle();
//	}
//	
//	if(v->m_drawPickLine)
//	{
//		m_vd.m_gs->useScissor(true);
//		m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//		m_vd.m_gs->setScissor((s32)m_viewport_to_gl_funk.x,(s32)m_viewport_to_gl_funk.y,(s32)m_viewport_to_gl_funk.z,(s32)m_viewport_to_gl_funk.w );
//
//		v->m_vd.m_gs->drawLine2D(v->m_drawPickLineP1, *v->m_vd.m_cursor_position, kkColorWhite);
//
//		m_vd.m_gs->setScissor(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//		m_vd.m_gs->useScissor(false);
//	}
//
//	//if( !v->m_vd.m_app->isGlobalInputBlocked() )
//	{
//		if( v->m_drawEditMode_hoverMark && v->m_vd.m_app->m_editMode == EditMode::Vertex )
//			v->_drawEditMode_hoverMark();
//	}
//
//
//    v->m_vd.m_gs->useDepth(true);
//	v->m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//
//	if( !m_mainViewport->m_isMaximized )
//	{
//		for( size_t i = 0, sz = v->m_children.size(); i < sz; ++i )
//		{
//			v->_drawRecursively(v->m_children[i]);
//		}
//	}
//}
//
//void Viewport::_drawRecursivelyBorders(Viewport* v)
//{
//	
//	v->m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//
//	kkColor border_color = m_vd.m_current_color_theme->viewport_border_color;
//#ifdef KK_DEBUG
////	border_color = g_debugColorArray[v->m_debug_color_id];
//#endif
//	bool draw_borders = true;
//	if( draw_borders )
//	{
//		v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.y),v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.y),border_color);
//		v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.w),v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.w),border_color);
//		v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.y),v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.w),border_color);
//		v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.y),v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.w),border_color);
//	}
//
//	if( !m_mainViewport->m_isMaximized )
//	{
//		for( size_t i = 0, sz = v->m_children.size(); i < sz; ++i )
//		{
//			v->_drawRecursivelyBorders(v->m_children[i]);
//		}
//	}
//
//	bool draw_big_border = false;
//	kkColor big_border_color = m_vd.m_current_color_theme->viewport_active_color;
//
//	if(*m_vd.m_state_app == AppState_main::Idle)
//	{
//		if( kkrooo::pointInRect( *v->m_vd.m_cursor_position, v->m_rect_modified ) )
//		{
//			if( !v->m_is_active )
//			{
//				draw_big_border = true;
//				big_border_color = m_vd.m_current_color_theme->viewport_mousehover_color;
//			}
//		}
//	}
//
//	if( v->m_is_active )
//	{
//		draw_big_border = true;
//	}
//
//	if( draw_big_border && v->m_vd.m_app->isDrawViewportBorders() == true )
//	{
//		const int border_size = 2;
//		for( int i = 0; i < border_size; ++i )
//		{
//			v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.y+i),v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.y+i),big_border_color);
//			v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x,(s32)v->m_rect_modified.w-i),v2i((s32)v->m_rect_modified.z,(s32)v->m_rect_modified.w-i),big_border_color);
//			v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.x+i,(s32)v->m_rect_modified.y),v2i((s32)v->m_rect_modified.x+i,(s32)v->m_rect_modified.w),big_border_color);
//			v->m_vd.m_gs->drawLine2D(v2i((s32)v->m_rect_modified.z-i,(s32)v->m_rect_modified.y),v2i((s32)v->m_rect_modified.z-i,(s32)v->m_rect_modified.w),big_border_color);
//		}
//	}
//
//	if(*m_vd.m_state_app == AppState_main::Idle && !m_mainViewport->m_isMaximized )
//	{
//		if( kkrooo::pointInRect( *v->m_vd.m_cursor_position, v->m_rect_modified ) )
//		{
//			if( m_vd.m_event_consumer->isLmbDownOnce() || m_vd.m_event_consumer->isMmbDownOnce() || m_vd.m_event_consumer->isRmbUp() )
//			{
//				if( !v->m_is_active && !v->m_isMainMenuActive )
//				{
//					kkSetActiveViewport(v);
//				}
//			}
//
//		}
//	}
//}

//
//void Viewport::_drawObb( const kkObb& obb, const kkColor& color)
//{				
//	m_vd.m_gs->drawLine3D( obb.v1, obb.v4, color );
//	m_vd.m_gs->drawLine3D( obb.v5, obb.v8, color );
//	m_vd.m_gs->drawLine3D( obb.v1, obb.v5, color );
//	m_vd.m_gs->drawLine3D( obb.v4, obb.v8, color );
//	m_vd.m_gs->drawLine3D( obb.v3, obb.v7, color );
//	m_vd.m_gs->drawLine3D( obb.v6, obb.v2, color );
//	m_vd.m_gs->drawLine3D( obb.v3, obb.v6, color );
//	m_vd.m_gs->drawLine3D( obb.v7, obb.v2, color );
//	m_vd.m_gs->drawLine3D( obb.v2, obb.v8, color );
//	m_vd.m_gs->drawLine3D( obb.v4, obb.v7, color );
//	m_vd.m_gs->drawLine3D( obb.v5, obb.v6, color );
//	m_vd.m_gs->drawLine3D( obb.v1, obb.v3, color );
//}
//
//void Viewport::_drawAabb( const kkAabb& aabb, const kkColor& color )
//{
//	auto & p1 = aabb.m_min;
//	auto & p2 = aabb.m_max;
//
//	kkVector4 positionOffset;
//
//	kkVector4 v1 = p1;
//	kkVector4 v2 = p2;
//				
//	kkVector4 v3( p1.KK_X, p1.KK_Y, p2.KK_Z );
//	kkVector4 v4( p2.KK_X, p1.KK_Y, p1.KK_Z );
//	kkVector4 v5( p1.KK_X, p2.KK_Y, p1.KK_Z );
//	kkVector4 v6( p1.KK_X, p2.KK_Y, p2.KK_Z);
//	kkVector4 v7( p2.KK_X, p1.KK_Y, p2.KK_Z );
//	kkVector4 v8( p2.KK_X, p2.KK_Y, p1.KK_Z);
//				
//	m_vd.m_gs->drawLine3D( v1 + positionOffset, v4 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v5 + positionOffset, v8 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v1 + positionOffset, v5 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v4 + positionOffset, v8 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v3 + positionOffset, v7 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v6 + positionOffset, v2 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v3 + positionOffset, v6 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v7 + positionOffset, v2 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v2 + positionOffset, v8 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v4 + positionOffset, v7 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v5 + positionOffset, v6 + positionOffset, color );
//	m_vd.m_gs->drawLine3D( v1 + positionOffset, v3 + positionOffset, color );
//}
//
//void Viewport::_drawScene3D()
//{
//	bool       fff = false;
//	
//	m_frame_skip++;
//
//	if( m_frame_skip == m_frame_limit )
//	{
//		_updateObjectsInFrustum();
//		m_frame_skip = 0;
//		fff = true;
//	}
//
//	if( fff )
//		m_hoveredObjects.clear();
//
//	m_vd.m_gs->useBackFaceCulling( m_vd.m_app->m_backfaceCull );
//
//	for( size_t i = 0; i < m_drawObjects.size(); ++i )
//	{
//
//		Scene3DObject*      object = m_drawObjects[i];
//		kkScene3DObjectType object_type = object->GetType();
//		auto & obb = object->Obb();
//
//		//printf("Draw num[%u] - [%s]\n", i, kkString(m_drawObjects[i]->getName()).to_kkStringA().data());
//
//		/*
//		for( int q = 0; q < 20; ++q )
//		{
//			m_vd.m_app->m_shaderPoint->m_pos = kkVector4(0.f,0.f+q,0.f+i,1.f);
//			m_vd.m_gs->drawPoint3D(kkVector4(0.f,4.f,0.f,1.f), m_vd.m_app->m_shaderPoint.ptr() );
//		}
//		*/
//
//		switch(object_type)
//		{
//		case kkScene3DObjectType::PolygonObject:
//		{
//			object->UpdateWorldMatrix();
//
//			if( m_draw_mode == _draw_mode::_draw_mode_material || m_draw_mode == _draw_mode::_draw_mode_lines_and_material )
//			{
//				for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
//				{
//					// рисование самой модели
//					m_vd.m_app->m_shader3DObjectDefault->m_diffuseColor = object->m_shaderParameter.m_diffuseColor;
//					m_vd.m_app->m_shader3DObjectDefault->m_diffTex = object->m_shaderParameter.m_diffuseTexture;
//					m_vd.m_gs->drawMesh(object->getHardwareModel(i2), object->GetMatrixWorld() , m_vd.m_app->m_shader3DObjectDefault.ptr() );
//				}
//			}
//
//			if( m_draw_mode == _draw_mode::_draw_mode_lines || m_draw_mode == _draw_mode::_draw_mode_lines_and_material
//				|| m_vd.m_app->getEditMode() == EditMode::Edge )
//			{
//				if( m_vd.m_app->getEditMode() == EditMode::Polygon )
//				{
//					for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
//					{
//						m_vd.m_gs->drawMesh(object->getHardwareModel(i2), object->GetMatrixWorld() , m_vd.m_app->m_shader3DObjectDefault_polymodeforlinerender.ptr() );
//					}
//				}
//
//				for( u64 i2 = 0, sz = object->getHardwareModelCount_lines(); i2 < sz; ++i2 )
//				{
//					if(object->isSelected())
//						m_vd.m_app->m_shaderLineModel->edge_color.set(1.f,1.f,1.f,1.f);
//					else
//						m_vd.m_app->m_shaderLineModel->edge_color = object->m_edgeColor;
//					m_vd.m_gs->drawMesh(object->getHardwareModel_lines(i2), object->GetMatrixWorld() , m_vd.m_app->m_shaderLineModel.ptr() );
//				}
//			}
//
//			switch (g_m_f)
//			{
//			case 0: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_ALWAYS); break;
//			case 1: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_EQUAL); break;
//			case 2: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_GEQUAL); break;
//			case 3: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_GREATER); break;
//			case 4: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_LEQUAL); break;
//			case 5: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_LESS); break;
//			case 6: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_NEVER); break;
//			case 7: m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_NOTEQUAL); break;
//			}
//			//m_vd.m_gs->setCompFunc(kkGraphicsSystemCompFunc::_NOTEQUAL);
//
//			m_vd.m_app->m_shaderPoint->setWorld( object->GetMatrixWorld() );
//			//m_vd.m_app->m_shaderPoint->m_world = kkMatrix4();
//
//			if( m_vd.m_app->m_editMode == EditMode::Vertex && object->m_isSelected )
//			{
//				for( u64 i2 = 0, sz = object->getHardwareModelCount_points(); i2 < sz; ++i2 )
//				{
//					m_vd.m_gs->drawMesh(object->getHardwareModel_points(i2), object->GetMatrixWorld() , m_vd.m_app->m_shaderPoint.ptr() );
//				}
//			}
//
//			/*object->prepareForRaytracing();
//			int icol = 0;
//			for( auto n : object->m_PolyModel->m_BVHNodes )
//			{
//				_drawAabb( n->m_aabb, g_debugColorArray[icol++] );
//			}
//			object->finishRaytracing();*/
//
//			if( m_vd.m_app->debugIsDrawObjectAabb() )
//			{
//				_drawAabb( object->Aabb(), kkColorYellowGreen );
//			}
//
//			if( m_vd.m_app->debugIsDrawObjectObb()  )
//			{
//				_drawObb( obb, kkColorRed );
//			}
//
//		}break;
//		default:
//			break;
//		}
//
//		if( fff && !m_vd.m_event_consumer->isMmbDown() && *m_vd.m_state_app != AppState_main::Gizmo && !g_mouseState.IsSelectByRect
//			&& m_vd.m_app->m_editMode == EditMode::Object )
//		{
//			if( kkrooo::pointInRect( *m_vd.m_cursor_position, m_rect_modified ) )
//			{
//				if( kkrooo::rayIntersection_obb(m_cursorRay->m_center, obb) )
//				{
//					// далее проверка на пересечение луч-треугольник
//					kkRayTriangleIntersectionResultSimple intersectionResult;
//					if( object->IsRayIntersect(m_cursorRay->m_center, intersectionResult) )
//					{
//						auto camera_position = m_activeCamera->getPositionCamera();
//
//						object->m_distanceToCamera = camera_position.distance(intersectionResult.m_intersectionPoint);
//						m_hoveredObjects.push_back(object);
//					}
//				}
//			}
//		}
//
//		if( m_hoveredObjects.size() )
//		{
//			sortMouseHoverObjects( m_hoveredObjects );
//		}
//	}
//	
//	
//
//	
//	if( m_hoveredObjects.size() )
//	{
//		//printf("[%s]\n",m_hoveredObjects[m_hoveredObjects.size()-1]->m_name.to_kkStringA().data() );
//		_drawObb( m_hoveredObjects[m_hoveredObjects.size()-1]->Obb(), kkColorRed );
//	}
//
//	//printf("m_hoveredObjects_count  [%llu]\n",m_hoveredObjects.size() );
//
//	//m_vd.m_gs->drawLine3D( intersectionPoint - kkVector4(0.1f,0.f,0.f), intersectionPoint + kkVector4(0.1f,0.f,0.f), kkColorLime );
//	//m_vd.m_gs->drawLine3D( intersectionPoint - kkVector4(0.0f,0.1f,0.f), intersectionPoint + kkVector4(0.0f,0.1f,0.f), kkColorLime );
//	//m_vd.m_gs->drawLine3D( intersectionPoint - kkVector4(0.0f,0.0f,0.1f), intersectionPoint + kkVector4(0.0f,0.0f,0.1f), kkColorLime );
//
//	if( m_vd.m_app->debugIsDrawSceneAabb() )
//	{
//		_drawAabb( (*m_scene3D_ptr)->getSceneAabb(), kkColorYellow );
//	}
//	
//	if( m_vd.m_app->debugIsDrawSelectedObjectsAabb() )
//	{
//		_drawAabb( (*m_scene3D_ptr)->getSelectionAabb(), kkColorBlue );
//	}
//
//	// TEST
//	/*m_vd.m_gs->drawLine3D(g_selFrust.m_right[0], g_selFrust.m_right[1], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_right[1], g_selFrust.m_right[3], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_right[3], g_selFrust.m_right[2], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_right[2], g_selFrust.m_right[0], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_top[0], g_selFrust.m_top[1], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_top[1], g_selFrust.m_top[3], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_top[3], g_selFrust.m_top[2], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_top[2], g_selFrust.m_top[0], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_left[0], g_selFrust.m_left[1], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_left[1], g_selFrust.m_left[3], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_left[3], g_selFrust.m_left[2], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_left[2], g_selFrust.m_left[0], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_bottom[0], g_selFrust.m_bottom[1], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_bottom[1], g_selFrust.m_bottom[3], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_bottom[3], g_selFrust.m_bottom[2], kkColorWhite);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_bottom[2], g_selFrust.m_bottom[0], kkColorWhite);
//
//	m_vd.m_gs->drawLine3D(g_selFrust.m_RC, g_selFrust.m_RC + (g_selFrust.m_RN*30.f), kkColorGreen);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_BC, g_selFrust.m_BC + (g_selFrust.m_BN*30.f), kkColorYellow);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_TC, g_selFrust.m_TC + (g_selFrust.m_TN*30.f), kkColorRed);
//	m_vd.m_gs->drawLine3D(g_selFrust.m_LC, g_selFrust.m_LC + (g_selFrust.m_LN*30.f), kkColorBlue);*/
//
//	//printf("%f\n", g_selFrust.m_RN.dot(g_selFrust.m_RC - kkVector4()) );
//
//	//if( g_selFrust.pointInFrust(kkVector4()) )
//	//{
//	//	//printf("in\n");
//	//}
//}
//
//
//void Viewport::_updateRecursively(Viewport* v)
//{
//	/// обновляем основную рамку
//	v->m_rect_modified.x = v->m_rect_origin.x - m_orig_indent.x;
//	v->m_rect_modified.z = v->m_rect_origin.z - m_orig_indent.x;
//	v->m_rect_modified.x *= m_resize_window_coef.x;
//	v->m_rect_modified.z *= m_resize_window_coef.x;
//	v->m_rect_modified.x += m_orig_indent.x;
//	v->m_rect_modified.z += m_orig_indent.x;
//
//	v->m_rect_modified.y = v->m_rect_origin.y - m_orig_indent.y;
//	v->m_rect_modified.w = v->m_rect_origin.w - m_orig_indent.y;
//	v->m_rect_modified.y *= m_resize_window_coef.y;
//	v->m_rect_modified.w *= m_resize_window_coef.y;
//	v->m_rect_modified.y += m_orig_indent.y;
//	v->m_rect_modified.w += m_orig_indent.y;
//
//	/// теперь можно легко настроить m_viewport_to_gl_funk - xy=левая нижняя точка для вьюпорта  zw=ширина и высота
//	/// славу богу значение по ИКС изменять не надо
//	v->m_viewport_to_gl_funk.x = v->m_rect_modified.x;
//	/// если в m_rect_modified.y указано малое значение (верх экрана), то в OGL он переместится вниз. надо исправлять
//	v->m_viewport_to_gl_funk.y = m_vd.m_window_client_size->y - v->m_rect_modified.y; 
//	/// ширина и высота
//	v->m_viewport_to_gl_funk.z = v->m_rect_modified.z - v->m_rect_modified.x;
//	v->m_viewport_to_gl_funk.w = v->m_rect_modified.w - v->m_rect_modified.y;
//	v->m_viewport_to_gl_funk.y -= v->m_viewport_to_gl_funk.w;
//	
//	if( !m_mainViewport->m_isMaximized )
//	{
//		for( size_t i = 0, sz = v->m_children.size(); i < sz; ++i )
//		{
//			_updateRecursively(v->m_children[i]);
//		}
//	}
//
//	v->m_activeCamera->getCamera()->setAspect((f32)v->m_viewport_to_gl_funk.z/(f32)v->m_viewport_to_gl_funk.w);
//	v->m_activeCamera->getAxisCamera()->setAspect((f32)v->m_viewport_to_gl_funk.z/(f32)v->m_viewport_to_gl_funk.w);
//
//}
//
//void Viewport::update()
//{
//	/// чтобы вычислить m_resize_window_coef нужно взять текущий размер окна, вычислить область вьюпорта,
//	v4f viewport_area;
//	viewport_area.x = m_orig_indent.x;
//	viewport_area.y = m_orig_indent.y;
//	viewport_area.z = m_vd.m_window_client_size->x - m_orig_indent.z;
//	viewport_area.w = m_vd.m_window_client_size->y - m_orig_indent.w;
//
//	auto viewport_area_sz = viewport_area.getWidthAndHeight();
//	auto viewport_area_origin_sz = m_viewport_area_origin.getWidthAndHeight();
//
//	m_resize_window_coef.x = viewport_area_sz.x / viewport_area_origin_sz.x;
//	m_resize_window_coef.y = viewport_area_sz.y / viewport_area_origin_sz.y;
//
//	if( m_mainViewport->m_isMaximized )
//	{
//		_updateRecursively(m_maximizedViewport);
//	}
//	else
//	{
//		_updateRecursively(this);
//	}
//}
//
//void Viewport::checkMouseEvents()
//{
//	// отжали ЛКМ.
//	if( m_vd.m_event_consumer->isLmbUp() )
//	{
//		//if( m_vd.m_app->m_editMode == EditMode::Object )
//		//{
//			if( g_mouseState.IsSelectByRect )
//			{
//				g_mouseState.IsSelectByRect = false;
//				_drawScene3D();
//				(*m_activeViewport->m_scene3D_ptr)->selectObjectsByRectangle( 
//					m_activeViewport->m_hoveredObjects, 
//					m_activeViewport->m_drawObjects, 
//					m_activeViewport->m_selectionFrame,
//					g_selFrust
//				);
//			}
//		//}
//
//	}
//}
//
//void Viewport::updateCursorRay()
//{
//	if( m_vd.m_app->isGlobalInputBlocked() )
//		return;
////	printf("%i %i\n",m_vd.m_cursor_position->x,m_vd.m_cursor_position->y);
//	//kkrooo::getRay(m_activeViewport->m_cursorRay, *m_vd.m_cursor_position, m_activeViewport->m_rect_modified, m_activeViewport->m_activeCamera->getCamera() );
//
//	v2i cursorPosition = *m_vd.m_cursor_position;
//	kkrooo::getRay(
//		m_activeViewport->m_cursorRay->m_center, 
//		cursorPosition, 
//		m_activeViewport->m_rect_modified, 
//		m_activeViewport->m_rect_modified.getWidthAndHeight(),
//		m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
//	m_activeViewport->m_cursorRay->m_center.update();
//	m_vd.m_app->setCursorRayCurrent( m_activeViewport->m_cursorRay->m_center );
//	
//	const s32 rayOffset = 10;
//	kkrooo::getRay(m_activeViewport->m_cursorRay->m_N, cursorPosition + v2i(0, -rayOffset), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
//	m_activeViewport->m_cursorRay->m_N.update();
//	kkrooo::getRay(m_activeViewport->m_cursorRay->m_S, cursorPosition + v2i(0, rayOffset), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
//	m_activeViewport->m_cursorRay->m_S.update();
//	kkrooo::getRay(m_activeViewport->m_cursorRay->m_W, cursorPosition + v2i(-rayOffset, 0), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
//	m_activeViewport->m_cursorRay->m_W.update();
//	kkrooo::getRay(m_activeViewport->m_cursorRay->m_E, cursorPosition + v2i(rayOffset, 0), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
//	m_activeViewport->m_cursorRay->m_E.update();
//
//	//auto d = m_activeViewport->m_cursorRay.distanceToLine( kkVector4(0.f,0.f,2.f), kkVector4(2.f, 0.f, 0.f, 0.f) );
//
////	printf("%f\n",d);
//
//	
//}
//
//kkVertex* Viewport::pickVertex(Scene3DObject* object)
//{
//	//if( m_drawEditMode_hoverMark )
//	//{
//	//	std::basic_string<kkVertex*> hovered_points;
//	//	auto camera_position = m_activeCamera->getPositionCamera();
//
//	//	auto & cverts = object->GetControlVertexArray();
//	//	auto & verts = object->GetVertexArray();
//	//	for( auto CV : cverts )
//	//	{
//	//		//auto & vert_inds = CV->getVertInds();
//	//		auto & verts = CV->getVerts();
//	//		auto V = verts[0];
//	//		auto point3D = math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot();
//	//		if( g_cursorSelFrust.pointInFrust(point3D) )
//	//		{
//	//			((ControlVertex*)CV)->m_distanceToCamera = camera_position.distance(point3D);
//	//			hovered_points.push_back((ControlVertex*)CV);
//	//		}
//	//	}
//	//	if(hovered_points.size()>1)
//	//	{
//	//		std::sort(hovered_points.begin(),hovered_points.end(),
//	//			[](ControlVertex* first, ControlVertex* second)
//	//			{
//	//				return first->m_distanceToCamera < second->m_distanceToCamera;
//	//			}
//	//		);
//
//	//	}
//	//	return hovered_points[0];
//	//}
//	
//	return nullptr;
//}
//
//kkScene3DObject* Viewport::pickObject()
//{
//	updateCursorRay();
//	kkScene3DObject* result = nullptr;
//	std::vector<Scene3DObject*> objects;
//	for( size_t i = 0; i < m_drawObjects.size(); ++i )
//	{
//		Scene3DObject* object = m_drawObjects[i];
//
//		if(object->isSelected())
//			continue;
//
//		auto & obb = object->Obb();
//		if( kkrooo::rayIntersection_obb(m_activeViewport->m_cursorRay->m_center, obb) )
//		{
//			kkRayTriangleIntersectionResultSimple intersectionResult;
//			if( object->IsRayIntersect(m_cursorRay->m_center, intersectionResult) )
//			{
//				auto camera_position = m_activeCamera->getPositionCamera();
//
//				object->m_distanceToCamera = camera_position.distance(intersectionResult.m_intersectionPoint);
//				objects.push_back(object);
//			}
//		}
//	}
//	if(objects.size())
//	{
//
//		std::sort(objects.begin(),objects.end(),
//			[](Scene3DObject* first, Scene3DObject* second)
//			{
//				return first->getDistanceToCamera() > second->getDistanceToCamera();
//			}
//		);
//
//		result = objects[0];
//	}
//	return result;
//}
//
//void Viewport::updateInputCamera()
//{
//	bool inRect = false;
//	if( kkrooo::pointInRect( *m_vd.m_cursor_position, m_activeViewport->m_rect_modified + v4f(0.f,m_vd.m_app->m_mainMenuStyle.menuBarHeight,0.f) ) || m_activeViewport->m_is_mouse_focus )
//	{
//		inRect = true;
//	}
//
//	if( inRect || m_activeViewport->m_is_mouse_focus )
//	{
//		if( *m_vd.m_mouseWheel > 0 )
//		{
//			m_activeViewport->m_activeCamera->zoomIn( *m_vd.m_state_keyboard , (s32)*m_vd.m_mouseWheel );
//			m_activeViewport->updateCursorRay();
//		}
//		else if( *m_vd.m_mouseWheel < 0 )
//		{
//			m_activeViewport->m_activeCamera->zoomOut( *m_vd.m_state_keyboard, (s32)*m_vd.m_mouseWheel );
//			m_activeViewport->updateCursorRay();
//		}
//		
//		if( m_vd.m_event_consumer->isMmbDown() || m_vd.m_event_consumer->isKeyDown( kkKey::K_SPACE )  )
//		{
//			m_activeViewport->m_is_mouse_focus = true;
//			if( *m_vd.m_state_keyboard == AppState_keyboard::Alt )
//			{
//				m_activeViewport->_rotate();
//			}
//			else
//			{
//				m_activeViewport->_panMove();
//			}
//		}
//
//		const s32 vertex_selection_area_val = 4;
//		m_activeViewport->m_activeCamera->update();
//		g_cursorSelFrust.createWithFrame(v4i(m_vd.m_cursor_position->x - vertex_selection_area_val, m_vd.m_cursor_position->y - vertex_selection_area_val,
//			m_vd.m_cursor_position->x + vertex_selection_area_val, m_vd.m_cursor_position->y + vertex_selection_area_val), 
//			m_activeViewport->m_rect_modified, m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//		m_drawEditMode_hoverMark = false;
//		if( (*m_activeViewport->m_scene3D_ptr)->isVertexHover(g_cursorSelFrust) )
//			m_drawEditMode_hoverMark = true;
//	}
//}
//
//void Viewport::updateInput()
//{
//	/*auto P = kkrooo::worldToScreen(m_activeCamera->m_kk_camera->getViewProjectionMatrix(), kkVector4(), 
//					m_rect_modified.getWidthAndHeight(), v2f());
//	printf("%i %i\n", P.x, P.y);*/
//
//	g_mouseState.reset();
//
//	if( m_ignoreInput )
//	{
//		m_ignoreInput = false;
//		return;
//	}
//	
//	f32 menuSizeY = m_vd.m_app->m_mainMenuStyle.menuBarHeight;
//
//	if( m_isMainMenuActive )
//		return;
//
//	g_mouseState.LMB_DOWN = m_vd.m_event_consumer->isLmbDownOnce();
//	g_mouseState.LMB_HOLD = m_vd.m_event_consumer->isLmbDown();
//	g_mouseState.LMB_UP   = m_vd.m_event_consumer->isLmbUp();
//	g_mouseState.RMB_DOWN = m_vd.m_event_consumer->isRmbDownOnce();
//	g_mouseState.RMB_HOLD = m_vd.m_event_consumer->isRmbDown();
//	g_mouseState.RMB_UP   = m_vd.m_event_consumer->isRmbUp();
//	g_mouseState.MMB_DOWN = m_vd.m_event_consumer->isMmbDownOnce();
//	g_mouseState.MMB_HOLD = m_vd.m_event_consumer->isMmbDown();
//	g_mouseState.MMB_UP   = m_vd.m_event_consumer->isMmbUp();
//
//	if( !m_drawContextMenu )
//		m_isContextHover = false;
//	
//
//	bool isGizmo = (*m_vd.m_state_app == AppState_main::Gizmo);
//
//	// когда крутим объект, и нажимаем RMB для отмены, отжатие LMB не должно снять выделение
//	static bool skipLMBHold = false; 
//
//
//	if( g_mouseState.RMB_DOWN )
//	{
//		skipLMBHold = true; 
//	}
//
//	if( g_mouseState.LMB_DOWN )
//	{
//		skipLMBHold = false; 
//	}
//
//	bool inRect = false;
//	if( kkrooo::pointInRect( *m_vd.m_cursor_position, m_activeViewport->m_rect_modified + v4f(0.f,menuSizeY,0.f) ) || m_activeViewport->m_is_mouse_focus )
//	{
//		inRect = true;
//		// определяю если курсор двигается
//		if( Kr::Gui::GuiSystem::m_mouseDelta.x != 0.f || Kr::Gui::GuiSystem::m_mouseDelta.y != 0.f )
//		{
//			g_mouseState.IsMove = true;
//		}
//	}
//
//	if( g_mouseState.LMB_UP && m_vd.m_app->m_state_app == AppState_main::CancelTransformation )
//	{
//		m_vd.m_app->m_state_app = AppState_main::Idle;
//	}
//
//	if( g_mouseState.LMB_HOLD )
//	{
//		if( !isGizmo && g_mouseState.IsMove && g_mouseState.IsFirstClick && m_vd.m_app->m_state_app != AppState_main::CancelTransformation )
//		{
//			g_mouseState.IsSelectByRect = true;
//		}
//	}
//
//	if( g_mouseState.MMB_DOWN  )
//	{
//		m_drawContextMenu = false;
//	}
//
//	if( g_mouseState.LMB_DOWN && inRect )
//	{
//		if( !g_mouseState.IsFirstClick && !m_isContextHover )
//		{
//			g_mouseState.IsFirstClick = true;
//			if( !m_contextMenuHovered )
//			{
//				m_drawContextMenu = false;
//
//				m_activeViewport->m_mouse_first_click_coords = *m_vd.m_cursor_position;
//				m_vd.m_app->setCursorRayFirstClick( m_activeViewport->m_cursorRay->m_center );
//			}
//		}
//	}
//	/*
//	if( m_objects_selected.size() == 1 )
//	{
//		printf("here\n");
//		//if( m_app->m_rightTabMode == RightTabMode::Edit )
//		//	m_app->_callOnActivateGUIPlugin();
//	}
//	*/
//	bool ignore_disabling_mouseInFocus = false;
//
//	if( inRect || m_activeViewport->m_is_mouse_focus )
//	{
//		if( m_vd.m_app->m_editMode == EditMode::Vertex )
//		{
//			if(g_mouseState.IsMove)
//			{
//				const s32 vertex_selection_area_val = 4;
//				g_cursorSelFrust.createWithFrame(v4i(m_vd.m_cursor_position->x - vertex_selection_area_val, m_vd.m_cursor_position->y - vertex_selection_area_val,
//					m_vd.m_cursor_position->x + vertex_selection_area_val, m_vd.m_cursor_position->y + vertex_selection_area_val), 
//					m_activeViewport->m_rect_modified, m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//			}
//
//			if( (*m_activeViewport->m_scene3D_ptr)->isVertexHover(g_cursorSelFrust) )
//			{
//				m_drawEditMode_hoverMark = true;
//			}
//			else
//			{
//				m_drawEditMode_hoverMark = false;
//			}
//		}
//
//		g_mouseState.InViewport = true;
//
//		/*if( *m_vd.m_mouseWheel > 0 )
//		{
//			m_activeViewport->m_activeCamera->zoomIn( *m_vd.m_state_keyboard , (s32)*m_vd.m_mouseWheel );
//			m_activeViewport->updateCursorRay();
//		}
//		else if( *m_vd.m_mouseWheel < 0 )
//		{
//			m_activeViewport->m_activeCamera->zoomOut( *m_vd.m_state_keyboard, (s32)*m_vd.m_mouseWheel );
//			m_activeViewport->updateCursorRay();
//		}*/
//		
//
//		if( *m_vd.m_state_app == AppState_main::Idle )
//		{
//			// если курсор движется, то беру луч
//			if( g_mouseState.IsMove && !g_mouseState.MMB_HOLD )
//			{
//				updateCursorRay();
//			}
//
//			if( g_mouseState.MMB_HOLD || m_vd.m_event_consumer->isKeyDown( kkKey::K_SPACE )  )
//			{
//				ignore_disabling_mouseInFocus = true;
//				/*m_activeViewport->m_is_mouse_focus = true;
//				if( *m_vd.m_state_keyboard == AppState_keyboard::Alt )
//				{
//					m_activeViewport->_rotate();
//				}
//				else
//				{
//					m_activeViewport->_panMove();
//				}*/
//			}
//		}
//
//		if( g_mouseState.LMB_UP && !isGizmo && !skipLMBHold && g_mouseState.IsFirstClick )
//		{
//
//			auto checkCursorHover = [&]()->bool
//			{
//				for( size_t i = 0; i < m_drawObjects.size(); ++i )
//				{
//					auto object = m_drawObjects[i];
//					auto & obb = object->Obb();
//
//					if( kkrooo::rayIntersection_obb(m_cursorRay->m_center, obb) )
//					{
//						// далее проверка на пересечение луч-треугольник
//						kkRayTriangleIntersectionResultSimple intersectionResult;
//						if( object->IsRayIntersect(m_cursorRay->m_center, intersectionResult) )
//						{
//							return true;
//						}
//					}
//				}
//				return false;
//			};
//
//			if( m_vd.m_app->m_editMode == EditMode::Object )
//			{
//				//g_ray = m_cursorRay;
//				if( m_activeViewport->m_hoveredObjects.size() )
//				{
//					if( m_activeViewport->m_hoveredObjects[m_activeViewport->m_hoveredObjects.size()-1]->isSelected() )
//					{
//						if( *m_vd.m_state_keyboard != AppState_keyboard::Ctrl && *m_vd.m_state_keyboard != AppState_keyboard::Alt )
//							(*m_activeViewport->m_scene3D_ptr)->deselectAll();
//						else
//							(*m_activeViewport->m_scene3D_ptr)->deselectObject( m_activeViewport->m_hoveredObjects[m_activeViewport->m_hoveredObjects.size()-1] );
//					}
//					else
//					{
//						if( *m_vd.m_state_keyboard != AppState_keyboard::Ctrl )
//							(*m_activeViewport->m_scene3D_ptr)->deselectAll();
//						(*m_activeViewport->m_scene3D_ptr)->selectObject( m_activeViewport->m_hoveredObjects[m_activeViewport->m_hoveredObjects.size()-1] );
//					}
//				}
//				else if(!g_mouseState.IsSelectByRect)
//					(*m_activeViewport->m_scene3D_ptr)->deselectAll();
//			}
//			else if( m_vd.m_app->m_editMode == EditMode::Vertex )
//			{
//				if( m_drawEditMode_hoverMark )
//				{
//					// все выделения и т.д. лучше произвести внутри 
//					(*m_activeViewport->m_scene3D_ptr)->doSelectVertexHover(g_cursorSelFrust, m_activeCamera);
//
//				}else if( *m_vd.m_state_keyboard != AppState_keyboard::Ctrl ) // если объекты выделены то их не надо терять
//				{
//						// если объектов под курсором нет то нужно снять выделение со всего
//					if( g_mouseState.InViewport && *m_vd.m_state_keyboard != AppState_keyboard::Alt )
//					{
//						(*m_activeViewport->m_scene3D_ptr)->deselectAll(); //printf("d3\n");
//					}
//				}
//			}
//			else if( m_vd.m_app->m_editMode == EditMode::Edge )
//			{
//				
//
//				if( checkCursorHover() )
//				{
//					if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(/*m_cursorRay, */&m_cursorRay->m_center/*, 0*/))
//					{
//						if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_N))
//						{
//							if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_E))
//							{
//								if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_S))
//								{
//									if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_W))
//									{
//									}
//								}
//							}
//						}
//					}
//				}
//				else
//				{
//					//printf("a");
//					if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_N))
//					{
//						if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_E))
//						{
//							if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_S))
//							{
//								if(!(*m_activeViewport->m_scene3D_ptr)->selectEdges(&m_cursorRay->m_W))
//								{
//									if( *m_vd.m_state_keyboard != AppState_keyboard::Alt && 
//										*m_vd.m_state_keyboard != AppState_keyboard::Ctrl )
//										(*m_activeViewport->m_scene3D_ptr)->deselectAll();
//								}
//							}
//						}
//					}
//				}
//
//				//printf("try edge\n");
//			}
//			else if( m_vd.m_app->m_editMode == EditMode::Polygon )
//			{
//				//if( checkCursorHover() )
//					(*m_activeViewport->m_scene3D_ptr)->selectPolygons(&m_cursorRay->m_center);
//				/*else
//				{
//				}*/
//			//	printf("try polygon\n");
//			}
//		}
//
//		if( g_mouseState.RMB_UP )
//		{
//			g_mouseState.IsSelectByRect = false;
//
//			if(!isGizmo)
//			{
//				m_drawContextMenu = true;
//
//				m_activeViewport->m_hoveredObjects_savedCopy.clear();
//				for( auto o : m_activeViewport->m_hoveredObjects )
//				{
//					m_activeViewport->m_drawContextMenu = true;
//					m_activeViewport->m_hoveredObjects_savedCopy.push_back( o );
//				}
//			}
//		}
//
//	}
//
//	if( g_mouseState.IsSelectByRect && g_mouseState.LMB_UP )
//	{
//		(*m_activeViewport->m_scene3D_ptr)->selectObjectsByRectangle( 
//			m_activeViewport->m_hoveredObjects, 
//			m_activeViewport->m_drawObjects, 
//			m_activeViewport->m_selectionFrame,
//			g_selFrust
//		);
//	}
//
//	//_processShortcuts();
//
//	_drawRecursivelyBorders(this);
//
//	if( !ignore_disabling_mouseInFocus )
//		m_activeViewport->m_is_mouse_focus = false;
//}
//
//void Viewport::_drawAxis(Viewport* v)
//{
//	m_vd.m_gs->setViewport(
//		(s32)v->m_rect_modified.z - 50,
//		m_vd.m_window_client_size->y - (s32)v->m_rect_modified.w + 10,
//		(s32)50,
//		(s32)50 
//	);
//
//	m_vd.m_gs->setActiveCamera(v->m_activeCamera->getAxisCamera());
//	m_vd.m_gs->drawLine3D(kkVector4(0.f,0.f,0.f,1.f),kkVector4(1.f,0.f,0.f,1.f),m_vd.m_current_color_theme->viewport_grid_color1);
//	m_vd.m_gs->drawLine3D(kkVector4(0.f,0.f,0.f,1.f),kkVector4(0.f,0.f,1.f,1.f),m_vd.m_current_color_theme->viewport_grid_color2);
//	m_vd.m_gs->drawLine3D(kkVector4(0.f,0.f,0.f,1.f),kkVector4(0.f,1.f,0.f,1.f),m_vd.m_current_color_theme->viewport_grid_color5);
//}
//
//void Viewport::_toggleFullscreen()
//{
//	static Viewport * old_active = nullptr;
//
//	if( !m_mainViewport->m_children.size() )
//		return;
//
//	if( !m_mainViewport->m_isMaximized )
//	{
//		old_active = m_activeViewport;
//
//		m_vd.m_app->setActiveViewport(m_maximizedViewport);
//
//		m_mainViewport->m_isMaximized    = true;
//		m_maximizedViewport->m_is_active = true;
//
//		_setMaximizedViewportPointers(old_active);
//	}
//	else
//	{
//		m_vd.m_app->setActiveViewport(old_active);
//		m_mainViewport->m_isMaximized    = false;
//
//		//if( m_maximizedViewport )
//		{
//			m_maximizedViewport->m_is_active = false;
//			_setMaximizedViewportPointers(nullptr);
//		}
//	}
//	update();
//}
//
//void Viewport::initMaximizedViewport()
//{
//	m_maximizedViewport = kkCreate<Viewport>(m_vd);
//
//	if(m_maximizedViewport->m_cameraPersp)
//		kkDestroy(m_maximizedViewport->m_cameraPersp);
//	if(m_maximizedViewport->m_cameraBack)
//		kkDestroy(m_maximizedViewport->m_cameraBack);
//	if(m_maximizedViewport->m_cameraFront)
//		kkDestroy(m_maximizedViewport->m_cameraFront);
//	if(m_maximizedViewport->m_cameraTop)
//		kkDestroy(m_maximizedViewport->m_cameraTop);
//	if(m_maximizedViewport->m_cameraBottom)
//		kkDestroy(m_maximizedViewport->m_cameraBottom);
//	if(m_maximizedViewport->m_cameraLeft)
//		kkDestroy(m_maximizedViewport->m_cameraLeft);
//	if(m_maximizedViewport->m_cameraRight)
//		kkDestroy(m_maximizedViewport->m_cameraRight);
//
//	m_maximizedViewport->m_cameraPersp = nullptr;
//    m_maximizedViewport->m_cameraBack  = nullptr;
//    m_maximizedViewport->m_cameraFront = nullptr;
//    m_maximizedViewport->m_cameraTop   = nullptr;
//    m_maximizedViewport->m_cameraBottom= nullptr;
//    m_maximizedViewport->m_cameraLeft  = nullptr;
//    m_maximizedViewport->m_cameraRight = nullptr;
//}
//
//// m_maximizedViewport должен управлять камерами активного вьюпорта
//void Viewport::_setMaximizedViewportPointers(Viewport* v)
//{
//	static Viewport* old_owner = nullptr; // скорее всего это значение равно old_active
//
//	if(v)
//	{
//		m_maximizedViewport->m_cameraPersp = v->m_cameraPersp;
//		m_maximizedViewport->m_cameraBack  = v->m_cameraBack;
//		m_maximizedViewport->m_cameraFront = v->m_cameraFront;
//		m_maximizedViewport->m_cameraTop   = v->m_cameraTop;
//		m_maximizedViewport->m_cameraBottom= v->m_cameraBottom;
//		m_maximizedViewport->m_cameraLeft  = v->m_cameraLeft;
//		m_maximizedViewport->m_cameraRight = v->m_cameraRight;
//
//		old_owner = v->m_cameraPersp->getOwner();
//
//		m_maximizedViewport->m_cameraBack->setOwner(m_maximizedViewport);
//		m_maximizedViewport->m_cameraFront->setOwner(m_maximizedViewport);
//		m_maximizedViewport->m_cameraTop->setOwner(m_maximizedViewport);
//		m_maximizedViewport->m_cameraBottom->setOwner(m_maximizedViewport);
//		m_maximizedViewport->m_cameraLeft->setOwner(m_maximizedViewport);
//		m_maximizedViewport->m_cameraRight->setOwner(m_maximizedViewport);
//
//		m_maximizedViewport->m_activeCamera= v->m_activeCamera;
//
//		m_maximizedViewport->m_isActivePerspective    = v->m_isActivePerspective;
//		m_maximizedViewport->m_isActiveTop    = v->m_isActiveTop;
//		m_maximizedViewport->m_isActiveBottom = v->m_isActiveBottom;
//		m_maximizedViewport->m_isActiveLeft   = v->m_isActiveLeft;
//		m_maximizedViewport->m_isActiveRight  = v->m_isActiveRight;
//		m_maximizedViewport->m_isActiveFront  = v->m_isActiveFront;
//		m_maximizedViewport->m_isActiveBack   = v->m_isActiveBack;
//		
//		m_maximizedViewport->m_isDrawGrid   = v->m_isDrawGrid;
//		m_maximizedViewport->m_draw_mode    = v->m_draw_mode;
//	}
//	else
//	{
//		m_maximizedViewport->m_cameraPersp = nullptr;
//		m_maximizedViewport->m_cameraBack  = nullptr;
//		m_maximizedViewport->m_cameraFront = nullptr;
//		m_maximizedViewport->m_cameraTop   = nullptr;
//		m_maximizedViewport->m_cameraBottom= nullptr;
//		m_maximizedViewport->m_cameraLeft  = nullptr;
//		m_maximizedViewport->m_cameraRight = nullptr;
//
//		m_activeViewport->m_activeCamera   = m_maximizedViewport->m_activeCamera;
//		m_activeViewport->m_isActivePerspective    = m_maximizedViewport->m_isActivePerspective;
//		m_activeViewport->m_isActiveTop    = m_maximizedViewport->m_isActiveTop;
//		m_activeViewport->m_isActiveBottom = m_maximizedViewport->m_isActiveBottom;
//		m_activeViewport->m_isActiveLeft   = m_maximizedViewport->m_isActiveLeft;
//		m_activeViewport->m_isActiveRight  = m_maximizedViewport->m_isActiveRight;
//		m_activeViewport->m_isActiveFront  = m_maximizedViewport->m_isActiveFront;
//		m_activeViewport->m_isActiveBack   = m_maximizedViewport->m_isActiveBack;
//		m_activeViewport->m_isDrawGrid     = m_maximizedViewport->m_isDrawGrid;
//		m_activeViewport->m_draw_mode      = m_maximizedViewport->m_draw_mode;
//
//		m_activeViewport->m_cameraBack->setOwner(old_owner);
//		m_activeViewport->m_cameraFront->setOwner(old_owner);
//		m_activeViewport->m_cameraTop->setOwner(old_owner);
//		m_activeViewport->m_cameraBottom->setOwner(old_owner);
//		m_activeViewport->m_cameraLeft->setOwner(old_owner);
//		m_activeViewport->m_cameraRight->setOwner(old_owner);
//	}
//}

//
//void Viewport::_drawEditMode_hoverMark()
//{
//	m_vd.m_gs->useScissor(true);
//	m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->setScissor((s32)m_viewport_to_gl_funk.x,(s32)m_viewport_to_gl_funk.y,(s32)m_viewport_to_gl_funk.z,(s32)m_viewport_to_gl_funk.w );
//
//	m_vd.m_gs->drawCircle2D(*m_vd.m_cursor_position, 5, 2, kkColorGreenYellow );
//	m_vd.m_gs->drawCircle2D(*m_vd.m_cursor_position, 6, 3, kkColorGreenYellow );
//	m_vd.m_gs->drawCircle2D(*m_vd.m_cursor_position, 7, 4, kkColorGreenYellow );
//
//	m_vd.m_gs->setScissor(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->useScissor(false);
//}
//
//void Viewport::_drawSelectionRectangle()
//{
//	//_setGLViewport();
//	m_vd.m_gs->useScissor(true);
//	m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->setScissor((s32)m_viewport_to_gl_funk.x,(s32)m_viewport_to_gl_funk.y,(s32)m_viewport_to_gl_funk.z,(s32)m_viewport_to_gl_funk.w );
//
//	auto p1 = *m_vd.m_cursor_position;
//	auto p2 = v2i( m_vd.m_cursor_position->x, m_mouse_first_click_coords.y );
//	auto p3 = v2i( m_mouse_first_click_coords.x, m_vd.m_cursor_position->y );
//	auto p4 = v2i( m_mouse_first_click_coords.x, m_mouse_first_click_coords.y );
//
//	m_vd.m_gs->drawLine2D(p1, p2, kkColorWhite);
//	m_vd.m_gs->drawLine2D(p3, p4, kkColorWhite);
//
//	m_vd.m_gs->drawLine2D(p1, p3, kkColorWhite);
//	m_vd.m_gs->drawLine2D(p2, p4, kkColorWhite);
//
//	
//	
//
//	m_vd.m_gs->setScissor(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->useScissor(false);
//
//	kkAabb aabb;
//	aabb.add( kkVector4( (f32)m_vd.m_cursor_position->x, (f32)m_vd.m_cursor_position->y, 0.f ) );
//	aabb.add( kkVector4( (f32)m_mouse_first_click_coords.x, (f32)m_mouse_first_click_coords.y, 0.f ) );
//
//	m_selectionFrame.x = (s32)aabb.m_min.KK_X;
//	m_selectionFrame.y = (s32)aabb.m_min.KK_Y;
//	m_selectionFrame.z = (s32)aabb.m_max.KK_X;
//	m_selectionFrame.w = (s32)aabb.m_max.KK_Y;
//
//
//	g_selFrust.createWithFrame(m_selectionFrame, m_activeViewport->m_rect_modified, m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//	// get 4 rays from screen
//	/*kkRay ray1, ray2, ray3, ray4;
//
//	kkrooo::getRay(ray1, v2i(m_selectionFrame.x, m_selectionFrame.y), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//	kkrooo::getRay(ray2, v2i(m_selectionFrame.z, m_selectionFrame.y), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//	kkrooo::getRay(ray3, v2i(m_selectionFrame.x, m_selectionFrame.w), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//	kkrooo::getRay(ray4, v2i(m_selectionFrame.z, m_selectionFrame.w), m_activeViewport->m_rect_modified, m_activeViewport->m_rect_modified.getWidthAndHeight(),m_activeViewport->m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
//	ray1.update();
//	ray2.update();
//	ray3.update();
//	ray4.update();
//
//	g_selFrust.m_top[0] = ray1.m_origin;
//	g_selFrust.m_top[1] = ray2.m_origin;
//	g_selFrust.m_top[2] = ray1.m_end;
//	g_selFrust.m_top[3] = ray2.m_end;
//
//	g_selFrust.m_right[0] = ray2.m_origin;
//	g_selFrust.m_right[1] = ray4.m_origin;
//	g_selFrust.m_right[2] = ray2.m_end;
//	g_selFrust.m_right[3] = ray4.m_end;
//
//	g_selFrust.m_bottom[0] = ray4.m_origin;
//	g_selFrust.m_bottom[1] = ray3.m_origin;
//	g_selFrust.m_bottom[2] = ray4.m_end;
//	g_selFrust.m_bottom[3] = ray3.m_end;
//
//	g_selFrust.m_left[0] = ray3.m_origin;
//	g_selFrust.m_left[1] = ray1.m_origin;
//	g_selFrust.m_left[2] = ray3.m_end;
//	g_selFrust.m_left[3] = ray1.m_end;
//
//	kkVector4 e1, e2;
//	
//	e1 = g_selFrust.m_right[1] - g_selFrust.m_right[0];
//	e2 = g_selFrust.m_right[2] - g_selFrust.m_right[0];
//	e1.cross(e2, g_selFrust.m_RN);
//	g_selFrust.m_RC = g_selFrust.m_right[0] + g_selFrust.m_right[1] + g_selFrust.m_right[2]+g_selFrust.m_right[3];
//	g_selFrust.m_RC *= 0.25;
//
//	e1 = g_selFrust.m_bottom[1] - g_selFrust.m_bottom[0];
//	e2 = g_selFrust.m_bottom[2] - g_selFrust.m_bottom[0];
//	e1.cross(e2, g_selFrust.m_BN);
//	g_selFrust.m_BC = g_selFrust.m_bottom[0] + g_selFrust.m_bottom[1] + g_selFrust.m_bottom[2]+g_selFrust.m_bottom[3];
//	g_selFrust.m_BC *= 0.25;
//
//	e1 = g_selFrust.m_top[1] - g_selFrust.m_top[0];
//	e2 = g_selFrust.m_top[2] - g_selFrust.m_top[0];
//	e1.cross(e2, g_selFrust.m_TN);
//	g_selFrust.m_TC = g_selFrust.m_top[0] + g_selFrust.m_top[1] + g_selFrust.m_top[2]+g_selFrust.m_top[3];
//	g_selFrust.m_TC *= 0.25;
//
//	e1 = g_selFrust.m_left[1] - g_selFrust.m_left[0];
//	e2 = g_selFrust.m_left[2] - g_selFrust.m_left[0];
//	e1.cross(e2, g_selFrust.m_LN);
//	g_selFrust.m_LC = g_selFrust.m_left[0] + g_selFrust.m_left[1] + g_selFrust.m_left[2]+g_selFrust.m_left[3];
//	g_selFrust.m_LC *= 0.25;*/
//}
//
//void Viewport::_drawGizmo2DPart(const v2i& point2d)
//{
//	m_vd.m_gs->useScissor(true);
//	m_vd.m_gs->setViewport(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->setScissor((s32)m_viewport_to_gl_funk.x,(s32)m_viewport_to_gl_funk.y,(s32)m_viewport_to_gl_funk.z,(s32)m_viewport_to_gl_funk.w );
//	
//	auto vp_sz = m_rect_modified.getWidthAndHeight();
//
//	if( m_activeViewport == this )
//	{
//		switch(m_vd.m_app->getSelectMode())
//		{
//		case SelectMode::JustSelect:
//		default:
//			break;
//		case SelectMode::Move:
//			m_vd.m_gizmo->drawMove2D( m_vd.m_cursor_position, point2d );
//			break;
//		case SelectMode::Rotate:
//			m_vd.m_gizmo->drawRotation2D( m_vd.m_cursor_position, vp_sz, m_rect_modified );
//			break;
//		case SelectMode::Scale:
//			m_vd.m_gizmo->drawScale2D( m_vd.m_cursor_position, point2d );
//			break;
//		}
//	}
//	m_vd.m_gs->setScissor(0,0,m_vd.m_window_client_size->x,m_vd.m_window_client_size->y);
//	m_vd.m_gs->useScissor(false);
//}
//
//void Viewport::onLoseFocus()
//{
//	g_mouseState.IsSelectByRect = false;
////	m_is_mouse_first_click        = false;
//	//m_drawContextMenu             = false;
//}
//
//void Viewport::_drawSelectedObjectFrame()
//{
//	kkVector4  ext;
//	f32 frameSize   = 0.f;
//	f32 frameIndent = 0.f;
//
//	for( u32 i = 0, sz = (*m_scene3D_ptr)->getNumOfSelectedObjects(); i < sz; ++i )
//	{
//		auto obj = (*m_scene3D_ptr)->getSelectedObject( i );
//
//		auto & aabb = obj->Aabb();
//		aabb.m_max.KK_W = 0.f;
//		aabb.m_min.KK_W = 0.f;
//
//		frameSize = aabb.m_max.distance( aabb.m_min )+0.01f;
//		frameSize /= 12.f;
//		frameIndent = frameSize * 0.2f;
//
//		//printf("frameSize %f\n",frameSize);
//
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y-frameSize,   aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_max.KK_Y+frameIndent,   aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
//		
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
//
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
//		
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
//
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
//		m_vd.m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
//	}
//}
//
//void Viewport::_drawSelectedObjectPivot()
//{
//	if( m_vd.m_app->isGlobalInputBlocked() )
//		return;
//
//	auto numOfObjects = (*m_scene3D_ptr)->getNumOfSelectedObjects();
//
//	if( !numOfObjects )
//		return;
//
//	f32 size = 0.f;
//
//	
//
//	if( m_activeCamera->getType() != ViewportCameraType::Perspective )
//    {
//        size = 0.5f / m_activeCamera->getZoomOrt();
//    }
//
//	kkVector4 point;
//	
//	if( numOfObjects == 1 )
//	{
//		auto obj = (*m_scene3D_ptr)->getSelectedObject( 0 );
//		point = obj->GetPivot();
//	}
//	else
//	{
//		auto	aabb = (*m_scene3D_ptr)->getSelectionAabb();
//		aabb.center(point);
//	}
//
//	if( m_vd.m_app->m_editMode == EditMode::Vertex
//		|| m_vd.m_app->m_editMode == EditMode::Edge
//		|| m_vd.m_app->m_editMode == EditMode::Polygon )
//	{
//		(*m_scene3D_ptr)->getSelectionAabb().center(point);
//
//	}
//
//	if( m_activeCamera->getType() == ViewportCameraType::Perspective )
//	{
//		size = (f32)m_activeCamera->getPositionCamera().distance(point);
//		size *= 0.1f;
//	}
//
//	// TEST target\direction
//	/*auto cameraTergaet = m_activeCamera->getCamera()->getDirection();
//	m_vd.m_gs->useDepth(true);
//	m_vd.m_gs->drawLine3D( cameraTergaet, kkVector4( cameraTergaet.KK_X, cameraTergaet.KK_Y, cameraTergaet.KK_Z + size ), kkColorRed);
//	m_vd.m_gs->drawLine3D( cameraTergaet, kkVector4( cameraTergaet.KK_X, cameraTergaet.KK_Y+ size, cameraTergaet.KK_Z  ), kkColorRed );
//	m_vd.m_gs->drawLine3D( cameraTergaet, kkVector4( cameraTergaet.KK_X+ size, cameraTergaet.KK_Y, cameraTergaet.KK_Z  ), kkColorRed );*/
//	
//	m_vd.m_gs->useDepth(false);
//
//	m_vd.m_gs->drawLine3D( point, kkVector4( point.KK_X, point.KK_Y, point.KK_Z + size ), kkColorLightGray );
//	m_vd.m_gs->drawLine3D( point, kkVector4( point.KK_X, point.KK_Y+ size, point.KK_Z  ), kkColorLightGray );
//	m_vd.m_gs->drawLine3D( point, kkVector4( point.KK_X+ size, point.KK_Y, point.KK_Z  ), kkColorLightGray );
//
//	if( m_activeViewport == this )
//	{
//		for( auto o : (*m_scene3D_ptr)->m_objects_selected )
//		{
//			if( m_vd.m_app->m_editMode == EditMode::Object 
//				|| (o->m_isObjectHaveSelectedVerts && m_vd.m_app->m_editMode == EditMode::Vertex)
//				|| ( o->m_isObjectHaveSelectedEdges && m_vd.m_app->m_editMode == EditMode::Edge)
//				|| ( o->m_isObjectHaveSelectedPolys && m_vd.m_app->m_editMode == EditMode::Polygon) )
//			{
//				m_vd.m_app->m_currentGizmoEvent.point2D = kkrooo::worldToScreen( m_activeCamera->m_kk_camera->getViewProjectionMatrix(), point, 
//					m_rect_modified.getWidthAndHeight(),
//					v2f(m_rect_modified.x,m_rect_modified.y) );
//
//				switch(m_vd.m_app->getSelectMode())
//				{
//				case SelectMode::JustSelect:
//				default:
//					break;
//				case SelectMode::Move:
//					m_vd.m_gizmo->drawMove(point, size, m_cursorRay->m_center);
//					break;
//				case SelectMode::Rotate:
//					m_vd.m_app->m_currentGizmoEvent.point2D = kkrooo::worldToScreen( m_activeCamera->m_kk_camera->getViewProjectionMatrix(), point, 
//						m_rect_modified.getWidthAndHeight(),
//						v2f(m_rect_modified.x,m_rect_modified.y) );
//
//					m_vd.m_gizmo->drawRotation(point, size, m_cursorRay->m_center);
//					break;
//				case SelectMode::Scale:
//					m_vd.m_gizmo->drawScale(point, size, m_cursorRay->m_center);
//					break;
//				}
//		
//				_drawGizmo2DPart( m_vd.m_app->m_currentGizmoEvent.point2D );
//				break;
//			}
//		}
//	}
//	
//
//	m_vd.m_gs->useDepth(true);
//}
//
//void Viewport::onDeleteObjects()
//{
//	_onDeleteObjects(this);
//}
//
//void Viewport::onWindowActivate()
//{
//	if(m_activeViewport)
//	{
//		m_activeViewport->m_cursorRay->m_center.m_origin.set(0.f,0.f,0.f,0.f);
//		m_activeViewport->m_cursorRay->m_center.m_end = m_activeViewport->m_cursorRay->m_center.m_origin;
//	}
//}
//
//void Viewport::_onDeleteObjects(Viewport* v)
//{
//	v->m_hoveredObjects.clear();
//	v->m_drawObjects.clear();
//	v->m_objects_inFrustum.clear();
//
//	for( size_t i = 0, sz = v->m_children.size(); i < sz; ++i )
//	{
//		_onDeleteObjects(v->m_children[i]);
//	}
//}
//
//void Viewport::onFrame()
//{
//	m_contextMenuHovered = false;
//	m_ignoreInput = false;
//}
//
//void Viewport::onNewObject()
//{
//}
//


//
//void Viewport::setDrawPickLine(bool v)
//{
//	m_drawPickLine = v;
//	m_drawPickLineP1 = *m_vd.m_cursor_position;
//}
//
