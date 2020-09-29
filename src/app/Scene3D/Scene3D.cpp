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
#include "../EventConsumer.h"
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
	m_gs = m_app->m_gs.ptr();
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
	kkLogWriteInfo("Clear scene.\n");

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

void Scene3D::_selectObjectsByFrame_object(
	std::basic_string<Scene3DObject*>& hov, 
	std::basic_string<Scene3DObject*>& drw, 
	const SelectionFrust& frust
)
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();
	
	for( auto * obj : drw )
	{
		if(!obj->m_polyModel->m_edges)
			continue;

		bool found = false;
		auto E = obj->m_polyModel->m_edges;
		for(u64 i = 0; i < obj->m_polyModel->m_edgesCount; ++i)
		{
			if( frust.lineInFrust( 
				math::mul( E->m_v1->m_position, 
					obj->GetMatrix()) + obj->GetPivot() , 
				math::mul( E->m_v2->m_position, 
					obj->GetMatrix()) + obj->GetPivot() ) 
				)
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
			E = E->m_mainNext;
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

void Scene3D::_selectObjectsByFrame_poly( const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( auto * obj : m_objects_selected )
	{
		obj->m_isObjectHaveSelectedPolys = false;

		bool need_update = false;
		auto P = obj->m_polyModel->m_polygons;
		for(u64 i = 0; i < obj->m_polyModel->m_polygonsCount; ++i )
		{
			auto PE = P->m_edges;
			for( u64 o = 0; o < P->m_edgeCount; ++o )
			{
				if( frust.lineInFrust( 
					math::mul( PE->m_element->m_v1->m_position, obj->GetMatrix()) + obj->GetPivot(), 
					math::mul( PE->m_element->m_v2->m_position, obj->GetMatrix()) + obj->GetPivot() 
				))
				{
					need_update = true;

					if( ks == AppState_keyboard::Alt )
					{
						if( P->m_flags & P->EF_SELECTED )
							P->m_flags ^= P->EF_SELECTED;
					}
					else
					{
						obj->m_isObjectHaveSelectedPolys = true;
						P->m_flags |= P->EF_SELECTED;
					}
					goto next_polygon;
				}
				PE = PE->m_right;
			}
		next_polygon:;
			P = P->m_mainNext;
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
					for( auto result : results )
					{					
						if( ks == AppState_keyboard::Alt )
						{
							if(result.m_polygon->m_flags & result.m_polygon->EF_SELECTED)
								result.m_polygon->m_flags ^= result.m_polygon->EF_SELECTED;
						}
						else
						{
							result.m_polygon->m_flags |= result.m_polygon->EF_SELECTED;
						}
					}
					need_update = true;
					break;
				}
			}
		}

		if( need_update )
		{
			obj->m_isObjectHaveSelectedPolys = false;
			obj->updatePolygonModel();
			_updateSelectionAabb_polygon();
			updateObjectPolySelectList();
		}
	}
}

void Scene3D::_selectObjectsByFrame_edge( const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();

	for( auto * obj : m_objects_selected )
	{
		obj->m_isObjectHaveSelectedEdges = false;
		bool need_update = false;
		auto current_edge = obj->m_polyModel->m_edges;
		for(u64 i = 0; i < obj->m_polyModel->m_edgesCount; ++i )
		{
			if( frust.lineInFrust( 
				math::mul( current_edge->m_v1->m_position, obj->GetMatrix()) + obj->GetPivot(), 
				math::mul( current_edge->m_v2->m_position, obj->GetMatrix()) + obj->GetPivot() 
				))
			{
				need_update = true;
				if( ks == AppState_keyboard::Alt )
				{
					if(current_edge->m_flags & current_edge->EF_SELECTED)
						current_edge->m_flags ^= current_edge->EF_SELECTED;
				}
				else
				{
					current_edge->m_flags |= current_edge->EF_SELECTED;
				}
			}
			current_edge = current_edge->m_mainNext;
		}
		if( need_update )
		{
			((Scene3DObject*)obj)->updateEdgeModel();
			_updateSelectionAabb_edge();
			updateObjectEdgeSelectList();
		}
	}
}

void Scene3D::_selectObjectsByFrame_vertex(  const SelectionFrust& frust )
{
	auto ks = m_app->getStateKeyboard();
	if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
		deselectAll();
	
	for( auto object : m_objects_selected )
	{
		object->m_isObjectHaveSelectedVerts = false;
		auto current_vertex = object->m_polyModel->m_verts;
		for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
		{
			if( frust.pointInFrust(math::mul(current_vertex->m_position, object->GetMatrix()) + object->GetPivot()) )
			{
				if( ks == AppState_keyboard::Alt )
				{
					if(current_vertex->m_flags & current_vertex->EF_SELECTED)
						current_vertex->m_flags ^= current_vertex->EF_SELECTED;
				}
				else
				{
					current_vertex->m_flags |= current_vertex->EF_SELECTED;
					object->m_isObjectHaveSelectedVerts = true;
				}
			}

			current_vertex = current_vertex->m_mainNext;
		}
		if(!object->m_isObjectHaveSelectedVerts)
		{
			current_vertex = object->m_polyModel->m_verts;
			for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
			{
				if(current_vertex->m_flags & current_vertex->EF_SELECTED)
				{
					object->m_isObjectHaveSelectedVerts = true;
					break;
				}
				current_vertex = current_vertex->m_mainNext;
			}
		}

		object->updateModelPointsColors();
	}
	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::selectObjectsByFrame( 
	std::basic_string<Scene3DObject*>& hoverObjects, 
	std::basic_string<Scene3DObject*>& drawObjects,
	const v4i& selFrame,
	const SelectionFrust& selFrust )
{
	switch(m_app->getEditMode())
	{
	case EditMode::Object:
	default:
		_selectObjectsByFrame_object(hoverObjects, drawObjects, selFrust);
		break;
	case EditMode::Vertex:
		_selectObjectsByFrame_vertex( selFrust );
		break;
	case EditMode::Edge:
		_selectObjectsByFrame_edge( selFrust );
		break;
	case EditMode::Polygon:
		_selectObjectsByFrame_poly( selFrust );
		break;
	}
	kkDrawAll();
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
	auto E = object->m_polyModel->m_edges;
	for(u64 i = 0; i < object->m_polyModel->m_edgesCount; ++i )
	{
		E->m_flags |= E->EF_SELECTED;
		E = E->m_mainNext;
	}
	object->updateEdgeModel();
	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();
}

void Scene3D::_selectAll_poly(Scene3DObject* object)
{
	auto P = object->m_polyModel->m_polygons;
	for( u64 i = 0; i < object->m_polyModel->m_polygonsCount; ++i )
	{
		P->m_flags |= P->EF_SELECTED;
		P = P->m_mainNext;
	}

	object->updatePolygonModel();
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_selectAll_vertex(Scene3DObject* object)
{
	auto current_vertex = object->m_polyModel->m_verts;
	for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
	{
		current_vertex->m_flags |= current_vertex->EF_SELECTED;
		current_vertex = current_vertex->m_mainNext;
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
	//printf("_deselectAll_vertex\n");
	auto current_vertex = object->m_polyModel->m_verts;
	for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
	{
		if(current_vertex->m_flags & current_vertex->EF_SELECTED)
			current_vertex->m_flags ^= current_vertex->EF_SELECTED;
		current_vertex = current_vertex->m_mainNext;
	}
	object->m_isObjectHaveSelectedVerts = false;
	object->updateModelPointsColors();
	_updateSelectionAabb_vertex();
	updateObjectVertexSelectList();
}

void Scene3D::deselectAll()
{
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
	auto P = object->m_polyModel->m_polygons;
	for( u64 i = 0; i < object->m_polyModel->m_polygonsCount; ++i )
	{
		if(P->m_flags & P->EF_SELECTED)
		{
			P->m_flags ^= P->EF_SELECTED;
		}
		else
		{
			P->m_flags |= P->EF_SELECTED;
		}
		P = P->m_mainNext;
	}
	object->updatePolygonModel();
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_selectInvert_edge(Scene3DObject* object)
{
	auto E = object->m_polyModel->m_edges;
	for( u64 i = 0; i < object->m_polyModel->m_edgesCount; ++i )
	{
		if(E->m_flags & E->EF_SELECTED)
		{
			E->m_flags ^= E->EF_SELECTED;
		}
		else
		{
			E->m_flags |= E->EF_SELECTED;
		}
		E = E->m_mainNext;
	}
	object->updateEdgeModel();
	_updateSelectionAabb_edge();
	updateObjectEdgeSelectList();
}

void Scene3D::_selectInvert_vertex(Scene3DObject* object)
{
	auto V = object->m_polyModel->m_verts;
	for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
	{
		if(V->m_flags & V->EF_SELECTED)
		{
			V->m_flags ^= V->EF_SELECTED;
		}
		else
		{
			V->m_flags |= V->EF_SELECTED;
		}
		V = V->m_mainNext;
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
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * object : m_objects_selected )
	{
		M = object->m_matrix;

		auto current_vertex = object->m_polyModel->m_verts;
		for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
		{
			if( current_vertex->m_flags & current_vertex->EF_SELECTED)
			{
				nnn = false;
				m_selectionAabb.add( math::mul( current_vertex->m_position, M ) + object->GetPivot() );
			}
			current_vertex = current_vertex->m_mainNext;
		}
	}
	if( nnn )
		_setDefaultSelectionAabb();
}
void Scene3D::_setDefaultSelectionAabb()
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
void Scene3D::_updateSelectionAabb_edge()
{
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;
	kkMatrix4 M;

	for( auto * object : m_objects_selected )
	{
		M = object->m_matrix;

		auto current_edge = object->m_polyModel->m_edges;
		for( u64 i = 0; i < object->m_polyModel->m_edgesCount; ++i )
		{
			if(current_edge->m_flags & current_edge->EF_SELECTED)
			{
				nnn = false;
				m_selectionAabb.add( math::mul( current_edge->m_v1->m_position, M ) + object->GetPivot() );
				m_selectionAabb.add( math::mul( current_edge->m_v2->m_position, M ) + object->GetPivot() );
			}
			current_edge = current_edge->m_mainNext;
		}
	}
	if( nnn )
		_setDefaultSelectionAabb();
}

void Scene3D::_updateSelectionAabb_polygon()
{
	if( !m_objects_selected.size() )
		return;

	m_selectionAabb.reset();
	bool nnn = true;

	kkMatrix4 M;

	for( auto * o : m_objects_selected )
	{
		M = o->m_matrix;
		auto P = o->m_polyModel->m_polygons;
		for( u64 i = 0; i < o->m_polyModel->m_polygonsCount; ++i )
		{
			if(P->m_flags & P->EF_SELECTED)
			{
				nnn = false;
				auto PV = P->m_verts;
				for( u64 i2 = 0; i2 < P->m_vertexCount; ++i2 )
				{
					m_selectionAabb.add( 
						math::mul( 
							PV->m_element->m_position, 
							M ) + o->GetPivot() 
					);
					PV = PV->m_right;
				}
			}
			P = P->m_mainNext;
		}
	}
	if( nnn )
		_setDefaultSelectionAabb();
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
	object->updateModelPointsColors();	
	m_app->setNeedToSave(true);
	_deselectAll_vertex(object);
}

void Scene3D::_deleteSelectedObjects_poly(Scene3DObject* object)
{
	object->deleteSelectedPolys();
	object->updateModelPointsColors();
	m_app->setNeedToSave(true);
	_deselectAll_poly(object);
}

void Scene3D::_deleteSelectedObjects_edge(Scene3DObject* object)
{
	object->deleteSelectedEdges();
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

void Scene3D::unregisterObject( Scene3DObject* o )
{
	if( m_objects.find_first_of(o) != std::string::npos )
		m_objects.erase( m_objects.begin() + m_objects.find_first_of(o) );

	if( m_objects_selected.find_first_of(o) != std::string::npos )
		m_objects_selected.erase( m_objects_selected.begin() + m_objects_selected.find_first_of(o) );
}

void Scene3D::rotateSelectedObjects(GizmoPart* gizmoPart,bool startStop, bool cancel, bool first )
{
	auto em = m_app->getEditMode();

	static f32 first_x = 0, first_y = 0, first_z = 0;
	static f32 toolTip_angle_x = 0.f, toolTip_angle_y = 0.f, toolTip_angle_z = 0.f;
	static kkStringA toolTip;

	static kkVector4 C;
	if( first )
		m_selectionAabb.center(C);

	f32 x = 0, y = 0, z = 0;

	auto VO = kkGetActiveViewport();
	auto camera  = VO->m_activeCamera->getCamera();

	auto p1 = m_app->m_currentGizmoEvent.point2D;
	auto p2 = m_app->m_cursor_position;
	auto p3 = p2 - p1;
	v2f point((float)p3.x,(float)p3.y);

	if( *gizmoPart == GizmoPart::X )
	{
		x = (f32)std::atan2((f32)point.x, (f32)point.y);
		if( first )
			first_x = x;
		x -= first_x;
	}

	if( *gizmoPart == GizmoPart::Y )
	{
		y = (f32)std::atan2((f32)point.x, (f32)point.y);
		if( first )
			first_y = y;
		y -= first_y;
	}

	if( *gizmoPart == GizmoPart::Z )
	{
		z = (f32)std::atan2((f32)point.x, (f32)point.y);
		if( first )
			first_z = z;
		z -= first_z;
	}

	kkMatrix4 M;
	float deg_f = 0.f;

	if( *gizmoPart == GizmoPart::Screen_plane )
	{
		auto vprc = VO->m_rect_modified;
		auto vpsz = vprc.getWidthAndHeight();

		auto p1 = v2i( (vpsz.x/2.f)+vprc.x,(vpsz.y/2.f)+vprc.y);
		auto p2 = m_app->m_cursor_position;
		auto point = p2 - p1;

		x = (f32)std::atan2((f32)point.x, (f32)point.y);
		if( first )
			first_x = x;
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

		VO->m_activeCamera->setObjectRotationAngle(-math::degToRad((float)deg_f));

		VO->m_activeCamera->update();
		M = VO->m_activeCamera->getObjectRotationMatrix();
	}

	//printf("%f %f\n", x, y);
	auto cameraRotation = VO->m_activeCamera->getAllRotation();
	//printf("%f\n", cameraRotation.y );

	if( *gizmoPart == GizmoPart::X )
	{
		static float last_x =  0.f;
		if( x > 0.f )
			x = -(math::PI+math::PI) + x;
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
			deg_f = math::radToDeg( std::abs( toolTip_angle_x ));

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

	if( *gizmoPart == GizmoPart::Y )
	{
		static float last_y =  0.f;
		if( y > 0.f )
			y = -(math::PI+math::PI) + y;
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
			deg_f = math::radToDeg( std::abs( toolTip_angle_y ));

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

	if( *gizmoPart == GizmoPart::Z )
	{
		static float last_z =  0.f;
		if( z > 0.f )
			z = -(math::PI+math::PI) + z;
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
			deg_f = math::radToDeg( std::abs( toolTip_angle_z ));

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
				o->UpdateAabb();
			}
		}break;
		case EditMode::Vertex:
		{
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->rotateVerts( M,info->m_verts, C - info->m_object->m_pivot );
			}
		}break;
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
		if( cancel )
		{
			m_app->m_state_app = AppState_main::CancelTransformation;
		}
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
			}
		}break;
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
			}
		}break;
		default:
			break;
		}
		
		updateSelectionAabb();
		updateSceneAabb();
	}
}

void Scene3D::scaleSelectedObjects(GizmoPart* gizmoPart, bool startStop, bool cancel,bool first )
{
	auto VO = kkGetActiveViewport();
	kkVector4 pivotFix;
	f32 x = 0, y = 0, z = 0;
	auto r1 = VO->m_rayOnClick.m_origin;
	auto r2 = VO->m_cursorRay->m_center.m_origin;
	auto camera  = VO->m_activeCamera->getCamera();

	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
	// но на самом деле это мир идёт ВВЕРХ, плюс Y
	auto cam_pos = -camera->getPositionInSpace();

	if( *gizmoPart == GizmoPart::X )
		x = (r2-r1).KK_X;
	if( *gizmoPart == GizmoPart::Y )
		y = (r2-r1).KK_Y;
	if( *gizmoPart == GizmoPart::Z )
		z = (r2-r1).KK_Z;
	if( *gizmoPart == GizmoPart::XY_plane )
	{
		x = (r2-r1).KK_X;
		y = (r2-r1).KK_Y;
	}

	if( *gizmoPart == GizmoPart::XZ_plane )
	{
		x = (r2-r1).KK_X;
		z = (r2-r1).KK_Z;
	}

	if( *gizmoPart == GizmoPart::ZY_plane )
	{
		y = (r2-r1).KK_Y;
		z = (r2-r1).KK_Z;
	}

	if( *gizmoPart == GizmoPart::Screen_plane )
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
	bool isPerspective = VO->m_activeCamera == VO->m_cameraPersp.ptr();
	if( isPerspective )
		L = 500.0f;
	
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
				o->UpdateAabb();
			}
		}break;
		case EditMode::Vertex:
		{
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				auto info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->scaleVerts( S,info->m_verts, C - info->m_object->m_pivot );
			}
		}break;
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
					// global
					if( m_app->m_isLocalScale  == false )
					{
						o->RestorePivot();
					}

					o->m_matrix      = o->m_matrixFixed;
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
					o->UpdateAabb(); // и в соответствии с новым AABB построятся остальные
				}
			}
			break;
		default:
			break;
		}
		
		updateSelectionAabb();
		updateSceneAabb();
	}
}

bool Scene3D::moveSelectedObjects(
	GizmoPart* gizmoPart,
	bool moving, 
	bool cancel, 
	bool first)
{
	auto em = m_app->getEditMode();
	kkVector4 pivotFix;
	static f32 x = 0, y = 0, z = 0;
	auto VO = kkGetActiveViewport();

	// старая версия. Проблема была в том, что 2д точки получаются корректными только когда они видны
	// если точка уходит за пределы видимости, значения получаются гигантскими и модель улетает.
	// замена на обычные координаты курсора
	// старая версия подходит только для масштабирования
	// так-же пока она будет использоваться для перемещения в плоскости экрана
	auto r1 = VO->m_rayOnClick.m_origin;
	auto r2 = VO->m_cursorRay->m_center.m_origin;
	//printf("%f %f %f\n", r2._f32[0], r2._f32[1], r2._f32[2]);

	static v2i p1 = m_app->m_cursor_position;
	auto p2 = m_app->m_cursor_position;

	if( first || cancel)
	{
		p1 = p2;
		x = y = z = 0;
	}

	auto point = p2 - p1;

	p1 = p2;

	auto camera  = VO->m_activeCamera->getCamera();

	// почему минус. если мы спускаемся вниз, то думаем, что камера идёт в минус Y
	// но на самом деле это мир идёт ВВЕРХ, плюс Y
	auto cam_pos = -camera->getPositionInSpace();
	
	auto cameraRotation = VO->m_activeCamera->getAllRotation();

	if( *gizmoPart == GizmoPart::X || *gizmoPart == GizmoPart::XY_plane || *gizmoPart == GizmoPart::XZ_plane )
	{
		x += point.x * std::cos( cameraRotation.y / 180.f * math::PI );
		x += point.y * std::sin( cameraRotation.y / 180.f * math::PI );
	}

	if( *gizmoPart == GizmoPart::Y|| *gizmoPart == GizmoPart::XY_plane || *gizmoPart == GizmoPart::ZY_plane)
	{
		if( cameraRotation.x < 0.f )
			y += -point.y;
		else
			y += point.y;
	}
	
	if( *gizmoPart == GizmoPart::Z || *gizmoPart == GizmoPart::ZY_plane || *gizmoPart == GizmoPart::XZ_plane )
	{
		z -= point.x * std::sin( cameraRotation.y / 180.f * math::PI );
		z += point.y * std::cos( cameraRotation.y / 180.f * math::PI );
	}

	bool isPerspective = VO->m_activeCamera == VO->m_cameraPersp.ptr();
	if( *gizmoPart == GizmoPart::Screen_plane )
	{
	//	printf("m");
		if( isPerspective )
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
	if( isPerspective )
	{
		if( *gizmoPart == GizmoPart::Screen_plane )
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
		if( *gizmoPart != GizmoPart::Screen_plane )
		{
			auto vpcam = VO->getActiveViewportCamera();
			L /= vpcam->getZoomOrt() * 100.f;
		}
	}
	//printf("%f\n", L );
	bool result = false;
	ObjectVertexSelectInfo * info;
	auto V = kkVector4(x*L,y*L,z*L);
	if( moving )
	{
		switch (em)
		{
		case EditMode::Object:
			for( auto * o : m_objects_selected )
			{
				pivotFix = o->GetPivotFixed();
				auto & pivot  = o->GetPivot();
				pivot = pivotFix + V;
				o->UpdateAabb();
			}
			break;
		case EditMode::Vertex:
			//printf("%llu\n",m_objectsVertexSelectInfo.size());
			for( u64 i = 0, sz = m_objectsVertexSelectInfo.size(); i < sz; ++i )
			{
				info = &m_objectsVertexSelectInfo[ i ];
				info->m_object->moveVerts( V, info->m_verts);
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

					result = true;
					o->ApplyPivot();
				}
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
			}
			break;
		default:
			break;
		}
		
	}
    
	updateSelectionAabb();
	updateSceneAabb();
	return result;
}


bool Scene3D::isVertexHover(const SelectionFrust& frust)
{
	for( auto * object : m_objects_selected )
	{
		auto current_vertex = object->m_polyModel->m_verts;
		for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
		{
			if( frust.pointInFrust(math::mul(current_vertex->m_position, object->GetMatrix()) + object->GetPivot()) )
				return true;
			current_vertex = current_vertex->m_mainNext;
		}
	}
	return false;
}


struct _hover_result
{
	kkVertex* vertex = nullptr;
	kkScene3DObject* object = nullptr;
	f32 distance = 0.f;
};

kkVertex* Scene3D::doSelectVertexHover(
	const SelectionFrust& frust,
	ViewportCamera* camera,
	bool select, 
	kkScene3DObject** out_object)
{
	kkVertex* selected_vertex = nullptr;
	std::vector<_hover_result> hovered_points;
	auto camera_position = camera->getPositionCamera();

	for( auto * object : m_objects_selected )
	{
		auto current_vertex = object->m_polyModel->m_verts;
		for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
		{
			auto point3D = math::mul(current_vertex->m_position, object->GetMatrix()) + object->GetPivot();
			if( frust.pointInFrust(point3D) )
			{
				_hover_result res;
				res.vertex = current_vertex;
				res.distance = camera_position.distance(point3D);
				res.object = object;
				hovered_points.push_back(res);
			}
			current_vertex = current_vertex->m_mainNext;
		}
	}
	if(hovered_points.size()>1)
	{
		std::sort(hovered_points.begin(),hovered_points.end(),
			[](const _hover_result& first, const _hover_result& second)
			{
				return first.distance < second.distance;
			}
		);
	}

	auto ks = m_app->getStateKeyboard();
	if( hovered_points.size() )
	{
		if(select)
		{
			if( ks != AppState_keyboard::Alt && ks != AppState_keyboard::Ctrl )
				deselectAll();
		}

		if( ks == AppState_keyboard::Alt )
		{
			if(select)
			{
				if(hovered_points[ 0 ].vertex->m_flags & kkVertex::EF_SELECTED)
					hovered_points[ 0 ].vertex->m_flags ^= kkVertex::EF_SELECTED;
			}
		}
		else
		{
			if(select)
				hovered_points[ 0 ].vertex->m_flags |= kkVertex::EF_SELECTED;
			selected_vertex = hovered_points[ 0 ].vertex;
			if(out_object)
			{
				*out_object = hovered_points[ 0 ].object;
			}
		}
	}
	else
	{
		if( ks != AppState_keyboard::Ctrl && select)
			deselectAll();
	}
	
	if(select)
	{
		for( auto * object : m_objects_selected )
		{
			object->m_isObjectHaveSelectedVerts = false;

			auto current_vertex = object->m_polyModel->m_verts;
			for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
			{
				if(current_vertex->m_flags & kkVertex::EF_SELECTED)
				{
					object->m_isObjectHaveSelectedVerts = true;
					break;
				}
				current_vertex = current_vertex->m_mainNext;
			}
			object->updateModelPointsColors();
		}

		_updateSelectionAabb_vertex();
		updateObjectVertexSelectList();
	}

	return selected_vertex;
}

void      Scene3D::updateObjectVertexSelectList()
{
	m_objectsVertexSelectInfo.clear();
	for( auto * object : m_objects_selected )
	{
		ObjectVertexSelectInfo info;
		info.m_object = object;

		auto current_vertex = object->m_polyModel->m_verts;
		for( u64 i = 0; i < object->m_polyModel->m_vertsCount; ++i )
		{
			if( current_vertex->m_flags & current_vertex->EF_SELECTED)
			{
				info.m_verts.insert( current_vertex );
			}
			current_vertex = current_vertex->m_mainNext;
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
		auto P = o->m_polyModel->m_polygons;

		for( size_t i = 0; i < o->m_polyModel->m_polygonsCount; ++i )
		{
			if( P->m_flags & P->EF_SELECTED )
			{
				auto PV = P->m_verts;
				for( size_t i2 = 0; i2 < P->m_vertexCount; ++i2 )
				{
					info.m_verts.insert( PV->m_element );
					PV = PV->m_right;
				}
			}
			P = P->m_mainNext;
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
		auto E = o->m_polyModel->m_edges;
		for( size_t i2 = 0; i2 < o->m_polyModel->m_edgesCount; ++i2 )
		{
			if(E->m_flags & E->EF_SELECTED)
			{
				info.m_verts.insert( E->m_v1 );
				info.m_verts.insert( E->m_v2 );
			}
			E = E->m_mainNext;
		}

		if( info.m_verts.size() )
			m_objectsEdgeSelectInfo.push_back(info);
	}
}
void Scene3D::applyMatrices()
{
	kkLogWriteInfo("Apply matrices.\n");
	for( u64 i = 0, sz = m_objects_selected.size(); i < sz; ++i )
	{
		m_objects_selected[i]->applyMatrices();
	}
	updateSceneAabb();
	updateSelectionAabb();
}

void Scene3D::resetMatrices()
{
	kkLogWriteInfo("Reset matrices.\n");
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

bool Scene3D::selectEdges(kkRay* ray)
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
		//auto scene3dobject = (Scene3DObject*)result.m_object;
		auto EN = result.m_polygon->m_edges;
		auto & M = result.m_object->GetMatrix();
		auto pivot = result.m_object->GetPivot();
		auto scene3dobject = (Scene3DObject*)result.m_object;
		scene3dobject->m_isObjectHaveSelectedEdges = false;
		for(u32 i = 0; i < result.m_polygon->m_edgeCount; ++i )
		{
			auto E = EN->m_element;

			auto d = ray->distanceToLine( 
				math::mul( E->m_v1->m_position, M) + pivot, 
				math::mul( E->m_v2->m_position, M ) + pivot 
			);
			d = d / (1.f * (result.m_T+1.f));
			if( d < 0.005f )
			{
				if( ks == AppState_keyboard::Alt )
				{
					if(E->m_flags & E->EF_SELECTED)
						E->m_flags ^= E->EF_SELECTED;
				}
				else
				{
					E->m_flags |= E->EF_SELECTED;
				}

				// m_isObjectHaveSelectedEdges = true; внутри след. метода
				scene3dobject->updateEdgeModel();
				
				_updateSelectionAabb_edge();
				updateObjectEdgeSelectList();
				return true;
			}

			EN = EN->m_right;
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
	auto P = object->m_polyModel->m_polygons;
	for( u64 i = 0; i < object->m_polyModel->m_polygonsCount; ++i )
	{
		if(P->m_flags & P->EF_SELECTED)
			P->m_flags ^= P->EF_SELECTED;
		P = P->m_mainNext;
	}

	object->updatePolygonModel();
	object->m_isObjectHaveSelectedPolys = false;
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}

void Scene3D::_deselectAll_edge(Scene3DObject* object)
{
	auto current_edge = object->m_polyModel->m_edges;
	for( u64 i = 0; i < object->m_polyModel->m_edgesCount; ++i )
	{
		if(current_edge->m_flags & current_edge->EF_SELECTED)
			current_edge->m_flags ^= current_edge->EF_SELECTED;
		current_edge = current_edge->m_mainNext;
	}
	object->m_isObjectHaveSelectedEdges = false;
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
	auto P = result[0].m_polygon;
	if( ks == AppState_keyboard::Alt )
	{
		if(P->m_flags & P->EF_SELECTED)
			P->m_flags ^= P->EF_SELECTED;
	}
	else
	{
		P->m_flags |= P->EF_SELECTED;
	}
	object->m_isObjectHaveSelectedPolys = false;
	((Scene3DObject*)result[0].m_object)->updatePolygonModel();
	
	_updateSelectionAabb_polygon();
	updateObjectPolySelectList();
}