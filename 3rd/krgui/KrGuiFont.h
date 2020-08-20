#ifndef __KK_KRGUI_FONT_H__
#define __KK_KRGUI_FONT_H__

namespace Kr
{
	namespace Gui
	{
		struct Font
		{
			std::vector<Texture*> m_textures; 
			Texture* m_defaultTexture = nullptr; // for default font

			GuiSystem* m_gui = nullptr;

			std::unordered_map<char16_t,FontGlyph> m_glyphs;

			Font(GuiSystem*);
			~Font();

			void addTexture( Texture* texture );
			void addCharacter( char16_t ch, const Vec4f& coords, int texture_slot, float underhang, float overhang, int textureWidth, int textureHeight );
		};
	}
}

#endif