#include "kkrooo.engine.h"
#include "../Common.h"

#include "Common/kkInfoSharedLibrary.h"
#include "../Application.h"
#include "../Functions.h" 
#include "../Viewport/Viewport.h" 

using namespace Kr;

void Application::_drawLeftToolBar()
{
	auto oldFont = m_KrGuiSystem->getCurrentFont();
	m_KrGuiSystem->setCurrentFont(m_iconsFont);
	
	m_KrGuiSystem->setDrawPosition(0.f, m_mainMenuHeight + m_mainToolBarHeight);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::SelectByName), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		kkDrawAll();
		if( !m_drawSelectByNameWindow ) m_drawSelectByNameWindow = true;
        else                            m_drawSelectByNameWindow = false;
	}
	m_KrGuiSystem->newLine();

	auto oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_selectMode == SelectMode::JustSelect )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::SelectModeSelect), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setSelectMode( SelectMode::JustSelect );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_selectMode == SelectMode::Move )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::TransformMove), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setSelectMode( SelectMode::Move );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_selectMode == SelectMode::Scale )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::TransformScale), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setSelectMode( SelectMode::Scale );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_selectMode == SelectMode::Rotate )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::TransformRotate), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setSelectMode( SelectMode::Rotate );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_editMode == EditMode::Vertex )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::EditVertex), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setEditMode( EditMode::Vertex );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_editMode == EditMode::Edge )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::EditEdge), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setEditMode( EditMode::Edge );
	}
	m_KrGuiSystem->newLine();

	oldDrawPosition = m_KrGuiSystem->getDrawPosition();
	if( m_editMode == EditMode::Polygon )
	{
		m_KrGuiSystem->setNextItemIgnoreInput();
		m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ActiveButtonLTB), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f));
	}
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, oldDrawPosition.y);
	if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::EditFace), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		setEditMode( EditMode::Polygon );
	}
	m_KrGuiSystem->newLine();


	
	m_KrGuiSystem->setDrawPosition(oldDrawPosition.x, m_window_client_size.y - m_bottomAreaHeight - 21.f);
	if( m_KrGuiSystem->beginButtonGroup(kkrooo::getIconFontString(IconFontSymbol::Viewport), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
	{
		m_state_app = AppState_main::GuiInput;
		m_KrGuiSystem->setDrawPosition(oldDrawPosition.x + 23.f, m_window_client_size.y - m_bottomAreaHeight - 21.f);
		if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ViewportLY_full), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
		{
			_init_viewports(ViewportLayoutType::Single);
			kkDrawAll();
		}
		if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ViewportLY_2h), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
		{
			_init_viewports(ViewportLayoutType::ParallelHor);
			kkDrawAll();
		}
		if( m_KrGuiSystem->addButton(kkrooo::getIconFontString(IconFontSymbol::ViewportLY_2v), &m_guiStyle_mainToolbarButtons, Gui::Vec2f(21.f,21.f), isGlobalInputBlocked() ? false : true) )
		{
			_init_viewports(ViewportLayoutType::ParallelVer);
			kkDrawAll();
		}
		m_KrGuiSystem->endButtonGroup();
	}
	m_KrGuiSystem->setCurrentFont(oldFont);
}

