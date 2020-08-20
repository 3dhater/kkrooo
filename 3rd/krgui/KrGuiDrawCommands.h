#ifndef __KK_KRGUI_DRAWCOMMANDS_H__
#define __KK_KRGUI_DRAWCOMMANDS_H__

namespace Kr
{
	namespace Gui
	{
		struct DrawCommands // mesh buffers
		{
			std::vector<Vertex>    verts;
			std::vector<unsigned short> inds;
			Vec4f clipRect; // scissor
			Texture texture;

			void clear()
			{
				verts.clear();
				inds.clear();
			}

			DrawCommands& operator=( const DrawCommands& other ) 
			{
				verts = other.verts;
				inds = other.inds;
				clipRect = other.clipRect;
				texture = other.texture;
				return *this;
			}
		};
	}
}

#endif