// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_TEXTURE_H__
#define __KKROO_TEXTURE_H__

#include "Classes/kkColor.h"

enum class kkImageFormat : u32
{
	One_bit,		// white or black
	R1,
	A1R5G5B5,		// 1 11111 11111 11111
	X1R5G5B5,		// x 11111 11111 11111
	A4R4G4B4,		// 1111 1111 1111 1111
	X4R4G4B4,		// xxxx 1111 1111 1111
	B4G4R4A4,
	R5G6B5,			// 11111 111111 11111
	B5G6R5,
	B5G5R5A1,
	A8,
	R8,
	R8G8,
	R8G8B8,			// u8 u8 u8
	R8G8B8A8,		// u8 u8 u8 u8
	R8G8B8G8,
	G8R8G8B8,
	B8G8R8A8,
	B8G8R8X8,
	X8R8G8B8,		// u8 u8 u8 u8
	A8R8G8B8,		// u8 u8 u8 u8
	R9G9B9E5,
	R10G10B10A2,
	R11G11B10,
	R16,
	R16G16,
	R16G16B16A16,
	R24G8,
	R24X8,
	X24G8,
	R32,
	R32G32,
	R32G8X24,
	R32G32B32,
	R32G32B32A32,	
	DXT1 = 0x83F1, // dds dxt1
	DXT3 = 0x83F2, // dds dxt3
	DXT5 = 0x83F3,	// dds dxt5
	DXT2,
	DXT4
};

// из этой хрени создаются OpenGL буферы
struct kkImage
{
	//allocator m_allocator;

	kkImage()
	{
	}

	~kkImage()
	{
		if( m_data8 )   kkMemory::free(m_data8);
		if( m_data16 )  kkMemory::free(m_data16);
		if( m_data32 )  kkMemory::free(m_data32);
		if( m_data64 )  kkMemory::free(m_data64);
	}

	kkColor getPixelColorUV( f32 u, f32 v )
	{
		if(u > 1.f) u = u - std::floor(u);
		if(v > 1.f) v = v - std::floor(v);
		if(u < 0.f) u = 1.f - (u + std::abs(std::floor(u)));
		if(v < 0.f) v = 1.f - (v + std::abs(std::floor(v)));

		v = 1.f - v;

		f32 TX( u * (f32)(m_width-1) );
		f32 TY( v * (f32)(m_height-1) );

		TX = std::floor(TX);
		TY = std::floor(TY);

		auto ptr = m_data8;

		ptr += (s32)TY * m_pitch;

		kkColor color;
		switch (m_format)
		{
		case kkImageFormat::R8G8B8A8:
			ptr += (s32)TX * 4;
			color.setAsByteRed( *ptr++ );
			color.setAsByteGreen( *ptr++ );
			color.setAsByteBlue( *ptr++ );
			color.setAsByteAlpha( *ptr++ );
			break;
		case kkImageFormat::One_bit:
		case kkImageFormat::R1:
		case kkImageFormat::A1R5G5B5:
		case kkImageFormat::X1R5G5B5:
		case kkImageFormat::A4R4G4B4:
		case kkImageFormat::X4R4G4B4:
		case kkImageFormat::B4G4R4A4:
		case kkImageFormat::R5G6B5:
		case kkImageFormat::B5G6R5:
		case kkImageFormat::B5G5R5A1:
		case kkImageFormat::A8:
		case kkImageFormat::R8:
		case kkImageFormat::R8G8:
		case kkImageFormat::R8G8B8:
		case kkImageFormat::R8G8B8G8:
		case kkImageFormat::G8R8G8B8:
		case kkImageFormat::B8G8R8A8:
		case kkImageFormat::B8G8R8X8:
		case kkImageFormat::X8R8G8B8:
		case kkImageFormat::A8R8G8B8:
		case kkImageFormat::R9G9B9E5:
		case kkImageFormat::R10G10B10A2:
		case kkImageFormat::R11G11B10:
		case kkImageFormat::R16:
		case kkImageFormat::R16G16:
		case kkImageFormat::R16G16B16A16:
		case kkImageFormat::R24G8:
		case kkImageFormat::R24X8:
		case kkImageFormat::X24G8:
		case kkImageFormat::R32:
		case kkImageFormat::R32G32:
		case kkImageFormat::R32G8X24:
		case kkImageFormat::R32G32B32:
		case kkImageFormat::R32G32B32A32:
		case kkImageFormat::DXT1:
		case kkImageFormat::DXT3:
		case kkImageFormat::DXT5:
		case kkImageFormat::DXT2:
		case kkImageFormat::DXT4:
		default:
			break;
		}

		return color;
	}

	kkColor getPixelColor( u32 x, u32 y )
	{
		f32 TX( x * (f32)(m_width-1) );
		f32 TY( y * (f32)(m_height-1) );

		TX = std::floor(TX);
		TY = std::floor(TY);

		auto ptr = m_data8;

		ptr += (s32)TY * m_pitch;

		kkColor color;
		switch (m_format)
		{
		case kkImageFormat::R8G8B8A8:
			ptr += (s32)TX * 4;
			color.setAsByteRed( *ptr++ );
			color.setAsByteGreen( *ptr++ );
			color.setAsByteBlue( *ptr++ );
			color.setAsByteAlpha( *ptr++ );
			break;
		case kkImageFormat::One_bit:
		case kkImageFormat::R1:
		case kkImageFormat::A1R5G5B5:
		case kkImageFormat::X1R5G5B5:
		case kkImageFormat::A4R4G4B4:
		case kkImageFormat::X4R4G4B4:
		case kkImageFormat::B4G4R4A4:
		case kkImageFormat::R5G6B5:
		case kkImageFormat::B5G6R5:
		case kkImageFormat::B5G5R5A1:
		case kkImageFormat::A8:
		case kkImageFormat::R8:
		case kkImageFormat::R8G8:
		case kkImageFormat::R8G8B8:
		case kkImageFormat::R8G8B8G8:
		case kkImageFormat::G8R8G8B8:
		case kkImageFormat::B8G8R8A8:
		case kkImageFormat::B8G8R8X8:
		case kkImageFormat::X8R8G8B8:
		case kkImageFormat::A8R8G8B8:
		case kkImageFormat::R9G9B9E5:
		case kkImageFormat::R10G10B10A2:
		case kkImageFormat::R11G11B10:
		case kkImageFormat::R16:
		case kkImageFormat::R16G16:
		case kkImageFormat::R16G16B16A16:
		case kkImageFormat::R24G8:
		case kkImageFormat::R24X8:
		case kkImageFormat::X24G8:
		case kkImageFormat::R32:
		case kkImageFormat::R32G32:
		case kkImageFormat::R32G8X24:
		case kkImageFormat::R32G32B32:
		case kkImageFormat::R32G32B32A32:
		case kkImageFormat::DXT1:
		case kkImageFormat::DXT3:
		case kkImageFormat::DXT5:
		case kkImageFormat::DXT2:
		case kkImageFormat::DXT4:
		default:
			break;
		}

		return color;
	}

	void fill( const kkColor& color )
	{
		u8* data = m_data8;
		for( u32 h = 0; h < m_height; ++h )
		{
			for( u32 w = 0; w < m_width; ++w )
			{
				*data = color.getAsByteRed();   ++data;
				*data = color.getAsByteGreen(); ++data;
				*data = color.getAsByteBlue();  ++data;
				*data = color.getAsByteAlpha(); ++data;
			}
		}
	}

	kkImageFormat m_format    = kkImageFormat::R8G8B8A8;
	u32	      m_width     = 0;
	u32		  m_height    = 0;
	u32		  m_bits      = 32;
	u32		  m_mipCount  = 1;
	u8     *    m_data8     = nullptr;
	u16    *    m_data16    = nullptr;
	u32    *    m_data32    = nullptr;
	u64    *    m_data64    = nullptr;
	u32		  m_dataSize  = 0;
	u32		  m_pitch     = 0;
	u32		  m_frames    = 1;
};

class kkTexture
{
	// информация о высоте ширине и т.д.
	kkImage m_info;
public:
	kkTexture(){}
	virtual ~kkTexture(){}

	kkImage& getInfo()
	{
		return m_info;
	}

	// OpenGL = result of glGenTextures
	virtual void* getHandle() = 0;

	virtual void fillNewData( u8* ptr ) = 0;
};

#endif