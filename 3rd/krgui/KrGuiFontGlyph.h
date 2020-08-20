#ifndef __KK_KRGUI_FONTGLYPH_H__
#define __KK_KRGUI_FONTGLYPH_H__

namespace Kr
{
	namespace Gui
	{
		struct FontGlyph
		{
			int textureID = 0;
			float width  = 0;
			float height = 0;

			//float bias = 0; // when next char move on previous char (like little `j` in some fonts)
			float underhang = 0;
			float overhang = 0;

			char16_t symbol = 0;
		
			// texture coords, left-top left-bottom etc.
			Vec2f lt, lb, rt, rb;
		};
	}
}

#endif