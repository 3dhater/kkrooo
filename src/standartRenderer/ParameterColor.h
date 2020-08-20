// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PARAMETER_COLOR_H__
#define __PARAMETER_COLOR_H__

#include "PluginSDK.h"


class ParameterColor : public kkMaterialEditorNode
{
	float m_rangeValue_red = 255.f;
	float m_rangeValue_green = 255.f;
	float m_rangeValue_blue = 255.f;

	kkColor m_rectangleColor;

	float m_inputColor_red   = 1.f;
	float m_inputColor_green = 1.f;
	float m_inputColor_blue  = 0.f;

	kkColor m_outColor;
	kkColor m_defaultColor = kkColorWhite;

public:
	ParameterColor();
	virtual ~ParameterColor();

	void CalculateValues();

	void SetInSocketValue( int socketId, float floatValue );
	void SetInSocketValue( int socketId, const v3f& vectorValue );
	void SetInSocketValue( int socketId, const kkColor& colorValue );
	void SetInSocketValue( int socketId, kkImageContainerNode* image );

	void GetOutSocketValue( int socketId, float& floatValue );
	void GetOutSocketValue( int socketId, v3f& vectorValue );
	void GetOutSocketValue( int socketId, kkColor& colorValue );
	void GetOutSocketValue( int socketId, kkImageContainerNode** image );

	void ResetSocketValue( int socketId );

	friend kkMaterialEditorNode* CreateParameter_Color();
	friend bool updateRectangle(kkMaterialEditorNodeGUIElementInfo* element);
};

kkMaterialEditorNode* CreateParameter_Color();

#endif