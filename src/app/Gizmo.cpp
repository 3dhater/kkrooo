#include "kkrooo.engine.h"
#include "Common.h"

#include "Common/kkInfoSharedLibrary.h"

#include "Common/kkUtil.h"

#include "Scene3D/Scene3D.h"
#include "Scene3D/Scene3DObject.h"
#include "Gizmo.h"
#include "Functions.h"
#include "Application.h"
#include "Plugins/Plugin.h"
#include "Shaders/simple.h"
#include "CursorRay.h"


Gizmo::Gizmo()
{
}

Gizmo::~Gizmo()
{
	for( int i = 0 ; i < 6; ++i )
	{
		kkDestroy( m_gizmoMoveObjects[ i ] );
		kkDestroy( m_gizmoMoveObjects_col[ i ] );

		kkDestroy( m_gizmoScaleObjects[ i ] );
	}
	for( int i = 0 ; i < 3; ++i )
	{
		kkDestroy( m_gizmoRotationObjects[ i ] );
		kkDestroy( m_gizmoRotationObjects_col[ i ] );
	}
}

void Gizmo::init()
{
	m_app = kkSingleton<Application>::s_instance;
	
	if( m_app->m_OBJplugin )
	{
		auto scene = m_app->getScene3D();

		m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/gizmo_move2.obj");
		
		auto & objects = (*scene)->getObjects();

		for( auto o : objects )
		{
			kkString n( o->GetName() );
			
					if( n == "1" )	m_gizmoMoveObjects[ 0 ] = o;
			else	if( n == "2" )	m_gizmoMoveObjects[ 1 ] = o;
			else	if( n == "3" )	m_gizmoMoveObjects[ 2 ] = o;
			else	if( n == "4" )	m_gizmoMoveObjects[ 3 ] = o;
			else	if( n == "5" )	m_gizmoMoveObjects[ 4 ] = o;
			else	if( n == "6" )	m_gizmoMoveObjects[ 5 ] = o;
		}

		(*scene)->unregisterObject(m_gizmoMoveObjects[ 0 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects[ 1 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects[ 2 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects[ 3 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects[ 4 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects[ 5 ]);

		m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/gizmo_move2_col.obj");
		
		objects = (*scene)->getObjects();

		for( auto o : objects )
		{
			kkString n( o->GetName() );
			
					if( n == "1" )	m_gizmoMoveObjects_col[ 0 ] = o;
			else	if( n == "2" )	m_gizmoMoveObjects_col[ 1 ] = o;
			else	if( n == "3" )	m_gizmoMoveObjects_col[ 2 ] = o;
			else	if( n == "4" )	m_gizmoMoveObjects_col[ 3 ] = o;
			else	if( n == "5" )	m_gizmoMoveObjects_col[ 4 ] = o;
			else	if( n == "6" )	m_gizmoMoveObjects_col[ 5 ] = o;
		}

		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 0 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 1 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 2 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 3 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 4 ]);
		(*scene)->unregisterObject(m_gizmoMoveObjects_col[ 5 ]);

		m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/gizmo_sc.obj");
		
		objects = (*scene)->getObjects();

		for( auto o : objects )
		{
			kkString n( o->GetName() );
			
					if( n == "1" )	m_gizmoScaleObjects[ 0 ] = o;
			else	if( n == "2" )	m_gizmoScaleObjects[ 1 ] = o;
			else	if( n == "3" )	m_gizmoScaleObjects[ 2 ] = o;
			else	if( n == "4" )	m_gizmoScaleObjects[ 3 ] = o;
			else	if( n == "5" )	m_gizmoScaleObjects[ 4 ] = o;
			else	if( n == "6" )	m_gizmoScaleObjects[ 5 ] = o;
		}

		(*scene)->unregisterObject(m_gizmoScaleObjects[ 0 ]);
		(*scene)->unregisterObject(m_gizmoScaleObjects[ 1 ]);
		(*scene)->unregisterObject(m_gizmoScaleObjects[ 2 ]);
		(*scene)->unregisterObject(m_gizmoScaleObjects[ 3 ]);
		(*scene)->unregisterObject(m_gizmoScaleObjects[ 4 ]);
		(*scene)->unregisterObject(m_gizmoScaleObjects[ 5 ]);

		m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/gizmo_rot.obj");
		
		objects = (*scene)->getObjects();

		for( auto o : objects )
		{
			kkString n( o->GetName() );
			
					if( n == "1" )	m_gizmoRotationObjects[ 0 ] = o;
			else	if( n == "2" )	m_gizmoRotationObjects[ 1 ] = o;
			else	if( n == "3" )	m_gizmoRotationObjects[ 2 ] = o;
		}

		(*scene)->unregisterObject(m_gizmoRotationObjects[ 0 ]);
		(*scene)->unregisterObject(m_gizmoRotationObjects[ 1 ]);
		(*scene)->unregisterObject(m_gizmoRotationObjects[ 2 ]);

		m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/gizmo_rot_col.obj");
		
		objects = (*scene)->getObjects();

		for( auto o : objects )
		{
			kkString n( o->GetName() );
			
					if( n == "1" )	m_gizmoRotationObjects_col[ 0 ] = o;
			else	if( n == "2" )	m_gizmoRotationObjects_col[ 1 ] = o;
			else	if( n == "3" )	m_gizmoRotationObjects_col[ 2 ] = o;
		}

		(*scene)->unregisterObject(m_gizmoRotationObjects_col[ 0 ]);
		(*scene)->unregisterObject(m_gizmoRotationObjects_col[ 1 ]);
		(*scene)->unregisterObject(m_gizmoRotationObjects_col[ 2 ]);
	}
}


void Gizmo::setGraphicsSystem( kkGraphicsSystem * gs )
{
	m_gs = gs;
}

GizmoPart Gizmo::updateInput(CursorRay* cursorRay)
{
	if(m_app->m_state_app == AppState_main::Idle)
	{
		m_gizmoPart = GizmoPart::Default;
		if( m_gizmoMoveObjects_col[ 0 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::X;
		}else if( m_gizmoMoveObjects_col[ 1 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::Y;
		}else if( m_gizmoMoveObjects_col[ 2 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::Z;
		}else if( m_gizmoMoveObjects_col[ 3 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::XY_plane;
		}else if( m_gizmoMoveObjects_col[ 4 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::ZY_plane;
		}else if( m_gizmoMoveObjects_col[ 5 ]->IsRayIntersect(cursorRay->m_center, m_intersectionResult) )
		{
			m_gizmoPart = GizmoPart::XZ_plane;
		}
	}
	return m_gizmoPart;
}
void Gizmo::drawMove(const kkVector4& pivot, f32 size, const kkRay& cursorRay )
{
	math::makeTranslationMatrix(pivot,m_matrix);

	m_matrix[ 0 ].KK_X = size;
	m_matrix[ 1 ].KK_Y = size;
	m_matrix[ 2 ].KK_Z = size;

	m_matrixNoTranslation[ 0 ].KK_X = size;
	m_matrixNoTranslation[ 1 ].KK_Y = size;
	m_matrixNoTranslation[ 2 ].KK_Z = size;

	for( int i = 0; i < 6; ++i )
	{
		m_gizmoMoveObjects[ i ]->SetMatrix(m_matrixNoTranslation);
		m_gizmoMoveObjects[ i ]->GetPivot().KK_X = pivot.KK_X;
		m_gizmoMoveObjects[ i ]->GetPivot().KK_Y = pivot.KK_Y;
		m_gizmoMoveObjects[ i ]->GetPivot().KK_Z = pivot.KK_Z;

		m_gizmoMoveObjects_col[ i ]->SetMatrix(m_matrixNoTranslation);
		m_gizmoMoveObjects_col[ i ]->GetPivot().KK_X = pivot.KK_X;
		m_gizmoMoveObjects_col[ i ]->GetPivot().KK_Y = pivot.KK_Y;
		m_gizmoMoveObjects_col[ i ]->GetPivot().KK_Z = pivot.KK_Z;
	}

	if( m_gizmoPart == GizmoPart::X || 
		m_gizmoPart == GizmoPart::XY_plane || 
		m_gizmoPart == GizmoPart::XZ_plane )
		m_app->m_shaderSimple->m_color.set(1.f,0.f,0.f,1.f);
	else
		m_app->m_shaderSimple->m_color.set(1.f,0.8f,0.8f,1.f);
	m_gs->drawMesh(m_gizmoMoveObjects[ 0 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());

	if( m_gizmoPart == GizmoPart::Y || 
		m_gizmoPart == GizmoPart::XY_plane || 
		m_gizmoPart == GizmoPart::ZY_plane) 
		m_app->m_shaderSimple->m_color.set(0.f,0.f,1.f,1.f);
	else
		m_app->m_shaderSimple->m_color.set(0.8f,0.8f,1.f,1.f);
	m_gs->drawMesh(m_gizmoMoveObjects[ 1 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());

	if( m_gizmoPart == GizmoPart::Z || 
		m_gizmoPart == GizmoPart::ZY_plane || 
		m_gizmoPart == GizmoPart::XZ_plane ) 
		m_app->m_shaderSimple->m_color.set(0.f,1.f,0.f,1.f);
	else
		m_app->m_shaderSimple->m_color.set(0.8f,1.f,0.8f,1.f);
	m_gs->drawMesh(m_gizmoMoveObjects[ 2 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());

	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( m_gizmoPart == GizmoPart::XY_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoMoveObjects[ 3 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}
	else
		m_gs->drawMesh(m_gizmoMoveObjects[ 3 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());

	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( m_gizmoPart == GizmoPart::ZY_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoMoveObjects[ 4 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}
	else
		m_gs->drawMesh(m_gizmoMoveObjects[ 4 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());


	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( m_gizmoPart == GizmoPart::XZ_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoMoveObjects[ 5 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}
	else
		m_gs->drawMesh(m_gizmoMoveObjects[ 5 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());
}

void Gizmo::drawMove2D(v2i * cp, const v2i& point2d)
{
	/*kkColor color = kkColorDarkGray;

	int size = 10;

	if( kkrooo::pointInRect(*cp, v4i(point2d.x-size,point2d.y-size,point2d.x+size,point2d.y+size)) 
		|| m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Screen_plane )
	{
		color = kkColorYellow;

		AppEvent e;
		e.type     = AppEventType::Gizmo;
		e.gizmo.type = AppEvent_gizmo::_type::Move;
		e.gizmo.part = AppEvent_gizmo::_part::Screen_plane;

		m_app->addAppEvent( e, AppEventPriority::High );
	}

	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y-size ), v2i( point2d.x+size, point2d.y-size ), color );
	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y+size ), v2i( point2d.x+size, point2d.y+size ), color );
	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y-size ), v2i( point2d.x-size, point2d.y+size ), color );
	m_gs->drawLine2D( v2i( point2d.x+size, point2d.y-size ), v2i( point2d.x+size, point2d.y+size ), color );*/
}


void Gizmo::drawScale(const kkVector4& pivot, f32 size, const kkRay& cursorRay )
{
	/*kkMatrix4 mat_for_active;
	math::makeTranslationMatrix(pivot,m_matrix);

	bool doMouseInput = false;
	if( m_app->isWindowActive(EWID_MAIN_WINDOW))
		doMouseInput = true;

	AppEvent e;
	e.type       = AppEventType::Gizmo;
	e.gizmo.type = AppEvent_gizmo::_type::Scale;


	mat_for_active = m_matrix;

	m_matrix[ 0 ].KK_X = size;
	m_matrix[ 1 ].KK_Y = size;
	m_matrix[ 2 ].KK_Z = size;

	mat_for_active[ 0 ].KK_X = size + m_scaleSize.KK_X;
	mat_for_active[ 1 ].KK_Y = size + m_scaleSize.KK_Y;
	mat_for_active[ 2 ].KK_Z = size + m_scaleSize.KK_Z;

	m_matrixNoTranslation[ 0 ].KK_X = size;
	m_matrixNoTranslation[ 1 ].KK_Y = size;
	m_matrixNoTranslation[ 2 ].KK_Z = size;

	m_gizmoScaleObjects[ 0 ]->SetMatrix(m_matrixNoTranslation);
	m_gizmoScaleObjects[ 1 ]->SetMatrix(m_matrixNoTranslation);
	m_gizmoScaleObjects[ 2 ]->SetMatrix(m_matrixNoTranslation);
	m_gizmoScaleObjects[ 3 ]->SetMatrix(m_matrixNoTranslation);
	m_gizmoScaleObjects[ 4 ]->SetMatrix(m_matrixNoTranslation);
	m_gizmoScaleObjects[ 5 ]->SetMatrix(m_matrixNoTranslation);

	for( int i = 0; i < 6; ++i )
	{
		m_gizmoScaleObjects[ i ]->GetPivot().KK_X = pivot.KK_X;
		m_gizmoScaleObjects[ i ]->GetPivot().KK_Y = pivot.KK_Y;
		m_gizmoScaleObjects[ i ]->GetPivot().KK_Z = pivot.KK_Z;
	}

	int hoverid = -1;
	if( doMouseInput )
	{
		if( m_gizmoScaleObjects[ 0 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 0;
			e.gizmo.part = AppEvent_gizmo::_part::X;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );

		}else if( m_gizmoScaleObjects[ 1 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 1;
			e.gizmo.part = AppEvent_gizmo::_part::Y;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );

		}else if( m_gizmoScaleObjects[ 2 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 2;
			e.gizmo.part = AppEvent_gizmo::_part::Z;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );

		}else if( m_gizmoScaleObjects[ 3 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 3;
			e.gizmo.part = AppEvent_gizmo::_part::XY_plane;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );

		}else if( m_gizmoScaleObjects[ 4 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 4;

			e.gizmo.part = AppEvent_gizmo::_part::ZY_plane;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );

		}else if( m_gizmoScaleObjects[ 5 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
		{
			hoverid = 5;

			e.gizmo.part = AppEvent_gizmo::_part::XZ_plane;

			if( m_app->m_state_app == AppState_main::Idle )
				m_app->addAppEvent( e, AppEventPriority::Medium );
		}
	}

	if( hoverid == 0 || hoverid == 3 || hoverid == 5 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::X
		|| m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XY_plane || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XZ_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,0.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 0 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
	{
		m_app->m_shaderSimple->m_color.set(1.f,0.8f,0.8f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 0 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}

	if( hoverid == 1 || hoverid == 3 || hoverid == 4 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Y
		|| m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XY_plane || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::ZY_plane) 
	{
		m_app->m_shaderSimple->m_color.set(0.f,0.f,1.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 1 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
	{
		m_app->m_shaderSimple->m_color.set(0.8f,0.8f,1.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 1 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}

	if( hoverid == 2 || hoverid == 4 || hoverid == 5 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Z 
		|| m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::ZY_plane || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XZ_plane)
	{
		m_app->m_shaderSimple->m_color.set(0.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 2 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
	{
		m_app->m_shaderSimple->m_color.set(0.8f,1.f,0.8f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 2 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
	}

	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( hoverid == 3 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XY_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 3 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
	{
		m_gs->drawMesh(m_gizmoScaleObjects[ 3 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());
	}

	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( hoverid == 4 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::ZY_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 4 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
		m_gs->drawMesh(m_gizmoScaleObjects[ 4 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());


	m_app->m_shaderSimple->m_color.set(0.5f,1.f,1.0f,1.f);
	if( hoverid == 5 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::XZ_plane )
	{
		m_app->m_shaderSimple->m_color.set(1.f,1.f,0.f,1.f);
		m_gs->drawMesh(m_gizmoScaleObjects[ 5 ]->getHardwareModel(0),mat_for_active,m_app->m_shaderSimple.ptr());
	}
	else
		m_gs->drawMesh(m_gizmoScaleObjects[ 5 ]->getHardwareModel_lines(0),m_matrix,m_app->m_shaderSimple.ptr());*/

}

void Gizmo::drawScale2D(v2i * cp, const v2i& point2d)
{
	/*kkColor color = kkColorDarkGray;

	int size = 10;

	if( kkrooo::pointInRect(*cp, v4i(point2d.x-size,point2d.y-size,point2d.x+size,point2d.y+size)) 
		|| m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Screen_plane )
	{
		color = kkColorYellow;

		AppEvent e;
		e.type     = AppEventType::Gizmo;
		e.gizmo.type = AppEvent_gizmo::_type::Scale;
		e.gizmo.part = AppEvent_gizmo::_part::Screen_plane;

		m_app->addAppEvent( e, AppEventPriority::High );
	}

	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y-size ), v2i( point2d.x+size, point2d.y-size ), color );
	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y+size ), v2i( point2d.x+size, point2d.y+size ), color );
	m_gs->drawLine2D( v2i( point2d.x-size, point2d.y-size ), v2i( point2d.x-size, point2d.y+size ), color );
	m_gs->drawLine2D( v2i( point2d.x+size, point2d.y-size ), v2i( point2d.x+size, point2d.y+size ), color );*/
}


void Gizmo::drawRotation(const kkVector4& pivot, f32 size, const kkRay& cursorRay )
{
	//math::makeTranslationMatrix(pivot,m_matrix);
	//
	//bool doMouseInput = false;
	//if( m_app->isWindowActive(EWID_MAIN_WINDOW))
	//	doMouseInput = true;

	//AppEvent e;
	//e.type       = AppEventType::Gizmo;
	//e.gizmo.type = AppEvent_gizmo::_type::Rotate;

	//m_matrix[ 0 ].KK_X = size;
	//m_matrix[ 1 ].KK_Y = size;
	//m_matrix[ 2 ].KK_Z = size;

	//m_matrixNoTranslation[ 0 ].KK_X = size;
	//m_matrixNoTranslation[ 1 ].KK_Y = size;
	//m_matrixNoTranslation[ 2 ].KK_Z = size;

	//for( int i = 0; i < 3; ++i )
	//{
	//	m_gizmoRotationObjects[ i ]->SetMatrix(m_matrixNoTranslation);
	//	m_gizmoRotationObjects[ i ]->GetPivot().KK_X = pivot.KK_X;
	//	m_gizmoRotationObjects[ i ]->GetPivot().KK_Y = pivot.KK_Y;
	//	m_gizmoRotationObjects[ i ]->GetPivot().KK_Z = pivot.KK_Z;

	//	m_gizmoRotationObjects_col[ i ]->SetMatrix(m_matrixNoTranslation);
	//	m_gizmoRotationObjects_col[ i ]->GetPivot().KK_X = pivot.KK_X;
	//	m_gizmoRotationObjects_col[ i ]->GetPivot().KK_Y = pivot.KK_Y;
	//	m_gizmoRotationObjects_col[ i ]->GetPivot().KK_Z = pivot.KK_Z;
	//}

	//int hoverid = -1;
	//if( doMouseInput )
	//{
	//	if( m_gizmoRotationObjects_col[ 0 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
	//	{
	//		hoverid = 0;
	//		e.gizmo.part = AppEvent_gizmo::_part::Y;

	//		if( m_app->m_state_app == AppState_main::Idle )
	//			m_app->addAppEvent( e, AppEventPriority::Medium );

	//	}else if( m_gizmoRotationObjects_col[ 1 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
	//	{
	//		hoverid = 1;
	//		e.gizmo.part = AppEvent_gizmo::_part::Z;

	//		if( m_app->m_state_app == AppState_main::Idle )
	//			m_app->addAppEvent( e, AppEventPriority::Medium );

	//	}else if( m_gizmoRotationObjects_col[ 2 ]->IsRayIntersect(cursorRay, m_intersectionResult) )
	//	{
	//		hoverid = 2;
	//		e.gizmo.part = AppEvent_gizmo::_part::X;

	//		if( m_app->m_state_app == AppState_main::Idle )
	//			m_app->addAppEvent( e, AppEventPriority::Medium );
	//	}
	//}


	//// BLUE
	//if( hoverid == 0 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Y)
	//	m_app->m_shaderSimple->m_color.set(0.f,0.f,1.f,1.f);
	//else
	//	m_app->m_shaderSimple->m_color.set(0.8f,0.8f,1.0f,1.f);
	//if( m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::X && m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::Z )
	//	m_gs->drawMesh(m_gizmoRotationObjects[ 0 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());

	//// GREEN
	//if( hoverid == 1 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Z) 
	//	m_app->m_shaderSimple->m_color.set(0.f,1.f,0.f,1.f);
	//else
	//	m_app->m_shaderSimple->m_color.set(0.8f,1.0f,0.8f,1.f);
	//if( m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::X && m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::Y )
	//	m_gs->drawMesh(m_gizmoRotationObjects[ 1 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());

	//// RED
	//if( hoverid == 2 || m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::X) 
	//	m_app->m_shaderSimple->m_color.set(1.f,0.f,0.f,1.f);
	//else
	//	m_app->m_shaderSimple->m_color.set(1.0f,0.8f,0.8f,1.f);
	//if( m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::Y && m_app->m_currentGizmoEvent.part != AppEvent_gizmo::_part::Z )
	//	m_gs->drawMesh(m_gizmoRotationObjects[ 2 ]->getHardwareModel(0),m_matrix,m_app->m_shaderSimple.ptr());
}

void Gizmo::drawRotation2D(v2i * cp, const v2f& vp_sz, const v4f& rect)
{
	/*s32 radius = (s32)(vp_sz.x * 0.5f);
	if( vp_sz.y < vp_sz.x )
		radius = (s32)(vp_sz.y * 0.5f);
		
	if( radius > 50 )
	{
		radius -= 50;

		if( radius <= 0 )
			radius = 50;
	}

	auto viewport_center = v2f( 
		rect.x + ((rect.z - rect.x)*0.5f),
		rect.y + ((rect.w - rect.y)*0.5f ));

	s32 L = (s32)kkVector4((float)cp->x,(float)cp->y, 0.f, 1.f).distance(
		kkVector4((float)viewport_center.x,
		(float)viewport_center.y,
			0.f,1.f
		));

	kkColor color = kkColorDarkGray;

	if( L > radius - 10 && L < radius + 10 )
	{
		color = kkColorYellow;
		AppEvent e;
		e.type       = AppEventType::Gizmo;
		e.gizmo.type = AppEvent_gizmo::_type::Rotate;
		e.gizmo.part = AppEvent_gizmo::_part::Screen_plane;

		if( m_app->m_state_app == AppState_main::Idle )
			m_app->addAppEvent( e, AppEventPriority::High );
	}

	if( m_app->m_currentGizmoEvent.part == AppEvent_gizmo::_part::Screen_plane )
		color = kkColorYellow;

	m_gs->drawCircle2D( v2i((int)viewport_center.x,(int)viewport_center.y),
		radius, 8, color );*/
}

