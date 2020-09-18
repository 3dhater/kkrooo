﻿#include "kkrooo.engine.h"
#include "../Common.h"

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "GraphicsSystem/kkTexture.h"

#include "../Application.h"
#include "GUIResources.h"

GUIResources::GUIResources()
{
}

GUIResources::~GUIResources()
{
	if( m_smallFontTexture )     kkDestroy(m_smallFontTexture);
	if( m_microFontTexture )     kkDestroy(m_microFontTexture);
	if( m_blenderIcons )     kkDestroy(m_blenderIcons);
	if( m_DIB_viewportSplitHor ) kkDestroy(m_DIB_viewportSplitHor);
	if( m_DIB_viewportSplitVer ) kkDestroy(m_DIB_viewportSplitVer);
}

#define KK_LOAD_GUI_BITMAP(str,ptr) \
    app->loadImage(str); if(image){ptr = gs->createTexture(image);kkDestroy(image);}else{KK_PRINT_FAILED;}

void GUIResources::loadResources(Application* app, kkGraphicsSystem* gs)
{
	gs->setLinearFilter(true);

	auto 
    image = KK_LOAD_GUI_BITMAP( u"../res/gui/icons16.png", m_blenderIcons);
	gs->setLinearFilter(false);
    image = KK_LOAD_GUI_BITMAP( u"../res/gui/smallFont.png", m_smallFontTexture);
    image = KK_LOAD_GUI_BITMAP( u"../res/gui/microFont.png", m_microFontTexture);
    image = KK_LOAD_GUI_BITMAP( u"../res/gui/b_vp_sh.png", m_DIB_viewportSplitHor);
    image = KK_LOAD_GUI_BITMAP( u"../res/gui/b_vp_sv.png", m_DIB_viewportSplitVer);
}