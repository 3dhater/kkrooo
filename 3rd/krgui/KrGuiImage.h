#ifndef __KK_KRGUI_IMAGE_H__
#define __KK_KRGUI_IMAGE_H__

namespace Kr
{
	namespace Gui
	{
		enum class ImageFormat : unsigned int
		{
			R8G8B8,			// u8 u8 u8
			R8G8B8A8,		// u8 u8 u8 u8
			A8R8G8B8,		// u8 u8 u8 u8
			BC1 = 0x83F1, // dds dxt1
			BC2 = 0x83F2, // dds dxt3
			BC3 = 0x83F3,	// dds dxt5
			Unknown
		};
		// only for rgba please
		struct Image 
		{
			using _byte = unsigned char;

			Image(){}
			~Image(){if( m_data )delete[] m_data;}
			void fill( const Vec4f& color )
			{
				unsigned char * data = m_data;
				for( unsigned int h = 0; h < m_height; ++h )
				{
					for( unsigned int w = 0; w < m_width; ++w )
					{
						*data = color.getAsByteBlue(); ++data;   // b
						*data = color.getAsByteGreen(); ++data;   // g
						*data = color.getAsByteRed(); ++data; // r
						*data = color.getAsByteAlpha(); ++data; // a
					}
				}
			}

			_byte * m_data = nullptr;
			unsigned int m_dataSize = 0;
			unsigned int m_width     = 0;
			unsigned int m_height    = 0;
			unsigned int m_bits      = 32;
			unsigned int m_mipCount  = 1;
			unsigned int m_pitch     = 0;
			ImageFormat m_format = ImageFormat::R8G8B8A8; // only for rgba please
		};
	}
}

#endif