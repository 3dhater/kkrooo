#ifndef __KK_KRGUI_CHARINFO_H__
#define __KK_KRGUI_CHARINFO_H__

namespace Kr
{
	namespace Gui
	{
		struct CharInfo
		{
			wchar_t symbol;
			Vec4f rect;
			float underhang = 0;
			float overhang = 0;
			int   textureSlot = 0;
		};
	}
}

#endif