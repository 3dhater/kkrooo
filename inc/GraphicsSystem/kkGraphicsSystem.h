// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_GRAPHICSSYSTEM_H__
#define __KKROO_GRAPHICSSYSTEM_H__

#include "Common/kkForward.h"

#include "Classes/Math/kkVector4.h"
#include "Classes/kkColor.h"
#include "Window/kkWindow.h"
#include "SceneSystem/kkCamera.h"


enum class kkMeshType
{
	Triangles,
	Lines,
	Points
};

enum class kkGraphicsSystemCompFunc
{
	_NEVER,
	_LESS ,
	_EQUAL ,
	_LEQUAL ,
	_GREATER ,
	_NOTEQUAL ,
	_GEQUAL    ,
	_ALWAYS     ,
};

class kkGraphicsSystem
{
protected:

	kkWindow* m_associated_window = nullptr;
	void _setAssociatedWindow( kkWindow* w )
	{
		w->setAssociatedGS( this );
		m_associated_window = w;
	}

	v2i m_back_buffer_size;
	u32 m_color_depth = 32;
	kkCamera * m_active_camera = nullptr;

	kkGraphicsSystemCompFunc m_compFunc = kkGraphicsSystemCompFunc::_LEQUAL;
	bool m_useLinearFilter = true;

public:

	kkGraphicsSystem(){}
	virtual ~kkGraphicsSystem(){}


	// wglMakeCurrent
	virtual void setActive(kkWindow*) = 0;

	virtual void initWindow(kkWindow*) = 0;

	virtual void beginDraw( bool clear_canvas ) = 0;
	virtual void endDraw() = 0;
	virtual void update() = 0;
	virtual void setViewport( s32 x, s32 y, s32 w, s32 h ) = 0;

	virtual void useDepth( bool ) = 0;
	virtual void useBackFaceCulling( bool ) = 0;

	virtual void useScissor( bool ) = 0;
	virtual void setScissor( int x1, int y1, int x2, int y2 ) = 0;
	
	virtual void setTarget( kkTexture* ) = 0;
	virtual void setDefaultTexture(kkTexture * t) = 0;
	//  c1----------
	//   |          |
	//   |_________c2
	virtual void drawRectangle( const v2i& c1, const v2i& c2, const kkColor& color1, const kkColor& color2) = 0;

	virtual void drawPoint3D( const kkVector4& p, kkShader * shader ) = 0;

	virtual void drawLine2D( const v2i& p1, const v2i& p2, const kkColor& color, kkShader * shader = nullptr ) = 0;
	virtual void drawCircle2D( const v2i& position, s32 radius, s32 smoothLevel, const kkColor& color, kkShader * shader = nullptr ) = 0;
	virtual void drawLine3D( const kkVector4& p1, const kkVector4& p2, const kkColor& color, kkShader * shader = nullptr ) = 0;
	virtual void setClearColor( const kkColor& c ) = 0;

	virtual kkMesh* createMesh(kkSMesh*, kkMeshType) = 0;
	virtual void drawMesh(kkMesh*, const kkMatrix4& W, kkShader * shader = nullptr) = 0;

	virtual kkTexture* createTexture(kkImage*) = 0;
	virtual kkTexture* createTexture(const v2i& rttSize,bool linear) = 0;
	
	virtual bool createShader( kkShader* out_shader, const char * v, const char * v_main, const char * f, const char * f_main,
		const char * g = nullptr, const char * g_main = nullptr) = 0;

	virtual void setCompFunc(kkGraphicsSystemCompFunc f) = 0;

	//----------------------------------------------------------------
	//----------------------------------------------------------------
	//----------------------------------------------------------------
	const v2i& getBackBufferSize() const
	{
		return m_back_buffer_size;
	}

	void setLinearFilter(bool v)
	{
		m_useLinearFilter = v;
	}

	u32 getColorDepth()
	{
		return m_color_depth;
	}

	virtual void setActiveCamera( kkCamera * c ) = 0;

	kkCamera * getActiveCamera()
	{
		return m_active_camera;
	}

};


#endif