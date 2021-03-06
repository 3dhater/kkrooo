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
struct CursorRay;
struct SelectionFrust;

struct ObjectVertexSelectInfo
{
	Scene3DObject* m_object = nullptr;
	std::unordered_set<kkVertex*> m_verts;
};

class Scene3D : public kkScene3D
{
	Application * m_app = nullptr;
	Gizmo * m_gizmo = nullptr;
	kkGraphicsSystem * m_gs = nullptr;

	std::basic_string<Scene3DObject*> m_objects;
	std::basic_string<Scene3DObject*> m_objects_selected;

	kkArray<ObjectVertexSelectInfo>   m_objectsVertexSelectInfo;
	kkArray<ObjectVertexSelectInfo>   m_objectsEdgeSelectInfo;
	kkArray<ObjectVertexSelectInfo>   m_objectsPolySelectInfo;


	s32 m_frame_skip = 0;
	s32 m_frame_limit = 1; // нужно сделать так чтобы при потере производительности это значение увеличивалось

	// все aabb всех объектов складываются сюда
	// если нет выделенных объектов то центрирование камеры идёт на центр aabb
	kkAabb m_sceneAabb;
	kkAabb m_selectionAabb;

	void _setDefaultSelectionAabb();
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

	void _selectObjectsByFrame_object( 
		std::basic_string<Scene3DObject*>& hov, 
		std::basic_string<Scene3DObject*>& drw, 
		const SelectionFrust& frust );
	void _selectObjectsByFrame_vertex( /*const v4i& r*/const SelectionFrust& frust );
	void _selectObjectsByFrame_edge( const SelectionFrust& );
	void _selectObjectsByFrame_poly( const SelectionFrust& );


	bool m_ignoreDeselect = false;

	//void _destroyTMP();
	//bool m_tmpBegin = false;
	//Scene3DObject * m_tmpObject = nullptr;

	//bool m_drawOptimize = false;
	//void _drawOptimize(kkCamera* camera);
	//void _drawSelectedObjectFrame();
	//bool m_cursorInViewport = false;
	//CursorRay* m_cursorRay = nullptr;
	friend class ViewportObject;
	friend class Viewport;

public:
	Scene3D();
	virtual ~Scene3D();

	u32 getNumOfObjectsOnScene();
	u32 getNumOfSelectedObjects();
	kkScene3DObject* getObjectOnScene( u32 );
	kkScene3DObject* getSelectedObject( u32 );
	
	//void drawAll(kkCamera* camera, DrawMode*, bool cursorInViewportObject, CursorRay* ray, bool activeViewport);
	//void drawObjectPivot(bool isPersp, ViewportObject* vp, bool activeViewport);
	//void drawGizmo3D(kkRay* center);
	//void drawGizmo2D();

	void applyMatrices();
	void resetMatrices();


	// ========================
	void ignoreDeselect(){ m_ignoreDeselect = true; }

	void      updateObjectVertexSelectList();
	void      updateObjectEdgeSelectList();
	void      updateObjectPolySelectList();

	void      clearScene();
	Scene3DObject * createNewPolygonalObject( const char16_t* name, PolygonalModel*, const v4f& );
	bool      nameIsFree( const char16_t* n );
	kkString  nameCreateNew( const char16_t* n );
	void      renameObject(kkScene3DObject*, const char16_t* newName);

	void	  updateSelectionAabb();
	void      updateSceneAabb();
	//void      updateObject2DPoints( std::basic_string<Scene3DObject*>& );
	//void      updateObject2DPoints_selected();
	//void      updateObject2DPoints_vertex();
	//void      updateObject2DPoints( kkScene3DObject* );

	const kkAabb& getSceneAabb();
	const kkAabb& getSelectionAabb();

	std::basic_string<Scene3DObject*>& getObjects();

	void selectObjectsByFrame( 
		std::basic_string<Scene3DObject*>& hoverObjects, 
		std::basic_string<Scene3DObject*>& drawObjects,
		const v4i& selFrame,
		const SelectionFrust& selFrust);
	void selectObject(kkScene3DObject*);
	void deselectObject(kkScene3DObject*);
	void selectAll();
    void deselectAll();
    void selectInvert();
    //void updateInput();
	void deleteSelectedObjects();

	// просто уберёт указанный объект из массивов
	// объект нужно будет удалить самостоятельно
	void unregisterObject( Scene3DObject* o );

	// предполагается, что тут будут перемещаться объекты, вершины и так далее
	bool moveSelectedObjects(GizmoPart*, bool, bool,bool first );
	void scaleSelectedObjects(GizmoPart*,bool, bool,bool first );
	void rotateSelectedObjects(GizmoPart*,bool, bool, bool first );

	//bool isVertexHover(v2i*);
	bool isVertexHover(const SelectionFrust&);
	kkVertex* doSelectVertexHover(const SelectionFrust& sfrust,ViewportCamera* camera, bool select, kkScene3DObject** );
	//void selectEdges(kkRay* ray);
	bool selectEdges(/*CursorRay* cursorRay, */kkRay* currentRay/*, int depth*/);
	void selectPolygons(kkRay* ray);

	void deleteObject( kkScene3DObject* );
};


#endif