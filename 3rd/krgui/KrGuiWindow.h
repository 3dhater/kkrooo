#ifndef __KK_KRGUI_WINDOW_H__
#define __KK_KRGUI_WINDOW_H__

namespace Kr
{
	namespace Gui
	{
		struct Window
		{
			void * OSWindow = nullptr;

			int m_currentItemId_inRect = 0;
			int m_lastItemId_inRect = 0;
		};
	}
}

#endif