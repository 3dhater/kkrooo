#include "kkrooo.engine.h"
#include "../Common.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Functions.h" 

using namespace Kr;

void Application::_drawMainToolBar()
{
	auto oldFont = m_KrGuiSystem->getCurrentFont();
	m_KrGuiSystem->setCurrentFont(m_iconsFont);
	
	m_KrGuiSystem->setDrawPointPosition(0.f, m_mainMenuHeight);// 20.f - must be menu height
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::Undo), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		printf("Undo\n");
	}
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::Redo), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		printf("Redo\n");
	}


	m_KrGuiSystem->setCurrentFont(oldFont);
}

