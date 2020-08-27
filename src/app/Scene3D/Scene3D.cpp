// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include "Common/kkUtil.h"
#include "Classes/Math/kkMath.h"

#include "../CursorRay.h"
#include "../SelectionFrust.h"

#include "../Viewport/ViewportCamera.h"
#include "../Geometry/PolygonalModel.h"
#include "../Geometry/Polygon3D.h"
#include "../Application.h"
#include "../Gizmo.h"
#include "Scene3D.h"
#include "Scene3DObject.h"

#include <algorithm>

const s32 g_edgeColorNumber = 62;
const kkColor g_edgeColors[] = 
{
	kkColorAqua,
	kkColorAquamarine,
	kkColorBisque,
	kkColorBlue,
	kkColorBlueViolet,
	kkColorBrown,
	kkColorBurlyWood,
	kkColorCadetBlue,
	kkColorChartreuse,
	kkColorChocolate,
	kkColorCoral,
	kkColorCornflowerBlue,
	kkColorCrimson,
	kkColorDarkBlue,
	kkColorDarkCyan,
	kkColorDarkGoldenRod,
	kkColorDarkGreen,
	kkColorDarkKhaki,
	kkColorDarkMagenta,
	kkColorDarkOliveGreen,
	kkColorDarkOrange,
	kkColorDarkOrchid,
	kkColorDarkRed,
	kkColorDarkSalmon,
	kkColorDarkSeaGreen,
	kkColorDarkSlateBlue,
	kkColorDarkSlateGray,
	kkColorDarkTurquoise,
	kkColorDarkViolet,
	kkColorDeepPink,
	kkColorDeepSkyBlue,
	kkColorDodgerBlue,
	kkColorFireBrick,
	kkColorForestGreen,
	kkColorFuchsia,
	kkColorGold,
	kkColorGoldenRod,
	kkColorGreen,
	kkColorGreenYellow,
	kkColorHotPink,
	kkColorIndianRed,
	kkColorIndigo,
	kkColorKhaki,
	kkColorLawnGreen,
	kkColorLightPink,
	kkColorLightSalmon,
	kkColorLightSeaGreen,
	kkColorLime,
	kkColorMediumAquaMarine,
	kkColorMediumOrchid,
	kkColorMediumPurple,
	kkColorMoccasin,
	kkColorOlive,
	kkColorOrange,
	kkColorPaleVioletRed,
	kkColorPeru,
	kkColorPlum,
	kkColorPowderBlue,
	kkColorPurple,
	kkColorRosyBrown,
	kkColorYellow,
	kkColorYellowGreen,
};

Scene3D::Scene3D()
{
	m_app = kkSingleton<Application>::s_instance;
	m_gizmo = m_app->m_gizmo.ptr();
}

Scene3D::~Scene3D()
{
	clearScene();
}

void Scene3D::test()
{
	for( auto o : m_objects )
	{
		for( auto cp : o->m_PolyModel->m_controlPoints )
		{
			if( cp->isSelected() )
				continue;
		}
	}
}

u32 Scene3D::getNumOfObjectsOnScene()
{
	return (u32)m_objects.size();
}

u32 Scene3D::getNumOfSelectedObjects()
{
	return (u32)m_objects_selected.size();
}

kkScene3DObject* Scene3D::getObjectOnScene( u32 i )
{
	assert(i<m_objects.size());
	return m_objects[i];
}

kkScene3DObject* Scene3D::getSelectedObject( u32 i )
{
	assert(i<m_objects_selected.size());
	return m_objects_selected[i];
}

void Scene3D::clearScene()
{
	deselectAll();

	for(size_t i = 0, sz = m_objects.size(); i < sz; ++i )
	{
		kkDestroy(m_objects[i]);
	}

	m_objects.clear();
}

Scene3DObject * Scene3D::createNewPolygonalObject( const char16_t* n, PolygonalModel* model, const v4f& position )
{
	kkString name = n;
	if(name.size())
		util::stringTrimSpace(name);

	if(!nameIsFree(name.data()))
	{
		name = nameCreateNew(name.data());
	}
	
	if(!name.size())
		name = nameCreateNew(u"Object");

	// обновление hardware буферов здесь ...
	Scene3DObject * new_object = kkCreate<Scene3DObject>(kkScene3DObjectType::PolygonObject,model);

	if( new_object->init() )
	{
		static s32 colorIndex = 0;
		new_object->m_edgeColor = g_edgeColors[colorIndex];
		++colorIndex;
		if(colorIndex == g_edgeColorNumber) colorIndex = 0;

		new_object->m_scene3D = this;
		new_object->SetPluginGUIWindow(m_app->m_edit_params_window);
		new_object->SetName(name.data());
		new_object->setPosition(kkVector4(position.x,position.y,position.z));
		new_object->ApplyPivot();
		
		new_object->UpdateAabb();
	//	new_object->UpdateScreenSpacePoints();

		m_objects.push_back( new_object );
		
		updateSceneAabb();
	}
	else
	{
		fprintf(stderr,"Failed to create Polygonal Object :(\n");
		kkDestroy(new_object);
	}

	m_app->setNeedToSave(true);

	return new_object;
}

// Объекты идендифицируются по имени, по этому каждый объект должен 
//    иметь уникальное имя.
bool Scene3D::nameIsFree( const char16_t* n )
{
	kkString new_name = n;
	kkString object_name;

	for(size_t i = 0, sz = m_objects.size(); i < sz; ++i )
	{
		object_name = m_objects[i]->GetName();

		if( object_name == new_name )
		{
			return false;
		}
	}
	return true;
}

void Scene3D::renameObject(kkScene3DObject* object, const char16_t* newName)
{
	kkString new_name = newName;
	s32 count = 0;
	while(true)
	{
		if(nameIsFree(new_name.c_str()))
		{
			object->SetName(new_name.c_str());
			return;
		}
		else
		{
			new_name = newName;
			new_name += count++;
		}
	}
}

kkString Scene3D::nameCreateNew( const char16_t* n )
{
	kkString new_name = n;
	for(u32 i = 0; i < 0xffff; ++i)
	{
		new_name += i;
		if( nameIsFree(new_name.data()))
		{
			break;
		}
		else
		{
			new_name = n;
		}
	}
	return new_name;
}

void Scene3D::updateSceneAabb()
{
	m_sceneAabb.reset();
	
	for(size_t i = 0, sz = m_objects.size(); i < sz; ++i )
	{
		m_sceneAabb.add( m_objects[i]->Aabb() );
	}
}

const kkAabb& Scene3D::getSceneAabb()
{
	return m_sceneAabb;
}

const kkAabb& Scene3D::getSelectionAabb()
{
	return m_selectionAabb;
}


std::basic_string<Scene3DObject*>& Scene3D::getObjects()
{
	return m_objects;
}

// rayobjects | drawobjects - отсортированные по дальности объекты
// самый близкий в конце
void Scene3D::_selectObjectsByRectangle_object( std::basic_string<Scene3DObject*>& rayobjects, std::basic_string<Scene3DObject*>& drawobjects, const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();
	
	for( auto * obj : drawobjects )
	{
		bool found = false;
		for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
		{
			auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
			for( u64 o = 0, sz2 = polygon->m_controlVertsInds.size(); o < sz2; ++o )
			{
				u64 o2 = o + 1;
				if(o2 == sz2) o2=0;
				auto cv1 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o] ];
				auto cv2 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o2] ];
				auto V1 = cv1->m_verts[0];
				auto V2 = cv2->m_verts[0];
				if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
				{
					found = true;
					if( ks == AppState_keyboard::Alt )
					{
						_deselectObject_object( obj );
					}
					else
					{
						_selectObject_object( obj );
					}
					goto next_object;
				}
			}
		}
	next_object:;

		// try ray-triangle
		if(!found)
		{
			kkRay ray[4];
			ray[0].m_origin = frust.m_top[0];
			ray[0].m_end = frust.m_top[3];
			ray[0].update();
			ray[1].m_origin = frust.m_top[1];
			ray[1].m_end = frust.m_top[2];
			ray[1].update();
			ray[2].m_origin = frust.m_bottom[0];
			ray[2].m_end = frust.m_bottom[3];
			ray[2].update();
			ray[3].m_origin = frust.m_bottom[1];
			ray[3].m_end = frust.m_bottom[2];
			ray[3].update();
			for(s32 i = 0; i < 4; ++i)
			{
				std::vector<kkRayTriangleIntersectionResultSimple> results;
				if( obj->IsRayIntersectMany(ray[i], results) )
				{
					if( ks == AppState_keyboard::Alt )
						_deselectObject_object( obj );
					else
						_selectObject_object( obj );
					break;
				}
			}
		}
	}

}

void Scene3D::_selectObjectsByRectangle_poly( const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( auto * obj : m_objects_selected )
	{
		obj->m_isObjectHaveSelectedPolys = false;

		bool need_update = false;
		for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
		{
			auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
			for( u64 o = 0, sz2 = polygon->m_controlVertsInds.size(); o < sz2; ++o )
			{
				u64 o2 = o + 1;
				if(o2 == sz2) o2=0;
				auto cv1 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o] ];
				auto cv2 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o2] ];

				auto V1 = cv1->m_verts[0];
				auto V2 = cv2->m_verts[0];

				if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
				{
					need_update = true;

					if( ks == AppState_keyboard::Alt )
					{

						polygon->Deselect();
						for(auto CV_index : polygon->m_controlVertsInds)
						{
							auto CV = (ControlVertex*)obj->m_PolyModel->m_controlPoints[CV_index];
							--CV->m_selectedPolysCounter;
							if( CV->m_selectedPolysCounter == 0 )
								CV->m_isSelected_poly = false;
						}
					}
					else
					{
						obj->m_isObjectHaveSelectedPolys = true;
						polygon->Select();
						for(auto CV_index : polygon->m_controlVertsInds)
						{
							auto CV = (ControlVertex*)obj->m_PolyModel->m_controlPoints[CV_index];
							++CV->m_selectedPolysCounter;
							CV->m_isSelected_poly = true;
						}
					}
					goto next_polygon;
				}
			}
		next_polygon:;
		}
		if(!obj->m_isObjectHaveSelectedPolys)
		{
			for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
			{
				auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
				if(polygon->IsSelected())
				{
					obj->m_isObjectHaveSelectedPolys = true;
					break;
				}
			}
		}

		// надо попробовать послать лучи по углам выделенной рамки
		if(!need_update)
		{
			kkRay ray[4];
			ray[0].m_origin = frust.m_top[0];
			ray[0].m_end = frust.m_top[3];
			ray[0].update();
			ray[1].m_origin = frust.m_top[1];
			ray[1].m_end = frust.m_top[2];
			ray[1].update();
			ray[2].m_origin = frust.m_bottom[0];
			ray[2].m_end = frust.m_bottom[3];
			ray[2].update();
			ray[3].m_origin = frust.m_bottom[1];
			ray[3].m_end = frust.m_bottom[2];
			ray[3].update();
			for(s32 i = 0; i < 4; ++i)
			{
				std::vector<kkRayTriangleIntersectionResultSimple> results;
				if( obj->IsRayIntersectMany(ray[i], results) )
				{
					/*std::sort( result.begin(),result.end(),
						[](const kkRayTriangleIntersectionResultSimple& first, const kkRayTriangleIntersectionResultSimple& second)
					{
						return first.m_T < second.m_T;
					} );*/
					
					for( auto result : results )
					{
						auto polygon = (Polygon3D*)obj->m_PolyModel->m_polygons[result.m_polygonIndex];
					
						if( ks == AppState_keyboard::Alt )
						{

							polygon->Deselect();
							for(auto CV_index : polygon->m_controlVertsInds)
							{
								auto CV = (ControlVertex*)obj->m_PolyModel->m_controlPoints[CV_index];
								--CV->m_selectedPolysCounter;
								if( CV->m_selectedPolysCounter == 0 )
									CV->m_isSelected_poly = false;
							}
						}
						else
						{
							obj->m_isObjectHaveSelectedPolys = true;
							polygon->Select();
							for(auto CV_index : polygon->m_controlVertsInds)
							{
								auto CV = (ControlVertex*)obj->m_PolyModel->m_controlPoints[CV_index];
								++CV->m_selectedPolysCounter;
								CV->m_isSelected_poly = true;
							}
						}
					}

					need_update = true;
					break;
				}
			}
		}

		if( need_update )
		{
			obj->updatePolygonModel();

			_updateSelectionAabb_polygon();
			updateObjectPolySelectList();
		}
	}
}

void Scene3D::_selectObjectsByRectangle_edge( const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( auto * obj : m_objects_selected )
	{
		bool need_update = false;

		for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
		{
			auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);

			for( u64 o = 0, sz2 = polygon->m_controlVertsInds.size(); o < sz2; ++o )
			{
				u64 o2 = o + 1;
				if(o2 == sz2) o2=0;
				auto cv1 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o] ];
				auto cv2 = (ControlVertex*)obj->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o2] ];

				auto V1 = cv1->m_verts[0];
				auto V2 = cv2->m_verts[0];

				if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
				{

					if( ks == AppState_keyboard::Alt )
					{
						auto pos = std::find(cv1->m_edgeWith.begin(), cv1->m_edgeWith.end(), cv2);
						if( pos != cv1->m_edgeWith.end() )
						{
							cv1->m_edgeWith.erase(pos);
							if( !cv1->m_edgeWith.size() ) cv1->m_isSelected_edge = false;
						}
						pos = std::find(cv2->m_edgeWith.begin(), cv2->m_edgeWith.end(), cv1);
						if( pos != cv2->m_edgeWith.end() )
						{
							cv2->m_edgeWith.erase(pos);
							if( !cv2->m_edgeWith.size() ) cv2->m_isSelected_edge = false;
						}
					}
					else
					{
						if( std::find(cv1->m_edgeWith.begin(), cv1->m_edgeWith.end(), cv2) == cv1->m_edgeWith.end() )
							cv1->m_edgeWith.push_back(cv2); 
						if( std::find(cv2->m_edgeWith.begin(), cv2->m_edgeWith.end(), cv1) == cv2->m_edgeWith.end() )
							cv2->m_edgeWith.push_back(cv1);

						cv1->m_isSelected_edge = true;
						cv2->m_isSelected_edge = true;
					}
					need_update = true;
				}
			}
		}

		if( need_update )
		{
			((Scene3DObject*)obj)->updateEdgeModel();
			((Scene3DObject*)obj)->m_isObjectHaveSelectedEdges = true;

			_updateSelectionAabb_edge();
			updateObjectEdgeSelectList();
		}
	}
}

/*

*/
void Scene3D::_selectObjectsByRectangle_vertex( /*const v4i& r*/ const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();
	
	for( auto object : m_objects_selected )
	{
		object->m_isObjectHaveSelectedVerts = false;
		auto & cverts = object->GetControlVertexArray();
		auto & verts = object->GetVertexArray();

		for( auto CV : cverts )
		{
			//auto & vert_inds = CV->getVertInds();
			//auto V = verts[vert_inds[0]];
			auto & verts = CV->getVerts();
			auto V = verts[0];
			if( frust.pointInFrust(math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot()) )
			{
				if( ks == AppState_keyboard::Alt )
				{
					CV->deselect();
				}
				else
				{
					object->m_isObjectHaveSelectedVerts = true;
					CV->select();
				}
			}
		}
		if(!object->m_isObjectHaveSelectedVerts)
		{
			for( auto CV : cverts )
			{
				if(CV->isSelected())
				{
					object->m_isObjectHaveSelectedVerts = true;
					break;
				}
			}
		}

		object->updateModelPointsColors();
	}
	
	//auto ks = m_app->getStateKeyboard();
	//if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//	deselectAll();
	//
	//// выбор вершин
	//// когда включен режим vertex, 2д точки рисуются на основе контрольных вершин.
	////   беру только те объекты, которые были ранее выделены
	//for( size_t i = 0, sz = m_objects_selected.size(); i <sz; ++i )
	//{
	//	auto object = m_objects_selected[ i ];
	//	for( u64 i2 = 0, sz2 = object->m_pointsInScreen.size(); i2 < sz2; ++i2 )
	//	{
	//		if( kkrooo::pointInRect( object->m_pointsInScreen[ i2 ].m_2d_coords, r) )
	//		{
	//			if( ks == AppState_keyboard::Alt )
	//			{
	//				object->m_PolyModel->m_controlPoints[ object->m_pointsInScreen[ i2 ].m_ID ]->deselect();
	//			}
	//			else
	//			{
	//				object->m_PolyModel->m_controlPoints[ object->m_pointsInScreen[ i2 ].m_ID ]->select();
	//			}
	//		}
	//	}
	//	object->updateModelPointsColors();
	//}

	//for( size_t i = 0, sz = m_objects_selected.size(); i <sz; ++i )
	//{
	//	auto object = m_objects_selected[ i ];
	//	object->m_isObjectHaveSelectedVerts = false;

	//	for( u64 i2 = 0, sz2 = object->m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
	//	{
	//		if( object->m_PolyModel->m_controlPoints[ i2 ]->isSelected() )
	//		{
	//			object->m_isObjectHaveSelectedVerts = true;
	//			break;
	//		}
	//	}
	//}

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::selectObjectsByRectangle( std::basic_string<Scene3DObject*>& ro, std::basic_string<Scene3DObject*>& dro, const v4i& r, const SelectionFrust& sfrust )
{
	switch(m_app->getEditMode())
	{
	case EditMode::Object:
	default:
		_selectObjectsByRectangle_object(ro, dro, sfrust);
		break;
	case EditMode::Vertex:
		_selectObjectsByRectangle_vertex( sfrust );
		break;
	case EditMode::Edge:
		_selectObjectsByRectangle_edge( sfrust );
		break;
	case EditMode::Polygon:
		_selectObjectsByRectangle_poly( sfrust );
		break;
	}
	//m_objects_selected.push_back(m_objects[i]);
}

void Scene3D::_selectObject_object(Scene3DObject* o)
{
	if( !o->m_isSelected )
	{
		m_objects_selected.push_back( o );
		o->m_isSelected = true;
		_updateSelectionAabb_object();
	}
}

void Scene3D::selectObject(kkScene3DObject* o)
{
	switch(m_app->getEditMode())
	{
	case EditMode::Object:
	default:
		_selectObject_object((Scene3DObject*)o);
		break;
	case EditMode::Vertex:
		break;
	}
}

void Scene3D::_deselectObject_object(Scene3DObject* o)
{
	if( o->m_isSelected )
	{
		m_objects_selected.erase( m_objects_selected.begin() + m_objects_selected.find_first_of(o) );
		o->m_isSelected = false;
		_updateSelectionAabb_object();
	}
}

void Scene3D::deselectObject(kkScene3DObject* o)
{
	if( m_ignoreDeselect )
	{
		m_ignoreDeselect = false;
		return;
	}
	switch(m_app->getEditMode())
	{
	case EditMode::Object:
	default:
		_deselectObject_object((Scene3DObject*)o);
		break;
	case EditMode::Vertex:
		break;
	}
}

void Scene3D::_selectAll_object()
{
	for( auto * o : m_objects )
	{
		o->m_isSelected = true;
		m_objects_selected.push_back(o);
	}
	_updateSelectionAabb_object();
}

void Scene3D::_selectAll_edge(Scene3DObject* object)
{
	_deselectAll_edge(object);
	for(u64 i = 0, sz = object->m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		auto polygon = (Polygon3D *)object->m_PolyModel->m_polygons.at(i);
		for( u64 o = 0, sz2 = polygon->m_controlVertsInds.size(); o < sz2; ++o )
		{
			u64 o2 = o + 1;
			if(o2 == sz2) o2=0;
			auto cv1 = (ControlVertex*)object->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o] ];
			auto cv2 = (ControlVertex*)object->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o2] ];

			cv1->m_edgeWith.push_back(cv2); 
			cv2->m_edgeWith.push_back(cv1);
			cv1->m_isSelected_edge = true;
			cv2->m_isSelected_edge = true;
		}
	}
	object->updateEdgeModel();
	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();
}

void Scene3D::_selectAll_poly(Scene3DObject* object)
{
	auto & cverts = object->GetControlVertexArray();
		
	auto & polys = object->GetPolygonArray();
	for( u64 i = 0, sz = polys.size(); i < sz; ++i )
	{
		auto P = polys[i];
		P->Select();
			
		auto & cvert_inds = P->GetControlVertInds();
		for( auto cv_index : cvert_inds )
		{
			auto CV = (ControlVertex*)cverts[cv_index];
			CV->m_isSelected_poly = true;
			++CV->m_selectedPolysCounter;
		}
	}

	object->updatePolygonModel();
	object->m_isObjectHaveSelectedPolys = true;

	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_selectAll_vertex(Scene3DObject* object)
{
	for( u64 i = 0, sz = object->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		object->m_PolyModel->m_controlPoints[ i ]->select();
	}

	object->m_isObjectHaveSelectedVerts = true;
	object->updateModelPointsColors();

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::selectAll()
{
	deselectAll();

	auto em = m_app->getEditMode();
	if(em == EditMode::Object)
		_selectAll_object();

	for( auto * o : m_objects_selected )
	{
		switch(em)
		{
		case EditMode::Object:
		default:
			break;
		case EditMode::Vertex:
			_selectAll_vertex(o);
			break;
		case EditMode::Edge:
			_selectAll_edge(o);
			break;
		case EditMode::Polygon:
			_selectAll_poly(o);
			break;
		}
	}
}

void Scene3D::_deselectAll_object(Scene3DObject* object)
{
	object->m_isSelected = false;
	m_objects_selected.clear();
	_updateSelectionAabb_object();
}

void Scene3D::_deselectAll_vertex(Scene3DObject* object)
{
	for( u64 i = 0, sz = object->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		object->m_PolyModel->m_controlPoints[ i ]->deselect();
	}
	object->m_isObjectHaveSelectedVerts = false;
	object->updateModelPointsColors();
	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::deselectAll()
{
//	printf("desel\n");
	if( m_ignoreDeselect )
	{
		m_ignoreDeselect = false;
		return;
	}

	for( auto * o : m_objects_selected )
	{
		switch(m_app->getEditMode())
		{
		case EditMode::Object:
		default:
			_deselectAll_object(o);
			break;
		case EditMode::Vertex:
			_deselectAll_vertex(o);
			break;
		case EditMode::Edge:
			_deselectAll_edge(o);
			break;
		case EditMode::Polygon:
			_deselectAll_poly(o);
			break;
		}
	}
}

void Scene3D::_selectInvert_object()
{
	static std::basic_string<Scene3DObject*> non_selected_objects;
	for( auto * o : m_objects )
	{
		if( !o->m_isSelected )
			non_selected_objects.push_back( o );
	}
	deselectAll();
	for( auto * o : non_selected_objects )
	{
		o->m_isSelected = true;
		m_objects_selected.push_back(o);
	}
	non_selected_objects.clear();
	_updateSelectionAabb_object();
}

void Scene3D::_selectInvert_poly(Scene3DObject* object)
{
	auto & cverts = object->GetControlVertexArray();

	// надо сохранить не выделенные полигоны
	std::vector<kkPolygon*> not_selected_polys;
	for( u64 i = 0, sz = object->m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		auto P = object->m_PolyModel->m_polygons[i];
		if(!P->IsSelected())
			not_selected_polys.emplace_back(P);
	}
		
	_deselectAll_poly(object);	

	object->m_isObjectHaveSelectedPolys = false;
	for(auto P : not_selected_polys)
	{
		object->m_isObjectHaveSelectedPolys = true;
		P->Select();
		auto & cvert_inds = P->GetControlVertInds();
		for( u64 i = 0, sz = cvert_inds.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)cverts[cvert_inds[i]];
			++CV->m_selectedPolysCounter;
			CV->m_isSelected_poly = true;
		}
	}

	object->updatePolygonModel();
	

	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_selectInvert_edge(Scene3DObject* object)
{
	// нужно сохранить пары контрольных вершин которые не образуют ребро
	// потом сныть всё выделение
	// и в конце выделить рёбра на основе пар
	std::vector<std::pair<ControlVertex*, ControlVertex*>> pairs;

	for(u64 i = 0, sz = object->m_PolyModel->m_polygons.size(); i < sz; ++i )
	{
		auto polygon = (Polygon3D *)object->m_PolyModel->m_polygons.at(i);
		for( u64 o = 0, sz2 = polygon->m_controlVertsInds.size(); o < sz2; ++o )
		{
			u64 o2 = o + 1;
			if(o2 == sz2) o2=0;
			auto cv1 = (ControlVertex*)object->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o] ];
			auto cv2 = (ControlVertex*)object->m_PolyModel->m_controlPoints[ polygon->m_controlVertsInds[o2] ];

			if( std::find(cv1->m_edgeWith.begin(), cv1->m_edgeWith.end(), cv2) == cv1->m_edgeWith.end() )
				pairs.emplace_back(std::pair<ControlVertex*, ControlVertex*>(cv1, cv2));
		}
	}
	_deselectAll_edge(object);
	if( pairs.size() )
	{
		for( auto & pair : pairs )
		{
			auto pos = std::find(pair.first->m_edgeWith.begin(), pair.first->m_edgeWith.end(), pair.second);
			if( pos == pair.first->m_edgeWith.end() )
			{
				pair.first->m_edgeWith.push_back(pair.second); 
				pair.second->m_edgeWith.push_back(pair.first);
				pair.first->m_isSelected_edge = true;
				pair.second->m_isSelected_edge = true;
			}
		}
	}

	object->updateEdgeModel();

	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();
}

void Scene3D::_selectInvert_vertex(Scene3DObject* object)
{
	object->m_isObjectHaveSelectedVerts = false;

	for( u64 i = 0, sz = object->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
	{
		if( object->m_PolyModel->m_controlPoints[ i ]->isSelected() )
			object->m_PolyModel->m_controlPoints[ i ]->deselect();
		else
		{
			object->m_PolyModel->m_controlPoints[ i ]->select();
			object->m_isObjectHaveSelectedVerts = true;
		}
	}
	object->updateModelPointsColors();

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::selectInvert()
{
	auto em = m_app->getEditMode();
	if( em == EditMode::Object )
		_selectInvert_object();

	for( auto * o : m_objects_selected )
	{
		switch(em)
		{
		case EditMode::Object:
		default:
			break;
		case EditMode::Vertex:
			_selectInvert_vertex(o);
			break;
		case EditMode::Edge:
			_selectInvert_edge(o);
			break;
		case EditMode::Polygon:
			_selectInvert_poly(o);
			break;
		}
	}
}

void	  Scene3D::updateSelectionAabb()
{
	switch(m_app->getEditMode())
	{
	case EditMode::Object:
	default:
		_updateSelectionAabb_object();
		break;
	case EditMode::Vertex:
		_updateSelectionAabb_vertex();
		updateObjectVertexSelectList();
		break;
	case EditMode::Edge:
		_updateSelectionAabb_edge();
		updateObjectVertexSelectList();
		break;
	case EditMode::Polygon:
		_updateSelectionAabb_polygon();
		updateObjectPolySelectList();
		break;
	}
}


void Scene3D::_updateSelectionAabb_object()
{
	m_selectionAabb.reset();
	for( auto * o : m_objects_selected )
	{
		m_selectionAabb.add( o->Aabb() );
	}
}

void Scene3D::_updateSelectionAabb_vertex()
{
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)o->m_PolyModel->m_controlPoints[ i ];

			if( CV->isSelected())
			{
				nnn = false;
				auto V = (Vertex*)CV->m_verts[0];
				m_selectionAabb.add( math::mul( V->m_Position, M ) + o->GetPivot() );
			}
		}
	}
	if( nnn )
	{
		if( m_objects_selected.size() )
		{
			for( auto * o : m_objects_selected )
			{
				m_selectionAabb.add(o->Aabb());
			}
		}
		else
		{
			m_selectionAabb = m_sceneAabb;
		}
	}
}

void Scene3D::_updateSelectionAabb_edge()
{
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)o->m_PolyModel->m_controlPoints[ i ];

			if( CV->isSelectedEdge())
			{
				nnn = false;
				auto V = (Vertex*)CV->m_verts[0];
				m_selectionAabb.add( math::mul( V->m_Position, M ) + o->GetPivot() );
			}
		}
	}
	if( nnn )
	{
		if( m_objects_selected.size() )
		{
			for( auto * o : m_objects_selected )
			{
				m_selectionAabb.add(o->Aabb());
			}
		}
		else
		{
			m_selectionAabb = m_sceneAabb;
		}
	}
}

void Scene3D::_updateSelectionAabb_polygon()
{
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	/*kkVector4 P;
	for( auto * o : m_objects_selected )
	{
		P += o->m_pivot;
	}
	P /= (f32)m_objects_selected.size();*/

	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_controlPoints.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)o->m_PolyModel->m_controlPoints[ i ];

			if( CV->isSelectedPoly())
			{
				nnn = false;
				auto V = (Vertex*)CV->m_verts[0];
				m_selectionAabb.add( math::mul( V->m_Position, M ) + o->GetPivot() );
			}
		}
	}
	if( nnn )
	{
		if( m_objects_selected.size() )
		{
			for( auto * o : m_objects_selected )
			{
				m_selectionAabb.add(o->Aabb());
			}
		}
		else
		{
			m_selectionAabb = m_sceneAabb;
		}
	}
}

void Scene3D::_deleteSelectedObjects_object()
{
	std::basic_string<Scene3DObject*> objects_to_destroy;
	
	for(size_t i = 0, sz = m_objects.size(); i < sz; ++i )
	{
		if( m_objects[ i ]->m_isSelected )
		{
			objects_to_destroy.push_back( m_objects[ i ] );
		}
	}
	
	for(size_t i = 0, sz = objects_to_destroy.size(); i < sz; ++i )
	{
		kkDestroy( objects_to_destroy[ i ] );
		m_objects.erase( m_objects.begin() + m_objects.find_first_of(objects_to_destroy[ i ]) );
	}

	if( objects_to_destroy.size() )
		m_app->setNeedToSave(true);

	m_objects_selected.clear();
}

void Scene3D::_deleteSelectedObjects_vertex(Scene3DObject* object)
{
	object->deleteSelectedVerts();
	//object->UpdateScreenSpacePoints();
	object->updateModelPointsColors();
	
	m_app->setNeedToSave(true);

	_deselectAll_vertex(object);
}

void Scene3D::_deleteSelectedObjects_poly(Scene3DObject* object)
{
	object->deleteSelectedPolys();
	//object->UpdateScreenSpacePoints();
	object->updateModelPointsColors();
	m_app->setNeedToSave(true);
	_deselectAll_poly(object);
}

void Scene3D::_deleteSelectedObjects_edge(Scene3DObject* object)
{
	object->deleteSelectedEdges();
	//object->UpdateScreenSpacePoints();
	object->updateModelPointsColors();
	
	m_app->setNeedToSave(true);

	_deselectAll_edge(object);
}

void Scene3D::deleteSelectedObjects()
{
	auto em = m_app->getEditMode();

	if(em == EditMode::Object)
		_deleteSelectedObjects_object();

	for( auto o : m_objects_selected )
	{
		switch(em)
		{
		case EditMode::Object:
		default:
			break;
		case EditMode::Vertex:
			_deleteSelectedObjects_vertex(o);
			break;
		case EditMode::Edge:
			_deleteSelectedObjects_edge(o);
			break;
		case EditMode::Polygon:
			_deleteSelectedObjects_poly(o);
			break;
		}
	}

	updateSceneAabb();
	updateSelectionAabb();
}

//void Scene3D::updateObject2DPoints( std::basic_string<Scene3DObject*>& objects )
//{
//	for( auto o : objects )
//	{
//		o->UpdateScreenSpacePoints();
//	}
//}
//
//void Scene3D::updateObject2DPoints_selected()
//{
//	for( auto o : m_objects_selected )
//	{
//		o->UpdateScreenSpacePoints();
//	}
//}
//
//void      Scene3D::updateObject2DPoints( kkScene3DObject* o )
//{
//	switch(m_app->getEditMode())
//	{
//	case EditMode::Object:
//	default:
//		o->UpdateScreenSpacePoints();
//		break;
//	case EditMode::Vertex:
//		o->UpdateScreenSpacePoints();
//		break;
//	}
//}

void Scene3D::unregisterObject( Scene3DObject* o )
{
	if( m_objects.find_first_of(o) != std::string::npos )
		m_objects.erase( m_objects.begin() + m_objects.find_first_of(o) );

	if( m_objects_selected.find_first_of(o) != std::string::npos )
		m_objects_selected.erase( m_objects_selected.begin() + m_objects_selected.find_first_of(o) );
}

void Scene3D::rotateSelectedObjects(bool startStop, const AppEvent_gizmo& e, bool cancel, bool first )
{
	auto em = m_app->getEditMode();

	bool blender_style = true;

	static f32 first_x = 0, first_y = 0, first_z = 0;
	static f32 toolTip_angle_x = 0.f, toolTip_angle_y = 0.f, toolTip_angle_z = 0.f;
	static kkStringA toolTip;

	static kkVector4 C;
	if( first )
		m_selectionAabb.center(C);

	f32 x = 0, y = 0, z = 0;

	auto camera  = m_app->m_active_viewport->getCamera();

	auto p1 = m_app->m_currentGizmoEvent.point2D;
	auto p2 = m_app->m_cursor_position;
	auto p3 = p2 - p1;
	v2f point((float)p3.x,(float)p3.y);

	if( e.part == AppEvent_gizmo::_part::_x )
	{
		if( blender_style )
		{
			x = (f32)std::atan2((f32)point.x, (f32)point.y);

			if( first )
			{
				first_x = x;
			}

			x -= first_x;
		}
	}

	if( e.part == AppEvent_gizmo::_part::_y )
	{
		if( blender_style )
		{
			y = (f32)std::atan2((f32)point.x, (f32)point.y);
			if( first )
			{
				first_y = y;
			}

			y -= first_y;
		}
	}

	if( e.part == AppEvent_gizmo::_part::_z )
	{
		if( blender_style )
		{
			z = (f32)std::atan2((f32)point.x, (f32)point.y);
			if( first )
			{
				first_z = z;
			}

			z -= first_z;
		}

		
	}

	kkMatrix4 M;
	float deg_f = 0.f;

	if( e.part == AppEvent_gizmo::_part::_screen_plane )
	{
		auto vprc = m_app->m_active_viewport->getRect();
		auto vpsz = vprc.getWidthAndHeight();

		auto p1 = v2i( (vpsz.x/2.f)+vprc.x,(vpsz.y/2.f)+vprc.y);
		auto p2 = m_app->m_cursor_position;
		auto point = p2 - p1;

		x = (f32)std::atan2((f32)point.x, (f32)point.y);
		if( first )
		{
			first_x = x;
		}

		x -= first_x;


		if( m_app->m_state_keyboard == AppState_keyboard::Shift )
		{
			int deg = (int)math::radToDeg( x );
			if( deg % 5 != 0 )
				deg -= deg % 5;
				deg_f = (float)deg;
		}
		else
		{
			deg_f = math::radToDeg( x );
		}

		m_app->m_active_viewport->getActiveViewportCamera()->setObjectRotationAngle(-math::degToRad((float)deg_f));

		m_app->m_active_viewport->getActiveViewportCamera()->update();
		M = m_app->m_active_viewport->getActiveViewportCamera()->getObjectRotationMatrix();
	}

	//printf("%f %f\n", x, y);

	auto cameraRotation = m_app->m_active_viewport->getActiveViewportCamera()->getAllRotation();

	//printf("%f\n", cameraRotation.y );


	if( e.part == AppEvent_gizmo::_part::_x )
	{
		if( blender_style )
		{
			static float last_x =  0.f;
			if( x > 0.f )
			{
				x = -(math::PI+math::PI) + x;
			}
			toolTip_angle_x += -x - last_x;
			last_x = -x;


			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
			{
				int   deg_i = (int)math::radToDeg( std::abs( toolTip_angle_x ));
				if( deg_i % 5 != 0 )
					deg_i -= (deg_i % 5);
				deg_f = (float)deg_i;
			}
			else
			{
				deg_f = math::radToDeg( std::abs( toolTip_angle_x ));
			}

			if( cameraRotation.y < 0.f )
			{
				kkQuaternion Q( -math::degToRad((float)deg_f),0.f,0.f);
				math::makeRotationMatrix(M,Q);
			}
			else	
			{
				kkQuaternion Q( math::degToRad((float)deg_f),0.f,0.f);
				math::makeRotationMatrix(M,Q);
			}
		}
	}

	if( e.part == AppEvent_gizmo::_part::_y )
	{
		if( blender_style )
		{
			static float last_y =  0.f;

			if( y > 0.f )
			{
				y = -(math::PI+math::PI) + y;
			}
			toolTip_angle_y += -y - last_y;
			last_y = -y;


			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
			{
				int deg_i = (int)math::radToDeg( std::abs( toolTip_angle_y ));
				if( deg_i % 5 != 0 )
					deg_i -= (deg_i % 5);

				deg_f = (float)deg_i;
			}
			else
			{
				deg_f = math::radToDeg( std::abs( toolTip_angle_y ));
			}

			if( cameraRotation.x > 90.f || cameraRotation.x < -90.f  )
			{
				kkQuaternion Q( 0.f,-math::degToRad((float)deg_f),0.f);
				math::makeRotationMatrix(M,Q);
			}
			else	
			{
				kkQuaternion Q( 0.f,math::degToRad((float)deg_f),0.f);
				math::makeRotationMatrix(M,Q);
			}
		}
	}

	if( e.part == AppEvent_gizmo::_part::_z )
	{
		if( blender_style )
		{
			static float last_z =  0.f;

			if( z > 0.f )
			{
				z = -(math::PI+math::PI) + z;
			}
			toolTip_angle_z += -z - last_z;
			last_z = -z;
			

			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
			{
				int deg = (int)math::radToDeg( std::abs( toolTip_angle_z ));
				if( deg % 5 != 0 )
					deg -= deg % 5;
				deg_f = (float)deg;
			}
			else
			{
				deg_f = math::radToDeg( std::abs( toolTip_angle_z ));
			}

			if( cameraRotation.y < 90.f && cameraRotation.y > -90.f )
			{
				kkQuaternion Q( 0.f,0.f, math::degToRad((float)deg_f) );
				math::makeRotationMatrix(M,Q);
			}
			else	
			{
				kkQuaternion Q( 0.f,0.f, -math::degToRad((float)deg_f) );
				math::makeRotationMatrix(M,Q);
			}
		}
	}

	kkVector4 pivotFix;


	

	auto num = m_objects_selected.size();

	if( startStop )
	{
		switch (em)
		{
		case EditMode::Object:
		{
			auto M2 = M;
			for( auto * o : m_objects_selected )
			{
				o->m_matrix = M * o->m_matrixFixed;

				o->m_matrixOnlyRotation = M * o->m_matrixOnlyRotationFixed;

				// global
				if( m_app->m_isLocalRotation == false && num > 1 )
				{
					pivotFix = o->GetPivotFixed();
					auto & pivot  = o->GetPivot();
					pivot = math::mul(pivotFix-C,M2)+C;
				}

//				o->updateMatrixPosition();
				o->UpdateAabb();
			}
		}
			break;
		case EditMode::Vertex:
		{
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
			}
		}
			break;
		case EditMode::Edge:
			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsEdgeSelectInfo[ i ];
				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
			}
			break;
		case EditMode::Polygon:
			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsPolySelectInfo[ i ];
				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
			}
			break;
		default:
			break;
		}
		
	}
	else
	{
		switch (em)
		{
		case EditMode::Object:
		{
			for( auto * o : m_objects_selected )
			{
				if( cancel )
				{
					// global
					if( m_app->m_isLocalRotation  == false && num > 1 )
					{
						o->RestorePivot();
					}

					o->m_matrix = o->m_matrixFixed;
					o->m_matrixOnlyRotation = o->m_matrixOnlyRotationFixed;

					o->UpdateAabb();
				}
				else
				{
					// global
					if( m_app->m_isLocalRotation  == false && num > 1 )
					{
						o->ApplyPivot();
					}

					o->m_matrixFixed = o->m_matrix;
					o->m_matrixOnlyRotationFixed = o->m_matrixOnlyRotation;
				}
			
			//	updateObject2DPoints(o);
			}
		}
			break;
		case EditMode::Edge:
		case EditMode::Vertex:
		case EditMode::Polygon:
		{
			for( auto * o : m_objects_selected )
			{
				if( cancel )
				{
					o->RestorePosition();
				}
				else
				{
					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
				}
			//	updateObject2DPoints(o);
			}
		}
			break;
		//case EditMode::Edge:
		//	break;
		//case EditMode::Polygon:
		//	break;
		default:
			break;
		}
		
		updateSelectionAabb();
		updateSceneAabb();
	}
}

void Scene3D::scaleSelectedObjects(bool startStop, const AppEvent_gizmo& e, bool cancel,bool first )
{
	kkVector4 pivotFix;

	f32 x = 0, y = 0, z = 0;

	auto r1 = m_app->m_cursorRayFirstClick.m_origin;
	auto r2 = m_app->m_cursorRayCurrent.m_origin;

	auto camera  = m_app->m_active_viewport->getCamera();

	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
	// но на самом деле это мир идёт ВВЕРХ, плюс Y
	auto cam_pos = -camera->getPositionInSpace();
	

	if( e.part == AppEvent_gizmo::_part::_x )
	{
		x = (r2-r1).KK_X;
	}

	if( e.part == AppEvent_gizmo::_part::_y )
	{
		y = (r2-r1).KK_Y;
	}

	if( e.part == AppEvent_gizmo::_part::_z )
	{
		z = (r2-r1).KK_Z;
	}

	if( e.part == AppEvent_gizmo::_part::_xy_plane )
	{
		x = (r2-r1).KK_X;
		y = (r2-r1).KK_Y;
	}

	if( e.part == AppEvent_gizmo::_part::_xz_plane )
	{
		x = (r2-r1).KK_X;
		z = (r2-r1).KK_Z;
	}

	if( e.part == AppEvent_gizmo::_part::_zy_plane )
	{
		y = (r2-r1).KK_Y;
		z = (r2-r1).KK_Z;
	}

	if( e.part == AppEvent_gizmo::_part::_screen_plane )
	{
		x = (r2-r1).KK_X;
		z = (r2-r1).KK_X;
		y = (r2-r1).KK_X;
	}

	//printf("%f %f\n", x, y);

	//auto D = 5000.f; // должно быть значение far камеры

	static kkVector4 C;
	if( first )
		m_selectionAabb.center(C);

	f32 L = 1.f;
	if( m_app->m_active_viewport->isPerspective() )
	{
		L = 500.0f;
	}
	
	x = x * L;
	y = y * L;
	z = z * L;

	//printf("%f\n", x );
	// здесь попробую сделать так, чтобы уменьшение масштаба было не линейным
	if(x<0.f){ x = x * 1.f / (1.f-x); }
	if(y<0.f){ y = y * 1.f / (1.f-y); }
	if(z<0.f){ z = z * 1.f / (1.f-z); }

	x += 1.f;
	y += 1.f;
	z += 1.f;

	if( x <= 0.f ) x = FLT_MIN;
	if( y <= 0.f ) y = FLT_MIN;
	if( z <= 0.f ) z = FLT_MIN;
	
	//printf("%f\n", m_gizmo->m_scaleSize.KK_X );

	if( startStop )
	{
		m_gizmo->m_scaleSize.KK_X  = x-1.f;
		m_gizmo->m_scaleSize.KK_Y  = y-1.f;
		m_gizmo->m_scaleSize.KK_Z  = z-1.f;
	}
	else
	{
		m_gizmo->m_scaleSize.KK_X  = 0;
		m_gizmo->m_scaleSize.KK_Y  = 0;
		m_gizmo->m_scaleSize.KK_Z  = 0;
	}

	kkMatrix4 S;
	math::makeScaleMatrix(kkVector4(x,y,z,1.f),S);

	////printf("%f\n", L );

	kkMatrix4 M;

	auto em = m_app->getEditMode();

	if( startStop )
	{
		switch (em)
		{
		case EditMode::Object:
		{
			auto n = m_objects_selected.size();
			for( auto * o : m_objects_selected )
			{
				//o->m_scaleMatrix = S;
				o->m_matrix = S * o->m_matrixFixed;

				// global
				if( n == 1)
				{
					pivotFix = o->GetPivotFixed();
					auto & pivot  = o->GetPivot();
					pivot = math::mul(pivotFix-pivotFix,S)+pivotFix;
				}
				else if( m_app->m_isLocalScale  == false )
				{
					pivotFix = o->GetPivotFixed();
					auto & pivot  = o->GetPivot();
					pivot = math::mul(pivotFix-C,S)+C; // для глобал нужно C
				}


//				o->updateMatrixPosition();
				o->UpdateAabb();
			}
		}
			break;
		case EditMode::Vertex:
		{
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
			}
		}
			break;
		case EditMode::Edge:
			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsEdgeSelectInfo[ i ];
				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
			}
			break;
		case EditMode::Polygon:
			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsPolySelectInfo[ i ];
				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
			}
			break;
		default:
			break;
		}
		
	}
	else
	{
		switch (em)
		{
		case EditMode::Object:
			for( auto * o : m_objects_selected )
			{
				if( cancel )
				{
					// global
					if( m_app->m_isLocalScale  == false )
					{
						o->RestorePivot();
					}

					o->m_matrix      = o->m_matrixFixed;
					//o->m_scaleMatrix = o->m_scaleMatrixFixed;
//					o->updateMatrixPosition();
					o->UpdateAabb();
				}
				else
				{
					// global
					if( m_app->m_isLocalScale  == false )
					{
						o->ApplyPivot();
					}

					o->m_matrixFixed = o->m_matrix;
				}
			//	updateObject2DPoints(o);
			}
			break;
		case EditMode::Edge:
		case EditMode::Vertex:
		case EditMode::Polygon:
			for( auto * o : m_objects_selected )
			{
				if( cancel )
				{
					o->RestorePosition();
				}
				else
				{
					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
				//	o->generateNormals();
					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
				}
			//	updateObject2DPoints(o);
			}
			break;
		//case EditMode::Edge:
		//	break;
		//case EditMode::Polygon:
		//	break;
		default:
			break;
		}
		
		updateSelectionAabb();
		updateSceneAabb();
	}
}

void Scene3D::moveSelectedObjects(bool startStop, const AppEvent_gizmo& e, bool cancel,bool first )
{
	auto em = m_app->getEditMode();

	kkVector4 pivotFix;

	static f32 x = 0, y = 0, z = 0;

	// старая версия. Проблема была в том, что 2д точки получаются корректными только когда они видны
	// если точка уходит за пределы видимости, значения получаются гигантскими и модель улетает.
	// замена на обычные координаты курсора
	// старая версия подходит только для масштабирования
	// так-же пока она будет использоваться для перемещения в плоскости экрана
	auto r1 = m_app->m_cursorRayFirstClick.m_origin;
	auto r2 = m_app->m_cursorRayCurrent.m_origin;

	static v2i p1 = m_app->m_cursor_position;
	auto p2 = m_app->m_cursor_position;
	
	//printf("%i %i\n",p2.x,p2.y);

	if( first )
	{
		p1 = p2;
		x = y = z = 0;
	}

	auto point = p2 - p1;

	p1 = p2;

	auto camera  = m_app->m_active_viewport->getCamera();

	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
	// но на самом деле это мир идёт ВВЕРХ, плюс Y
	auto cam_pos = -camera->getPositionInSpace();
	
	auto cameraRotation = m_app->m_active_viewport->getActiveViewportCamera()->getAllRotation();

	if( e.part == AppEvent_gizmo::_part::_x || e.part == AppEvent_gizmo::_part::_xy_plane || e.part == AppEvent_gizmo::_part::_xz_plane )
	{
		x += point.x * std::cos( cameraRotation.y / 180.f * math::PI );
		x += point.y * std::sin( cameraRotation.y / 180.f * math::PI );
	}

	if( e.part == AppEvent_gizmo::_part::_y || e.part == AppEvent_gizmo::_part::_xy_plane || e.part == AppEvent_gizmo::_part::_zy_plane)
	{
		if( cameraRotation.x < 0.f )
			y += -point.y;
		else
			y += point.y;
	}
	
	if( e.part == AppEvent_gizmo::_part::_z || e.part == AppEvent_gizmo::_part::_zy_plane || e.part == AppEvent_gizmo::_part::_xz_plane )
	{
		z -= point.x * std::sin( cameraRotation.y / 180.f * math::PI );
		z += point.y * std::cos( cameraRotation.y / 180.f * math::PI );
	}

	if( e.part == AppEvent_gizmo::_part::_screen_plane )
	{
	//	printf("m");
		if( m_app->m_active_viewport->isPerspective() )
		{
			const f32 s = 1.f;
			z = (r2-r1).KK_Z * s;
			x = (r2-r1).KK_X * s;
			y = (r2-r1).KK_Y * s;
		}
		else
		{
			z = (r2-r1).KK_Z;
			x = (r2-r1).KK_X;
			y = (r2-r1).KK_Y;
		}
	}

	//printf("%f %f\n", r1.KK_X, r2.KK_X);

	auto D = 5000.f; // должно быть значение far камеры

	static kkVector4 C;
	if( first )
		m_selectionAabb.center(C);

	f32 L = 1.f;
	if( m_app->m_active_viewport->isPerspective() )
	{
		if( e.part == AppEvent_gizmo::_part::_screen_plane )
			D *= 0.00001f; // делаю чтобы было побыстрее
		else
			D *= 0.15f;

		L = C.distance(cam_pos);
		if( L == 0.f )
			L += 0.01f;

		L /= D;
	}
	else
	{
		if( e.part != AppEvent_gizmo::_part::_screen_plane )
		{
			auto vpcam = m_app->m_active_viewport->getActiveViewportCamera();
			L /= vpcam->getZoomOrt() * 100.f;
		}
	}
	
	//printf("%f\n", L );

	ObjectVertexSelectInfo * info;
	auto V = kkVector4(x*L,y*L,z*L);
	if( startStop )
	{
		switch (em)
		{
		case EditMode::Object:
			for( auto * o : m_objects_selected )
			{
				pivotFix = o->GetPivotFixed();
				auto & pivot  = o->GetPivot();

				pivot = pivotFix + V;

//				o->updateMatrixPosition();
				o->UpdateAabb();
			}
			break;
		case EditMode::Vertex:
			//printf("%llu\n",m_objectsVertexSelectInfo.size());
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->moveVerts( V,info->m_verts);
			}
			break;
		case EditMode::Edge:
			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
			{
				info = &m_objectsEdgeSelectInfo[ i ];
				info->m_object->moveVerts( V,info->m_verts);
			}
			break;
		case EditMode::Polygon:
			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
			{
				info = &m_objectsPolySelectInfo[ i ];
				info->m_object->moveVerts( V,info->m_verts);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		// нажимается правой кнопкой. Левая кнопка может быть ещё нажата
		if( cancel )
		{
			m_app->m_state_app = AppState_main::CancelTransformation;
		}

		switch (em)
		{
		case EditMode::Object:
			for( auto * o : m_objects_selected )
			{
				if( cancel )
				{
					o->RestorePivot();
					o->UpdateAabb();
				}
				else
				{
					o->ApplyPivot();
				}
			//	updateObject2DPoints(o);
			}
			break;
		case EditMode::Edge:
		case EditMode::Vertex:
		case EditMode::Polygon:
			for( auto * o : m_objects_selected )
			{
				if( cancel )
					o->RestorePosition();
				else
				{
					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
				}
			//	updateObject2DPoints(o);
			}
			break;
		/*case EditMode::Edge:
			break;*/
		//case EditMode::Polygon:
		//	break;
		default:
			break;
		}
		
	}
    
	updateSelectionAabb();
	updateSceneAabb();
}


bool Scene3D::isVertexHover(const SelectionFrust& frust)
{
	for( auto * object : m_objects_selected )
	{
		auto & cverts = object->GetControlVertexArray();
		auto & verts = object->GetVertexArray();
		for( auto CV : cverts )
		{
			//auto & vert_inds = CV->getVertInds();
			auto & verts = CV->getVerts();
			auto V = verts[0];
			if( frust.pointInFrust(math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot()) )
			{
				return true;
			}
		}
	}
	return false;
}


void Scene3D::doSelectVertexHover(const SelectionFrust& frust,ViewportCamera* camera)
{
	std::basic_string<ControlVertex*> hovered_points;
	auto camera_position = camera->getPositionCamera();

	for( auto * object : m_objects_selected )
	{
		auto & cverts = object->GetControlVertexArray();
		auto & verts = object->GetVertexArray();
		for( auto CV : cverts )
		{
			//auto & vert_inds = CV->getVertInds();
			auto & verts = CV->getVerts();
			auto V = verts[0];
			auto point3D = math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot();
			if( frust.pointInFrust(point3D) )
			{
				((ControlVertex*)CV)->m_distanceToCamera = camera_position.distance(point3D);
				hovered_points.push_back((ControlVertex*)CV);
			}
		}
	}
	if(hovered_points.size()>1)
	{
		std::sort(hovered_points.begin(),hovered_points.end(),
			[](ControlVertex* first, ControlVertex* second)
			{
				return first->m_distanceToCamera < second->m_distanceToCamera;
			}
		);
	}

	auto ks = m_app->getStateKeyboard();
	if( hovered_points.size() )
	{
		if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
			deselectAll();

		hovered_points[ 0 ]->m_isSelected = true;
		if( ks == AppState_keyboard::Alt )
			hovered_points[ 0 ]->m_isSelected = false;
	}
	else
	{
		if( ks != AppState_keyboard::Ctrl )
			deselectAll();
	}
	
	for( auto * object : m_objects_selected )
	{
		object->m_isObjectHaveSelectedVerts = false;

		auto & cverts = object->GetControlVertexArray();
		for( auto CV : cverts )
		{
			if(CV->isSelected())
			{
				object->m_isObjectHaveSelectedVerts = true;
				break;
			}
		}
		object->updateModelPointsColors();
	}

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void      Scene3D::updateObjectVertexSelectList()
{
	m_objectsVertexSelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;

		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
		{
			if( o->m_PolyModel->m_controlPoints[ i2 ]->isSelected() )
			{
				info.m_verts.push_back( (ControlVertex*)o->m_PolyModel->m_controlPoints[ i2 ] );
			}
		}

		if( info.m_verts.size() )
			m_objectsVertexSelectInfo.push_back(info);
	}
}

void      Scene3D::updateObjectPolySelectList()
{
	m_objectsPolySelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;

		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
		{
			if( o->m_PolyModel->m_controlPoints[ i2 ]->isSelectedPoly() )
			{
				info.m_verts.push_back( (ControlVertex*)o->m_PolyModel->m_controlPoints[ i2 ] );
			}
		}

		if( info.m_verts.size() )
			m_objectsPolySelectInfo.push_back(info);
	}
}

void      Scene3D::updateObjectEdgeSelectList()
{
	m_objectsEdgeSelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;

		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_controlPoints.size(); i2 < sz2; ++i2 )
		{
			if( o->m_PolyModel->m_controlPoints[ i2 ]->isSelectedEdge() )
			{
				info.m_verts.push_back( (ControlVertex*)o->m_PolyModel->m_controlPoints[ i2 ] );
			}
		}

		if( info.m_verts.size() )
			m_objectsEdgeSelectInfo.push_back(info);
	}
}


// Цель - преобразовать координаты controlVertex в соответствии с матрицами, перестроить модели и сбросить матрицы
void Scene3D::applyMatrices()
{
	for( u64 i = 0, sz = m_objects_selected.size(); i < sz; ++i )
	{
		m_objects_selected[i]->applyMatrices();
	}
	updateSceneAabb();
	updateSelectionAabb();
}

void Scene3D::resetMatrices()
{
	for( auto * o : m_objects_selected )
	{
		o->resetMatrices();
	}
	updateSceneAabb();
	updateSelectionAabb();
}


void Scene3D::deleteObject( kkScene3DObject* o )
{
	deselectAll();
	selectObject((Scene3DObject*)o);
	deleteSelectedObjects();
}

//void Scene3D::selectEdges(kkRay* ray)
bool Scene3D::selectEdges(/*CursorRay* cursorRay, */kkRay* ray/*, int depth*/)
{
	if(!m_objects_selected.size()) return false;
	if( !ray ) return false;

	std::vector<kkRayTriangleIntersectionResultSimple> results;

	for( auto * o : m_objects_selected )
	{
		o->IsRayIntersectMany(*ray, results);
	}

	if(!results.size())return false;

	std::sort( results.begin(),results.end(),
		[](const kkRayTriangleIntersectionResultSimple& first, const kkRayTriangleIntersectionResultSimple& second)
	{
		return first.m_T < second.m_T;
	} );

	auto ks = m_app->getStateKeyboard();

	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( u64 o = 0, osz = results.size(); o < osz; ++o )
	{
		auto & result = results[o];
		auto & verts = result.m_object->GetVertexArray();
		auto & cverts = result.m_object->GetControlVertexArray();
		auto firstPolygon = result.m_object->GetPolygon( result.m_polygonIndex );
		auto & cvs = firstPolygon->GetControlVertInds();
		auto pivot = result.m_object->GetPivot();
		auto & M = result.m_object->GetMatrix();

		for( u64 i = 0, sz = cvs.size(); i < sz; ++i )
		{
			u64 i2 = i + 1;
			if(i2 == sz) i2=0;

			auto cv1 = (ControlVertex*)cverts[ cvs[ i ] ];
			auto cv2 = (ControlVertex*)cverts[ cvs[ i2 ] ];

			auto v1 = (Vertex*)cv1->m_verts[0];
			auto v2 = (Vertex*)cv2->m_verts[0];

			auto d = ray->distanceToLine( math::mul( v1->m_Position, M) + pivot, math::mul( v2->m_Position, M ) + pivot );

			d = d / (1.f * (result.m_T+1.f));

			//printf("d [%f]\n", d);
			((Scene3DObject*)result.m_object)->m_isObjectHaveSelectedEdges = false;

			if( d < 0.005f )
			{
				if( ks == AppState_keyboard::Alt )
				{
					auto pos = std::find(cv1->m_edgeWith.begin(), cv1->m_edgeWith.end(), cv2);
					if( pos != cv1->m_edgeWith.end() )
					{
						cv1->m_edgeWith.erase(pos);
						if( !cv1->m_edgeWith.size() ) cv1->m_isSelected_edge = false;
					}

					pos = std::find(cv2->m_edgeWith.begin(), cv2->m_edgeWith.end(), cv1);
					if( pos != cv2->m_edgeWith.end() )
					{
						cv2->m_edgeWith.erase(pos);
						if( !cv2->m_edgeWith.size() ) cv2->m_isSelected_edge = false;
					}
				}
				else
				{
					if( std::find(cv1->m_edgeWith.begin(), cv1->m_edgeWith.end(), cv2) == cv1->m_edgeWith.end() )
						cv1->m_edgeWith.push_back(cv2); 
					if( std::find(cv2->m_edgeWith.begin(), cv2->m_edgeWith.end(), cv1) == cv2->m_edgeWith.end() )
						cv2->m_edgeWith.push_back(cv1);

					//printf("%llu\n", cv1->m_edgeWith.size());
					cv1->m_isSelected_edge = true;
					cv2->m_isSelected_edge = true;
				}
			//	printf("select\n");

				((Scene3DObject*)result.m_object)->updateEdgeModel();
				((Scene3DObject*)result.m_object)->m_isObjectHaveSelectedEdges = true;

				_updateSelectionAabb_edge();
				updateObjectEdgeSelectList();
				return true;
			}
		}
	}

	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	{
		deselectAll();
	}

	return false;
}

void Scene3D::_deselectAll_poly(Scene3DObject* object)
{
	auto & cverts = object->GetControlVertexArray();
	for( u64 i = 0, sz = cverts.size(); i < sz; ++i )
	{
		auto CV = (ControlVertex*)cverts[i];
		if(CV->m_isSelected_poly)
			CV->m_isSelected_poly = false;
		CV->m_selectedPolysCounter = 0;
	}
		
	auto & polys = object->GetPolygonArray();
	for( u64 i = 0, sz = polys.size(); i < sz; ++i )
	{
		polys[i]->Deselect();
	}

	object->updatePolygonModel();
	object->m_isObjectHaveSelectedPolys = false;

	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_deselectAll_edge(Scene3DObject* object)
{
	object->m_isObjectHaveSelectedEdges = false;
	auto & cverts = object->GetControlVertexArray();
	for( u64 i = 0, sz = cverts.size(); i < sz; ++i )
	{
		if( ((ControlVertex*)cverts[i])->m_edgeWith.size() )
			((ControlVertex*)cverts[i])->m_edgeWith.clear();
		((ControlVertex*)cverts[i])->m_isSelected_edge = false;
	}
	object->updateEdgeModel();

	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();
}

void Scene3D::selectPolygons(kkRay* ray)
{
	if(!m_objects_selected.size()) return;
	if( !ray ) return;

	std::vector<kkRayTriangleIntersectionResultSimple> result;

	for( auto * o : m_objects_selected )
	{
		o->IsRayIntersectMany(*ray, result);
	}
	
	auto ks = m_app->getStateKeyboard();

	if(!result.size())
	{
		if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
			deselectAll();
		_updateSelectionAabb_polygon();
		updateObjectPolySelectList();
		return;
	}

	std::sort( result.begin(),result.end(),
		[](const kkRayTriangleIntersectionResultSimple& first, const kkRayTriangleIntersectionResultSimple& second)
	{
		return first.m_T < second.m_T;
	} );

	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	auto object = (Scene3DObject*)result[0].m_object;
	auto firstPolygon = result[0].m_object->GetPolygon( result[0].m_polygonIndex );
	auto & cvinds = firstPolygon->GetControlVertInds();
	auto & cverts  = result[0].m_object->GetControlVertexArray();

	object->m_isObjectHaveSelectedPolys = false;
	if( ks == AppState_keyboard::Alt )
	{
		for( u64 i = 0, sz = cvinds.size(); i < sz; ++i )
		{
			auto CV = ((ControlVertex*)cverts[cvinds[i]]);
			
			--CV->m_selectedPolysCounter;

			// больше нет полигонов которые бы работали с этой вершиной
			// значит можно снять выделение
			if( CV->m_selectedPolysCounter == 0 )
				CV->m_isSelected_poly = false;
			//CV->m_selectedPolys.erase();
		}
		firstPolygon->Deselect();
	}
	else
	{
		for( u64 i = 0, sz = cvinds.size(); i < sz; ++i )
		{
			auto CV = ((ControlVertex*)cverts[cvinds[i]]);
			CV->m_isSelected_poly = true;

			// когда выделяется полигон, нужно дать знать контрольной вершине что это было сделано
			// это даст возможность трансформировать вершину(это-же сделано и с рёбрами)
			// если выбрано 2 полигона, имеющих общую вершину, то при снятии выделения одного, приведёт к потере возможности трансформации
			// по этому вершина должна хранить индексы полигонов. возможно проще добавить счётчик
			//CV->m_selectedPolys.emplace_back(result[0].m_polygonIndex);
			++CV->m_selectedPolysCounter;
		}
		firstPolygon->Select();
		object->m_isObjectHaveSelectedPolys = true;
	}
	((Scene3DObject*)result[0].m_object)->updatePolygonModel();
	
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}