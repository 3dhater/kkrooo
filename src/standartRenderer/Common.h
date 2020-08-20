// SPDX-License-Identifier: GPL-3.0-only
#ifndef __COMMON_H__
#define __COMMON_H__

#include "PluginSDK.h"

#define RENDERER_ID_MAGIC_1 0xFFAAFFAA
#define RENDERER_ID_MAGIC_2 0xAAFFAAFF

struct pixel
{
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	u8 a = 0;
	pixel(){}
	pixel(u8 R, u8 G, u8 B, u8 A)
		:
		r(R),
		g(G),
		b(B),
		a(A)
	{
	}

	void operator+=( const pixel& p )
	{
		a += p.a;
		r += p.r;
		g += p.g;
		b += p.b;
	}

	pixel operator/( pixel v ) const
	{
		return pixel(r / v.r, g / v.g, b / v.b, a / v.a);
	}

	pixel operator/( u8 v ) const
	{
		return pixel(r / v, g / v, b / v, a / v);
	}

	void fromColor( const kkColor& color )
	{
		r = color.getAsByteRed();
		g = color.getAsByteGreen();
		b = color.getAsByteBlue();
		a = color.getAsByteAlpha();;
	}
};

enum class MaterialType : int 
{
	Default  = 0,
	Toon     = 1
};
struct StandartRendererIDStruct
{
	StandartRendererIDStruct(MaterialType mt):materialType(mt){}
	unsigned int magic[2] = { RENDERER_ID_MAGIC_1, RENDERER_ID_MAGIC_2 };
	MaterialType materialType = MaterialType::Default;
};

class StaticData
{
	StandartRendererIDStruct m_id_defaultMaterial = StandartRendererIDStruct(MaterialType::Default);
	StandartRendererIDStruct m_id_toonMaterial    = StandartRendererIDStruct(MaterialType::Toon);
public:
	StaticData()
	{
		m_plugin_uid_defaultMaterial.m_data = &m_id_defaultMaterial;
		m_plugin_uid_defaultMaterial.m_dataSize = sizeof(StandartRendererIDStruct);

		m_plugin_uid_toonMaterial.m_data = &m_id_toonMaterial;
		m_plugin_uid_toonMaterial.m_dataSize = sizeof(StandartRendererIDStruct);
	}
	~StaticData(){}

	kkPluginID m_plugin_uid_defaultMaterial;
	kkPluginID m_plugin_uid_toonMaterial;

	kkPluginID m_plugin_uid_colorParameter;

	kkPluginCommonInterface* m_ci = nullptr;
};

inline bool IsValidMaterial(const kkPluginID& id)
{
	if( id.m_dataSize != sizeof(StandartRendererIDStruct) )
		return false;
	
	StandartRendererIDStruct * i = (StandartRendererIDStruct*)id.m_data;

	if( i->magic[ 0 ] != RENDERER_ID_MAGIC_1 )
		return false;
	if( i->magic[ 1 ] != RENDERER_ID_MAGIC_2 )
		return false;

	switch(i->materialType)
	{
	case MaterialType::Default:
	case MaterialType::Toon:
		break;
	default:
		return false;
	}

	return true;
}

#endif