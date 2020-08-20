// SPDX-License-Identifier: GPL-3.0-only
#ifndef __MATERIAL_DEFAULT_H__
#define __MATERIAL_DEFAULT_H__

#include "PluginSDK.h"


class MaterialDefaultType;

// параметры материала (прозрачность, отражение и т.д.)
class MaterialDefault : public kkMaterialImplementation
{
	kkColor m_inputBaseColor = kkColorWhite;
public:
	MaterialDefault();
	virtual ~MaterialDefault();

	friend class MaterialDefaultType;
};

// параметры ноды (размер, сокеты и т.д.)
class MaterialDefaultType : public kkMaterialEditorNode
{
	MaterialDefault* m_materialImpl = nullptr;
	
	kkColor m_defaultColor = kkColorWhite;
	kkImageContainerNode* m_inImage_ptr = nullptr;
public:
	MaterialDefaultType();
	virtual ~MaterialDefaultType();
	//virtual void make_pure(){}
	
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

	friend kkMaterialEditorNode* CreateMaterial_Default();
};



kkMaterialEditorNode* CreateMaterial_Default();

#endif