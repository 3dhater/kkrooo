#ifndef __KK_KRGUI_VERTEX_H__
#define __KK_KRGUI_VERTEX_H__

namespace Kr
{
	namespace Gui
	{
		struct Vertex
		{
			Vec2f position;
			Vec2f textCoords;
			Vec4f color = Vec4f(1.f,1.f,1.f,1.f); // RGBA
		};
	}
}

#endif