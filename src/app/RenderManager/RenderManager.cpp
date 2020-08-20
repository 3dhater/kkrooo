// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "GraphicsSystem/kkTexture.h"
#include "GraphicsSystem/kkGraphicsSystem.h"
#include "Common/kkUtil.h"

#include "KrGui.h"

#include "Window/kkWindow.h"
#include "Renderer/kkRenderer.h"
#include "RenderManager.h"
#include "../Application.h"
#include "../Scene3D/Scene3D.h"
#include "../Scene3D/Scene3DObject.h"
#include "../Viewport/Viewport.h"
#include "../Viewport/ViewportOptimizations.h"
#include "../Functions.h" 


using namespace Kr;

bool RenderManager_NodeEditorCallback(
	Gui::NodeEditor* nodeEditor,
	Gui::NodeEditor::callbackReason reason, 
	Gui::NodeEditorNode* firstNode, 
	Gui::NodeEditorNodeSocket* socket1, 
	Gui::NodeEditorNode* secondNode, 
	Gui::NodeEditorNodeSocket* socket2
)
{
	return true;
};

RenderManager::RenderManager()
{
	m_guiStyle_iconButtons.buttonTextIdleColor  = Gui::ColorWhite;
	m_guiStyle_iconButtons.buttonTextHoverColor = Gui::ColorRed;
	m_guiStyle_iconButtons.buttonTextPushColor  = Gui::ColorDarkRed;
	m_guiStyle_iconButtons.buttonBackgroundAlpha = 0.f;
}

RenderManager::~RenderManager()
{
	m_threadStateMask = kk::threads::need_to_stop;
	_destroyThread();
	_destroyImage();
}

void RenderManager::init(kkGraphicsSystem * gs, Gui::GuiSystem* gui, kkWindow * window)
{
	m_gs           = gs;
	m_gui          = gui;
	m_renderWindow = window;
	m_app          = kkSingleton<Application>::s_instance;

	m_nodeEditor.callback = RenderManager_NodeEditorCallback;
	m_nodeEditorNode.m_rounding.x = 0.f;
	m_nodeEditorNode.m_rounding.y = 0.f;
	m_nodeEditorNode.m_rounding.z = 0.f;
	m_nodeEditorNode.m_rounding.w = 0.f;

	m_outSize.set(640.f, 480.f);

	m_renderInfo.settings = &m_renderSettings;

	resizeImage();
	_fitView();
}

void RenderManager::_fitView()
{
	m_nodeEditor.m_eyePosition.set(0.f,0.f);
	m_nodeEditor.m_zoomValue = 1.f;
}

void RenderManager::drawWindow()
{
	_checkThread();

	auto windowRect = m_renderWindow->getClientRect();
	auto windowSize = windowRect.getWidthAndHeight();

	m_nodeEditor.m_size.set(windowSize.x - 300, windowSize.y-30);

	if( m_gui->beginNodeEditor(&m_nodeEditor) )
	{
		m_nodeEditorNode.m_size.set(m_outSize.x,m_outSize.y);
		if( m_gui->beginNode(&m_nodeEditorNode) )
		{
			m_gui->setCurrentNodeContentOffset(Gui::Vec2f( -(m_outSize.x*0.5f), -(m_outSize.y*0.5f) ));
			Gui::Style style;
			style.rectangleIdleColor1.x = m_renderSettings.backgroundColor.m_data[0];
			style.rectangleIdleColor1.y = m_renderSettings.backgroundColor.m_data[1];
			style.rectangleIdleColor1.z = m_renderSettings.backgroundColor.m_data[2];
			style.rectangleIdleColor2  = style.rectangleIdleColor1;
			style.rectangleHoverColor1 = style.rectangleIdleColor1;
			style.rectangleHoverColor2 = style.rectangleIdleColor1;
			m_gui->addRectangle(&style,Gui::Vec2f(m_outSize.x,m_outSize.y) );
			m_gui->addPictureBox(Gui::Vec2f(m_outSize.x,m_outSize.y), (unsigned long long)m_texture->getHandle() );
			m_gui->endNode();
		}
		m_gui->endNodeEditor();
	}

	auto oldFont = m_gui->getCurrentFont();
	m_gui->setCurrentFont(m_app->m_iconsFont);
	m_gui->setDrawPointPosition(0,windowSize.y - 25.f);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::Save), &m_guiStyle_iconButtons, Gui::Vec2f(21.f,21.f), m_app->isGlobalInputBlocked() ? false : true) )
	{
		_saveOutputImage();
	}
	m_gui->setDrawPointPosition(23.f,windowSize.y - 25.f);
	if( m_gui->addButton(kkrooo::getIconFontString(IconFontSymbol::ClearOutputImage), &m_guiStyle_iconButtons, Gui::Vec2f(21.f,21.f), m_app->isGlobalInputBlocked() ? false : true) )
	{
		_clearOutputImage();
	}
	m_gui->setCurrentFont(oldFont);

	m_gui->setDrawPointPosition( m_nodeEditor.m_size.x+5, windowSize.y - 25.f );
	if( m_gui->addButton(u"Render", 0, Gui::Vec2f(80, 20),m_buttonRender,true,Gui::Vec4f(4.f,4.f,4.f,4.f)) )
	{
		_startRender();
	}
	m_gui->setDrawPointPosition( windowSize.x - 85.f, windowSize.y - 25.f );
	if( m_gui->addButton(u"Stop", 0, Gui::Vec2f(80, 20),m_buttonStop,true,Gui::Vec4f(4.f,4.f,4.f,4.f)) )
	{
		_stopRender();
	}
}

void RenderManager::_destroyImage()
{
	if( m_image )
	{
		kkDestroy(m_image);
		m_image = nullptr;
	}
	if( m_texture )
	{
		kkDestroy(m_texture);
		m_texture = nullptr;
	}
}

void RenderManager::resizeImage()
{
	if( m_image )
	{
		_destroyImage();
	}

	m_image = kkCreate<kkImage>();
	m_image->m_bits     = 32;
	m_image->m_width    = (u32)m_outSize.x;
	m_image->m_height   = (u32)m_outSize.y;
	m_image->m_pitch    = m_image->m_width * 4;
	m_image->m_dataSize = m_image->m_pitch * m_image->m_height;
	m_image->m_data8    = (u8*)kkMemory::allocate(m_image->m_dataSize);
	m_image->fill(kkColorBlack);

	m_gs->setLinearFilter(false);
	m_texture = m_gs->createTexture(m_image);
}

void RenderManager_threadFuction(RenderManager * data)
{
	std::mutex m;
	
	/*while(true)
	{*/
		data->m_activeRenderer->Render(&data->m_renderInfo);
		//data->needToUpdateImage();

		/*if( data->m_threadStateMask & kk::threads::mask::need_to_stop )
		{
			break;
		}*/
	//}

	m.lock();
	data->m_threadStateMask = kk::threads::finish;
	m.unlock();
}

void RenderManager::_startRender()
{
	_fitView();
	m_buttonRender = false;
	m_buttonStop   = true;

	m_currentScene = *m_app->getScene3D();
	m_activeViewport = m_app->getActiveViewport();
	m_activeRenderer = m_app->getActiveRenderer();

	auto kkcamera = m_activeViewport->getCamera();
	auto camera_position = kkcamera->getPositionInSpace();
	auto frust = kkcamera->getFrustum();

	kkVector4 center;

	m_renderInfo.image   = m_image;

	auto oldAspectRation = kkcamera->getAspect();

	kkcamera->setAspect(m_outSize.x / m_outSize.y);
	kkcamera->update();
	/*auto V = kkcamera->getViewMatrix();
	auto P = kkcamera->getProjectionMatrix();
	m_renderInfo.VPInvert = P*V;
	m_renderInfo.VPInvert.invert();*/
	m_renderInfo.VPInvert = kkcamera->getViewProjectionInvertMatrix();
	m_renderInfo.VP       = kkcamera->getViewProjectionMatrix();
	m_renderInfo.V        = kkcamera->getViewMatrix();
	m_renderInfo.P        = kkcamera->getProjectionMatrix();

	kkcamera->setAspect(oldAspectRation);

	m_objectsForRender.clear();

	auto & all_objects = m_currentScene->getObjects();
	for( auto o : all_objects )
	{
		/*kkVector4 center;
		kkAabb    aabb;
		kkObb     obb;
		aabb = o->Aabb();
		aabb.center(center);
		o->m_distanceToCamera = camera_position.distance(center);

		if( OBBInFrustum( obb, frust ) )*/
		//{
			m_objectsForRender.push_back(o);
		//}
	}

	m_renderInfo.objects = &m_objectsForRender;
	m_renderInfo.resetStates();
	m_renderInfo.isStarted = true;

	m_threadStateMask = kk::threads::run;
	m_renderThread    = new std::thread(RenderManager_threadFuction, this);
	m_app->blockGlobalInput(true);
}

void RenderManager::_stopRender()
{
	m_buttonStop   = false;
	m_threadStateMask = kk::threads::need_to_stop;
	m_renderInfo.needToStop = true;
	m_app->blockGlobalInput(false);
}

void RenderManager::_destroyThread()
{
	if( m_renderThread )
	{
		m_renderThread->join();
		delete m_renderThread;
		m_renderThread = nullptr;
	}
}

void RenderManager::_checkThread()
{
	if( !m_threadStateMask )
	{
		m_buttonRender = true;
		m_buttonStop   = false;
		m_app->blockGlobalInput(false);
	}
	else if(m_threadStateMask & kk::threads::finish)
	{
		m_threadStateMask = 0;
		_destroyThread();
		updateImage();
		m_app->blockGlobalInput(false);
	}
	else if(m_threadStateMask & kk::threads::run)
	{
		updateImage();
	}
}

void RenderManager::updateImage()
{
	std::mutex m;
	std::lock_guard<std::mutex> g(m);
//	if( m_needToUpdateImage )
	//{
		m_texture->fillNewData(m_image->m_data8);
		//m_needToUpdateImage = false;
	//}
}

//void RenderManager::needToUpdateImage()
//{
//	std::mutex m;
//	std::lock_guard<std::mutex> g(m);
//	m_needToUpdateImage = true;
//}

void RenderManager::_saveOutputImage()
{
	m_app->saveImageToFile(m_image);
}

void RenderManager::stopRender()
{
	if( m_renderInfo.isStarted )
	{
		_stopRender();
		while(!m_renderInfo.isStopped)
		{
		}
		m_threadStateMask = 0;
		_checkThread();
		m_app->blockGlobalInput(false);
	}
}

void RenderManager::_clearOutputImage()
{
	m_image->fill(kkColorBlack);
	updateImage();
}