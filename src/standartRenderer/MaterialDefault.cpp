// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include "Common.h"
#include "MaterialDefault.h"

MaterialDefault::MaterialDefault()
{
	m_diffuseColor.setRed(0.8f);
	m_diffuseColor.setGreen(0.8f);
	m_diffuseColor.setBlue(0.8f);
	m_diffuseColor.setAlpha(1.f);
}

MaterialDefault::~MaterialDefault()
{
}


// ////////////////////////////////////////////

MaterialDefaultType::MaterialDefaultType()
{
}

MaterialDefaultType::~MaterialDefaultType()
{
}

void MaterialDefaultType::SetInSocketValue( int socketId, float floatValue ){}
void MaterialDefaultType::SetInSocketValue( int socketId, const v3f& vectorValue ){}

void MaterialDefaultType::SetInSocketValue( int socketId, const kkColor& colorValue )
{
	//printf("SetInSocketValue\n");
	switch (socketId)
	{
	case 0:
	default:
		m_materialImpl->m_inputBaseColor = colorValue;
		break;
	}
}

void MaterialDefaultType::SetInSocketValue( int socketId, kkImageContainerNode* image )
{
	m_inImage_ptr = image;
	if( image )
	{
		m_materialImpl->SetDiffuseTexture(image);
	}
}


void MaterialDefaultType::GetOutSocketValue( int socketId, float& floatValue ){}
void MaterialDefaultType::GetOutSocketValue( int socketId, v3f& vectorValue ){}
void MaterialDefaultType::GetOutSocketValue( int socketId, kkColor& colorValue ){}
void MaterialDefaultType::GetOutSocketValue( int socketId, kkImageContainerNode** image )
{
}

void MaterialDefaultType::ResetSocketValue( int socketId )
{
	//printf("RESET color\n");
	m_materialImpl->m_inputBaseColor = m_defaultColor;
	m_inImage_ptr = nullptr;
	m_materialImpl->SetDiffuseTexture(nullptr);
}

void MaterialDefaultType::CalculateValues()
{
	MaterialDefault * m = (MaterialDefault *)m_materialImpl;
	m->m_diffuseColor = m_materialImpl->m_inputBaseColor;
}


kkMaterialEditorNode* CreateMaterial_Default()
{
	auto materialType = kkCreate<MaterialDefaultType>();
	materialType->SetMaterialImplementation(kkCreate<MaterialDefault>());
	
	materialType->m_materialImpl = (MaterialDefault*)materialType->GetMaterialImplementation();

	materialType->SetSize(170.f, 200.f);
	materialType->SetName(u"Default");
	materialType->SetColorHead(kkColorAqua);
	materialType->GetMaterialImplementation()->SetPluginID( kkSingleton<StaticData>::s_instance->m_plugin_uid_defaultMaterial );

	kkMaterialEditorNodeGUIElementInfo guiElement;
	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Text;
	guiElement.m_text = u"Base Color";
	guiElement.m_positionOffset.set(-70.f, -38.f);
	materialType->AddGUIElement(guiElement);
	// kkMaterialEditorNodeSocketFlags_BaseTexture значит что сокет может принят\передать указатель на текстуру, и она будет считаться программой
	// как основная (diffuse) текстура.
	materialType->AddSocket(kkMaterialEditorSocketType::ColorIn, -30.f, kkMaterialEditorNodeSocketFlags_BaseTexture );
	
	return reinterpret_cast<kkMaterialEditorNode*>( materialType );
}

