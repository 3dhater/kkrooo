// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_MATERIAL_EDITOR_H__
#define __KK_MATERIAL_EDITOR_H__

#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"

#include "Material/kkMaterial.h"
#include "GraphicsSystem/kkTexture.h"
#include "GraphicsSystem/kkGraphicsSystem.h"

#include "Renderer/kkRenderer.h"


class Application;

class kkDefaultMaterial : public kkMaterialImplementation
{
public:
	kkDefaultMaterial();
	virtual ~kkDefaultMaterial();
};

struct MaterialParameter
{
	kkString m_name;
	kkRenderer* m_renderer = nullptr;
	kkMaterialNode_onCreateParameter m_onCreation = nullptr;
};

struct MaterialType
{
	kkString m_name;
	kkRenderer* m_renderer = nullptr;
	kkMaterialNode_onCreateType m_onCreation = nullptr;
};

enum MeterialEditorSocketFlag_
{
	MeterialEditorSocketFlag_inputMaterial  = 1,
	MeterialEditorSocketFlag_outputMaterial = 2,
	MeterialEditorSocketFlag_inputColor     = 4,
	MeterialEditorSocketFlag_outputColor    = 8,
	MeterialEditorSocketFlag_inputVector    = 16,
	MeterialEditorSocketFlag_outputVector   = 32,
	MeterialEditorSocketFlag_inputValue     = 64,
	MeterialEditorSocketFlag_outputValue    = 128,
};

struct MaterialEditorNodeCollection
{
	MaterialEditorNodeCollection(){}
	~MaterialEditorNodeCollection()
	{
		for( u64 i = 0, sz = m_nodes.size(); i < sz; ++i )
		{
			kkDestroy(m_nodes[i]);
		}
	}
	Kr::Gui::NodeEditor       m_nodeEditor;
	kkArray<kkMaterialEditorNode*> m_nodes; // все ноды находятся здесь. проще рисовать и удалять.
	
	// материалов может быть сколько угодно
	// но output соединяется только с одной.
	// вот это и должна быть она - точнее материал из этой ноды
	kkMaterialImplementation*    m_materialImplementation = nullptr;

	int                          m_uniquieIDs = 0; // уникальные ID для нод в текущем материале
	float                        m_nodeEditorZoom = 1.f;
	Kr::Gui::Vec2f               m_eyePosition;
	kkTexture*                   previewTexture_ptr = nullptr;
	kkString m_name;

	bool m_outputConnected = false;
};

struct RenderPreviewData
{
	kkRenderInfo renderInfo;
	kkMaterialEditorImage* materialEditorImage = nullptr;
};


struct ImageContainer
{
	ImageContainer();
	~ImageContainer();
	
	kkArray<kkImageContainerNode*> m_images;

	//kkImage* getImage(const char16_t* f, Application* app, kkGraphicsSystem* gs);
	kkImageContainerNode* getImage(const char16_t* f, Application* app, kkGraphicsSystem* gs);
	//kkImage* reloadImage(const char16_t* f, Application* app, kkGraphicsSystem* gs);
	void removeImage(kkImage*);
};

class PolygonalModel;
class MaterialEditor
{
	float m_redrawOutputTimer = 0.f;
	bool  m_redrawOutput = false;
	void  _redrawOutput();

	kkMainSystem * m_mainSystem = nullptr;
	float * m_deltaTime = nullptr;
	Kr::Gui::GuiSystem * m_gui = nullptr;
	kkGraphicsSystem * m_gs = nullptr;

	ImageContainer m_imageContainer;

	kkArray<MaterialEditorNodeCollection*> m_materials;
	kkMaterialImplementation m_bgMaterial;
	
	kkArray<MaterialParameter> m_materialParameterList;
	kkArray<MaterialType> m_materialTypeList;

	kkWindow * m_materialEditorWindow = nullptr;

	Application * m_app = nullptr;

	/*Kr::Gui::Group m_materialEditorMaterialsGroup;
	Kr::Gui::Group m_materialEditorParametersGroup;
	Kr::Gui::Group m_materialEditorListGroup;*/
	Kr::Gui::Style m_materialEditorListGroupStyle;
	Kr::Gui::Style m_materialEditorParametersGroupStyle;
	Kr::Gui::Style m_guiStyle_materialTypesRendererText;
	Kr::Gui::Style m_guiStyle_nodeEditor;
	Kr::Gui::Style m_guiStyle_materialList_iconButtons;
	Kr::Gui::Style m_guiStyle_materialList_normalButtons;
	Kr::Gui::Style m_guiStyle_socketMaterial;
	Kr::Gui::Style m_guiStyle_socketColor;
	Kr::Gui::Style m_guiStyle_socketVector;
	Kr::Gui::Style m_guiStyle_socketValue;

	Scene3DObject* m_ballObject = nullptr;
	Scene3DObject* m_bgObject = nullptr;

	//kkMatrix4 m_viewMatrix;
	//kkMatrix4 m_projectionMatrix;//Little;
	//kkMatrix4 m_VPinvert;//Little;

	s32 m_selectedMaterialIndex = -1;

	//kkImage*   m_checkerboard8x8_image = nullptr;
	kkImageContainerNode* m_checkerboard8x8 = nullptr;


	kkArray<kkScene3DObject*> m_renderObjects;
	RenderPreviewData m_renderPreviewData;

	void _updateObjectsMaterialParameters();
	void _assignCurrentMaterialToSelectedObjects();

	void _clearOutputImage();
	void _createNode(kkMaterialEditorNode* newNode, bool isMaterial, kkRenderer* renderer);
	void _createParameterNode(MaterialParameter&);
	void _createMaterialTypeNode(MaterialType& type);
	void _addDeleteNodeButton(kkMaterialEditorNode*);
	void _doSocketWork( kkMaterialEditorNode* currentNode, MaterialEditorNodeCollection* selectedMaterial, Kr::Gui::NodeEditor* nodeEditor );

public:
	MaterialEditor();
	~MaterialEditor();

	bool init( kkGraphicsSystem *, Kr::Gui::GuiSystem * , kkWindow * );

	void addMaterialParameter(const char16_t * name, kkRenderer* r, kkMaterialNode_onCreateParameter onCreation);
	void addMaterialType(const char16_t * name, kkRenderer* e, kkMaterialNode_onCreateType onCreation );

	size_t getMaterialCount();
	MaterialEditorNodeCollection* getMaterial( u64 index );

	void drawWindow();

	void createNewMaterial();
	void selectMaterial(int id);
	void deleteMaterial(int id);

	void setRenderObjectMaterial(kkMaterialImplementation*);

	void onSocket();

	//kkImage* loadImage(const char16_t* f);
	kkImageContainerNode* loadImage(const char16_t* f);
	void removeImage(kkImageContainerNode* i);
	void reloadImage(kkImageContainerNode* i);
	kkTexture* getTexture(kkImage* i);
	void onPickSocketInput(Kr::Gui::NodeEditorNodeSocket*, Kr::Gui::NodeEditorNode*);
};

#endif