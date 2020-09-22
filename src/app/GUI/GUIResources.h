#ifndef __GUIResources_H__
#define __GUIResources_H__

class kkTexture;
struct GUIResources
{
	GUIResources();
	~GUIResources();

	kkTexture * m_smallFontTexture = nullptr;
	kkTexture * m_microFontTexture = nullptr;
	kkTexture * m_blenderIcons = nullptr;

	kkTexture* m_projTexture = nullptr;


	void loadResources(Application* app, kkGraphicsSystem* gs);
};

#endif