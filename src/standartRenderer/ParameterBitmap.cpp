// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include "Common.h"
#include "ParameterBitmap.h"

ParameterBitmap::ParameterBitmap()
{
}

ParameterBitmap::~ParameterBitmap()
{
}

void ParameterBitmap::SetInSocketValue( int socketId, float floatValue )
{
	/*switch (socketId)
	{
	case 0:
	default:
		m_inputColor_red     = floatValue;
		break;
	case 1:
		m_inputColor_green   = floatValue;
		break;
	case 2:
		m_inputColor_blue    = floatValue;
		break;
	}*/
}

void ParameterBitmap::SetInSocketValue( int socketId, const v3f& vectorValue ){}
void ParameterBitmap::SetInSocketValue( int socketId, const kkColor& colorValue ){}
void ParameterBitmap::SetInSocketValue( int socketId, kkImageContainerNode* image ){}

void ParameterBitmap::GetOutSocketValue( int socketId, float& floatValue ){}
void ParameterBitmap::GetOutSocketValue( int socketId, v3f& vectorValue ){}

void ParameterBitmap::GetOutSocketValue( int socketId, kkColor& colorValue )
{
	switch(socketId)
	{
	case 0:
	default:
		colorValue = m_outColor;
		break;
	}
}

void ParameterBitmap::GetOutSocketValue( int socketId, kkImageContainerNode** image )
{
	*image = m_outImage;
}

void ParameterBitmap::ResetSocketValue( int socketId )
{
}

void ParameterBitmap::CalculateValues()
{
	m_inputColor_red   = m_rangeValue_red    * math::kkColorDivider;
	m_inputColor_green = m_rangeValue_green  * math::kkColorDivider;
	m_inputColor_blue  = m_rangeValue_blue   * math::kkColorDivider;

	m_outColor.set(m_inputColor_red, m_inputColor_green, m_inputColor_blue);
}


bool loadButtonCallback(kkMaterialEditorNodeGUIElementInfo* element)
{
	ParameterBitmap * parameter = (ParameterBitmap *)element->m_userData;

	auto CI = kkSingleton<StaticData>::s_instance->m_ci;
	CI->OpenImageFileDialog(&parameter->m_filePath);

	if( CI->FSFileExist(parameter->m_filePath.data()) )
	{
		for( auto & e : parameter->m_GUIElements )
		{
			if( e.m_type == kkMaterialEditorNodeGUIElementType::PictureBox )
			{
				// do not delete m_imageNodePtr when you call LoadImage
				// it will be deleted from app

				if( e.m_imageNodePtr ) // image already loaded
				{
					CI->RemoveImage(e.m_imageNodePtr);
				}

				e.m_imageNodePtr   = CI->LoadImage(parameter->m_filePath.data());
				parameter->m_outImage = e.m_imageNodePtr;
			}
		}
	}

	return true;
}

bool reloadButtonCallback(kkMaterialEditorNodeGUIElementInfo* element)
{
	auto CI = kkSingleton<StaticData>::s_instance->m_ci;
	ParameterBitmap * parameter = (ParameterBitmap *)element->m_userData;
	for( auto & e : parameter->m_GUIElements )
	{
		if( e.m_type == kkMaterialEditorNodeGUIElementType::PictureBox )
		{
			if( e.m_imageNodePtr ) // image already loaded
			{
				CI->ReloadImage(e.m_imageNodePtr);
			}
		}
	}

	return true;
}

bool deleteButtonCallback(kkMaterialEditorNodeGUIElementInfo* element)
{
	auto CI = kkSingleton<StaticData>::s_instance->m_ci;
	ParameterBitmap * parameter = (ParameterBitmap *)element->m_userData;
	for( auto & e : parameter->m_GUIElements )
	{
		if( e.m_type == kkMaterialEditorNodeGUIElementType::PictureBox )
		{
			if( e.m_imageNodePtr ) // image already loaded
			{
				CI->RemoveImage(e.m_imageNodePtr);
				e.m_imageNodePtr = nullptr;
				parameter->m_outImage = nullptr;
			}
		}
	}
	return true;
}

bool onSetTextCallback(kkMaterialEditorNodeGUIElementInfo* element, kkString* text)
{
	auto CI = kkSingleton<StaticData>::s_instance->m_ci;
	ParameterBitmap * parameter = (ParameterBitmap *)element->m_userData;
	
	if( parameter->m_outImage )
	{
		text->append( parameter->m_outImage->m_image->m_width );
		text->append( u"x" );
		text->append( parameter->m_outImage->m_image->m_height );
	}
	else
	{
		text->append(u"...");
	}
	return true;
}

kkMaterialEditorNode* CreateParameter_Bitmap()
{
	auto parameter = kkCreate<ParameterBitmap>();
	parameter->SetSize(170.f, 250.f);
	parameter->SetColorHead(kkColorYellowGreen);

	parameter->AddSocket( kkMaterialEditorSocketType::ColorOut, 0.f, kkMaterialEditorNodeSocketFlags_Texture );

	parameter->SetName(u"Bitmap");

	kkMaterialEditorNodeGUIElementInfo guiElement;
	guiElement.m_size.set(150.f, 150.f);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::PictureBox;
	guiElement.m_positionOffset.set(-75.f, -95.f);

	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Button;
	guiElement.m_positionOffset.set(-75.f, 60.f);
	guiElement.m_size.set(50.f, 15.f);
	guiElement.m_rounding.set(5.f,5.f,0.f,0.f);
	guiElement.m_text = u"Load";
	
	guiElement.m_callback = loadButtonCallback;
	guiElement.m_userData = parameter;

	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Button;
	guiElement.m_positionOffset.set(-25.f, 60.f);
	guiElement.m_size.set(50.f, 15.f);
	guiElement.m_rounding.set(0.f,0.f,0.f,0.f);
	guiElement.m_text = u"Reload";
	guiElement.m_callback = reloadButtonCallback;
	guiElement.m_userData = parameter;
	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Button;
	guiElement.m_positionOffset.set(25.f, 60.f);
	guiElement.m_size.set(50.f, 15.f);
	guiElement.m_rounding.set(0.f,0.f,5.f,5.f);
	guiElement.m_text = u"Delete";
	guiElement.m_callback = deleteButtonCallback;
	guiElement.m_userData = parameter;
	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Text;
	guiElement.m_positionOffset.set(-75.f, 105.f);
	guiElement.m_text = u"";
	guiElement.m_font = kkGUITextFontType::Small;
	guiElement.m_textCallback = onSetTextCallback;
	parameter->AddGUIElement(guiElement);

	return parameter;
}
