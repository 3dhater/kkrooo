// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SCENE3D_IMPL_H__
#define __SCENE3D_IMPL_H__

#include <string>

#include "Classes/kkAABB.h"
#include "Classes/Containers/kkArray.h"

#include "Scene3D/kkScene3D.h"
#include "Scene3D/kkScene3DObject.h"
#include "../AppEvent.h"

class kkScene3DObject;
class Scene3DObject;
class PolygonalModel;
class Application ;
class Gizmo;
class ViewportCamera;
class ControlVertex;
struct CursorRay;
struct SelectionFrust;

struct ObjectVertexSelectInfo
{
	Scene3DObject* m_object = nullptr;
	std::basic_string<ControlVertex*> m_verts;
};

class Scene3D : public kkScene3D
{
	Application * m_app = nullptr;
	Gizmo * m_gizmo = nullptr;

	std::basic_string<Scene3DObject*> m_objects;
	std::basic_string<Scene3DObject*> m_objects_selected;

	kkArray<ObjectVertexSelectInfo>   m_objectsVertexSelectInfo;
	kkArray<ObjectVertexSelectInfo>   m_objectsEdgeSelectInfo;
	kkArray<ObjectVertexSelectInfo>   m_objectsPolySelectInfo;


	// все aabb всех объектов складываются сюда
	// если нет выделенных объектов то центрирование камеры идёт на центр aabb
	kkAabb m_sceneAabb;
	kkAabb m_selectionAabb;

	void _updateSelectionAabb_object();
	void _updateSelectionAabb_vertex();
	void _updateSelectionAabb_edge();
	void _updateSelectionAabb_polygon();

	void _selectObject_object(Scene3DObject*);
	
	void _deselectObject_object(Scene3DObject*);
	
	void _selectAll_object();
	void _selectAll_vertex(Scene3DObject*);
	void _selectAll_edge(Scene3DObject*);
	void _selectAll_poly(Scene3DObject*);
    
	void _deselectAll_object(Scene3DObject*);
	void _deselectAll_vertex(Scene3DObject*);
	void _deselectAll_edge(Scene3DObject*);
	void _deselectAll_poly(Scene3DObject*);
    
	void _selectInvert_object();
	void _selectInvert_vertex(Scene3DObject*);
	void _selectInvert_edge(Scene3DObject*);
	void _selectInvert_poly(Scene3DObject*);

	void _deleteSelectedObjects_object();
	void _deleteSelectedObjects_vertex(Scene3DObject*);
	void _deleteSelectedObjects_edge(Scene3DObject*);
	void _deleteSelectedObjects_poly(Scene3DObject*);

	void _selectObjectsByRectangle_object( std::basic_string<Scene3DObject*>&, std::basic_string<Scene3DObject*>&, const SelectionFrust& frust );
	void _selectObjectsByRectangle_vertex( /*const v4i& r*/const SelectionFrust& frust );
	void _selectObjectsByRectangle_edge( const SelectionFrust& );
	void _selectObjectsByRectangle_poly( const SelectionFrust& );

	bool m_ignoreDeselect = false;

	//void _destroyTMP();
	//bool m_tmpBegin = false;
	//Scene3DObject * m_tmpObject = nullptr;

	friend class Viewport;

public:
	Scene3D();
	virtual ~Scene3D();

	u32 getNumOfObjectsOnScene();
	u32 getNumOfSelectedObjects();
	kkScene3DObject* getObjectOnScene( u32 );
	kkScene3DObject* getSelectedObject( u32 );
	//void beginTMPObject( kkScene3DObjectType );
	//void endTMPObject();

	void applyMatrices();
	void resetMatrices();


	// ========================
	void test();
	void ignoreDeselect(){ m_ignoreDeselect = true; }

	void      updateObjectVertexSelectList();
	void      updateObjectEdgeSelectList();
	void      updateObjectPolySelectList();

	void      clearScene();
	Scene3DObject * createNewPolygonalObject( const char16_t* name, PolygonalModel*, const v4f& );
	bool      nameIsFree( const char16_t* n );
	kkString  nameCreateNew( const char16_t* n );

	void	  updateSelectionAabb();
	void      updateSceneAabb();
	//void      updateObject2DPoints( std::basic_string<Scene3DObject*>& );
	//void      updateObject2DPoints_selected();
	//void      updateObject2DPoints_vertex();
	//void      updateObject2DPoints( kkScene3DObject* );

	const kkAabb& getSceneAabb();
	const kkAabb& getSelectionAabb();

	std::basic_string<Scene3DObject*>& getObjects();

	void selectObjectsByRectangle( std::basic_string<Scene3DObject*>&, std::basic_string<Scene3DObject*>&,  const v4i&, const SelectionFrust& );
	void selectObject(kkScene3DObject*);
	void deselectObject(kkScene3DObject*);
	void selectAll();
    void deselectAll();
    void selectInvert();
    
	void deleteSelectedObjects();

	// просто уберёт указанный объект из массивов
	// объект нужно будет удалить самостоятельно
	void unregisterObject( Scene3DObject* o );

	// предполагается, что тут будут перемещаться объекты, вершины и так далее
	void moveSelectedObjects(bool, const AppEvent_gizmo& , bool,bool first );
	
	void scaleSelectedObjects(bool, const AppEvent_gizmo& , bool,bool first );
	void rotateSelectedObjects(bool, const AppEvent_gizmo& , bool, bool first );

	//bool isVertexHover(v2i*);
	bool isVertexHover(const SelectionFrust&);
	void doSelectVertexHover(const SelectionFrust& sfrust,ViewportCamera* camera);
	//void selectEdges(kkRay* ray);
	bool selectEdges(/*CursorRay* cursorRay, */kkRay* currentRay/*, int depth*/);
	void selectPolygons(kkRay* ray);

	void deleteObject( kkScene3DObject* );
};


#endif