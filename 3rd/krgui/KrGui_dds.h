#ifndef __KK_KRGUI_DDS_H__
#define __KK_KRGUI_DDS_H__

namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			struct dds_pixelformat
			{
				unsigned int size = sizeof(dds_pixelformat);
				unsigned int flags = 0x00000041; //DDS_RGBA
				unsigned int fourCC = 0;
				unsigned int RGBBitCount = 32;
				unsigned int RBitMask = 0x000000ff;
				unsigned int GBitMask = 0x0000ff00;
				unsigned int BBitMask = 0x00ff0000;
				unsigned int ABitMask = 0xff000000;
			};

			struct dds_header
			{
				unsigned int size = sizeof(dds_header);     // offset 0
				// DDS_HEIGHT | DDS_WIDTH
				unsigned int flags = 0x00000002 | 0x00000004;    // offset 4
				unsigned int height = 0;   // offset 8
				unsigned int width = 0;    // offset 12
				unsigned int pitchOrLinearSize = 0; // offset 16
				unsigned int depth = 0;    // offset 20
				unsigned int mipMapCount = 1;  // offset 24
				unsigned int reserved1[11]; 
				dds_pixelformat ddspf;
				unsigned int caps = 0;
				unsigned int caps2 = 0;
				unsigned int caps3 = 0;
				unsigned int caps4 = 0;
				unsigned int reserved2 = 0;
			};
		}
	}
}

#endif