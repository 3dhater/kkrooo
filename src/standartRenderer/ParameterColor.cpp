// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include "Common.h"
#include "ParameterColor.h"

ParameterColor::ParameterColor()
{
}

ParameterColor::~ParameterColor()
{
}

void ParameterColor::SetInSocketValue( int socketId, float floatValue )
{
	switch (socketId)
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
	}
}

void ParameterColor::SetInSocketValue( int socketId, const v3f& vectorValue ){}
void ParameterColor::SetInSocketValue( int socketId, const kkColor& colorValue ){}
void ParameterColor::SetInSocketValue( int socketId, kkImageContainerNode* image ){}

void ParameterColor::GetOutSocketValue( int socketId, float& floatValue ){}
void ParameterColor::GetOutSocketValue( int socketId, v3f& vectorValue ){}

void ParameterColor::GetOutSocketValue( int socketId, kkColor& colorValue )
{
	switch(socketId)
	{
	case 0:
	default:
		colorValue = m_outColor;
		break;
	}
}

void ParameterColor::GetOutSocketValue( int socketId, kkImageContainerNode** image ){}

void ParameterColor::ResetSocketValue( int socketId )
{
	switch (socketId)
	{
	case 0: // input base color
	default:
		m_outColor = m_defaultColor;
		break;
	}
}

void ParameterColor::CalculateValues()
{
	m_inputColor_red   = m_rangeValue_red    * math::kkColorDivider;
	m_inputColor_green = m_rangeValue_green  * math::kkColorDivider;
	m_inputColor_blue  = m_rangeValue_blue   * math::kkColorDivider;

	m_outColor.set(m_inputColor_red, m_inputColor_green, m_inputColor_blue);
}

bool updateRectangle(kkMaterialEditorNodeGUIElementInfo* element)
{
	ParameterColor * parameter = (ParameterColor *)element->m_userData;

	parameter->m_rectangleColor.m_data[ 0 ] = parameter->m_rangeValue_red   * math::kkColorDivider;
	parameter->m_rectangleColor.m_data[ 1 ] = parameter->m_rangeValue_green * math::kkColorDivider;
	parameter->m_rectangleColor.m_data[ 2 ] = parameter->m_rangeValue_blue  * math::kkColorDivider;

	return true;
}

kkMaterialEditorNode* CreateParameter_Color()
{
	auto parameter = kkCreate<ParameterColor>();
	parameter->SetSize(170.f, 200.f);
	parameter->SetColorHead(kkColorYellowGreen);
	parameter->AddSocket(kkMaterialEditorSocketType::ColorOut, 0.f);
	parameter->SetName(u"Color");

	kkMaterialEditorNodeGUIElementInfo guiElement;
	guiElement.m_size.set(120.f, 15.f);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::RangeSliderFloat;
	guiElement.m_positionOffset.set(-50.f, 0.f);
	guiElement.m_rangeFloat_min = 0.f;
	guiElement.m_rangeFloat_max = 255.f;
	guiElement.m_rangeHorizontal = true;
	guiElement.m_rangeFloat_value = &parameter->m_rangeValue_red;
	
	guiElement.m_callback = updateRectangle;
	guiElement.m_userData = parameter;

	parameter->AddGUIElement(guiElement);

	updateRectangle(&guiElement); // just update parameter->m_rectangleColor
	
	guiElement.m_type = kkMaterialEditorNodeGUIElementType::RangeSliderFloat;
	guiElement.m_positionOffset.set(-50.f, 20.f);
	guiElement.m_rangeFloat_min = 0.f;
	guiElement.m_rangeFloat_max = 255.f;
	guiElement.m_rangeHorizontal = true;
	guiElement.m_rangeFloat_value = &parameter->m_rangeValue_green;
	
	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::RangeSliderFloat;
	guiElement.m_positionOffset.set(-50.f, 40.f);
	guiElement.m_rangeFloat_min = 0.f;
	guiElement.m_rangeFloat_max = 255.f;
	guiElement.m_rangeHorizontal = true;
	guiElement.m_rangeFloat_value = &parameter->m_rangeValue_blue;

	parameter->AddGUIElement(guiElement);

	guiElement.m_type = kkMaterialEditorNodeGUIElementType::Rectangle;
	guiElement.m_positionOffset.set(-50.f, -70.f);
	guiElement.m_size.set(100.f,50.f);
	guiElement.m_dynamicColor = &parameter->m_rectangleColor;

	parameter->AddGUIElement(guiElement);

	return parameter;
}
