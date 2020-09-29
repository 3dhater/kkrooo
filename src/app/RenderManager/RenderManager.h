#ifndef __RENDER_MANAGER_H__
#define __RENDER_MANAGER_H__

class Scene3D;
class Application;
class RenderManager
{
	Application * m_app       = nullptr;
	kkGraphicsSystem * m_gs   = nullptr;
	kkWindow * m_renderWindow = nullptr;
	Scene3D *  m_currentScene = nullptr;
	ViewportObject* m_activeViewport = nullptr;

	kkImage *   m_image   = nullptr;
	kkTexture * m_texture = nullptr;

	Kr::Gui::GuiSystem* m_gui = nullptr;
	Kr::Gui::NodeEditor m_nodeEditor;
	Kr::Gui::NodeEditorNode m_nodeEditorNode;

	kkRenderer* m_activeRenderer = nullptr;
	kkRenderInfo m_renderInfo;

	v2f m_outSize;

	kkArray<kkScene3DObject*> m_objectsForRender;

	bool m_buttonRender = true;
	bool m_buttonStop   = false;

	void _fitView();
	void _destroyImage();

	void _startRender();
	void _stopRender();
	void _checkThread();
	void _destroyThread();

	//bool m_needToUpdateImage = false;
	
	Kr::Gui::Style m_guiStyle_iconButtons;
	void _saveOutputImage();
	void _clearOutputImage();

	kkRenderSettings m_renderSettings;


	std::thread* m_renderThread = nullptr;
	u32 m_threadStateMask = 0;
	friend void RenderManager_threadFuction(RenderManager * data);

public:
	RenderManager();
	~RenderManager();

	void init(kkGraphicsSystem * gs, Kr::Gui::GuiSystem* gui, kkWindow * window);

	void drawWindow();

	void resizeImage();
	void updateImage();
	//void needToUpdateImage();
	void stopRender();
};

#endif