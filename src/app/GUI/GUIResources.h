// SPDX-License-Identifier: GPL-3.0-only
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

	kkTexture * m_DIB_viewportSplitHor = nullptr;
	kkTexture * m_DIB_viewportSplitVer = nullptr;

	void loadResources(Application* app, kkGraphicsSystem* gs);
};

#endif