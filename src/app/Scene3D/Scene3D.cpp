#include "kkrooo.engine.h"
#include "../Common.h"

#include "Common/kkUtil.h"
#include "Classes/Math/kkMath.h"

#include "Geometry/kkPolygonalModel.h"

#include "../CursorRay.h"
#include "../SelectionFrust.h"

#include "../Viewport/Viewport.h"
#include "../Viewport/ViewportCamera.h"
#include "../Viewport/ViewportOptimizations.h"
#include "../Geometry/PolygonalModel.h"
#include "../Application.h"
#include "../ApplicationState.h"
#include "../Gizmo.h"
#include "Scene3D.h"
#include "Scene3DObject.h"

#include <algorithm>

extern ViewportMouseState g_mouseState;

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
	if(m_objects_selected.size())
		return m_objects_selected[i];
	return nullptr;
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
	//auto ks = m_app->getStateKeyboard();
	//if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//	deselectAll();
	//
	//for( auto * obj : drawobjects )
	//{
	//	bool found = false;
	//	for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
	//	{
	//		auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
	//		for( u64 o = 0, sz2 = polygon->m_verts.size(); o < sz2; ++o )
	//		{
	//			u64 o2 = o + 1;
	//			if(o2 == sz2) o2=0;
	//			auto cv1 = (ControlVertex*)((Vertex*)polygon->m_verts[o])->m_controlVertex;
	//			auto cv2 = (ControlVertex*)((Vertex*)polygon->m_verts[o2])->m_controlVertex;
	//			auto V1 = cv1->m_verts[0];
	//			auto V2 = cv2->m_verts[0];
	//			if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
	//			{
	//				found = true;
	//				if( ks == AppState_keyboard::Alt )
	//				{
	//					_deselectObject_object( obj );
	//				}
	//				else
	//				{
	//					_selectObject_object( obj );
	//				}
	//				goto next_object;
	//			}
	//		}
	//	}
	//next_object:;

	//	// try ray-triangle
	//	if(!found)
	//	{
	//		kkRay ray[4];
	//		ray[0].m_origin = frust.m_top[0];
	//		ray[0].m_end = frust.m_top[3];
	//		ray[0].update();
	//		ray[1].m_origin = frust.m_top[1];
	//		ray[1].m_end = frust.m_top[2];
	//		ray[1].update();
	//		ray[2].m_origin = frust.m_bottom[0];
	//		ray[2].m_end = frust.m_bottom[3];
	//		ray[2].update();
	//		ray[3].m_origin = frust.m_bottom[1];
	//		ray[3].m_end = frust.m_bottom[2];
	//		ray[3].update();
	//		for(s32 i = 0; i < 4; ++i)
	//		{
	//			std::vector<kkRayTriangleIntersectionResultSimple> results;
	//			if( obj->IsRayIntersectMany(ray[i], results) )
	//			{
	//				if( ks == AppState_keyboard::Alt )
	//					_deselectObject_object( obj );
	//				else
	//					_selectObject_object( obj );
	//				break;
	//			}
	//		}
	//	}
	//}
}

void Scene3D::_selectObjectsByRectangle_poly( const SelectionFrust& frust )
{
	//auto ks = m_app->getStateKeyboard();
	//if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//	deselectAll();

	//for( auto * obj : m_objects_selected )
	//{
	//	obj->m_isObjectHaveSelectedPolys = false;

	//	bool need_update = false;
	//	for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
	//	{
	//		auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
	//		for( u64 o = 0, sz2 = polygon->m_verts.size(); o < sz2; ++o )
	//		{
	//			u64 o2 = o + 1;
	//			if(o2 == sz2) o2=0;
	//			auto cv1 = (ControlVertex*)((Vertex*)polygon->m_verts[o])->m_controlVertex;
	//			auto cv2 = (ControlVertex*)((Vertex*)polygon->m_verts[o2])->m_controlVertex;

	//			auto V1 = cv1->m_verts[0];
	//			auto V2 = cv2->m_verts[0];

	//			if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
	//			{
	//				need_update = true;

	//				if( ks == AppState_keyboard::Alt )
	//				{
	//					polygon->Deselect();
	//				}
	//				else
	//				{
	//					obj->m_isObjectHaveSelectedPolys = true;
	//					polygon->Select();
	//				}
	//				goto next_polygon;
	//			}
	//		}
	//	next_polygon:;
	//	}
	//	if(!obj->m_isObjectHaveSelectedPolys)
	//	{
	//		for(u64 i = 0, sz = obj->m_PolyModel->m_polygons.size(); i < sz; ++i )
	//		{
	//			auto polygon = (Polygon3D *)obj->m_PolyModel->m_polygons.at(i);
	//			if(polygon->IsSelected())
	//			{
	//				obj->m_isObjectHaveSelectedPolys = true;
	//				break;
	//			}
	//		}
	//	}

	//	// надо попробовать послать лучи по углам выделенной рамки
	//	if(!need_update)
	//	{
	//		kkRay ray[4];
	//		ray[0].m_origin = frust.m_top[0];
	//		ray[0].m_end = frust.m_top[3];
	//		ray[0].update();
	//		ray[1].m_origin = frust.m_top[1];
	//		ray[1].m_end = frust.m_top[2];
	//		ray[1].update();
	//		ray[2].m_origin = frust.m_bottom[0];
	//		ray[2].m_end = frust.m_bottom[3];
	//		ray[2].update();
	//		ray[3].m_origin = frust.m_bottom[1];
	//		ray[3].m_end = frust.m_bottom[2];
	//		ray[3].update();
	//		for(s32 i = 0; i < 4; ++i)
	//		{
	//			std::vector<kkRayTriangleIntersectionResultSimple> results;
	//			if( obj->IsRayIntersectMany(ray[i], results) )
	//			{
	//				/*std::sort( result.begin(),result.end(),
	//					[](const kkRayTriangleIntersectionResultSimple& first, const kkRayTriangleIntersectionResultSimple& second)
	//				{
	//					return first.m_T < second.m_T;
	//				} );*/
	//				
	//				for( auto result : results )
	//				{
	//					auto polygon = (Polygon3D*)obj->m_PolyModel->m_polygons[result.m_polygonIndex];
	//				
	//					if( ks == AppState_keyboard::Alt )
	//					{

	//						polygon->Deselect();
	//					}
	//					else
	//					{
	//						obj->m_isObjectHaveSelectedPolys = true;
	//						polygon->Select();
	//					}
	//				}

	//				need_update = true;
	//				break;
	//			}
	//		}
	//	}

	//	if( need_update )
	//	{
	//		obj->m_PolyModel->updateCVForPolygonSelect();
	//		obj->updatePolygonModel();

	//		_updateSelectionAabb_polygon();
	//		updateObjectPolySelectList();
	//	}
	//}
}

void Scene3D::_selectObjectsByRectangle_edge( const SelectionFrust& frust )
{
	/*auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( auto * obj : m_objects_selected )
	{
		bool need_update = false;
		for(u64 i = 0, sz = obj->m_PolyModel->m_edges.size(); i < sz; ++i )
		{
			auto E = obj->m_PolyModel->m_edges[i];
			auto cv1 = E->m_firstPoint;
			auto cv2 = E->m_secondPoint;
			auto V1 = cv1->m_verts[0];
			auto V2 = cv2->m_verts[0];
			if( frust.lineInFrust( math::mul( V1->getPosition(), obj->GetMatrix()) + obj->GetPivot() , math::mul( V2->getPosition(), obj->GetMatrix()) + obj->GetPivot() ) )
			{
				need_update = true;
				if( ks == AppState_keyboard::Alt )
				{
					E->m_isSelected = false;
				}
				else
				{
					E->m_isSelected = true;
				}
			}
		}
		if( need_update )
		{
			((Scene3DObject*)obj)->m_PolyModel->updateCVEdgeWith();
			((Scene3DObject*)obj)->updateEdgeModel();
			((Scene3DObject*)obj)->m_isObjectHaveSelectedEdges = true;

			_updateSelectionAabb_edge();
			updateObjectEdgeSelectList();
		}
	}*/
}

void Scene3D::_selectObjectsByRectangle_vertex( /*const v4i& r*/ const SelectionFrust& frust )
{
	/*auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();
	
	for( auto object : m_objects_selected )
	{
		object->m_isObjectHaveSelectedVerts = false;
		auto & cverts = object->GetControlVertexArray();
		auto & verts = object->GetVertexArray();

		for( auto CV : cverts )
		{
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
	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();*/
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
	/*_deselectAll_edge(object);
	for(u64 i = 0, sz = object->m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = object->m_PolyModel->m_edges[i];
		E->m_isSelected = true;
	}
	object->m_isObjectHaveSelectedEdges = true;
	object->m_PolyModel->updateCVEdgeWith();
	object->updateEdgeModel();
	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();*/
}

void Scene3D::_selectAll_poly(Scene3DObject* object)
{
	/*auto & cverts = object->GetControlVertexArray();
		
	auto & polys = object->GetPolygonArray();
	for( u64 i = 0, sz = polys.size(); i < sz; ++i )
	{
		auto P = polys[i];
		P->Select();
			
	}

	object->m_PolyModel->updateCVForPolygonSelect();
	object->updatePolygonModel();
	object->m_isObjectHaveSelectedPolys = true;

	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();*/
}

void Scene3D::_selectAll_vertex(Scene3DObject* object)
{
	/*for( u64 i = 0, sz = object->m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		object->m_PolyModel->m_controlVerts[ i ]->select();
	}

	object->m_isObjectHaveSelectedVerts = true;
	object->updateModelPointsColors();

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();*/
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
	/*for( u64 i = 0, sz = object->m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		object->m_PolyModel->m_controlVerts[ i ]->deselect();
	}
	object->m_isObjectHaveSelectedVerts = false;
	object->updateModelPointsColors();
	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();*/
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
	//auto & cverts = object->GetControlVertexArray();

	//// надо сохранить не выделенные полигоны
	//std::vector<kkPolygon*> not_selected_polys;
	//for( u64 i = 0, sz = object->m_PolyModel->m_polygons.size(); i < sz; ++i )
	//{
	//	auto P = object->m_PolyModel->m_polygons[i];
	//	if(!P->IsSelected())
	//		not_selected_polys.emplace_back(P);
	//}
	//	
	//_deselectAll_poly(object);	

	//object->m_isObjectHaveSelectedPolys = false;
	//for(auto P : not_selected_polys)
	//{
	//	object->m_isObjectHaveSelectedPolys = true;
	//	P->Select();
	//}

	//object->m_PolyModel->updateCVForPolygonSelect();
	//object->updatePolygonModel();
	//

	//_updateSelectionAabb_polygon();
	//updateObjectPolySelectList();
}

void Scene3D::_selectInvert_edge(Scene3DObject* object)
{
	/*std::unordered_set<Edge*> edges_to_select;
	for(u64 i = 0, sz = object->m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = object->m_PolyModel->m_edges[i];
		if(!E->m_isSelected)
			edges_to_select.insert(E);
	}
	_deselectAll_edge(object);
	for(auto E : edges_to_select)
	{
		object->m_isObjectHaveSelectedEdges = true;
		E->m_isSelected = true;
	}
	object->m_PolyModel->updateCVEdgeWith();
	object->updateEdgeModel();

	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();*/
}

void Scene3D::_selectInvert_vertex(Scene3DObject* object)
{
	/*object->m_isObjectHaveSelectedVerts = false;

	for( u64 i = 0, sz = object->m_PolyModel->m_controlVerts.size(); i < sz; ++i )
	{
		if( object->m_PolyModel->m_controlVerts[ i ]->isSelected() )
			object->m_PolyModel->m_controlVerts[ i ]->deselect();
		else
		{
			object->m_PolyModel->m_controlVerts[ i ]->select();
			object->m_isObjectHaveSelectedVerts = true;
		}
	}
	object->updateModelPointsColors();

	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();*/
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
		updateObjectEdgeSelectList();
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
	/*if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_controlVerts.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)o->m_PolyModel->m_controlVerts[ i ];

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
	}*/
}

void Scene3D::_updateSelectionAabb_edge()
{
	/*if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_edges.size(); i < sz; ++i )
		{
			auto E = o->m_PolyModel->m_edges[i];
			if(E->m_isSelected)
			{
				nnn = false;
				auto V = (Vertex*)E->m_firstPoint->m_verts[0];
				m_selectionAabb.add( math::mul( V->m_Position, M ) + o->GetPivot() );
				V = (Vertex*)E->m_secondPoint->m_verts[0];
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
	}*/
}

void Scene3D::_updateSelectionAabb_polygon()
{
	/*if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;

	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;

		for( u64 i = 0, sz = o->m_PolyModel->m_controlVerts.size(); i < sz; ++i )
		{
			auto CV = (ControlVertex*)o->m_PolyModel->m_controlVerts[ i ];

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
	}*/
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
//	auto em = m_app->getEditMode();
//
//	bool blender_style = true;
//
//	static f32 first_x = 0, first_y = 0, first_z = 0;
//	static f32 toolTip_angle_x = 0.f, toolTip_angle_y = 0.f, toolTip_angle_z = 0.f;
//	static kkStringA toolTip;
//
//	static kkVector4 C;
//	if( first )
//		m_selectionAabb.center(C);
//
//	f32 x = 0, y = 0, z = 0;
//
//	auto camera  = m_app->m_active_viewport->getCamera();
//
//	auto p1 = m_app->m_currentGizmoEvent.point2D;
//	auto p2 = m_app->m_cursor_position;
//	auto p3 = p2 - p1;
//	v2f point((float)p3.x,(float)p3.y);
//
//	if( e.part == AppEvent_gizmo::_part::_x )
//	{
//		if( blender_style )
//		{
//			x = (f32)std::atan2((f32)point.x, (f32)point.y);
//
//			if( first )
//			{
//				first_x = x;
//			}
//
//			x -= first_x;
//		}
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_y )
//	{
//		if( blender_style )
//		{
//			y = (f32)std::atan2((f32)point.x, (f32)point.y);
//			if( first )
//			{
//				first_y = y;
//			}
//
//			y -= first_y;
//		}
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_z )
//	{
//		if( blender_style )
//		{
//			z = (f32)std::atan2((f32)point.x, (f32)point.y);
//			if( first )
//			{
//				first_z = z;
//			}
//
//			z -= first_z;
//		}
//
//		
//	}
//
//	kkMatrix4 M;
//	float deg_f = 0.f;
//
//	if( e.part == AppEvent_gizmo::_part::_screen_plane )
//	{
//		auto vprc = m_app->m_active_viewport->getRect();
//		auto vpsz = vprc.getWidthAndHeight();
//
//		auto p1 = v2i( (vpsz.x/2.f)+vprc.x,(vpsz.y/2.f)+vprc.y);
//		auto p2 = m_app->m_cursor_position;
//		auto point = p2 - p1;
//
//		x = (f32)std::atan2((f32)point.x, (f32)point.y);
//		if( first )
//		{
//			first_x = x;
//		}
//
//		x -= first_x;
//
//
//		if( m_app->m_state_keyboard == AppState_keyboard::Shift )
//		{
//			int deg = (int)math::radToDeg( x );
//			if( deg % 5 != 0 )
//				deg -= deg % 5;
//				deg_f = (float)deg;
//		}
//		else
//		{
//			deg_f = math::radToDeg( x );
//		}
//
//		m_app->m_active_viewport->getActiveViewportCamera()->setObjectRotationAngle(-math::degToRad((float)deg_f));
//
//		m_app->m_active_viewport->getActiveViewportCamera()->update();
//		M = m_app->m_active_viewport->getActiveViewportCamera()->getObjectRotationMatrix();
//	}
//
//	//printf("%f %f\n", x, y);
//	auto cameraRotation = m_app->m_active_viewport->getActiveViewportCamera()->getAllRotation();
//	//printf("%f\n", cameraRotation.y );
//
//
//	if( e.part == AppEvent_gizmo::_part::_x )
//	{
//		if( blender_style )
//		{
//			static float last_x =  0.f;
//			if( x > 0.f )
//			{
//				x = -(math::PI+math::PI) + x;
//			}
//			toolTip_angle_x += -x - last_x;
//			last_x = -x;
//
//
//			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
//			{
//				int   deg_i = (int)math::radToDeg( std::abs( toolTip_angle_x ));
//				if( deg_i % 5 != 0 )
//					deg_i -= (deg_i % 5);
//				deg_f = (float)deg_i;
//			}
//			else
//			{
//				deg_f = math::radToDeg( std::abs( toolTip_angle_x ));
//			}
//
//			if( cameraRotation.y < 0.f )
//			{
//				kkQuaternion Q( -math::degToRad((float)deg_f),0.f,0.f);
//				math::makeRotationMatrix(M,Q);
//			}
//			else	
//			{
//				kkQuaternion Q( math::degToRad((float)deg_f),0.f,0.f);
//				math::makeRotationMatrix(M,Q);
//			}
//		}
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_y )
//	{
//		if( blender_style )
//		{
//			static float last_y =  0.f;
//
//			if( y > 0.f )
//			{
//				y = -(math::PI+math::PI) + y;
//			}
//			toolTip_angle_y += -y - last_y;
//			last_y = -y;
//
//
//			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
//			{
//				int deg_i = (int)math::radToDeg( std::abs( toolTip_angle_y ));
//				if( deg_i % 5 != 0 )
//					deg_i -= (deg_i % 5);
//
//				deg_f = (float)deg_i;
//			}
//			else
//			{
//				deg_f = math::radToDeg( std::abs( toolTip_angle_y ));
//			}
//
//			if( cameraRotation.x > 90.f || cameraRotation.x < -90.f  )
//			{
//				kkQuaternion Q( 0.f,-math::degToRad((float)deg_f),0.f);
//				math::makeRotationMatrix(M,Q);
//			}
//			else	
//			{
//				kkQuaternion Q( 0.f,math::degToRad((float)deg_f),0.f);
//				math::makeRotationMatrix(M,Q);
//			}
//		}
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_z )
//	{
//		if( blender_style )
//		{
//			static float last_z =  0.f;
//
//			if( z > 0.f )
//			{
//				z = -(math::PI+math::PI) + z;
//			}
//			toolTip_angle_z += -z - last_z;
//			last_z = -z;
//			
//
//			if( m_app->m_state_keyboard == AppState_keyboard::Shift )
//			{
//				int deg = (int)math::radToDeg( std::abs( toolTip_angle_z ));
//				if( deg % 5 != 0 )
//					deg -= deg % 5;
//				deg_f = (float)deg;
//			}
//			else
//			{
//				deg_f = math::radToDeg( std::abs( toolTip_angle_z ));
//			}
//
//			if( cameraRotation.y < 90.f && cameraRotation.y > -90.f )
//			{
//				kkQuaternion Q( 0.f,0.f, math::degToRad((float)deg_f) );
//				math::makeRotationMatrix(M,Q);
//			}
//			else	
//			{
//				kkQuaternion Q( 0.f,0.f, -math::degToRad((float)deg_f) );
//				math::makeRotationMatrix(M,Q);
//			}
//		}
//	}
//
//	kkVector4 pivotFix;
//
//	auto num = m_objects_selected.size();
//
//	if( startStop )
//	{
//		switch (em)
//		{
//		case EditMode::Object:
//		{
//			auto M2 = M;
//			for( auto * o : m_objects_selected )
//			{
//				o->m_matrix = M * o->m_matrixFixed;
//
//				o->m_matrixOnlyRotation = M * o->m_matrixOnlyRotationFixed;
//
//				// global
//				if( m_app->m_isLocalRotation == false && num > 1 )
//				{
//					pivotFix = o->GetPivotFixed();
//					auto & pivot  = o->GetPivot();
//					pivot = math::mul(pivotFix-C,M2)+C;
//				}
//
////				o->updateMatrixPosition();
//				o->UpdateAabb();
//			}
//		}
//			break;
//		case EditMode::Vertex:
//		{
//			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsVertexSelectInfo[ i ];
//				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
//			}
//		}
//			break;
//		case EditMode::Edge:
//			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsEdgeSelectInfo[ i ];
//				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
//			}
//			break;
//		case EditMode::Polygon:
//			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsPolySelectInfo[ i ];
//				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
//			}
//			break;
//		default:
//			break;
//		}
//		
//	}
//	else
//	{
//		switch (em)
//		{
//		case EditMode::Object:
//		{
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//				{
//					// global
//					if( m_app->m_isLocalRotation  == false && num > 1 )
//					{
//						o->RestorePivot();
//					}
//
//					o->m_matrix = o->m_matrixFixed;
//					o->m_matrixOnlyRotation = o->m_matrixOnlyRotationFixed;
//
//					o->UpdateAabb();
//				}
//				else
//				{
//					// global
//					if( m_app->m_isLocalRotation  == false && num > 1 )
//					{
//						o->ApplyPivot();
//					}
//
//					o->m_matrixFixed = o->m_matrix;
//					o->m_matrixOnlyRotationFixed = o->m_matrixOnlyRotation;
//				}
//			
//			//	updateObject2DPoints(o);
//			}
//		}
//			break;
//		case EditMode::Edge:
//		case EditMode::Vertex:
//		case EditMode::Polygon:
//		{
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//				{
//					o->RestorePosition();
//				}
//				else
//				{
//					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
//					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
//				}
//			//	updateObject2DPoints(o);
//			}
//		}
//			break;
//		//case EditMode::Edge:
//		//	break;
//		//case EditMode::Polygon:
//		//	break;
//		default:
//			break;
//		}
//		
//		updateSelectionAabb();
//		updateSceneAabb();
//	}
}

void Scene3D::scaleSelectedObjects(bool startStop, const AppEvent_gizmo& e, bool cancel,bool first )
{
//	kkVector4 pivotFix;
//
//	f32 x = 0, y = 0, z = 0;
//
//	auto r1 = m_app->m_cursorRayFirstClick.m_origin;
//	auto r2 = m_app->m_cursorRayCurrent.m_origin;
//
//	auto camera  = m_app->m_active_viewport->getCamera();
//
//	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
//	// но на самом деле это мир идёт ВВЕРХ, плюс Y
//	auto cam_pos = -camera->getPositionInSpace();
//	
//
//	if( e.part == AppEvent_gizmo::_part::_x )
//	{
//		x = (r2-r1).KK_X;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_y )
//	{
//		y = (r2-r1).KK_Y;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_z )
//	{
//		z = (r2-r1).KK_Z;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_xy_plane )
//	{
//		x = (r2-r1).KK_X;
//		y = (r2-r1).KK_Y;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_xz_plane )
//	{
//		x = (r2-r1).KK_X;
//		z = (r2-r1).KK_Z;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_zy_plane )
//	{
//		y = (r2-r1).KK_Y;
//		z = (r2-r1).KK_Z;
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_screen_plane )
//	{
//		x = (r2-r1).KK_X;
//		z = (r2-r1).KK_X;
//		y = (r2-r1).KK_X;
//	}
//
//	//printf("%f %f\n", x, y);
//
//	//auto D = 5000.f; // должно быть значение far камеры
//
//	static kkVector4 C;
//	if( first )
//		m_selectionAabb.center(C);
//
//	f32 L = 1.f;
//	if( m_app->m_active_viewport->isPerspective() )
//	{
//		L = 500.0f;
//	}
//	
//	x = x * L;
//	y = y * L;
//	z = z * L;
//
//	//printf("%f\n", x );
//	// здесь попробую сделать так, чтобы уменьшение масштаба было не линейным
//	if(x<0.f){ x = x * 1.f / (1.f-x); }
//	if(y<0.f){ y = y * 1.f / (1.f-y); }
//	if(z<0.f){ z = z * 1.f / (1.f-z); }
//
//	x += 1.f;
//	y += 1.f;
//	z += 1.f;
//
//	if( x <= 0.f ) x = FLT_MIN;
//	if( y <= 0.f ) y = FLT_MIN;
//	if( z <= 0.f ) z = FLT_MIN;
//	
//	//printf("%f\n", m_gizmo->m_scaleSize.KK_X );
//
//	if( startStop )
//	{
//		m_gizmo->m_scaleSize.KK_X  = x-1.f;
//		m_gizmo->m_scaleSize.KK_Y  = y-1.f;
//		m_gizmo->m_scaleSize.KK_Z  = z-1.f;
//	}
//	else
//	{
//		m_gizmo->m_scaleSize.KK_X  = 0;
//		m_gizmo->m_scaleSize.KK_Y  = 0;
//		m_gizmo->m_scaleSize.KK_Z  = 0;
//	}
//
//	kkMatrix4 S;
//	math::makeScaleMatrix(kkVector4(x,y,z,1.f),S);
//
//	////printf("%f\n", L );
//
//	kkMatrix4 M;
//
//	auto em = m_app->getEditMode();
//
//	if( startStop )
//	{
//		switch (em)
//		{
//		case EditMode::Object:
//		{
//			auto n = m_objects_selected.size();
//			for( auto * o : m_objects_selected )
//			{
//				//o->m_scaleMatrix = S;
//				o->m_matrix = S * o->m_matrixFixed;
//
//				// global
//				if( n == 1)
//				{
//					pivotFix = o->GetPivotFixed();
//					auto & pivot  = o->GetPivot();
//					pivot = math::mul(pivotFix-pivotFix,S)+pivotFix;
//				}
//				else if( m_app->m_isLocalScale  == false )
//				{
//					pivotFix = o->GetPivotFixed();
//					auto & pivot  = o->GetPivot();
//					pivot = math::mul(pivotFix-C,S)+C; // для глобал нужно C
//				}
//
//
////				o->updateMatrixPosition();
//				o->UpdateAabb();
//			}
//		}
//			break;
//		case EditMode::Vertex:
//		{
//			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsVertexSelectInfo[ i ];
//				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
//			}
//		}
//			break;
//		case EditMode::Edge:
//			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsEdgeSelectInfo[ i ];
//				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
//			}
//			break;
//		case EditMode::Polygon:
//			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
//			{
//				auto info = &m_objectsPolySelectInfo[ i ];
//				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
//			}
//			break;
//		default:
//			break;
//		}
//		
//	}
//	else
//	{
//		switch (em)
//		{
//		case EditMode::Object:
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//				{
//					// global
//					if( m_app->m_isLocalScale  == false )
//					{
//						o->RestorePivot();
//					}
//
//					o->m_matrix      = o->m_matrixFixed;
//					//o->m_scaleMatrix = o->m_scaleMatrixFixed;
////					o->updateMatrixPosition();
//					o->UpdateAabb();
//				}
//				else
//				{
//					// global
//					if( m_app->m_isLocalScale  == false )
//					{
//						o->ApplyPivot();
//					}
//
//					o->m_matrixFixed = o->m_matrix;
//				}
//			//	updateObject2DPoints(o);
//			}
//			break;
//		case EditMode::Edge:
//		case EditMode::Vertex:
//		case EditMode::Polygon:
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//				{
//					o->RestorePosition();
//				}
//				else
//				{
//					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
//				//	o->generateNormals();
//					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
//				}
//			//	updateObject2DPoints(o);
//			}
//			break;
//		//case EditMode::Edge:
//		//	break;
//		//case EditMode::Polygon:
//		//	break;
//		default:
//			break;
//		}
//		
//		updateSelectionAabb();
//		updateSceneAabb();
//	}
}

void Scene3D::moveSelectedObjects(bool startStop, const AppEvent_gizmo& e, bool cancel,bool first )
{
//	auto em = m_app->getEditMode();
//
//	kkVector4 pivotFix;
//
//	static f32 x = 0, y = 0, z = 0;
//
//	// старая версия. Проблема была в том, что 2д точки получаются корректными только когда они видны
//	// если точка уходит за пределы видимости, значения получаются гигантскими и модель улетает.
//	// замена на обычные координаты курсора
//	// старая версия подходит только для масштабирования
//	// так-же пока она будет использоваться для перемещения в плоскости экрана
//	auto r1 = m_app->m_cursorRayFirstClick.m_origin;
//	auto r2 = m_app->m_cursorRayCurrent.m_origin;
//
//	static v2i p1 = m_app->m_cursor_position;
//	auto p2 = m_app->m_cursor_position;
//	
//	//printf("%i %i\n",p2.x,p2.y);
//
//	if( first )
//	{
//		p1 = p2;
//		x = y = z = 0;
//	}
//
//	auto point = p2 - p1;
//
//	p1 = p2;
//
//	auto camera  = m_app->m_active_viewport->getCamera();
//
//	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
//	// но на самом деле это мир идёт ВВЕРХ, плюс Y
//	auto cam_pos = -camera->getPositionInSpace();
//	
//	auto cameraRotation = m_app->m_active_viewport->getActiveViewportCamera()->getAllRotation();
//
//	if( e.part == AppEvent_gizmo::_part::_x || e.part == AppEvent_gizmo::_part::_xy_plane || e.part == AppEvent_gizmo::_part::_xz_plane )
//	{
//		x += point.x * std::cos( cameraRotation.y / 180.f * math::PI );
//		x += point.y * std::sin( cameraRotation.y / 180.f * math::PI );
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_y || e.part == AppEvent_gizmo::_part::_xy_plane || e.part == AppEvent_gizmo::_part::_zy_plane)
//	{
//		if( cameraRotation.x < 0.f )
//			y += -point.y;
//		else
//			y += point.y;
//	}
//	
//	if( e.part == AppEvent_gizmo::_part::_z || e.part == AppEvent_gizmo::_part::_zy_plane || e.part == AppEvent_gizmo::_part::_xz_plane )
//	{
//		z -= point.x * std::sin( cameraRotation.y / 180.f * math::PI );
//		z += point.y * std::cos( cameraRotation.y / 180.f * math::PI );
//	}
//
//	if( e.part == AppEvent_gizmo::_part::_screen_plane )
//	{
//	//	printf("m");
//		if( m_app->m_active_viewport->isPerspective() )
//		{
//			const f32 s = 1.f;
//			z = (r2-r1).KK_Z * s;
//			x = (r2-r1).KK_X * s;
//			y = (r2-r1).KK_Y * s;
//		}
//		else
//		{
//			z = (r2-r1).KK_Z;
//			x = (r2-r1).KK_X;
//			y = (r2-r1).KK_Y;
//		}
//	}
//
//	//printf("%f %f\n", r1.KK_X, r2.KK_X);
//
//	auto D = 5000.f; // должно быть значение far камеры
//
//	static kkVector4 C;
//	if( first )
//		m_selectionAabb.center(C);
//
//	f32 L = 1.f;
//	if( m_app->m_active_viewport->isPerspective() )
//	{
//		if( e.part == AppEvent_gizmo::_part::_screen_plane )
//			D *= 0.00001f; // делаю чтобы было побыстрее
//		else
//			D *= 0.15f;
//
//		L = C.distance(cam_pos);
//		if( L == 0.f )
//			L += 0.01f;
//
//		L /= D;
//	}
//	else
//	{
//		if( e.part != AppEvent_gizmo::_part::_screen_plane )
//		{
//			auto vpcam = m_app->m_active_viewport->getActiveViewportCamera();
//			L /= vpcam->getZoomOrt() * 100.f;
//		}
//	}
//	
//	//printf("%f\n", L );
//
//	ObjectVertexSelectInfo * info;
//	auto V = kkVector4(x*L,y*L,z*L);
//	if( startStop )
//	{
//		switch (em)
//		{
//		case EditMode::Object:
//			for( auto * o : m_objects_selected )
//			{
//				pivotFix = o->GetPivotFixed();
//				auto & pivot  = o->GetPivot();
//
//				pivot = pivotFix + V;
//
////				o->updateMatrixPosition();
//				o->UpdateAabb();
//			}
//			break;
//		case EditMode::Vertex:
//			//printf("%llu\n",m_objectsVertexSelectInfo.size());
//			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
//			{
//				info = &m_objectsVertexSelectInfo[ i ];
//				info->m_object->moveVerts( V,info->m_verts);
//			}
//			break;
//		case EditMode::Edge:
//			for( u64 i = 0, sz = m_objectsEdgeSelectInfo.size(); i < sz; ++i )
//			{
//				info = &m_objectsEdgeSelectInfo[ i ];
//				info->m_object->moveVerts( V,info->m_verts);
//			}
//			break;
//		case EditMode::Polygon:
//			for( u64 i = 0, sz = m_objectsPolySelectInfo.size(); i < sz; ++i )
//			{
//				info = &m_objectsPolySelectInfo[ i ];
//				info->m_object->moveVerts( V,info->m_verts);
//			}
//			break;
//		default:
//			break;
//		}
//	}
//	else
//	{
//		// нажимается правой кнопкой. Левая кнопка может быть ещё нажата
//		if( cancel )
//		{
//			m_app->m_state_app = AppState_main::CancelTransformation;
//		}
//
//		switch (em)
//		{
//		case EditMode::Object:
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//				{
//					o->RestorePivot();
//					o->UpdateAabb();
//				}
//				else
//				{
//					o->ApplyPivot();
//				}
//			//	updateObject2DPoints(o);
//			}
//			break;
//		case EditMode::Edge:
//		case EditMode::Vertex:
//		case EditMode::Polygon:
//			for( auto * o : m_objects_selected )
//			{
//				if( cancel )
//					o->RestorePosition();
//				else
//				{
//					o->ApplyPosition(); // метод так-же обновит оригинальный AABB
//					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
//				}
//			//	updateObject2DPoints(o);
//			}
//			break;
//		/*case EditMode::Edge:
//			break;*/
//		//case EditMode::Polygon:
//		//	break;
//		default:
//			break;
//		}
//		
//	}
//    
//	updateSelectionAabb();
//	updateSceneAabb();
}


bool Scene3D::isVertexHover(const SelectionFrust& frust)
{
	//for( auto * object : m_objects_selected )
	//{
	//	auto & cverts = object->GetControlVertexArray();
	//	auto & verts = object->GetVertexArray();
	//	for( auto CV : cverts )
	//	{
	//		//auto & vert_inds = CV->getVertInds();
	//		auto & verts = CV->getVerts();
	//		auto V = verts[0];
	//		if( frust.pointInFrust(math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot()) )
	//		{
	//			return true;
	//		}
	//	}
	//}
	return false;
}


void Scene3D::doSelectVertexHover(const SelectionFrust& frust,ViewportCamera* camera)
{
	//std::basic_string<ControlVertex*> hovered_points;
	//auto camera_position = camera->getPositionCamera();

	//for( auto * object : m_objects_selected )
	//{
	//	auto & cverts = object->GetControlVertexArray();
	//	auto & verts = object->GetVertexArray();
	//	for( auto CV : cverts )
	//	{
	//		//auto & vert_inds = CV->getVertInds();
	//		auto & verts = CV->getVerts();
	//		auto V = verts[0];
	//		auto point3D = math::mul(V->getPosition(), object->GetMatrix()) + object->GetPivot();
	//		if( frust.pointInFrust(point3D) )
	//		{
	//			((ControlVertex*)CV)->m_distanceToCamera = camera_position.distance(point3D);
	//			hovered_points.push_back((ControlVertex*)CV);
	//		}
	//	}
	//}
	//if(hovered_points.size()>1)
	//{
	//	std::sort(hovered_points.begin(),hovered_points.end(),
	//		[](ControlVertex* first, ControlVertex* second)
	//		{
	//			return first->m_distanceToCamera < second->m_distanceToCamera;
	//		}
	//	);
	//}

	//auto ks = m_app->getStateKeyboard();
	//if( hovered_points.size() )
	//{
	//	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//		deselectAll();

	//	hovered_points[ 0 ]->m_isSelected = true;
	//	if( ks == AppState_keyboard::Alt )
	//		hovered_points[ 0 ]->m_isSelected = false;
	//}
	//else
	//{
	//	if( ks != AppState_keyboard::Ctrl )
	//		deselectAll();
	//}
	//
	//for( auto * object : m_objects_selected )
	//{
	//	object->m_isObjectHaveSelectedVerts = false;

	//	auto & cverts = object->GetControlVertexArray();
	//	for( auto CV : cverts )
	//	{
	//		if(CV->isSelected())
	//		{
	//			object->m_isObjectHaveSelectedVerts = true;
	//			//auto vvv = (Vertex*)(((ControlVertex*)CV)->m_verts[0]);
	//			//printf("%f %f %f\n", vvv->m_Position._f32[0], vvv->m_Position._f32[1], vvv->m_Position._f32[2]);
	//			break;
	//		}
	//	}
	//	object->updateModelPointsColors();
	//}

	//_updateSelectionAabb_vertex();
	//updateObjectVertexSelectList();
}

void      Scene3D::updateObjectVertexSelectList()
{
	/*m_objectsVertexSelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;

		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
		{
			if( o->m_PolyModel->m_controlVerts[ i2 ]->isSelected() )
			{
				info.m_verts.insert( (ControlVertex*)o->m_PolyModel->m_controlVerts[ i2 ] );
			}
		}

		if( info.m_verts.size() )
			m_objectsVertexSelectInfo.push_back(info);
	}*/
}

void      Scene3D::updateObjectPolySelectList()
{
	/*m_objectsPolySelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;

		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_controlVerts.size(); i2 < sz2; ++i2 )
		{
			if( o->m_PolyModel->m_controlVerts[ i2 ]->isSelectedPoly() )
			{
				info.m_verts.insert( (ControlVertex*)o->m_PolyModel->m_controlVerts[ i2 ] );
			}
		}

		if( info.m_verts.size() )
			m_objectsPolySelectInfo.push_back(info);
	}*/
}

void      Scene3D::updateObjectEdgeSelectList()
{
	/*m_objectsEdgeSelectInfo.clear();
	for( auto * o : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = o;
		for( size_t i2 = 0, sz2 = o->m_PolyModel->m_edges.size(); i2 < sz2; ++i2 )
		{
			auto E = o->m_PolyModel->m_edges[i2];
			if(E->m_isSelected)
			{
				info.m_verts.insert( E->m_firstPoint );
				info.m_verts.insert( E->m_secondPoint );
			}
		}

		if( info.m_verts.size() )
			m_objectsEdgeSelectInfo.push_back(info);
	}*/
}

void Scene3D::drawAll(kkCamera* camera, DrawMode* draw_mode, bool cursorInViewportObject, CursorRay* ray)
{
	bool optimize = false;
	++m_frame_skip;
	if( m_frame_skip == m_frame_limit )
	{
		m_frame_skip = 0;
		optimize = true;
		m_objects_mouseHover.clear();
		m_objects_inFrustum_unsorted.clear();
		auto frust = camera->getFrustum();
		auto camera_position = camera->getPositionInSpace();
		for( size_t i = 0, sz = m_objects.size(); i < sz; ++i )
		{
			auto object = m_objects[i];
			auto obb  = object->Obb();
			auto aabb = object->Aabb();
			kkVector4 center;
			aabb.center(center);
			object->m_distanceToCamera = camera_position.distance(center);
			if(OBBInFrustum( obb, frust ))
			{
				m_objects_inFrustum_unsorted.push_back(object);
			}
		}
		m_objects_inFrustum_sorted = m_objects_inFrustum_unsorted;
		sortObjectsInFrustum( m_objects_inFrustum_sorted );
	}

	
	for(size_t i = 0, sz = m_objects_inFrustum_sorted.size(); i < sz; ++i)
	{
		auto object = m_objects_inFrustum_sorted[i];
		kkScene3DObjectType object_type = object->GetType();
		switch(object_type)
		{
		case kkScene3DObjectType::PolygonObject:
		{
			object->UpdateWorldMatrix();

			if( *draw_mode == DrawMode::Material || *draw_mode == DrawMode::EdgesAndMaterial )
			{
				for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
				{
					kkGSDrawModel(object->getHardwareModel(i2), object->GetMatrixWorld(), object->m_shaderParameter.m_diffuseColor, object->m_shaderParameter.m_diffuseTexture);
				}
			}

			if( *draw_mode == DrawMode::Edge || *draw_mode == DrawMode::EdgesAndMaterial
				|| m_app->getEditMode() == EditMode::Edge )
			{
				/*if( m_app->getEditMode() == EditMode::Polygon )
				{
					for( u64 i2 = 0, sz = object->getHardwareModelCount(); i2 < sz; ++i2 )
					{
						m_gs->drawMesh(object->getHardwareModel(i2), object->GetMatrixWorld() , m_vd.m_app->m_shader3DObjectDefault_polymodeforlinerender.ptr() );
					}
				}*/

				for( u64 i2 = 0, sz = object->getHardwareModelCount_lines(); i2 < sz; ++i2 )
				{
					if(object->isSelected())
						kkGSDrawModelEdge(object->getHardwareModel_lines(i2), object->GetMatrixWorld(), kkColorWhite);
					else
						kkGSDrawModelEdge(object->getHardwareModel_lines(i2), object->GetMatrixWorld(), object->m_edgeColor);
				}
			}

			/*m_app->m_shaderPoint->setWorld( object->GetMatrixWorld() );
			if( m_app->m_editMode == EditMode::Vertex && object->m_isSelected )
			{
				for( u64 i2 = 0, sz = object->getHardwareModelCount_points(); i2 < sz; ++i2 )
				{
					m_vd.m_gs->drawMesh(object->getHardwareModel_points(i2), object->GetMatrixWorld() , m_vd.m_app->m_shaderPoint.ptr() );
				}
			}*/

			/*if( m_app->debugIsDrawObjectAabb() )
			{
				_drawAabb( object->Aabb(), kkColorYellowGreen );
			}

			if( m_app->debugIsDrawObjectObb()  )
			{
				_drawObb( obb, kkColorRed );
			}*/

		}break;
		default:
			break;
		}
		
		if( optimize && !kkIsMmbDown() && *kkGetAppState_main() != AppState_main::Gizmo && !g_mouseState.IsSelectByRect
			&& m_app->m_editMode == EditMode::Object )
		{
			if( cursorInViewportObject )
			{
				if( kkrooo::rayIntersection_obb(ray->m_center, object->m_obb) )
				{
					// далее проверка на пересечение луч-треугольник
					kkRayTriangleIntersectionResultSimple intersectionResult;
					if( object->IsRayIntersect(ray->m_center, intersectionResult) )
					{
						auto camera_position = camera->getPositionInSpace();
						object->m_distanceToCamera = camera_position.distance(intersectionResult.m_intersectionPoint);
						m_objects_mouseHover.push_back(object);
					}
				}
			}
		}

		if( m_objects_mouseHover.size() )
		{
			sortMouseHoverObjects( m_objects_mouseHover );
		}
	}
	if( m_objects_mouseHover.size() )
	{
		kkGSDrawObb( m_objects_mouseHover[0]->Obb(), kkColorRed );
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
	m_app->setEditMode(EditMode::Object);
	deselectAll();
	selectObject((Scene3DObject*)o);
	deleteSelectedObjects();
}

bool Scene3D::selectEdges(/*CursorRay* cursorRay, */kkRay* ray/*, int depth*/)
{
	//if(!m_objects_selected.size()) return false;
	//if( !ray ) return false;

	//std::vector<kkRayTriangleIntersectionResultSimple> results;

	//for( auto * o : m_objects_selected )
	//{
	//	o->IsRayIntersectMany(*ray, results);
	//}

	//if(!results.size())return false;

	//std::sort( results.begin(),results.end(),
	//	[](const kkRayTriangleIntersectionResultSimple& first, const kkRayTriangleIntersectionResultSimple& second)
	//{
	//	return first.m_T < second.m_T;
	//} );

	//auto ks = m_app->getStateKeyboard();

	//if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//	deselectAll();

	//for( u64 o = 0, osz = results.size(); o < osz; ++o )
	//{
	//	auto & result = results[o];
	//	auto & verts = result.m_object->GetVertexArray();
	//	auto & cverts = result.m_object->GetControlVertexArray();
	//	auto firstPolygon = result.m_object->GetPolygon( result.m_polygonIndex );
	//	auto & Pvs = firstPolygon->GetVerts();
	//	auto pivot = result.m_object->GetPivot();
	//	auto & M = result.m_object->GetMatrix();

	//	auto scene3dobject = (Scene3DObject*)result.m_object;

	//	for( u64 i = 0, sz = Pvs.size(); i < sz; ++i )
	//	{
	//		u64 i2 = i + 1;
	//		if(i2 == sz) i2=0;

	//		auto v1 = (Vertex*)Pvs[i];
	//		auto v2 = (Vertex*)Pvs[i2];

	//		auto d = ray->distanceToLine( math::mul( v1->m_Position, M) + pivot, math::mul( v2->m_Position, M ) + pivot );

	//		// тут просто шаманизм
	//		d = d / (1.f * (result.m_T+1.f));

	//		scene3dobject->m_isObjectHaveSelectedEdges = false;

	//		if( d < 0.005f )
	//		{
	//			auto cv1 = v1->m_controlVertex;
	//			auto cv2 = v2->m_controlVertex;
	//			Edge* find_edge = nullptr;
	//			for(auto e : scene3dobject->m_PolyModel->m_edges)
	//			{
	//				if((e->m_firstPoint == cv1 && e->m_secondPoint == cv2)
	//					|| (e->m_firstPoint == cv2 && e->m_secondPoint == cv1))
	//				{
	//					find_edge = e;
	//					break;
	//				}
	//			}
	//			if(!find_edge)
	//				continue;

	//			if( ks == AppState_keyboard::Alt )
	//			{
	//				find_edge->m_isSelected = false;
	//			}
	//			else
	//			{
	//				find_edge->m_isSelected = true;
	//				//if( find_edge->m_firstPoint->m_onEdge && find_edge->m_secondPoint->m_onEdge )
	//				if(!find_edge->m_secondPolygon)
	//					printf("E");
	//				scene3dobject->m_isObjectHaveSelectedEdges = true;
	//			}

	//			scene3dobject->m_PolyModel->updateCVEdgeWith();
	//			scene3dobject->updateEdgeModel();

	//			_updateSelectionAabb_edge();
	//			updateObjectEdgeSelectList();
	//			return true;
	//		}
	//	}
	//}

	//if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
	//{
	//	deselectAll();
	//}

	return false;
}

void Scene3D::_deselectAll_poly(Scene3DObject* object)
{
	/*auto & cverts = object->GetControlVertexArray();
	auto & polys = object->GetPolygonArray();
	for( u64 i = 0, sz = polys.size(); i < sz; ++i )
	{
		polys[i]->Deselect();
	}
	object->m_PolyModel->updateCVForPolygonSelect();

	object->updatePolygonModel();
	object->m_isObjectHaveSelectedPolys = false;

	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();*/
}

void Scene3D::_deselectAll_edge(Scene3DObject* object)
{
	/*object->m_isObjectHaveSelectedEdges = false;
	auto & cverts = object->GetControlVertexArray();
	for( u64 i = 0, sz = object->m_PolyModel->m_edges.size(); i < sz; ++i )
	{
		auto E = object->m_PolyModel->m_edges[i];
		E->m_isSelected = false;
	}
	((Scene3DObject*)object)->m_PolyModel->updateCVEdgeWith();
	object->updateEdgeModel();

	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();*/
}

void Scene3D::selectPolygons(kkRay* ray)
{
	/*if(!m_objects_selected.size()) return;
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
	auto firstPolygon = object->GetPolygon( result[0].m_polygonIndex );
	auto & Pvs = firstPolygon->GetVerts();
	auto & cverts  = result[0].m_object->GetControlVertexArray();

	if( ks == AppState_keyboard::Alt )
	{
		firstPolygon->Deselect();
	}
	else
	{
		firstPolygon->Select();
	}
	object->m_isObjectHaveSelectedPolys = false;
	for(auto P : object->m_PolyModel->m_polygons)
	{
		if(P->IsSelected())
		{
			object->m_isObjectHaveSelectedPolys = true;
			break;
		}
	}
	object->m_PolyModel->updateCVForPolygonSelect();
	((Scene3DObject*)result[0].m_object)->updatePolygonModel();
	
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();*/
}