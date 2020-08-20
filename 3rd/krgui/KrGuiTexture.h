#ifndef __KK_KRGUI_TEXTURE_H__
#define __KK_KRGUI_TEXTURE_H__

namespace Kr
{
	namespace Gui
	{
		struct Texture
		{
			unsigned long long texture_value   = 0;
			unsigned long long texture_address = 0; // address for texture object / or texture ID for OpenGL

			int width  = 0;
			int height = 0;

			bool can_destroy = true;
		};
	}
}

#endif