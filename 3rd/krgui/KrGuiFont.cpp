#include "KrGui.h"

using namespace Kr;

Gui::Font::Font(Gui::GuiSystem* gui)
	:m_gui(gui)
{
}

Gui::Font::~Font()
{
	for( size_t i = 0; i < m_textures.size(); ++i )
	{
		if( m_textures[i]->can_destroy )
			Gui::GraphicsSystem_OpenGL3DestroyTexture( m_textures[i] );
	}
}

void Gui::Font::addTexture( Gui::Texture* texture )
{
	m_textures.push_back(texture);
}

void Gui::Font::addCharacter( char16_t ch, const Vec4f& coords, int texture_slot, float underhang, float overhang, int textureWidth, int textureHeight )
{
	float uvPerPixel_x = 1.f / (float)textureWidth;
	float uvPerPixel_y = 1.f / (float)textureHeight;

	Gui::FontGlyph glyph;

	glyph.underhang     = underhang;
	glyph.overhang      = overhang;
	glyph.textureID = texture_slot;
	glyph.width     = coords.z - coords.x;
	glyph.height    = coords.w - coords.y;
	glyph.symbol    = ch;
	glyph.lb.set(coords.x * uvPerPixel_x, (coords.w) * uvPerPixel_y);
	glyph.lt.set(coords.x * uvPerPixel_x, coords.y * uvPerPixel_y);
	glyph.rb.set((coords.z) * uvPerPixel_x, (coords.w) * uvPerPixel_y);
	glyph.rt.set((coords.z) * uvPerPixel_x, coords.y * uvPerPixel_y);

	m_glyphs[ch] = glyph;
}