// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PARAMETER_BITMAP_H__
#define __PARAMETER_BITMAP_H__

#include "PluginSDK.h"


class ParameterBitmap : public kkMaterialEditorNode
{
	float m_rangeValue_red = 177.f;
	float m_rangeValue_green = 177.f;
	float m_rangeValue_blue = 177.f;

	kkColor m_rectangleColor;

	float m_inputColor_red   = 1.f;
	float m_inputColor_green = 1.f;
	float m_inputColor_blue  = 0.f;

	kkColor m_outColor;

	kkString m_filePath;
	kkImageContainerNode* m_outImage = nullptr;
public:
	ParameterBitmap();
	virtual ~ParameterBitmap();

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


	friend kkMaterialEditorNode* CreateParameter_Bitmap();
	friend bool loadButtonCallback(kkMaterialEditorNodeGUIElementInfo* element);
	friend bool reloadButtonCallback(kkMaterialEditorNodeGUIElementInfo* element);
	friend bool deleteButtonCallback(kkMaterialEditorNodeGUIElementInfo* element);
	friend bool onSetTextCallback(kkMaterialEditorNodeGUIElementInfo* element, kkString* text);
};

kkMaterialEditorNode* CreateParameter_Bitmap();

#endif