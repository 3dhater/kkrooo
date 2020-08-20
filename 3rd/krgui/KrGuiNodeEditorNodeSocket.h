#ifndef __KK_KRGUI_NODEEDITORNODESOCKET_H__
#define __KK_KRGUI_NODEEDITORNODESOCKET_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorNodeSocket
		{
			void * userData = nullptr;
			unsigned long long userDataSize = 0;

			Style * style = nullptr;

			// (0,0) = center of the node
			Vec2f position = Vec2f(0,0);
			Vec2f size = Vec2f(10,10);
			int id = -1;
			int flags = 0; // user flags

			//NodeEditorNodeSocket * connectedWith = nullptr;

			// read only
			// позиция относительно всего экрана
			Vec2f _realPosition = Vec2f(0,0);
			NodeEditorNode * parentNode = nullptr;

			Vec4f rounding = Vec4f(5.f,5.f,5.f,5.f);
		};
	}
}

#endif