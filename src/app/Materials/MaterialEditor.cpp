#include "kkrooo.engine.h"

#include "Classes/Math/kkMath.h"
#include "KrGui.h"
#include "Geometry/kkPolygonalModel.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Plugins/Plugin.h"
#include "../Application.h"
#include "MaterialEditor.h"
#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"
#include "../Geometry/GeometryCreator.h"
#include "../Geometry/PolygonalModel.h"
#include "../Functions.h" 
#include "Material/kkMaterial.h"
#include "kkMaterialTypeImpl.h"
#include "Renderer/kkRenderer.h"

using namespace Kr;
constexpr float g_nodeRounding = 10.f;

class kkMaterialEditorNodeEditorNodeImpl : public kkMaterialEditorNodeEditorNode
{
public:
	kkMaterialEditorNodeEditorNodeImpl(){}
	virtual ~kkMaterialEditorNodeEditorNodeImpl(){}

	Kr::Gui::NodeEditorNode m_node;
};

class kkMaterialEditorNodeImpl : public kkMaterialEditorNode
{
public:
	kkMaterialEditorNodeImpl(){}
	virtual ~kkMaterialEditorNodeImpl(){}

	virtual void CalculateValues(){}

	virtual void SetInSocketValue( int socketId, float floatValue ){}
	virtual void SetInSocketValue( int socketId, const v3f& vectorValue ){}
	virtual void SetInSocketValue( int socketId, const kkColor& colorValue ){}
	virtual void SetInSocketValue( int socketId, kkImageContainerNode* image ){};

	virtual void GetOutSocketValue( int socketId, float& floatValue ){}
	virtual void GetOutSocketValue( int socketId, v3f& vectorValue ){}
	virtual void GetOutSocketValue( int socketId, kkColor& colorValue ){}
	virtual void GetOutSocketValue( int socketId, kkImageContainerNode** image ){};

	virtual void ResetSocketValue( int socketId ){};
};

MaterialEditorNodeCollection* g_currentNodeCollection = nullptr;
MaterialEditor *    g_materialEditor  = nullptr;
kkDefaultMaterial * g_defaultMaterial = nullptr;

kkDefaultMaterial::kkDefaultMaterial(){}
kkDefaultMaterial::~kkDefaultMaterial(){}

void ClearPreviewImage(kkMaterialEditorImage* MEimage)
{
	MEimage->image.fill(kkColorBlack);
	MEimage->texture->fillNewData(MEimage->image.m_data8);
}

bool NodeEditorCallback(
	Gui::NodeEditor* nodeEditor,
	Gui::NodeEditor::callbackReason reason, 
	Gui::NodeEditorNode* firstNode, 
	Gui::NodeEditorNodeSocket* socket1, 
	Gui::NodeEditorNode* secondNode, 
	Gui::NodeEditorNodeSocket* socket2
)
{

	switch(reason)
	{
	case Gui::NodeEditor::_pickSocket:
	{
		if( socket1->flags & MeterialEditorSocketFlag_inputMaterial )
		{
			nodeEditor->removeSocketConnection(socket1);

			g_materialEditor->onSocket();
		}
		else if( socket1->flags & MeterialEditorSocketFlag_inputColor )
		{
			g_materialEditor->onPickSocketInput(socket1, firstNode);
			nodeEditor->removeSocketConnection(socket1);
			g_materialEditor->onSocket();
		}
	}
	break;
	case Gui::NodeEditor::_socketsConnected:
	{
		//printf("_socketsConnected\n");
		g_materialEditor->onSocket();
	}break;
	case Gui::NodeEditor::_connectSockets:
	{
		if( firstNode != secondNode )
		{
			if( socket1->flags & MeterialEditorSocketFlag_outputMaterial && socket2->flags & MeterialEditorSocketFlag_outputMaterial )
				return false;
			if( socket1->flags & MeterialEditorSocketFlag_inputMaterial && socket2->flags & MeterialEditorSocketFlag_inputMaterial )
				return false;

			Gui::NodeEditorNodeSocket * outputNodeSocket   = nullptr;
			if( socket1->flags & MeterialEditorSocketFlag_inputMaterial && socket2->flags & MeterialEditorSocketFlag_outputMaterial )
			{
				outputNodeSocket   = socket1;
			}
			if( socket1->flags & MeterialEditorSocketFlag_inputColor && socket2->flags & MeterialEditorSocketFlag_outputColor )
			{
				outputNodeSocket   = socket1;
			}
			
			if( socket1->flags & MeterialEditorSocketFlag_outputMaterial && socket2->flags & MeterialEditorSocketFlag_inputMaterial )
			{
				outputNodeSocket   = socket2;
			}
			if( socket1->flags & MeterialEditorSocketFlag_outputColor && socket2->flags & MeterialEditorSocketFlag_inputColor )
			{
				outputNodeSocket   = socket2;
			}

			if( outputNodeSocket )
			{
				nodeEditor->removeSocketConnection(outputNodeSocket);
				return true;
			}

			return false;
		}
		else
		{
			return false;
		}
	}
		break;
	case Gui::NodeEditor::_removeConnection:
	{
	}break;
	default:
		break;
	}
	return true;
};

MaterialEditor::MaterialEditor()
{
	m_mainSystem = kkGetMainSystem();
	m_deltaTime = m_mainSystem->getDeltaTime();
}
MaterialEditor::~MaterialEditor()
{
	//if( m_checkerboard8x8_image ) kkDestroy(m_checkerboard8x8_image);
	if( m_checkerboard8x8 )
	{
		if( m_checkerboard8x8->m_image ) kkDestroy(m_checkerboard8x8->m_image);
		if( m_checkerboard8x8->m_texture ) kkDestroy(m_checkerboard8x8->m_texture);
		kkDestroy(m_checkerboard8x8);
	}

	if( g_defaultMaterial )
		kkDestroy(g_defaultMaterial);
	g_defaultMaterial = nullptr;

	//m_ballObject->finishRaytracing();
	//m_bgObject->finishRaytracing();

	if( m_ballObject ) kkDestroy( m_ballObject );
	if( m_bgObject )   kkDestroy( m_bgObject );

	for( auto m : m_materials )
	{
		kkDestroy( m );
	}
}

void MaterialEditor::setRenderObjectMaterial(kkMaterialImplementation* m)
{
	if( m )
	{
		m_ballObject->SetMaterialImplementation(m);
	}
	else
	{
		m_ballObject->SetMaterialImplementation(g_defaultMaterial);
	}
}

size_t MaterialEditor::getMaterialCount(){	return m_materials.size();}
MaterialEditorNodeCollection* MaterialEditor::getMaterial( u64 index ){	return m_materials[index];}
void MaterialEditor::addMaterialParameter(const char16_t * name, kkRenderer* r, kkMaterialNode_onCreateParameter onCreation)
{
	MaterialParameter param;
	param.m_name = name;
	param.m_renderer = r;
	param.m_onCreation = onCreation;
	m_materialParameterList.push_back(param);
}

void MaterialEditor::addMaterialType(const char16_t * name, kkRenderer* r, kkMaterialNode_onCreateType onCreation )
{
	MaterialType type;
	type.m_name = name;
	type.m_renderer = r;
	type.m_onCreation = onCreation;
	m_materialTypeList.push_back(type);
}

bool MaterialEditor::init( kkGraphicsSystem * gs, Gui::GuiSystem * gui, kkWindow * window )
{
	g_materialEditor  = this;
	m_app = kkSingleton<Application>::s_instance;
	m_materialEditorWindow = window;
	m_gs = gs;
	m_gui = gui;

	m_checkerboard8x8 = kkCreate<kkImageContainerNode>();
	m_checkerboard8x8->m_image = m_app->loadImage(u"../res/gui/checkerboard.png");
	if(!m_checkerboard8x8->m_image)
	{
		KK_PRINT_FAILED;
		return false;
	}

	auto scene = m_app->getScene3D();

	m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/material_ball.obj");
	m_app->m_OBJplugin->call_onImportMesh(m_app->getPluginCommonInterface(),u"../res/3d/material_bg.obj");
	auto & objects = (*scene)->getObjects();
	for( auto o : objects )
	{
		kkString n( o->GetName() );
		if( n == "material_ball" ) m_ballObject = o;
		if( n == "material_bg" ) m_bgObject = o;
	}
	(*scene)->unregisterObject(m_ballObject);
	(*scene)->unregisterObject(m_bgObject);

	

//	m_lightPos.set(-1.3f,4.8f,1.8f,1.f);
	

	/*m_materialEditorListGroup.m_size  = Gui::Vec2f(262,680);
	m_materialEditorListGroup.m_style = &m_materialEditorListGroupStyle;
	
	m_materialEditorParametersGroup.m_style = &m_materialEditorParametersGroupStyle;
	m_materialEditorMaterialsGroup.m_style = &m_materialEditorParametersGroupStyle;*/
	m_materialEditorParametersGroupStyle.groupBackgroundAlpha = 0.1f;

	m_materialEditorParametersGroupStyle.groupHoverColor1 = 0xff666666;
	m_materialEditorParametersGroupStyle.groupHoverColor2 = m_materialEditorParametersGroupStyle.groupHoverColor1;

	m_materialEditorListGroupStyle = m_materialEditorParametersGroupStyle;
    m_materialEditorListGroupStyle.groupBackgroundAlpha = 0.1f;
	m_materialEditorListGroupStyle.rectangleIdleColor1 = 0xff999999;
	m_materialEditorListGroupStyle.rectangleIdleColor2 = 0xff999999;
	m_materialEditorListGroupStyle.rectangleHoverColor1 = 0xff838383;
	m_materialEditorListGroupStyle.rectangleHoverColor2 = 0xff838383;

	m_guiStyle_materialTypesRendererText.commonTextColor = Gui::ColorRed;
	m_guiStyle_materialTypesRendererText.commonTextSpacing = 0.f;
	m_guiStyle_materialList_iconButtons.buttonTextIdleColor = Gui::ColorWhite;
	m_guiStyle_materialList_iconButtons.buttonTextHoverColor = Gui::ColorRed;
	m_guiStyle_materialList_iconButtons.buttonTextPushColor = Gui::ColorDarkRed;
	m_guiStyle_materialList_iconButtons.buttonBackgroundAlpha = 0.f;

	m_guiStyle_materialList_normalButtons.buttonIdleColor2  = m_guiStyle_materialList_normalButtons.buttonIdleColor1;
	m_guiStyle_materialList_normalButtons.buttonHoverColor2 = m_guiStyle_materialList_normalButtons.buttonHoverColor1;
	m_guiStyle_materialList_normalButtons.buttonPushColor2 = m_guiStyle_materialList_normalButtons.buttonPushColor1;

	m_guiStyle_socketMaterial.nodeEditorSocketIdleColor = Gui::ColorGreenYellow;
	m_guiStyle_socketColor.nodeEditorSocketIdleColor = Gui::ColorYellow;
	m_guiStyle_socketValue.nodeEditorSocketIdleColor = Gui::ColorGray;
	m_guiStyle_socketVector.nodeEditorSocketIdleColor = Gui::ColorBlue;
	
	m_guiStyle_nodeEditor.nodeEditorNodeColor1.w = 0.8f;
	m_guiStyle_nodeEditor.nodeEditorNodeColor2.w = 0.8f;

	m_gs->setLinearFilter(false);

	kkCamera * camera = kkGetSceneSystem()->createCamera();
	camera->setCameraType(kkCameraType::Free);
	camera->setFOV(math::degToRad(50.f));
	camera->setPosition(kkVector4(1.f, 2.f, 1.f, 1.f));
	camera->setRotation(kkVector4(math::degToRad(-20.f), 0.f, 0.f, 0.f));
	camera->setAspect(1.f);
	camera->update();

	m_renderPreviewData.renderInfo.VPInvert = camera->getViewProjectionInvertMatrix();
	m_renderPreviewData.renderInfo.VP       = camera->getViewProjectionMatrix();
	m_renderPreviewData.renderInfo.V        = camera->getViewMatrix();
	m_renderPreviewData.renderInfo.P        = camera->getProjectionMatrix();
	kkDestroy(camera);

	m_renderObjects.push_back(m_bgObject);
	m_renderObjects.push_back(m_ballObject);

	m_renderPreviewData.renderInfo.objects = &m_renderObjects;

	g_defaultMaterial = kkCreate<kkDefaultMaterial>();
	g_defaultMaterial->SetOpacity(1.0f);
	m_ballObject->SetMaterialImplementation(g_defaultMaterial);
	m_bgObject->SetMaterialImplementation(&m_bgMaterial);
	
	m_bgMaterial.SetDiffuseTexture(m_checkerboard8x8);

	return true;
}

void MaterialEditor::drawWindow()
{
	int deleteMaterialId = -1;

	m_gui->setScrollMultipler(32.f);
	m_gui->switchWindow(&m_app->m_guiMaterialEditorWindow);

	auto windowClientRect = m_materialEditorWindow->getClientRect();
	float windowH = float(windowClientRect.w - windowClientRect.y);
	float windowW = float(windowClientRect.z - windowClientRect.x);
	float windowH_half = windowH*0.5f;

	auto materialListWidth = 200.f;
	auto materialNodeListWidth = 170.f;
	auto materialNodeListHIndent = 15.f;
	auto materialParamsListHIndent = 15.f;
	/*m_materialEditorListGroup.m_size.set(materialListWidth,windowH-30.f);
	m_materialEditorMaterialsGroup.m_size.set(materialNodeListWidth, windowH_half-materialNodeListHIndent);
	m_materialEditorParametersGroup.m_size.set(materialNodeListWidth, windowH_half-materialParamsListHIndent);*/
	
	MaterialEditorNodeCollection* selectedMaterial = nullptr;
	if( m_selectedMaterialIndex != -1 )
	{
		selectedMaterial = m_materials[m_selectedMaterialIndex];
	}

    if(m_gui->beginGroup(Gui::Vec2f(materialListWidth,windowH-30.f), 0, &m_materialEditorListGroupStyle))
    {
		auto sz = m_materials.size();
		float contentHeight = 0.f;
		for( u64 i = 0; i < sz; ++i )
		{
			auto M = m_materials[i];
			
			m_gui->setNextItemIgnoreInput();
			m_gui->addPictureBox(Gui::Vec2f(32,32), (unsigned long long)M->previewTexture_ptr->getHandle());

			auto oldDrawPoint = m_gui->getDrawPointPosition();
			m_gui->addRectangle( &m_materialEditorListGroupStyle ,Gui::Vec2f(168,32), 1.f, Gui::Vec4f() );
			m_gui->setDrawPointPosition(oldDrawPoint.x,oldDrawPoint.y);
			if( m_gui->addButton(0, 0, Gui::Vec2f(167,15), true, true, Gui::Vec4f(4.f,4.f,4.f,4.f) ) )
			{
				m_selectedMaterialIndex = (s32)i;
				selectMaterial((s32)i);
			}
			
			auto oldFont = m_gui->getCurrentFont();
			m_gui->setCurrentFont(m_app->m_iconsFont);
			m_gui->setDrawPointPosition(oldDrawPoint.x+154,oldDrawPoint.y+17);
			if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::Cross1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(12,12), true,  true, Gui::Vec4f(4.f,4.f,4.f,4.f) ) )
			{
				deleteMaterialId = (s32)i;
			}
			if( selectedMaterial == M )
			{
				m_gui->setDrawPointPosition(oldDrawPoint.x,oldDrawPoint.y+14);
				if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::AssignMaterial), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(18,15),  true, true, Gui::Vec4f(4.f,4.f,4.f,4.f) ) )
				{
					_assignCurrentMaterialToSelectedObjects();
				}
			}
			m_gui->setCurrentFont(oldFont);

			m_gui->newLine(3);

			auto endDrawPoint = m_gui->getDrawPointPosition();

			m_gui->setDrawPointPosition(oldDrawPoint.x,oldDrawPoint.y);

			m_gui->setCurrentFont(m_app->m_smallFont);
			static Gui::Style nameStyle;
			
			if( m_selectedMaterialIndex == i )
				nameStyle.commonTextColor = Gui::ColorRed;
			else	
				nameStyle.commonTextColor = Gui::ColorBlack;

			m_gui->setNextItemIgnoreInput();
			m_gui->addText(M->m_name.data(),&nameStyle);
			m_gui->setCurrentFont(nullptr);

			m_gui->setDrawPointPosition(endDrawPoint.x,endDrawPoint.y);
			m_gui->newLine();

			contentHeight += 32.f;
		}
		m_gui->setCurrentGroupContentHeight(contentHeight);
        m_gui->endGroup();
    }
	/*m_gui->newLine(5.f);
	m_gui->moveLeftRight(15.f);*/
	m_gui->setDrawPointPosition(windowClientRect.x + 15.f, windowClientRect.w - 25.f);
	if( m_gui->addButton(u"New material", 0, Gui::Vec2f(170, 20),true,true,Gui::Vec4f(4.f,4.f,4.f,4.f)) )
	{
		createNewMaterial();
	}


    m_gui->setDrawPointPosition(materialListWidth,0);
	if( selectedMaterial )
	{
		selectedMaterial->m_nodeEditor.m_size.x = windowW - materialListWidth - materialNodeListWidth;
		selectedMaterial->m_nodeEditor.m_size.y = windowH;
	
		selectedMaterial->m_nodeEditor.m_zoomValue   = selectedMaterial->m_nodeEditorZoom;
		selectedMaterial->m_nodeEditor.m_eyePosition = selectedMaterial->m_eyePosition;
	}

	if( selectedMaterial )
	{
		if( m_gui->beginNodeEditor(&selectedMaterial->m_nodeEditor) )
		{
			for( auto n : selectedMaterial->m_nodes )
			{
				auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)n->GetKrGuiNode();
				if( m_gui->beginNode(&krgui_node->m_node) )
				{
					static Gui::Style nodeHeadStyle;

					switch(n->GetNodeType())
					{
						case kkMaterialEditorNodeType::_output:
						nodeHeadStyle.rectangleIdleColor1.x = 0.8f;
						nodeHeadStyle.rectangleIdleColor1.y = 0.1f;
						nodeHeadStyle.rectangleIdleColor1.z = 0.1f;
						break;
						default:
						nodeHeadStyle.rectangleIdleColor1.x = n->GetColorHead().getRed();
						nodeHeadStyle.rectangleIdleColor1.y = n->GetColorHead().getGreen();
						nodeHeadStyle.rectangleIdleColor1.z = n->GetColorHead().getBlue();
						break;
					}
					nodeHeadStyle.rectangleIdleColor2.x = 0.485f;

					auto halfSize_x = krgui_node->m_node.m_size.x * 0.5f;
					auto halfSize_y = krgui_node->m_node.m_size.y * 0.5f;

					m_gui->setNextItemIgnoreInput();
					m_gui->setCurrentNodeContentOffset(Gui::Vec2f(
						-halfSize_x, 
						-halfSize_y-1.f
					));
					m_gui->addRectangle(&nodeHeadStyle, Gui::Vec2f(
						krgui_node->m_node.m_size.x,
						20.f
					), 0.2f, Gui::Vec4f(10.f,0.f,0.f,10.f));

					auto currentFont = m_gui->getCurrentFont();

					m_gui->setCurrentNodeContentOffset(Gui::Vec2f(-halfSize_x + 4.f, -halfSize_y+1.f));
					m_gui->setNextItemIgnoreInput();
					switch(n->GetNodeType())
					{
						case kkMaterialEditorNodeType::_parameter:
						case kkMaterialEditorNodeType::_material:
						m_gui->addText(n->GetName());
						m_gui->setCurrentFont(m_app->m_microFont);
						m_gui->setNextItemIgnoreInput();
						m_gui->setCurrentNodeContentOffset(Gui::Vec2f(-halfSize_x + 4.f, -halfSize_y+20.f));
						m_gui->addText(n->GetRenderer()->GetName(), &m_guiStyle_materialTypesRendererText);
						break;
						case kkMaterialEditorNodeType::_output:
						m_gui->addText(u"Output");
						break;
					}
					
					m_gui->setCurrentFont(currentFont);

					switch(n->GetNodeType())
					{
					case kkMaterialEditorNodeType::_material:
						break;
					case kkMaterialEditorNodeType::_output:
						m_gui->setNextItemIgnoreInput();
						m_gui->setCurrentNodeContentOffset(Gui::Vec2f(-85.f, -95.f));
						m_gui->addPictureBox(Gui::Vec2f(170,170), (unsigned long long)n->GetImage()->texture->getHandle());
						m_gui->setCurrentNodeContentOffset(Gui::Vec2f(-85.f, 91.f));
						m_gui->setNextItemIgnoreInput();
						m_gui->addText(u"Material");
						break;
					default:
					//	printf("IMPLEMENT!!! line %i in %s\n", KK_LINE, KK_FILE);
						break;
					}

					auto & guiElements = n->GetGUIElements();

					for( u64 i = 0, sz = guiElements.size(); i < sz; ++i )
					{
						auto * element = &guiElements[ i ];

						switch(element->m_type)
						{
						case kkMaterialEditorNodeGUIElementType::PictureBox:
						{
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							Gui::Style style;
							style.rectangleIdleColor1   = style.rectangleIdleColor1;
							style.rectangleIdleColor2   = style.rectangleIdleColor1;
							m_gui->setNextItemIgnoreInput();
							if( element->m_imageNodePtr)
							{
								if( element->m_imageNodePtr->m_texture )
								{
									m_gui->addPictureBox(Gui::Vec2f(element->m_size.x, element->m_size.y), (unsigned long long)element->m_imageNodePtr->m_texture->getHandle() );
								}
							}
							else
							{
								m_gui->addRectangle(&style,Gui::Vec2f(element->m_size.x, element->m_size.y), element->m_opacity );
							}
						}break;
						case kkMaterialEditorNodeGUIElementType::Rectangle:
						{
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							Gui::Style style;
							
							if( element->m_dynamicColor )
							{
								style.rectangleIdleColor1.x = element->m_dynamicColor->m_data[0];
								style.rectangleIdleColor1.y = element->m_dynamicColor->m_data[1];
								style.rectangleIdleColor1.z = element->m_dynamicColor->m_data[2];
							}
							else
							{
								style.rectangleIdleColor1.x = element->m_color.m_data[0];
								style.rectangleIdleColor1.y = element->m_color.m_data[1];
								style.rectangleIdleColor1.z = element->m_color.m_data[2];
							}

							style.rectangleIdleColor2   = style.rectangleIdleColor1;


							m_gui->setNextItemIgnoreInput();
							m_gui->addRectangle(&style,Gui::Vec2f(element->m_size.x, element->m_size.y), element->m_opacity, 
								Gui::Vec4f(element->m_rounding.x,element->m_rounding.y,element->m_rounding.z,element->m_rounding.w) );
						}break;
						case kkMaterialEditorNodeGUIElementType::RangeSliderFloat:
						{
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							if( m_gui->addRangeSlider( element->m_rangeFloat_min, element->m_rangeFloat_max, element->m_rangeFloat_value, 
								Gui::Vec2f(element->m_size.x, element->m_size.y), element->m_rangeHorizontal, 1.f, nullptr, Gui::Vec4f(5.f,5.f,5.f,5.f)) 
								)
							{
								if( element->m_callback )
								{
									element->m_callback(element);
									_updateObjectsMaterialParameters();
									_redrawOutput();
								}
							}
						}break;
						case kkMaterialEditorNodeGUIElementType::Text:
						{
							auto currentFont = m_gui->getCurrentFont();
							switch (element->m_font)
							{
							case kkGUITextFontType::Default:
							default:
								m_gui->setCurrentFont(nullptr);
								break;
							case kkGUITextFontType::Micro:
								m_gui->setCurrentFont(m_app->m_microFont);
								break;
							case kkGUITextFontType::Small:
								m_gui->setCurrentFont(m_app->m_smallFont);
								break;
							}
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							m_gui->setNextItemIgnoreInput();

							if( element->m_textCallback )
							{
								kkString s;
								element->m_textCallback(element, &s);
								m_gui->addText(s.data());
							}
							else
							{
								m_gui->addText(element->m_text);
							}
							m_gui->setCurrentFont(currentFont);
						}
							break;
						case kkMaterialEditorNodeGUIElementType::ButtonIcon:
						{
							auto currentFont = m_gui->getCurrentFont();
							m_gui->setCurrentFont(m_app->m_iconsFont);
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							if( m_gui->addButton(element->m_text,&m_guiStyle_materialList_iconButtons, Gui::Vec2f(element->m_size.x, element->m_size.y)) )
							{
								if( element->m_callback )
								{
									if( element->m_callback(element) )
									{
										_redrawOutput();
										_updateObjectsMaterialParameters();
										m_gui->setCurrentFont(currentFont);
										i = sz;
									}
								}
							}
							m_gui->setCurrentFont(currentFont);
							break;
						}
						case kkMaterialEditorNodeGUIElementType::Button:
						default:
						{
							m_gui->setCurrentNodeContentOffset(Gui::Vec2f(element->m_positionOffset.x, element->m_positionOffset.y));
							if( m_gui->addButton(element->m_text,&m_guiStyle_materialList_normalButtons, Gui::Vec2f(element->m_size.x, element->m_size.y), 
								true,  true, Gui::Vec4f(element->m_rounding.x,element->m_rounding.y,element->m_rounding.z,element->m_rounding.w) ) )
							{
								if( element->m_callback )
								{
									if( element->m_callback(element) )
									{
										_redrawOutput();
										_updateObjectsMaterialParameters();
										m_gui->setCurrentFont(currentFont);
										i = sz;
									}
								}
							}
							break;
						}
						}

					}
				
					m_gui->endNode();
				}
			}

		}
		m_gui->endNodeEditor();
	}

	if( selectedMaterial )
	{
		selectedMaterial->m_nodeEditorZoom = selectedMaterial->m_nodeEditor.m_zoomValue;
		selectedMaterial->m_eyePosition    = selectedMaterial->m_nodeEditor.m_eyePosition;
	}

    m_gui->setDrawPointPosition(materialListWidth+3,windowH-13);
	auto currentFont = m_gui->getCurrentFont();
	m_gui->setCurrentFont(m_app->m_iconsFont);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::Add1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(10.f,10.f))
		|| m_gui->isLastItemPressed() )
	{
		if( selectedMaterial )
		{
			m_gui->nodeEditorZoomIn(&selectedMaterial->m_nodeEditor, 5.0f * (*m_deltaTime) );
			selectedMaterial->m_nodeEditorZoom = selectedMaterial->m_nodeEditor.m_zoomValue;
		}
	}
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::Sub1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(10.f,10.f))
		|| m_gui->isLastItemPressed() )
	{
		if( selectedMaterial )
		{
			m_gui->nodeEditorZoomOut(&selectedMaterial->m_nodeEditor, 5.0f * (*m_deltaTime));
			selectedMaterial->m_nodeEditorZoom = selectedMaterial->m_nodeEditor.m_zoomValue;
		}
	}
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::Eq1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(10.f,10.f)) )
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_nodeEditorZoom = 1.f;
		}
	}
    m_gui->setDrawPointPosition(windowW-materialNodeListWidth-30,windowH-10);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::ArrowB1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(13.f,9.f))
		|| m_gui->isLastItemPressed() )
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_eyePosition.y -= 250.0f * (*m_deltaTime);
		}
	}
	m_gui->setDrawPointPosition(windowW-materialNodeListWidth-31,windowH-25);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::PushCircle), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(15.f,15.f)) )
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_eyePosition.x = 0.0f;
			selectedMaterial->m_eyePosition.y = 0.0f;
		}
	}
	m_gui->setDrawPointPosition(windowW-materialNodeListWidth-30,windowH-34);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::ArrowT1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(13.f,9.f)) 
		|| m_gui->isLastItemPressed() )
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_eyePosition.y += 250.0f * (*m_deltaTime);
		}
	}
	m_gui->setDrawPointPosition(windowW-materialNodeListWidth-40,windowH-24);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::ArrowL1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(9.f,13.f)) 
		|| m_gui->isLastItemPressed())
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_eyePosition.x += 250.0f * (*m_deltaTime);
		}
	}
	m_gui->setDrawPointPosition(windowW-materialNodeListWidth-16,windowH-24);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::ArrowR1), &m_guiStyle_materialList_iconButtons, Gui::Vec2f(9.f,13.f)) 
		|| m_gui->isLastItemPressed())
	{
		if( selectedMaterial )
		{
			selectedMaterial->m_eyePosition.x -= 250.0f * (*m_deltaTime);
		}
	}
	m_gui->setCurrentFont(currentFont);

    m_gui->setDrawPointPosition(windowW-materialNodeListWidth,0);
	m_gui->addText(u"Materials:");
    m_gui->setDrawPointPosition(windowW-materialNodeListWidth,materialNodeListHIndent);


//	if( m_materialEditorMaterialsGroup.m_inRect )
//		printf("%f\n",m_materialEditorMaterialsGroup.m_scrollValue);
    if(m_gui->beginGroup(Gui::Vec2f(materialNodeListWidth, windowH_half-materialNodeListHIndent), 0, &m_materialEditorParametersGroupStyle))
    {
		for( size_t i = 0, sz = m_materialTypeList.size(); i < sz; ++i )
		{
			auto & type = m_materialTypeList[i];
			auto drawPointer1 = m_gui->getDrawPointPosition();
			if( m_gui->addButton(type.m_name.data(), 0, Gui::Vec2f(materialNodeListWidth, 20),selectedMaterial ? true : false,
				true,Gui::Vec4f(4.f,4.f,4.f,4.f)))
			{
				if( type.m_onCreation )
				{
					_createMaterialTypeNode(type);
				}
			}
			m_gui->setDrawPointPosition(drawPointer1.x, drawPointer1.y);
			m_gui->setCurrentFont(m_app->m_microFont);
			m_gui->addText(type.m_renderer->GetName(), &m_guiStyle_materialTypesRendererText);
			m_gui->setCurrentFont(nullptr);
			m_gui->setDrawPointPosition(drawPointer1.x, drawPointer1.y);
			m_gui->newLine(22.5f);
		}

        m_gui->endGroup();
    }
    m_gui->setDrawPointPosition(windowW-materialNodeListWidth,windowH_half);
	m_gui->addText(u"Parameters:");
	m_gui->setDrawPointPosition(windowW-materialNodeListWidth,windowH_half+materialParamsListHIndent);
    if(m_gui->beginGroup(Gui::Vec2f(materialNodeListWidth, windowH_half-materialParamsListHIndent), 0, &m_materialEditorParametersGroupStyle))
    {
		for( size_t i = 0, sz = m_materialParameterList.size(); i < sz; ++i )
		{
			auto & param = m_materialParameterList[i];
			auto drawPointer1 = m_gui->getDrawPointPosition();
			if( m_gui->addButton(param.m_name.data(), 0, Gui::Vec2f(materialNodeListWidth, 20),selectedMaterial ? true : false,
				true,Gui::Vec4f(4.f,4.f,4.f,4.f)))
			{
				if( param.m_onCreation )
				{
					_createParameterNode(param);
				}
			}
			m_gui->setDrawPointPosition(drawPointer1.x, drawPointer1.y);
			m_gui->setCurrentFont(m_app->m_microFont);
			m_gui->addText(param.m_renderer->GetName(), &m_guiStyle_materialTypesRendererText);
			m_gui->setCurrentFont(nullptr);
			m_gui->setDrawPointPosition(drawPointer1.x, drawPointer1.y);
			m_gui->newLine(22.5f);
		}
        m_gui->endGroup();
    }
	if( deleteMaterialId != -1 )
	{
		deleteMaterial(deleteMaterialId);
	}

	if( m_redrawOutput )
	{
		m_redrawOutputTimer += *m_deltaTime;
		if( m_redrawOutputTimer > 0.1f )
		{
			onSocket();
			m_redrawOutput = false;
		}
	}
}

void MaterialEditor::_redrawOutput()
{
	m_redrawOutput = true;
	m_redrawOutputTimer = 0.f;
}

bool deleteNodeButtonCallback(kkMaterialEditorNodeGUIElementInfo* info )
{
	MaterialEditorNodeCollection* selectedMaterial = (MaterialEditorNodeCollection*)info->m_userData;
	for( u64 i = 0, sz = selectedMaterial->m_nodes.size(); i < sz; ++i )
	{
		auto node = selectedMaterial->m_nodes[i];
		auto & guiElements = node->GetGUIElements();
		for( u64 k = 0, ksz = guiElements.size(); k < ksz; ++k )
		{
			if( info->m_id == guiElements[k].m_id )
			{
				printf("Delete material node [%i]\n", info->m_id);

				auto & ge = node->GetGUIElements();
				for( auto & _ge : ge )
				{
					if( _ge.m_imageNodePtr )
					{
						g_materialEditor->removeImage(_ge.m_imageNodePtr);
					}
				}
				
				auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)node->GetKrGuiNode();

				for( size_t p = 0, psz = krgui_node->m_node.m_sockets.size(); p < psz; ++p )
				{
					auto * socket = &krgui_node->m_node.m_sockets[p];
					selectedMaterial->m_nodeEditor.removeSocketConnection(socket);
				}

				selectedMaterial->m_nodeEditor.removeNodeZOrdering(&krgui_node->m_node);

				kkDestroy(selectedMaterial->m_nodes[i]);
				selectedMaterial->m_nodes.erase( i );
				
				g_materialEditor->onSocket();

				return true;
			}
		}
	}
	return false;
}

// создание нового материала
void MaterialEditor::createNewMaterial()
{
	static int counter = 0;	 // для уникального имени. В будущем при добавлении возможности переименования, нужно сделать проверку уникальности

	// создание ноды
	auto node = kkCreate<kkMaterialEditorNodeImpl>();
	node->CreateImage(170,170); // картинка с превью
	node->GetImage()->image.fill(kkColorBlack);
	node->GetImage()->texture = m_gs->createTexture(&node->GetImage()->image);
	
	// настройки ноды для KrGui
	if( !node->GetGuiNodeEditorNode() )
	{
		node->SetGuiNodeEditorNode(kkCreate<kkMaterialEditorNodeEditorNodeImpl>());
	}
	auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)node->GetKrGuiNode();
	krgui_node->m_node.m_rounding.x = g_nodeRounding;
	krgui_node->m_node.m_rounding.y = g_nodeRounding;
	krgui_node->m_node.m_rounding.z = g_nodeRounding;
	krgui_node->m_node.m_rounding.w = g_nodeRounding;
	krgui_node->m_node.m_size.set(190, 255);

	
	
	// сокет material input
	Gui::NodeEditorNodeSocket socket;
	socket.position = Gui::Vec2f(-95.f, 100.f); // позиция относительно центра
	socket.size     = Gui::Vec2f(10.f,10.f);    
	socket.id       = 0;
	socket.flags    = MeterialEditorSocketFlag_inputMaterial;
	socket.style    = &m_guiStyle_socketMaterial;
	//socket.userData = &m_renderPreviewData;
	socket.parentNode = &krgui_node->m_node;
	krgui_node->m_node.addSocket(socket);
	
	// материал в редакторе материалов представляет собой экземпляр MaterialEditorNodeCollection
	MaterialEditorNodeCollection* nodeCollection = kkCreate<MaterialEditorNodeCollection>();
	nodeCollection->m_name += u"Default";
	nodeCollection->m_name += counter++;
	printf("Create: ");
	printf("%s\n",nodeCollection->m_name.to_kkStringA().data());

	nodeCollection->m_nodes.push_back( node );
	node->SetName(u"> Output <");

	// нужно иметь ввиду что устанавливается в userData
	// нужно иметь доступ к материалу из коллбэков
	// нужно убрать userData1
	krgui_node->m_node.m_userData = node;

	// материалу как элементу редактора материала указывается реальный материал который используется в рендеринге
	// если к оупут ноде ничего не присоединено, то должен стоять дефолтный материал
	// а значит нужно следить за присоединением - давать нужный материал и делать рендеринг превьюшки
	nodeCollection->m_materialImplementation     = g_defaultMaterial;
	
	//node->GetKrGuiNode()->userData1 = g_defaultMaterial;

	nodeCollection->m_nodeEditor.addNodeZOrdering(&krgui_node->m_node);

	m_materials.push_back(nodeCollection);
	
	//auto M = m_materials[m_materials.size()-1];
	//M->m_second->previewTexture_ptr = node->m_image->texture;
	nodeCollection->previewTexture_ptr = node->GetImage()->texture;

	if( m_selectedMaterialIndex == -1 )
		selectMaterial(0);
	
	//M->m_second->m_nodeEditor.m_style  = &m_guiStyle_nodeEditor;
	nodeCollection->m_nodeEditor.m_style  = &m_guiStyle_nodeEditor;
	//M->m_second->m_nodeEditor.callback = NodeEditorCallback;
	nodeCollection->m_nodeEditor.callback = NodeEditorCallback;
}

void MaterialEditor::selectMaterial(int id)
{
	m_selectedMaterialIndex = id;
	g_currentNodeCollection = m_materials[m_selectedMaterialIndex];
	m_renderPreviewData.materialEditorImage = m_materials[id]->m_nodes[0]->GetImage();
	m_renderPreviewData.renderInfo.image   = &m_renderPreviewData.materialEditorImage->image;
	setRenderObjectMaterial(g_currentNodeCollection->m_materialImplementation);
}

void MaterialEditor::deleteMaterial(int id)
{
	auto m = m_materials[id];

	printf("Delete material [%s]\n", m->m_name.to_kkStringA().data());

	for( auto n : m->m_nodes )
	{
		auto & guiElements = n->GetGUIElements();
		for( auto & ge : guiElements )
		{
			if( ge.m_imageNodePtr )
			{
				m_imageContainer.removeImage(ge.m_imageNodePtr->m_image);
			}
		}
	}

	auto & objects = m_app->m_current_scene3D->getObjects();
	for( auto o : objects )
	{
		if( o->IsMaterial(m->m_name.data()) )
		{
			o->SetMaterial(nullptr);
		}
	}

	kkDestroy( m_materials[id]);
	m_materials.erase(id);
	
	if( id < m_selectedMaterialIndex )
	{
		--m_selectedMaterialIndex;
		g_currentNodeCollection = m_materials[m_selectedMaterialIndex];
		setRenderObjectMaterial(g_currentNodeCollection->m_materialImplementation);
	}
	else if( id == m_selectedMaterialIndex )
	{
		m_selectedMaterialIndex = -1;
		g_currentNodeCollection = nullptr;
		setRenderObjectMaterial(nullptr);
	}
}

void MaterialEditor::_addDeleteNodeButton(kkMaterialEditorNode* node)
{
	auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)node->GetKrGuiNode();
	MaterialEditorNodeCollection* selectedMaterial = m_materials[m_selectedMaterialIndex];
	kkMaterialEditorNodeGUIElementInfo deleteButton;
	deleteButton.m_id = selectedMaterial->m_uniquieIDs++;
	deleteButton.m_text = kkrooo::getIconFontString(IconFontSymbol::Cross1);
	deleteButton.m_type = kkMaterialEditorNodeGUIElementType::ButtonIcon;
	deleteButton.m_callback = deleteNodeButtonCallback;
	deleteButton.m_userData = selectedMaterial;
	deleteButton.m_size.set(11.f,11.f);

	float nodeHalfSizeX = krgui_node->m_node.m_size.x * 0.5f;
	float nodeHalfSizeY = krgui_node->m_node.m_size.y * 0.5f;

	deleteButton.m_positionOffset.set( nodeHalfSizeX, -nodeHalfSizeY );
	deleteButton.m_positionOffset.x -= 15.f;
	deleteButton.m_positionOffset.y += 3.f;

	node->AddGUIElement(deleteButton);
}

void MaterialEditor::_createNode(kkMaterialEditorNode* newNode, bool isMaterial, kkRenderer* renderer)
{
	MaterialEditorNodeCollection* selectedMaterial = m_materials[m_selectedMaterialIndex];
	newNode->SetNodeType( isMaterial ? kkMaterialEditorNodeType::_material : kkMaterialEditorNodeType::_parameter );
	newNode->SetRenderer(renderer);
	
	selectedMaterial->m_nodes.push_back(newNode);
	
	if( !newNode->GetGuiNodeEditorNode() )
	{
		newNode->SetGuiNodeEditorNode(kkCreate<kkMaterialEditorNodeEditorNodeImpl>());
	}
	auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)newNode->GetKrGuiNode();
	krgui_node->m_node.m_rounding.x = g_nodeRounding;
	krgui_node->m_node.m_rounding.y = krgui_node->m_node.m_rounding.x;
	krgui_node->m_node.m_rounding.z = krgui_node->m_node.m_rounding.x;
	krgui_node->m_node.m_rounding.w = krgui_node->m_node.m_rounding.x;
	krgui_node->m_node.m_size.x = newNode->GetSize().x;
	krgui_node->m_node.m_size.y = newNode->GetSize().y;
	krgui_node->m_node.m_userData = newNode;
	krgui_node->m_node.m_position.set(-selectedMaterial->m_nodeEditor.m_eyePosition.x,-selectedMaterial->m_nodeEditor.m_eyePosition.y);

	selectedMaterial->m_nodeEditor.addNodeZOrdering(&krgui_node->m_node);


	float nodeHalfSizeX = krgui_node->m_node.m_size.x * 0.5f;
	float nodeHalfSizeY = krgui_node->m_node.m_size.y * 0.5f;

	auto nodeSockets = newNode->GetSockets();
	int id = 0;
	for( auto & s : nodeSockets )
	{
		float x_pos = nodeHalfSizeX;

		switch(s.m_type)
		{
		case kkMaterialEditorSocketType::ColorIn:
		case kkMaterialEditorSocketType::ValueIn:
		case kkMaterialEditorSocketType::VectorIn:
			x_pos = -nodeHalfSizeX;
			break;
		}

		int flags = 0;
		Gui::Style * style = nullptr;

		switch (s.m_type)
		{
		case kkMaterialEditorSocketType::MaterialOut:
		default:
			flags = MeterialEditorSocketFlag_outputMaterial;
			style = &m_guiStyle_socketMaterial;
			break;
		case kkMaterialEditorSocketType::ColorIn:
			flags = MeterialEditorSocketFlag_inputColor;
			style = &m_guiStyle_socketColor;
			break;
		case kkMaterialEditorSocketType::ColorOut:
			flags = MeterialEditorSocketFlag_outputColor;
			style = &m_guiStyle_socketColor;
			break;
		case kkMaterialEditorSocketType::ValueIn:
			flags = MeterialEditorSocketFlag_inputValue;
			style = &m_guiStyle_socketValue;
			break;
		case kkMaterialEditorSocketType::ValueOut:
			flags = MeterialEditorSocketFlag_outputValue;
			style = &m_guiStyle_socketValue;
			break;
		case kkMaterialEditorSocketType::VectorIn:
			flags = MeterialEditorSocketFlag_inputVector;
			style = &m_guiStyle_socketVector;
			break;
		case kkMaterialEditorSocketType::VectorOut:
			flags = MeterialEditorSocketFlag_outputVector;
			style = &m_guiStyle_socketVector;
			break;
		}

		krgui_node->m_node.addSocket(
			Gui::Vec2f(x_pos,s.m_socketCenterOffset),
			Gui::Vec2f(10.f,10.f),
			id++, 
			flags, 
			style
		);
	}

	if( isMaterial )
	{
		auto socketOffset = -nodeHalfSizeY + 40.f;
		Gui::Vec2f socketPosition(nodeHalfSizeX, socketOffset);
		krgui_node->m_node.addSocket(socketPosition, Gui::Vec2f(10.f,10.f), 0, MeterialEditorSocketFlag_outputMaterial, &m_guiStyle_socketMaterial);
	}

	_addDeleteNodeButton(newNode);
}

void MaterialEditor::_createMaterialTypeNode(MaterialType& type)
{
	auto node = type.m_onCreation();
	_createNode(node, true, type.m_renderer);
}

void MaterialEditor::_createParameterNode(MaterialParameter& param)
{
	auto node = param.m_onCreation();
	_createNode(node, false, param.m_renderer);
}

// Когда ноды соединяются или отсоединяются, нужно обновить текущий материал, превью и т.д.
//   Сначала, берутся ноды материалы. Идёт проход по дереву, получая все значения. Значения передаются в материал.
//   Потом, проверяется, подключен ли материал к output ноду. Если да, то рендеринг превьюшки.
void MaterialEditor::onSocket()
{
	auto selectedMaterial = m_materials[m_selectedMaterialIndex];
	auto * nodeEditor     = &selectedMaterial->m_nodeEditor;
	bool isPreviewReady = selectedMaterial->m_outputConnected;
	selectedMaterial->m_outputConnected = false;

	bool noMaterial = true;

	for( u64 i = 0, sz = selectedMaterial->m_nodes.size(); i < sz; ++i )
	{
		auto node = selectedMaterial->m_nodes[ i ];
		auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)node->GetKrGuiNode();
		if( node->GetNodeType() == kkMaterialEditorNodeType::_material )
		{
			for( size_t o = 0, osz = krgui_node->m_node.m_sockets.size(); o < osz; ++o )
			{
				node->ResetSocketValue((int)o);
			}
		

			noMaterial = false;
			// материал получен.
			// к материалу могут подключаться множество нод-параметров.
			// нужно передать в материал необходимые данные. Нужно проходиться по input сокетам.
			// берется инпут сокет, нода с которой сокет соединён. Начинается рекурсивная функция,
			//  которая так-же будет проходиться по инпут сокетам.
			// Как только будет достигнут лист, будут устанавливаться соответствующие значения (скорее всего как SetOutFloat)
			//  потом выход из рекурсии, будет браться значение (GetOutFloat), потом делается нужная обработка, и опять отправка в аут (SetOutFloat)

			// должна быть та самая рекурсивная функция
			_doSocketWork(node, /*nullptr,*/ selectedMaterial, nodeEditor);

			// потом проверка, соединён ли этот материал с превьюшкой
			// ...
			for( size_t o = 0, osz = krgui_node->m_node.m_sockets.size(); o < osz; ++o )
			{
				if( krgui_node->m_node.m_sockets[o].flags & MeterialEditorSocketFlag_outputMaterial )
				{
					

					for( size_t k = 0, ksz = selectedMaterial->m_nodeEditor.m_socketConnections.size(); k < ksz; ++k )
					{
						if( selectedMaterial->m_nodeEditor.m_socketConnections[k].first == &krgui_node->m_node.m_sockets[o]
							||
							selectedMaterial->m_nodeEditor.m_socketConnections[k].second == &krgui_node->m_node.m_sockets[o])
						{
							selectedMaterial->m_outputConnected = true;
							
							m_ballObject->SetMaterialImplementation(node->GetMaterialImplementation());
							//printf("update preview\n");

							auto outputNode = selectedMaterial->m_nodes[0];

							m_renderPreviewData.renderInfo.resetStates();
							m_renderPreviewData.renderInfo.isStarted = true;
							node->GetRenderer()->Render(&m_renderPreviewData.renderInfo);

							outputNode->GetImage()->texture->fillNewData(outputNode->GetImage()->image.m_data8);

							break;
						}
					}

					if( selectedMaterial->m_outputConnected )
					{
						o = osz;
						g_currentNodeCollection->m_materialImplementation = node->GetMaterialImplementation();
					}
					else// if(isPreviewReady)
					{
						m_ballObject->SetMaterialImplementation(g_defaultMaterial);
						_clearOutputImage();
						//printf("clear preview\n");
					}
				}
			}
		}
	}

	if( noMaterial )
	{
		_clearOutputImage();
	}

	_updateObjectsMaterialParameters();
}

void MaterialEditor::_clearOutputImage()
{
	auto selectedMaterial = m_materials[m_selectedMaterialIndex];
	auto outputNode = selectedMaterial->m_nodes[0];
	outputNode->GetImage()->image.fill(kkColorBlack);
	outputNode->GetImage()->texture->fillNewData(outputNode->GetImage()->image.m_data8);
}

// вся работа по получению данных из параметров и переносу их в ноду материала должна производиться рекурсивно этой функцией
void MaterialEditor::_doSocketWork( 
	kkMaterialEditorNode* node, 
	MaterialEditorNodeCollection* selectedMaterial, 
	Gui::NodeEditor* nodeEditor )
{
	// в первую очередь нужно дойти до листьев

	// готово для дальнейших действий
//	bool ready = false;

	// нужно получить реальные сокеты ноды, для этого нужна сама нода
	auto krgui_node = (kkMaterialEditorNodeEditorNodeImpl*)node->GetKrGuiNode();
	auto sockets_size = krgui_node->m_node.m_sockets.size(); // количество сокетов
	for( u64 o = 0; o < sockets_size; ++o ) // проход по всем сокетам
	{
		auto socket = &krgui_node->m_node.m_sockets[o]; // беру сокет

		// проверяю, является ли он input
		// если input, то рекурсия, если нет, то либо это лист либо идёт выход из рекурсии
		if( socket->flags & MeterialEditorSocketFlag_inputColor
			|| socket->flags & MeterialEditorSocketFlag_inputValue 
			|| socket->flags & MeterialEditorSocketFlag_inputVector )
		{

			// это инпут. Далее, нужно проверить, соединён ли сокет
			for( size_t p = 0, psz = nodeEditor->m_socketConnections.size(); p < psz; ++p ) // проход по соединениям
			{
				// соединение найдено
				if( nodeEditor->m_socketConnections[p].first == socket || nodeEditor->m_socketConnections[p].second == socket )
				{
					// теперь надо взять ту ноду с которой соединена текущая нода, и войти в рекурсию
					auto nodeEditorNodeFirst  = (kkMaterialEditorNode*)nodeEditor->m_socketConnections[p].first->parentNode->m_userData;
					auto nodeEditorNodeSecond = (kkMaterialEditorNode*)nodeEditor->m_socketConnections[p].second->parentNode->m_userData;
					kkMaterialEditorNode* next_Node = nodeEditorNodeFirst == node ? nodeEditorNodeSecond : nodeEditorNodeFirst;
					_doSocketWork(next_Node, selectedMaterial, nodeEditor);

					// нужно взять сам сокет той ноды что слева
					Gui::NodeEditorNodeSocket * next_NodeSocket 
						= 
						socket == nodeEditor->m_socketConnections[p].first 
						? 
						nodeEditor->m_socketConnections[p].second
						:
						nodeEditor->m_socketConnections[p].first;

					kkColor out_color;
					float   out_float;
					v3f     out_vector;
					kkImageContainerNode* out_image = nullptr;

					next_Node->GetOutSocketValue(next_NodeSocket->id, out_color);
					next_Node->GetOutSocketValue(next_NodeSocket->id, out_float);
					next_Node->GetOutSocketValue(next_NodeSocket->id, out_vector);
					next_Node->GetOutSocketValue(next_NodeSocket->id, &out_image);

					node->SetInSocketValue((int)o, out_color);
					node->SetInSocketValue((int)o, out_float);
					node->SetInSocketValue((int)o, out_vector);
					node->SetInSocketValue((int)o, out_image);

					break;
				}
			}
//			ready = true;
		}
	}

	// если сокетов нет то устанавливаю так чтобы был дальнейший шаг.
	if( !sockets_size )
	{
//		ready = true;
	}



	// в данном месте сначала должен быть лист, потом при выходе из функции, работа продолжится с цикла выше, потом break и опять
	//   данное место. Должны обработаться все ноды дерева.

	// далее, нужно сделать 3 основные действия
	// 1. получить значения через сокеты (если есть соединение) - делать выше в цикле так как там обход по сокетам
	// 2. выполнить функцию ноды (обработать данные)
	// 3. установить результат в оутпут (возможно внутри CalculateValues)
	node->CalculateValues();
}


// Когда я делаю assign, то ноды материала может не быть. Но к выбранному объекту всё равно уже нужно будет присоединить 
//   текущий МАТЕРИАЛ - nodeCollection
//  Пусть у объекта будет ещё и имя материала(nodeCollection), тогда если изменили материал в output, или удалили ноду, или ещё что
//    то можно определить какие объекты нужно отредактировать.
void MaterialEditor::_assignCurrentMaterialToSelectedObjects()
{
	auto selectedMaterial = m_materials[m_selectedMaterialIndex];


	// здесь, нужно дать объектам материал kkMaterial для рендеринга (пока рендеринга нет пусть будет так)
	//  и установить имя материала. Главное имя. Именно по нему будет происходить идентификация.
	//  При удалении материала, так-же нужно будет передавать nullptr вместо имени
	auto num_of_objects = m_app->m_current_scene3D->getNumOfSelectedObjects();
	for( u32 i = 0; i < num_of_objects; ++i )
	{
		auto selectedObject = m_app->m_current_scene3D->getSelectedObject(i);
		selectedObject->SetMaterialImplementation( selectedMaterial->m_materialImplementation );
		selectedObject->SetMaterial( selectedMaterial->m_name.data() );
	}

	_updateObjectsMaterialParameters();
}

// для realtime материала нужно создать отдельные от kkMaterial объекта параметры...
void MaterialEditor::_updateObjectsMaterialParameters()
{
	auto selectedMaterial = m_materials[m_selectedMaterialIndex];
	auto & objects = m_app->m_current_scene3D->getObjects();
	for( auto o : objects )
	{

		// проверка, установлено ли имя материала, и если да, то совпадает ли с именем текущего материала.
		if( o->IsMaterial(selectedMaterial->m_name.data()) )
		{
			auto objectType = o->GetType();
			switch(objectType)
			{
			case kkScene3DObjectType::PolygonObject:
			{
				// устанавливаю параметры шейдера
				Scene3DObject * scene3dobject = (Scene3DObject *)o;

				scene3dobject->SetMaterialImplementation(selectedMaterial->m_materialImplementation);

				// base color
				if( selectedMaterial->m_materialImplementation )
				{
					//printf("here\n");
					scene3dobject->setShaderParameter_diffuseColor(selectedMaterial->m_materialImplementation->GetDiffuseColor());
					scene3dobject->setShaderParameter_diffuseTexture(selectedMaterial->m_materialImplementation->GetDiffuseTexture());
				}
			}
				break;
			default:
				break;
			}
		}

	}
}

void MaterialEditor::onPickSocketInput(Gui::NodeEditorNodeSocket* gui_socket, Gui::NodeEditorNode* gui_node)
{
	assert(gui_socket);
	assert(gui_node);
	auto material_node = (kkMaterialEditorNode*)gui_node->m_userData;
	material_node->ResetSocketValue(gui_socket->id);
}

kkImageContainerNode* MaterialEditor::loadImage(const char16_t* f)
{
	assert(f);
	return m_imageContainer.getImage(f, m_app, m_gs);
}
void MaterialEditor::removeImage(kkImageContainerNode* i)
{
	assert(i);
	m_imageContainer.removeImage(i->m_image);
}
void MaterialEditor::reloadImage(kkImageContainerNode* i)
{
	assert(i);
	if( i )
	{
		kkDestroy( i->m_image );
		kkDestroy( i->m_texture );
		i->m_image = m_app->loadImage(i->m_filePath.data());
		if( i->m_image )
		{
			i->m_texture = m_gs->createTexture(i->m_image);
		}
	}
}
kkTexture* MaterialEditor::getTexture(kkImage* image)
{
	for( auto i : m_imageContainer.m_images )
	{
		if( i->m_image == image )
			return i->m_texture;
	}
	return nullptr;
}
ImageContainer::ImageContainer(){}
ImageContainer::~ImageContainer()
{
	for( auto i : m_images )
	{
		if( i->m_texture ) kkDestroy(i->m_texture);
		if( i->m_image ) kkDestroy(i->m_image);
		delete i;
	}
}	
//kkImage* ImageContainer::getImage(const char16_t* f, Application* app, kkGraphicsSystem* gs)
kkImageContainerNode* ImageContainer::getImage(const char16_t* f, Application* app, kkGraphicsSystem* gs)
{
	kkString filePath(f);
	for( auto i : m_images )
	{
		if( i->m_filePath == filePath )
		{
			if( i->m_refCounter )
			{
				++i->m_refCounter;
				return i;
			}
			else // если удалено или получение впервые, то грузим
			{
				i->m_image = app->loadImage(f);
				if( i->m_image )
				{
					printf("Load image [%s]\n", filePath.to_kkStringA().data());
					i->m_texture = gs->createTexture(i->m_image);
					++i->m_refCounter;
					return i;
				}
			}
		}
	}
	kkImageContainerNode* node = new kkImageContainerNode;
	node->m_filePath = filePath;
	node->m_refCounter = 1;
	node->m_image = app->loadImage(f);
	if(!node->m_image)
	{
		delete node;
		return nullptr;
	}
	printf("Load image [%s]\n", filePath.to_kkStringA().data());
	node->m_texture = gs->createTexture(node->m_image);
	m_images.push_back(node);
	return node;
}
void ImageContainer::removeImage(kkImage* image)
{
	for( auto i : m_images )
	{
		if( i->m_image == image )
		{
			--i->m_refCounter;
			if( i->m_refCounter == 0 )
			{
				printf("Unload image [%s]\n", i->m_filePath.to_kkStringA().data());
				kkDestroy( i->m_image );
				kkDestroy( i->m_texture );
				i->m_image   = nullptr;
				i->m_texture = nullptr;
				i->m_filePath.clear();
			}
		}
	}
}
