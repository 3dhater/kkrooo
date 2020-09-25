// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SCENE3D_OBJECT_IMPL_H__
#define __SCENE3D_OBJECT_IMPL_H__

#include <string>

#include "Classes/Containers/kkArray.h"
#include "Classes/kkColor.h"
#include "Scene3D/kkScene3DObject.h"
#include "Material/kkMaterial.h"

struct DefaultVert
{
	v3f Position;
	v2f TCoords;
	v3f Normal;
	v4f Color;
};

struct LineModelVertex
{
	v3f _position;
	v4f _color;
};

//struct Point2DInformation
//{
//	v2i m_2d_coords;
//	s32 m_ID = -1; //дополнительная информация для 2D точек
//
//	void operator=(int i)
//	{
//		m_ID = -1;
//	}
//	void operator=(const Point2DInformation& other)
//	{
//		m_2d_coords = other.m_2d_coords;
//		m_ID = other.m_ID;
//	}
//};

struct kkSMesh;
class kkMesh;
class PolygonalModel;
class Scene3D;
class kkGraphicsSystem;
class PluginGUIWindow;

class Scene3DObjectCommon : public kkScene3DObject
{
protected:
	kkScene3DObjectType m_scene3DObjectType = kkScene3DObjectType::PolygonObject;
	kkString  m_name;
	kkVector4 m_pivot;

	// для GUI
	kkVector4 m_rotationPitchYawRoll;
	kkVector4 m_rotationAngles;
	kkVector4 m_scale;

	// значение равное m_pivot но не изменияется
	// его нужно сохранить вызвав applyPivot
	// используется тогда когда необходимо не изменять оригинальный m_pivot
	kkVector4 m_pivotFixed;

	// Нужен для перемещения точки m_pivot
	// При перемещении m_pivot нежелательно изменять координаты вершин.
	// Проще изменить это значение
	// При экспорте, нужно уметь применять этот вектр.
	// При перемещении объекта, изменяем m_pivot
	// Вершины остаются на месте, изменения происходят в матрице
	// Если объект не трогали, но изменили позицию m_pivot (поменяли m_pivotOffset), то нужно вычислить правильную позицию вершин (при экспорте)
	kkVector4 m_pivotOffset;

	// матрица для вращения и перемещения
	// но нужно как-то отменять изменение
	// для отмены нужно сохранять значение. пусть она сохранится в m_matrixFixed
	kkMatrix4 m_matrix;
	kkMatrix4 m_matrixFixed;

	// вращение без масштаба
	kkMatrix4 m_matrixOnlyRotation;
	kkMatrix4 m_matrixOnlyRotationFixed;
	
	// обычную матрицу которую используют как world не просто использовать для редактирования сетки
	// по этому world матрица создаётся отдельно
	kkMatrix4 m_worldMatrix;

	kkAabb    m_aabbOriginal;
	kkObb     m_obb;           // это m_aabbOriginal только повёрнутый в соответствии с m_matrix
	kkAabb    m_aabbModified;  // эта страится в соответствии с получившейся m_obb

	// если в режиме объекта то это точки для выбора рамкой sad
	// если в режиме вершин то это вершины в плоскости экрана
	/*kkString_base<v2i,kkDefault_allocator> m_pointsInScreen;
	kkString_base<v2i,kkDefault_allocator> m_pointsInScreen_IDs;*/ //дополнительная информация для 2D точек
	//kkString_base<Point2DInformation,kkDefault_allocator> m_pointsInScreen;

	kkMaterialImplementation * m_materialImplementation = nullptr;
	const char16_t* m_materialName = nullptr;

	kkColor m_edgeColor = kkColorWhite;
public:
	Scene3DObjectCommon(){
		m_scale.set(1.f,1.f,1.f,1.f);
	}
	virtual ~Scene3DObjectCommon()
	{
		if( m_parameterWindowData )
		{
			kkMemory::free(m_parameterWindowData);
			m_parameterWindowData = nullptr;
		}
	}

	const char16_t* GetName(){		return m_name.c_str();	}
	void SetMatrix(const kkMatrix4& m){ 	m_matrix = m; 	}
	kkAabb& Aabb(){ 	return m_aabbModified;  }
	kkObb&  Obb(){      return m_obb; 	}
	void ApplyFixedMatrix(){m_matrixFixed = m_matrix;}

	void UpdateAabb()
	{
		// предполагается, что m_aabbOriginal соответствует размерам модели без трансформаций (kkMatrix4(), то есть как бы её не крутили и масштабировали
		//   аабб всегда останется такой как при создании модели. Ведь сама модель такой и остаётся...вся магия в матрицах)
		// это означает что worldMatrix можно применять на значениях m_aabbOriginal и получить текущий m_obb
		// имея m_obb можно вычислить видимый m_aabbModified.

		//нужно написать функцию для рисования аабб


		/*
											//       v6-----------v2
											//      /|			/ |
											//	   / |         /  |
			v1 = m_min;						//	  /  |        /   |
			v2 = m_max;						//	 /   v3______/____v7
			v3.set( v1.x, v1.y, v2.z );		//	v5--/-------v8   /
			v4.set( v2.x, v1.y, v1.z );		//	|  /		|   /
			v5.set( v1.x, v2.y, v1.z );		//	| /			|  /
			v6.set( v1.x, v2.y, v2.z );		//	|/			| /
			v7.set( v2.x, v1.y, v2.z );		//	/			|/
			v8.set( v2.x, v2.y, v1.z );		//	v1----------v4
		*/
		m_obb.v1 = m_aabbOriginal.m_min;
		m_obb.v2 = m_aabbOriginal.m_max;
		m_obb.v3.set( m_obb.v1.KK_X, m_obb.v1.KK_Y, m_obb.v2.KK_Z );
		m_obb.v4.set( m_obb.v2.KK_X, m_obb.v1.KK_Y, m_obb.v1.KK_Z );		
		m_obb.v5.set( m_obb.v1.KK_X, m_obb.v2.KK_Y, m_obb.v1.KK_Z );		
		m_obb.v6.set( m_obb.v1.KK_X, m_obb.v2.KK_Y, m_obb.v2.KK_Z );		
		m_obb.v7.set( m_obb.v2.KK_X, m_obb.v1.KK_Y, m_obb.v2.KK_Z );		
		m_obb.v8.set( m_obb.v2.KK_X, m_obb.v2.KK_Y, m_obb.v1.KK_Z );	

		m_obb.v1.KK_W = 1.f;
		m_obb.v2.KK_W = 1.f;
		m_obb.v3.KK_W = 1.f;
		m_obb.v4.KK_W = 1.f;
		m_obb.v5.KK_W = 1.f;
		m_obb.v6.KK_W = 1.f;
		m_obb.v7.KK_W = 1.f;
		m_obb.v8.KK_W = 1.f;

		auto M = m_matrix;
		M[3].KK_X = m_pivot.KK_X;
		M[3].KK_Y = m_pivot.KK_Y;
		M[3].KK_Z = m_pivot.KK_Z;

		m_obb.v1 = math::mul(m_obb.v1, M);
		m_obb.v2 = math::mul(m_obb.v2, M);
		m_obb.v3 = math::mul(m_obb.v3, M);
		m_obb.v4 = math::mul(m_obb.v4, M);
		m_obb.v5 = math::mul(m_obb.v5, M);
		m_obb.v6 = math::mul(m_obb.v6, M);
		m_obb.v7 = math::mul(m_obb.v7, M);
		m_obb.v8 = math::mul(m_obb.v8, M);
		
		m_aabbModified.reset();
		m_aabbModified.add(m_obb.v1);
		m_aabbModified.add(m_obb.v2);
		m_aabbModified.add(m_obb.v3);
		m_aabbModified.add(m_obb.v4);
		m_aabbModified.add(m_obb.v5);
		m_aabbModified.add(m_obb.v6);
		m_aabbModified.add(m_obb.v7);
		m_aabbModified.add(m_obb.v8);
	}

	kkScene3DObjectType GetType(){		return m_scene3DObjectType;	}
	kkVector4& GetPivot(){ return m_pivot; }
	kkVector4& GetPivotFixed(){ return m_pivotFixed; }
	void RestorePivot(){ m_pivot = m_pivotFixed; }
	void ApplyPivot(){   m_pivotFixed = m_pivot; }
	kkMatrix4& GetMatrix(){		return m_matrix;	}
	kkMatrix4& GetMatrixWorld(){	return m_worldMatrix;	}
	
	kkVector4& GetRotationPitchYawRoll(){return m_rotationPitchYawRoll;}
	kkVector4& GetRotationAngles(){return m_rotationAngles;}
	kkVector4& GetScale(){return m_scale;}

	void UpdateWorldMatrix()
	{
		auto R = m_matrix;
		R[ 3u ].KK_X = 0.f;
		R[ 3u ].KK_Y = 0.f;
		R[ 3u ].KK_Z = 0.f;

		kkMatrix4 T;
		T[ 3u ].KK_X = m_pivot.KK_X;
		T[ 3u ].KK_Y = m_pivot.KK_Y;
		T[ 3u ].KK_Z = m_pivot.KK_Z;

		m_worldMatrix = R;

		m_worldMatrix = T * m_worldMatrix;
	}


	void * m_parameterWindowData = nullptr;

	void* AllocateParametersWindowData(u32 size)
	{
		if( m_parameterWindowData )
			kkMemory::free(m_parameterWindowData);
		m_parameterWindowData = kkMemory::allocate(size);
		return m_parameterWindowData;
	}

	void* DropParametersWindowData()
	{
		auto ptr = m_parameterWindowData;
		m_parameterWindowData = nullptr;
		return ptr;
	}

	void SetParametersWindowData(void* d)
	{
		m_parameterWindowData = d;
	}

	void SetMaterialImplementation( kkMaterialImplementation * material ){ m_materialImplementation = material; }
	kkMaterialImplementation* GetMaterialImplementation(){ return m_materialImplementation;}
	void SetMaterial( const char16_t* name )
	{ 
		//printf("SetMaterialName:\n");
		m_materialName = name; 
	};
	bool IsMaterial( const char16_t* name ){ if(!name)return false; if(!m_materialName) return false; 
		return kkString(name) == kkString(m_materialName); };
	const char16_t* GetMaterial(){return m_materialName;};

};

// вся настройка графики объекта для realtime рендеринга здесь
struct Scene3DObjectShaderParameter
{
	kkColor     m_diffuseColor = kkColorWhite;
	kkImageContainerNode* m_diffuseTexture = nullptr;

	const char16_t* m_materialName_ptr = nullptr;

};

struct Edge;
class Scene3DObject : public Scene3DObjectCommon
{
	static Scene3DObjectShaderParameter m_globalShaderParameter;
	Scene3DObjectShaderParameter m_shaderParameter;

	bool m_isSelected = false;

	//bool m_isEdgesCreated = false;
	//std::vector<Edge*> m_edges;

	PolygonalModel*  m_polyModel       = nullptr;

	// в будущем нужно умень разбить model на множество m_HardwareModel m_SoftwareModel
	// текущий вариант только для демонстрации
	kkArray<kkMesh *>  m_HardwareModels_polys;
	kkArray<kkMesh *>  m_HardwareModels_edges;
	kkArray<kkMesh *>  m_HardwareModels_points;
	
	kkArray<kkSMesh*>  m_SoftwareModels_polys;
	kkArray<kkSMesh*>  m_SoftwareModels_edges;
	kkArray<kkSMesh*>  m_SoftwareModels_points;

	enum _NEW_SOFTWARE_MODEL_TYPE
	{
		ENSMT_TRIANGLES,
		ENSMT_LINES,
		ENSMT_POINTS
	};

	void       _destroyHardwareModels();
	void       _destroyHardwareModels_edges();
	void       _destroySoftwareModels();
	void       _destroySoftwareModels_edges();
	void       _destroyHardwareModels_polys();
	void       _destroySoftwareModels_polys();
	kkSMesh *  _createNewSoftwareModel( _NEW_SOFTWARE_MODEL_TYPE );

	// надо подсчитать сколько отрезков линий нужно чтобы нарисовать рёбра полигона
	void _countNumOfLines();
	u32 m_numOfLines = 0;

	void _createSoftwareModel_polys();
	void _createSoftwareModel_edges();
	void _createSoftwareModel_points();

	bool _createHardwareModel_edges();
	bool _createHardwareModel_polys();

	bool _buildModel_polymodel();
	bool _rebuildModel();

	const u32 m_triLimit = 5000;
	const u32 m_lineLimit = 1500;
	const u32 m_pointLimit = 1500;

	f32 m_distanceToCamera = 0.f;

	kkGraphicsSystem * m_GS       = nullptr;
	Scene3D *          m_scene3D  = nullptr;

	PluginGUIWindow*   m_paramsWindow = nullptr;

	friend class RenderManager;
	friend class MaterialEditor;
	friend class Scene3D;
	friend class ViewportObject;
	friend class Application;
	friend void Scene3DObject_isRayIntersect( 
		int* out_result,
		int* stop_flag,
		kkPolygon* begin,
		kkPolygon* end,
		Scene3DObject* object,
		kkRay* ray,
		kkRayTriangleIntersectionResultSimple* iResult,
		kkRayTriangleIntersectionAlgorithm alg
		);

	// нужно знать, выделены ли вершины или другие вещи или нет
	bool m_isObjectHaveSelectedVerts = false;
	bool m_isObjectHaveSelectedEdges = false;
	bool m_isObjectHaveSelectedPolys = false;
	
public:
	Scene3DObject(kkScene3DObjectType, PolygonalModel *);
	virtual ~Scene3DObject();

	void SetName( const char16_t * );
	kkPolygonalModel* GetModel();

	bool IsRayIntersect( const kkRay& ray, kkRayTriangleIntersectionResultSimple& result, kkRayTriangleIntersectionAlgorithm alg = kkRayTriangleIntersectionAlgorithm::MollerTrumbore );
	bool IsRayIntersectMany( const kkRay& ray, std::vector<kkRayTriangleIntersectionResultSimple>& result, kkRayTriangleIntersectionAlgorithm alg = kkRayTriangleIntersectionAlgorithm::MollerTrumbore );

	void PrepareForRaytracing(kkRenderInfo*);
	void RayTest( std::vector<kkTriangleRayTestResult>& outTriangles, const kkRay& ray, kkMaterialImplementation * renderObjectMaterial );
	void RayTestGrid( std::vector<kkTriangleRayTestResult>& outTriangles, const v2i& point, const kkRay& ray, kkMaterialImplementation* renderObjectMaterial );
	void FinishRaytracing();
	
	//void UpdateScreenSpacePoints();
	void ApplyPosition();
	void RestorePosition();
	kkPluginGUIWindow* GetPluginGUIWindow();
	void SetPluginGUIWindow( kkPluginGUIWindow* w );

	// ========================
	bool init();
	f32 getDistanceToCamera() const { return m_distanceToCamera; }
	void moveVerts(const kkVector4&, std::unordered_set<kkVertex*>& );
	void rotateVerts(const kkMatrix4&, std::unordered_set<kkVertex*>&, const kkVector4& selectionCenter );
	void scaleVerts(const kkMatrix4&, std::unordered_set<kkVertex*>&, const kkVector4& selectionCenter );
	void generateNormals();
	void deleteSelectedVerts();
	void deleteSelectedEdges();
	void deleteSelectedPolys();
	bool  isSelected(){ return m_isSelected; }

	kkMesh    * getHardwareModel(u64);
	kkMesh    * getHardwareModel_lines(u64);
	kkMesh    * getHardwareModel_points(u64);
	u64       getHardwareModelCount();
	u64       getHardwareModelCount_lines(); // отдельные методы так как у линии  другие лимиты
	u64       getHardwareModelCount_points();
	void        updateModelPointsColors();

	void        setPosition(const kkVector4& p);
	void        updateAABB_vertex();

	//void createEdges();
	//void _deleteEdges(); // лучше держать рёбра созданными

	void applyMatrices();
	void resetMatrices();
	
	void updateEdgeModel();
	void updatePolygonModel();

	void UpdateMaterial()
	{
		if( m_materialImplementation )
		{
			setShaderParameter_diffuseColor(m_materialImplementation->GetDiffuseColor());
		}
	}

	void SaveShaderParameter()
	{
		m_shaderParameter.m_materialName_ptr = m_materialName;
		m_globalShaderParameter = m_shaderParameter;
	};
	void RestoreShaderParameter()
	{
		m_shaderParameter = m_globalShaderParameter;
		m_materialName = m_shaderParameter.m_materialName_ptr;
		/*if( m_materialName )
		{
			wprintf(L"m_materialName [%s]\n", m_materialName);
		}
		else
		{
			printf("nullptr\n");
		}*/
	};

	void setShaderParameter_diffuseColor( const kkColor& color );
	void setShaderParameter_diffuseTexture( kkImageContainerNode* texture );
	void ChangePivotPosition(const kkVector4& position);
	void SelecVertsByAdd();
	void SelecVertsBySub();
	void SelecPolygonsByAdd();
	void SelecEdgesByAdd();
	void SelecEdgesBySub();
	void SelecEdgesByRing();
	void SelecEdgesByLoop();

	void AttachObject(kkScene3DObject*);
	void BreakVerts();
	void ConnectVerts();
	void ChamferVerts(f32 len, bool addPolygon);
	void Weld(kkVertex* CV1, kkVertex* CV2);
	void WeldSelectedVerts(f32 len);
	void ConnectEdges();
};


#endif