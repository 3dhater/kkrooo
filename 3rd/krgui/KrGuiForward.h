#ifndef __KK_KRGUI_FWD_H__
#define __KK_KRGUI_FWD_H__

namespace Kr
{
	namespace Gui
	{
		class GuiSystem;
		struct Font;
		struct Texture;
		struct Image;
		struct NodeEditorNode;
		struct NodeEditor;
		struct NodeEditorNodeSocket;
		struct Window;


		bool GraphicsSystem_OpenGL3Init();
		void GraphicsSystem_OpenGL3Release();
		void GraphicsSystem_OpenGL3Draw(GuiSystem*);
		Texture* GraphicsSystem_OpenGL3CreateTexture( Image* image );
		void GraphicsSystem_OpenGL3DestroyTexture( Texture* );
	}
}

#endif