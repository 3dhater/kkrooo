#include "kkrooo.engine.h"
#include "GraphicsSystem/kkTexture.h"
#include "KrGui.h"
#include "../Common.h"
#include "Viewport.h"
#include "ViewportCamera.h"
#include "ViewportOptimizations.h"
#include "GraphicsSystem/kkGraphicsSystem.h"
#include "../GUI/ColorTheme.h"
#include "../ApplicationState.h"
#include "../CursorRay.h"
#include "../ShortcutManager.h"
#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"
#include "../SelectionFrust.h"
#include "../Application.h"
#include "../EventConsumer.h"
#include "../Gizmo.h"

SelectionFrust g_cursorSelFrust;
SelectionFrust g_hoverSelFrust;
ViewportMouseState g_mouseState;
kkGraphicsSystem* g_GS = nullptr;
Kr::Gui::GuiSystem* g_GUI = nullptr;

ViewportObject::ViewportObject()
{
	m_app = kkSingleton<Application>::s_instance;
}
ViewportObject::~ViewportObject()
{
	//if(m_silhouetteFBO)
	//	kkDestroy(m_silhouetteFBO);
	delete m_cursorRay;
}
void ViewportObject::_updateObjectsInFrustum()
{
	m_objects_inFrustum.clear();
	m_drawObjects.clear();
	// пока просто добавлю все объекты
	auto objects = m_app->m_current_scene3D->getObjects();
	auto frust = m_activeCamera->getCamera()->getFrustum();
	auto camera_position = m_activeCamera->getPositionCamera();

	kkAabb    aabb;
	kkObb     obb;
	kkVector4 center;
	kkVector4 extent;
	u32     object_index = 0;
	for( size_t i = 0, sz = objects.size(); i < sz; ++i )
	{
		auto object = objects[i];
		obb  = object->Obb();
		aabb = object->Aabb();
		aabb.center(center);
		object->m_distanceToCamera = camera_position.distance(center);
		if( OBBInFrustum( obb, frust ) )
		{
			++object_index;
			m_objects_inFrustum.push_back(object);
			m_drawObjects.push_back(object);
		}
	}
	// нужно оптимизировать. не сортировать зря.
	// по сути можно добавить bool, и проверять выше.
	// если изменили позицию камеры или повернули или добавили объект то ИСТИНА, надо обновить
	//    возможно перед вызовом _updateObjectsInFrustum()
	sortObjectsInFrustum( m_drawObjects );
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
	this->update(m_app->m_window_client_size);
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
	if( m_activeCamera->m_isRotated && m_activeCamera != m_cameraPersp.ptr() )
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
		kkDrawAll();
	}

	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetPerspective) ){setActiveCamera( m_cameraPersp.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetFront) ){setActiveCamera( m_cameraFront.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetBack) ){setActiveCamera( m_cameraBack.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetRight) ){setActiveCamera( m_cameraRight.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetLeft) ){setActiveCamera( m_cameraLeft.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetTop) ){setActiveCamera( m_cameraTop.ptr() );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::SetBottom) ){setActiveCamera( m_cameraBottom.ptr() );}

	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeMaterial) ){setDrawMode( DrawMode::Material );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeLines) ){setDrawMode( DrawMode::Edge );}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::DrawModeMaterialAndLines) ){setDrawMode( DrawMode::EdgesAndMaterial );}
	
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::ToggleDrawModeMaterial) ){toggleDrawModeMaterial();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Viewport::ToggleDrawModeLines) ){toggleDrawModeLines();}


	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Camera::Reset) ){resetCamera();}
	if( m_shortcutManager->isShortcutActive(ShortcutCommand_Camera::ToSelection) ){moveToSelection();}
}
void ViewportObject::moveToSelection()
{
	if( !m_app->m_current_scene3D->getNumOfObjectsOnScene() )
	{
		resetCamera();
	}
	else
	{
		kkAabb aabb;
		if( m_app->m_current_scene3D->getNumOfSelectedObjects() )
		{
			// установить камеру на центр aabb выбранных объектов
			aabb = m_app->m_current_scene3D->getSelectionAabb();
		}
		else
		{
			aabb = m_app->m_current_scene3D->getSceneAabb();
		}
		m_activeCamera->centerToAabb(aabb);
		kkDrawAll();
	}
}
void ViewportObject::toggleDrawModeMaterial()
{
	static bool is_materail_mode = false;

	if( m_draw_mode == DrawMode::Edge )
	{
		if( is_materail_mode )
		{
			m_draw_mode = DrawMode::Material;
			is_materail_mode = false;
		}
		else
			m_draw_mode = DrawMode::EdgesAndMaterial;
	}
	else if( m_draw_mode == DrawMode::EdgesAndMaterial )
		m_draw_mode = DrawMode::Edge;
	else if( m_draw_mode == DrawMode::Material )
	{
		m_draw_mode = DrawMode::Edge;
		is_materail_mode = true;
	}
	kkDrawAll();
}

void ViewportObject::toggleDrawModeLines()
{
	if( m_draw_mode == DrawMode::Material )
		m_draw_mode = DrawMode::EdgesAndMaterial;
	else if( m_draw_mode == DrawMode::EdgesAndMaterial )
		m_draw_mode = DrawMode::Material;
	kkDrawAll();
}
void ViewportObject::setDrawMode( DrawMode m )
{
	m_draw_mode = m;
	kkDrawAll();
}
void ViewportObject::resetCamera()
{
	m_activeCamera->reset();
	m_activeCamera->m_isRotated   = false;
	this->update(m_app->m_window_client_size);
	kkDrawAll();
}
void ViewportObject::updateCursorRay()
{
	if( m_app->m_globalInputBlock )
		return;
	kkrooo::getRay(
		m_cursorRay->m_center, 
		m_app->m_cursor_position, 
		m_rect_modified, 
		m_rect_modified.getWidthAndHeight(),
		m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_center.update();

	const s32 rayOffset = 10;
	kkrooo::getRay(m_cursorRay->m_N, m_app->m_cursor_position + v2i(0, -rayOffset), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_N.update();
	kkrooo::getRay(m_cursorRay->m_S, m_app->m_cursor_position + v2i(0, rayOffset), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_S.update();
	kkrooo::getRay(m_cursorRay->m_W, m_app->m_cursor_position + v2i(-rayOffset, 0), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_W.update();
	kkrooo::getRay(m_cursorRay->m_E, m_app->m_cursor_position + v2i(rayOffset, 0), m_rect_modified, m_rect_modified.getWidthAndHeight(),m_activeCamera->getCamera()->getViewProjectionInvertMatrix() );
	m_cursorRay->m_E.update();
}
void ViewportObject::_panMove()
{
	m_activeCamera->movePan( m_app->m_state_keyboard, Kr::Gui::GuiSystem::m_mouseDelta.x, Kr::Gui::GuiSystem::m_mouseDelta.y );
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
// return true if this viewport set focus
bool ViewportObject::updateInput(const v2i& windowSize, const v2f& mouseDelta, bool inFocus)
{
	bool res = false;
	_update_frame(mouseDelta);
	update(windowSize);
	
	g_mouseState.IsMove = false;
	m_cursorInRect = false;
	if( kkrooo::pointInRect( m_app->m_cursor_position, m_rect_modified ))
	{
		m_cursorInRect = true;
		// определяю если курсор двигается
		if( Kr::Gui::GuiSystem::m_mouseDelta.x != 0.f 
			|| Kr::Gui::GuiSystem::m_mouseDelta.y != 0.f )
			g_mouseState.IsMove = true;
	}
	if( g_mouseState.RMB_DOWN && m_cursorInRect )
	{
		if( !g_mouseState.IsFirstClickRMB )
		{
			res = true;
			g_mouseState.IsFirstClickRMB = true;
		}
	}
	if( g_mouseState.MMB_DOWN && m_cursorInRect )
	{
		if( !g_mouseState.IsFirstClickMMB )
		{
			res = true;
			g_mouseState.IsFirstClickMMB = true;
		}
	}
	if( g_mouseState.LMB_DOWN && m_cursorInRect )
	{
		if( !g_mouseState.IsFirstClickLMB )
		{
			res = true;
			g_mouseState.IsFirstClickLMB = true;
			m_mouse_first_click_coords = m_app->m_cursor_position;
		}
	}

	if( m_app->m_editMode == EditMode::Vertex )
	{
		if(g_mouseState.IsMove)
		{
			const s32 vertex_selection_area_val = 4;
			g_hoverSelFrust.createWithFrame(v4i(m_app->m_cursor_position.x - vertex_selection_area_val, m_app->m_cursor_position.y - vertex_selection_area_val,
				m_app->m_cursor_position.x + vertex_selection_area_val, m_app->m_cursor_position.y + vertex_selection_area_val), 
				m_rect_modified, m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
		}
		if( m_app->m_current_scene3D->isVertexHover(g_hoverSelFrust) )
		{
			m_drawEditMode_hoverMark = true;
		}
		else
		{
			m_drawEditMode_hoverMark = false;
		}
	}

	if(m_cursorInRect)
	{
		if( m_app->m_mouseWheel > 0 )
		{
			m_activeCamera->zoomIn( m_app->m_state_keyboard, (s32)m_app->m_mouseWheel );
			updateCursorRay();
			kkDrawAll();
		}
		else if( m_app->m_mouseWheel < 0 )
		{
			m_activeCamera->zoomOut( m_app->m_state_keyboard, (s32)m_app->m_mouseWheel );
			updateCursorRay();
			kkDrawAll();
		}
	}

	if(!inFocus)
		return res;

	updateCursorRay();
	//g_mouseState.reset();
	g_mouseState.LMB_DOWN = m_app->m_event_consumer->isLmbDownOnce();
	if(g_mouseState.LMB_DOWN)
		m_rayOnClick = m_cursorRay->m_center;
	g_mouseState.LMB_HOLD = m_app->m_event_consumer->isLmbDown();
	g_mouseState.LMB_UP   = m_app->m_event_consumer->isLmbUp();
	g_mouseState.RMB_DOWN = m_app->m_event_consumer->isRmbDownOnce();
	g_mouseState.RMB_HOLD = m_app->m_event_consumer->isRmbDown();
	g_mouseState.RMB_UP   = m_app->m_event_consumer->isRmbUp();
	g_mouseState.MMB_DOWN = m_app->m_event_consumer->isMmbDownOnce();
	g_mouseState.MMB_HOLD = m_app->m_event_consumer->isMmbDown();
	g_mouseState.MMB_UP   = m_app->m_event_consumer->isMmbUp();

	bool isGizmo = (m_app->m_state_app == AppState_main::Gizmo);

	if( m_app->m_state_app == AppState_main::Idle )
	{
		// если курсор движется, то беру луч
		if( g_mouseState.IsMove && !g_mouseState.MMB_HOLD )
			updateCursorRay();
	}

	if( (g_mouseState.MMB_HOLD  && g_mouseState.IsFirstClickMMB) 
		|| m_app->m_event_consumer->isKeyDown( kkKey::K_SPACE )
		)
	{
		if( m_app->m_state_keyboard == AppState_keyboard::Alt )
			_rotate();
		else
			_panMove();
		m_app->m_state_app = AppState_main::CameraTransformation;
	}

	//if( g_mouseState.LMB_UP && !isGizmo && g_mouseState.IsFirstClickLMB )
	//{
	//	auto checkCursorHover = [&]()->bool
	//	{
	//		for( size_t i = 0; i < m_drawObjects.size(); ++i )
	//		{
	//			auto object = m_drawObjects[i];
	//			auto & obb = object->Obb();
	//			if( kkrooo::rayIntersection_obb(m_cursorRay->m_center, obb) )
	//			{
	//				// далее проверка на пересечение луч-треугольник
	//				kkRayTriangleIntersectionResultSimple intersectionResult;
	//				if( object->IsRayIntersect(m_cursorRay->m_center, intersectionResult) )
	//					return true;
	//			}
	//		}
	//		return false;
	//	};

	if(m_app->m_state_app == AppState_main::CancelTransformation)
	{
		g_mouseState.IsFirstClickLMB = false;
		m_app->m_state_app = AppState_main::Idle;
	}

	//printf("[%i] [%i] [%i]\n", (s32)g_mouseState.LMB_UP, (s32)g_mouseState.IsFirstClickLMB,
	//	(s32)g_mouseState.IsSelectByRect);

	if( m_app->m_editMode == EditMode::Object )
	{
		if(g_mouseState.LMB_UP 
			&& m_app->m_state_app != AppState_main::Gizmo
			&& m_app->m_state_app != AppState_main::GuiInput
			&& g_mouseState.IsFirstClickLMB )
		{
			if( m_hoveredObjects.size() )
			{
				if( m_hoveredObjects[m_hoveredObjects.size()-1]->isSelected() )
				{
					if( m_app->m_state_keyboard != AppState_keyboard::Ctrl && m_app->m_state_keyboard != AppState_keyboard::Alt )
						m_app->m_current_scene3D->deselectAll();
					else
						m_app->m_current_scene3D->deselectObject( m_hoveredObjects[m_hoveredObjects.size()-1] );
				}
				else
				{
					if( m_app->m_state_keyboard != AppState_keyboard::Ctrl )
						m_app->m_current_scene3D->deselectAll();
					m_app->m_current_scene3D->selectObject( m_hoveredObjects[m_hoveredObjects.size()-1] );
				}
			}
			else if(!g_mouseState.IsSelectByFrame)
				m_app->m_current_scene3D->deselectAll();
		}
	}
	else if( m_app->m_editMode == EditMode::Vertex )
	{
		if(g_mouseState.LMB_UP 
			&& m_app->m_state_app != AppState_main::Gizmo
			&& m_app->m_state_app != AppState_main::GuiInput
			&& g_mouseState.IsFirstClickLMB )
		{
			if( m_drawEditMode_hoverMark )
			{
				// все выделения и т.д. лучше произвести внутри 
				m_app->m_current_scene3D->doSelectVertexHover(g_hoverSelFrust, m_activeCamera);
			}else if( m_app->m_state_keyboard != AppState_keyboard::Ctrl ) // если объекты выделены то их не надо терять
			{
					// если объектов под курсором нет то нужно снять выделение со всего
				if( /*g_mouseState.InViewport &&*/ m_app->m_state_keyboard != AppState_keyboard::Alt )
					m_app->m_current_scene3D->deselectAll();
			}
		}
	}
	//	else if( m_app->m_editMode == EditMode::Edge )
	//	{
	//		if( checkCursorHover() )
	//		{
	//			if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_center))
	//			{
	//				if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_N))
	//				{
	//					if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_E))
	//					{
	//						if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_S))
	//						{
	//							if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_W))
	//							{
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//		else
	//		{
	//			//printf("a");
	//			if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_N))
	//			{
	//				if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_E))
	//				{
	//					if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_S))
	//					{
	//						if(!m_app->m_current_scene3D->selectEdges(&m_cursorRay->m_W))
	//						{
	//							if( m_app->m_state_keyboard != AppState_keyboard::Alt && 
	//								m_app->m_state_keyboard != AppState_keyboard::Ctrl )
	//								m_app->m_current_scene3D->deselectAll();
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else if( m_app->m_editMode == EditMode::Polygon )
	//	{
	//		m_app->m_current_scene3D->selectPolygons(&m_cursorRay->m_center);
	//	}
	//}
	if( g_mouseState.LMB_HOLD )
	{
		if( !isGizmo && g_mouseState.IsMove && g_mouseState.IsFirstClickLMB && m_app->m_state_app != AppState_main::CancelTransformation )
		{
			g_mouseState.IsSelectByFrame = true;
		}
	}
	if(g_mouseState.LMB_UP)
	{
		if(m_app->m_state_app == AppState_main::GuiInput)
			m_app->m_state_app = AppState_main::Idle;
	}
	if( g_mouseState.RMB_UP || m_app->m_event_consumer->isKeyUp(kkKey::K_ESCAPE, false) )
	{
		if(g_mouseState.IsSelectByFrame)
		{
			g_mouseState.IsSelectByFrame = false;
			g_mouseState.IsFirstClickLMB = false;
		}
	}
	if( g_mouseState.LMB_UP)
	{
		if(g_mouseState.IsSelectByFrame)
		{
			g_mouseState.IsSelectByFrame = false;
			g_mouseState.IsFirstClickLMB = false;
			//printf("SELECT BY FRAME\n");
			m_app->m_current_scene3D->selectObjectsByFrame(
				m_hoveredObjects, 
				m_drawObjects, 
				m_selectionFrame, 
				g_cursorSelFrust);
		}
	}
	if(!g_mouseState.LMB_HOLD)
	{
		g_mouseState.IsFirstClickLMB = false;
	}
	if(!g_mouseState.MMB_HOLD)
	{
		g_mouseState.IsFirstClickMMB = false;
	}
	if(!g_mouseState.RMB_HOLD)
	{
		g_mouseState.IsFirstClickRMB = false;
	}

	/*if( g_mouseState.LMB_DOWN && m_cursorInRect )
	{
	}*/
	processShortcuts();

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
			if((m_app->m_state_keyboard == AppState_keyboard::Ctrl) && g_mouseState.RMB_HOLD && g_mouseState.IsFirstClickRMB)
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
			if((m_app->m_state_keyboard == AppState_keyboard::Ctrl) && g_mouseState.RMB_HOLD && g_mouseState.IsFirstClickRMB)
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
			if((m_app->m_state_keyboard == AppState_keyboard::Ctrl) && g_mouseState.RMB_HOLD && g_mouseState.IsFirstClickRMB)
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
			if((m_app->m_state_keyboard == AppState_keyboard::Ctrl) && g_mouseState.RMB_HOLD && g_mouseState.IsFirstClickRMB)
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
void ViewportObject::drawEditMode_hoverMark()
{
	m_app->m_gs->useScissor(true);
	m_app->m_gs->setViewport(0,0,m_app->m_window_client_size.x,m_app->m_window_client_size.y);
	m_app->m_gs->setScissor((s32)m_gs_viewport.x,(s32)m_gs_viewport.y,(s32)m_gs_viewport.z,(s32)m_gs_viewport.w );

	m_app->m_gs->drawCircle2D(m_app->m_cursor_position, 5, 2, kkColorGreenYellow );
	m_app->m_gs->drawCircle2D(m_app->m_cursor_position, 6, 3, kkColorGreenYellow );
	m_app->m_gs->drawCircle2D(m_app->m_cursor_position, 7, 4, kkColorGreenYellow );

	m_app->m_gs->setScissor(0,0,m_app->m_window_client_size.x,m_app->m_window_client_size.y);
	m_app->m_gs->useScissor(false);
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
void ViewportObject::drawSelectionFrame(bool inFocus)
{
	if(!inFocus)
		return;
	
	kkGSSetScissor(true, v4i((s32)m_gs_viewport.x, (s32)m_gs_viewport.y, (s32)m_gs_viewport.z, (s32)m_gs_viewport.w));
	auto p1 = m_app->m_cursor_position;
	auto p2 = v2i( m_app->m_cursor_position.x, m_mouse_first_click_coords.y );
	auto p3 = v2i( m_mouse_first_click_coords.x, m_app->m_cursor_position.y );
	auto p4 = v2i( m_mouse_first_click_coords.x, m_mouse_first_click_coords.y );
	kkGSDrawLine2D(p1, p2, kkColorWhite);
	kkGSDrawLine2D(p3, p4, kkColorWhite);
	kkGSDrawLine2D(p1, p3, kkColorWhite);
	kkGSDrawLine2D(p2, p4, kkColorWhite);
	kkGSSetScissor(false,v4i());

	kkAabb aabb;
	aabb.add( kkVector4( (f32)m_app->m_cursor_position.x, (f32)m_app->m_cursor_position.y, 0.f ) );
	aabb.add( kkVector4( (f32)m_mouse_first_click_coords.x, (f32)m_mouse_first_click_coords.y, 0.f ) );

	m_selectionFrame.set((s32)aabb.m_min.KK_X, (s32)aabb.m_min.KK_Y, (s32)aabb.m_max.KK_X, (s32)aabb.m_max.KK_Y);
	g_cursorSelFrust.createWithFrame(m_selectionFrame, m_rect_modified, m_activeCamera->getCamera()->getViewProjectionInvertMatrix());
}
void ViewportObject::drawSelectedObjectFrame()
{
	kkVector4  ext;
	f32 frameSize   = 0.f;
	f32 frameIndent = 0.f;

	for( u32 i = 0, sz = m_app->m_current_scene3D->getNumOfSelectedObjects(); i < sz; ++i )
	{
		auto obj = m_app->m_current_scene3D->getSelectedObject( i );

		auto & aabb = obj->Aabb();
		aabb.m_max.KK_W = 0.f;
		aabb.m_min.KK_W = 0.f;

		frameSize = aabb.m_max.distance( aabb.m_min )+0.01f;
		frameSize /= 12.f;
		frameIndent = frameSize * 0.2f;

		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );

		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );

		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y-frameSize,   aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_max.KK_Y+frameIndent,   aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
		
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );

		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X+frameSize, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y-frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_min.KK_X-frameIndent, aabb.m_max.KK_Y+frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
		
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_min.KK_Z-frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z-frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_min.KK_Z+frameSize ), kkColorLightGray );

		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X-frameSize, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y+frameSize, aabb.m_max.KK_Z+frameIndent ), kkColorLightGray );
		m_app->m_gs->drawLine3D( kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z+frameIndent ), kkVector4( aabb.m_max.KK_X+frameIndent, aabb.m_min.KK_Y-frameIndent, aabb.m_max.KK_Z-frameSize ), kkColorLightGray );
	}
}
void ViewportObject::drawObjectPivot(bool inFocus)
{
	if( m_app->m_globalInputBlock )
		return;

	auto numOfObjects = m_app->m_current_scene3D->getNumOfSelectedObjects();

	if( !numOfObjects )
		return;

	f32 size = 0.f;
	if( m_activeCamera->getType() != ViewportCameraType::Perspective )
	{
		size = 0.5f / m_activeCamera->getZoomOrt();
	}

	kkVector4 point;
	
	if( numOfObjects == 1 )
	{
		auto obj = m_app->m_current_scene3D->getSelectedObject( 0 );
		point = obj->GetPivot();
	}
	else
	{
		auto	aabb = m_app->m_current_scene3D->getSelectionAabb();
		aabb.center(point);
	}

	if( m_app->m_editMode == EditMode::Vertex
		|| m_app->m_editMode == EditMode::Edge
		|| m_app->m_editMode == EditMode::Polygon )
	{
		m_app->m_current_scene3D->getSelectionAabb().center(point);
	}

	if( m_activeCamera->getType() == ViewportCameraType::Perspective )
	{
		size = (f32)m_activeCamera->getPositionCamera().distance(point);
		size *= 0.1f;
	}
	m_app->m_gs->drawLine3D( point, kkVector4( point.KK_X, point.KK_Y, point.KK_Z + size ), kkColorLightGray );
	m_app->m_gs->drawLine3D( point, kkVector4( point.KK_X, point.KK_Y+ size, point.KK_Z  ), kkColorLightGray );
	m_app->m_gs->drawLine3D( point, kkVector4( point.KK_X+ size, point.KK_Y, point.KK_Z  ), kkColorLightGray );

	if( inFocus )
	{
		for( auto o : m_app->m_current_scene3D->m_objects_selected )
		{
			if( m_app->m_editMode == EditMode::Object 
				|| (o->m_isObjectHaveSelectedVerts && m_app->m_editMode == EditMode::Vertex)
				|| ( o->m_isObjectHaveSelectedEdges && m_app->m_editMode == EditMode::Edge)
				|| ( o->m_isObjectHaveSelectedPolys && m_app->m_editMode == EditMode::Polygon) )
			{
				m_app->m_currentGizmoEvent.point2D = kkrooo::worldToScreen( m_activeCamera->m_kk_camera->getViewProjectionMatrix(), point, 
					m_rect_modified.getWidthAndHeight(),
					v2f(m_rect_modified.x,m_rect_modified.y) );

				switch(m_app->m_selectMode)
				{
				case SelectMode::JustSelect:
				default:
					break;
				case SelectMode::Move:
					m_app->m_gizmo->drawMove(point, size, m_cursorRay->m_center);
					break;
				case SelectMode::Rotate:
					m_app->m_currentGizmoEvent.point2D = kkrooo::worldToScreen( m_activeCamera->m_kk_camera->getViewProjectionMatrix(), point, 
						m_rect_modified.getWidthAndHeight(),
						v2f(m_rect_modified.x,m_rect_modified.y) );
					m_app->m_gizmo->drawRotation(point, size, m_cursorRay->m_center);
					break;
				case SelectMode::Scale:
					m_app->m_gizmo->drawScale(point, size, m_cursorRay->m_center);
					break;
				}
				break;
			}
		}
	}
}
void ViewportObject::drawGizmo2D()
{
	kkGSSetScissor(true, v4i((s32)m_gs_viewport.x,(s32)m_gs_viewport.y,(s32)m_gs_viewport.z,(s32)m_gs_viewport.w));
	auto vp_sz = m_rect_modified.getWidthAndHeight();
	switch(m_app->m_selectMode)
	{
	case SelectMode::JustSelect:
	default:
		break;
	case SelectMode::Move:
		m_app->m_gizmo->drawMove2D( &m_app->m_cursor_position, m_app->m_currentGizmoEvent.point2D );
		break;
	case SelectMode::Rotate:
		m_app->m_gizmo->drawRotation2D( &m_app->m_cursor_position, vp_sz, m_rect_modified );
		break;
	case SelectMode::Scale:
		m_app->m_gizmo->drawScale2D( &m_app->m_cursor_position, m_app->m_currentGizmoEvent.point2D );
		break;
	}
	kkGSSetScissor(false, v4i());
}
void ViewportObject::drawScene(bool inFocus)
{
	bool       fff = false;
	m_frame_skip++;
	if( m_frame_skip == m_frame_limit )
	{
		_updateObjectsInFrustum();
		m_frame_skip = 0;
		fff = true;
	}

	if( fff )
		m_hoveredObjects.clear();

	for( size_t i = 0; i < m_drawObjects.size(); ++i )
	{
		Scene3DObject*      object = m_drawObjects[i];
		kkScene3DObjectType object_type = object->GetType();
		auto & obb = object->Obb();
		switch(object_type)
		{
		case kkScene3DObjectType::PolygonObject:
		{
			object->UpdateWorldMatrix();
			if( m_draw_mode == DrawMode::Material || m_draw_mode == DrawMode::EdgesAndMaterial )
			{
				for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
				{
					kkGSDrawModel(
						object->getHardwareModel(i2), 
						object->GetMatrixWorld(),
						object->m_shaderParameter.m_diffuseColor,
						object->m_shaderParameter.m_diffuseTexture,
						object->isSelected());
					//// рисование самой модели
					//m_vd.m_app->m_shader3DObjectDefault->m_diffuseColor = object->m_shaderParameter.m_diffuseColor;
					//m_vd.m_app->m_shader3DObjectDefault->m_diffTex = object->m_shaderParameter.m_diffuseTexture;
					//m_vd.m_gs->drawMesh(object->getHardwareModel(i2), object->GetMatrixWorld() , m_vd.m_app->m_shader3DObjectDefault.ptr() );
				}
			}

			if( m_draw_mode == DrawMode::Edge || m_draw_mode == DrawMode::EdgesAndMaterial
				|| m_app->m_editMode == EditMode::Edge )
			{
				if( m_app->m_editMode == EditMode::Polygon )
				{
					for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
					{
						kkGSDrawModelLineModePolyEdit(object->getHardwareModel(i2), object->GetMatrixWorld());
					}
				}

				for( u64 i2 = 0, sz = object->getHardwareModelCount_lines(); i2 < sz; ++i2 )
				{
					if(object->isSelected())
						kkGSDrawModelEdge(object->getHardwareModel_lines(i2),object->GetMatrixWorld(),kkColorWhite);
					else
						kkGSDrawModelEdge(object->getHardwareModel_lines(i2),object->GetMatrixWorld(),object->m_edgeColor);
				}
			}

			if( m_app->m_editMode == EditMode::Vertex && object->m_isSelected )
			{
				for( u64 i2 = 0, sz = object->getHardwareModelCount_points(); i2 < sz; ++i2 )
				{
					kkGSDrawModelPoint(object->getHardwareModel_points(i2), object->GetMatrixWorld() );
				}
			}
			/*if( m_vd.m_app->debugIsDrawObjectAabb() )
				_drawAabb( object->Aabb(), kkColorYellowGreen );
			if( m_vd.m_app->debugIsDrawObjectObb()  )
				_drawObb( obb, kkColorRed );*/

		}break;
		default:
			break;
		}

		if( fff && !m_app->m_event_consumer->isMmbDown() && m_app->m_state_app != AppState_main::Gizmo && !g_mouseState.IsSelectByFrame
			&& m_app->m_editMode == EditMode::Object )
		{
			if( kkrooo::pointInRect( m_app->m_cursor_position, m_rect_modified ) )
			{
				if( kkrooo::rayIntersection_obb(m_cursorRay->m_center, obb) )
				{
					// далее проверка на пересечение луч-треугольник
					kkRayTriangleIntersectionResultSimple intersectionResult;
					if( object->IsRayIntersect(m_cursorRay->m_center, intersectionResult) )
					{
						auto camera_position = m_activeCamera->getPositionCamera();

						object->m_distanceToCamera = camera_position.distance(intersectionResult.m_intersectionPoint);
						m_hoveredObjects.push_back(object);
					}
				}
			}
		}

		if( m_hoveredObjects.size() )
		{
			sortMouseHoverObjects( m_hoveredObjects );
		}
	}
	if( m_hoveredObjects.size() )
		kkGSDrawObb( m_hoveredObjects[m_hoveredObjects.size()-1]->Obb(), kkColorRed );
	if( m_app->debugIsDrawSceneAabb() )
		kkGSDrawAabb( (*kkGetScene3D())->getSceneAabb(), kkColorYellow );
	if( m_app->debugIsDrawSelectedObjectsAabb() )
		kkGSDrawAabb( (*kkGetScene3D())->getSelectionAabb(), kkColorBlue );
	drawSelectedObjectFrame();
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
	m_app = kkSingleton<Application>::s_instance;
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
	kkSetActiveViewport(m_viewportInFocus);
}

void Viewport::updateInput(const v2f& mouseDelta)
{
	//g_mouseState.reset();
	/*g_mouseState.LMB_DOWN = kkIsLmbDownOnce();
	g_mouseState.LMB_HOLD = kkIsLmbDown();
	g_mouseState.LMB_UP   = kkIsLmbUp();
	g_mouseState.RMB_DOWN = kkIsRmbDownOnce();
	g_mouseState.RMB_HOLD = kkIsRmbDown();
	g_mouseState.RMB_UP   = kkIsRmbUp();
	g_mouseState.MMB_DOWN = kkIsMmbDownOnce();
	g_mouseState.MMB_HOLD = kkIsMmbDown();
	g_mouseState.MMB_UP   = kkIsMmbUp();*/
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
				kkSetActiveViewport(m_viewportInFocus);
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
			bool inFocus = m_viewportInFocus == vp;
			
			auto next = vp->m_right;
			kkGSSetDepth(false);
			kkGSSetViewport((s32)vp->m_gs_viewport.x, (s32)vp->m_gs_viewport.y, (s32)vp->m_gs_viewport.z, (s32)vp->m_gs_viewport.w);
			vp->drawBG(m_windowSize, colorTheme);
			kkGSSetDepth(true);

			// draw 3d
			kkGSSetViewport((s32)vp->m_gs_viewport.x, (s32)vp->m_gs_viewport.y, (s32)vp->m_gs_viewport.z, (s32)vp->m_gs_viewport.w);
			vp->beginDraw();
			vp->drawGrid(colorTheme);
			vp->drawScene(inFocus);

			// draw 2d top layer
			kkGSSetDepth(false);
			vp->drawObjectPivot(inFocus);
			kkGSSetViewport(0,0, m_windowSize.x, m_windowSize.y);
			if(inFocus)
			{
				for( auto o : m_app->m_current_scene3D->m_objects_selected )
				{
					if( m_app->m_editMode == EditMode::Object 
						|| (o->m_isObjectHaveSelectedVerts && m_app->m_editMode == EditMode::Vertex)
						|| ( o->m_isObjectHaveSelectedEdges && m_app->m_editMode == EditMode::Edge)
						|| ( o->m_isObjectHaveSelectedPolys && m_app->m_editMode == EditMode::Polygon) )
					vp->drawGizmo2D();
				}
			}
			if( vp->m_drawEditMode_hoverMark )
				vp->drawEditMode_hoverMark();
			if( g_mouseState.IsSelectByFrame )
				vp->drawSelectionFrame(inFocus);
			vp->drawName(inFocus);
			kkGSSetDepth(true);

			if(vp == end)
				break;
			vp = next;
		}
	}
}

bool Viewport::cursorInViewport()
{
	if(m_viewports)
	{
		auto end = m_viewports->m_left;
		auto vp = m_viewports;
		while(true)
		{
			auto next = vp->m_right;
			if( kkrooo::pointInRect( m_app->m_cursor_position, vp->m_rect_modified ))
			{
				return true;
			}
			if(vp == end)
				break;
			vp = next;
		}
	}
	return false;
}