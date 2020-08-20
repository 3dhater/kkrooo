#ifndef __KK_KRGUI_GROUP_H__
#define __KK_KRGUI_GROUP_H__

namespace Kr
{
	namespace Gui
	{
		struct Group
		{
			float   m_scrollValue     = 0.f;
			//bool    m_expanded = true;
			Vec2f m_size;
			Style* m_style = nullptr;
		

			// read only
			// не использовать. для внутреннего пользования
			Vec4f m_clipRect; // область для scissor. 
			float   m_contentHeight = 0; // высота содержимого. должен учитываться во всех контролах которые будут внутри группы
			bool    m_inRect = false;
			//Group * m_parent  = nullptr;
		};
	}
}

#endif