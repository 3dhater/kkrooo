// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_GS_H__
#define __KK_GS_H__

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "Window/kkWindow.h"

#include "kkOpenGL.h"

struct ImGuiContext;

class kkGraphicsSystemImpl : public kkGraphicsSystem
{
	kkOpenGL * m_open_gl = nullptr;
public:

	kkGraphicsSystemImpl();
	virtual ~kkGraphicsSystemImpl();

	void initWindow(kkWindow*);

	void setActive(kkWindow*);


	bool initGS( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth );

	void drawPoint3D( const kkVector4& p, kkShader * shader );

	void beginDraw( bool clear_canvas );
	void endDraw();
	void update();
	void setViewport( s32 x, s32 y, s32 w, s32 h );
	void setDefaultTexture(kkTexture * t);
	void drawRectangle( const v2i& c1, const v2i& c2, const kkColor& color1, const kkColor& color2 );
	void drawLine2D( const v2i& p1, const v2i& p2, const kkColor& color, kkShader * shader = nullptr );
	void drawCircle2D( const v2i& position, s32 radius, s32 smoothLevel, const kkColor& color, kkShader * shader = nullptr );
	void drawLine3D( const kkVector4& p1, const kkVector4& p2, const kkColor& color, kkShader * shader = nullptr );
	kkMesh* createMesh(kkSMesh*, kkMeshType);
	void drawMesh(kkMesh*, const kkMatrix4& W, kkShader * shader = nullptr);
	kkTexture* createTexture(kkImage*);
	kkTexture* createTexture(const v2i& rttSize,bool linear);
	bool createShader( kkShader* out_shader, const char * v, const char * v_main, const char * f, const char * f_main,
		const char * g, const char * g_main);
	void setClearColor( const kkColor& c );
	void setActiveCamera( kkCamera * c );
	void useDepth( bool );
	void useBackFaceCulling( bool );
	void useScissor( bool );
	void setScissor( int x1, int y1, int x2, int y2 );
	void setCompFunc(kkGraphicsSystemCompFunc f);
	void setTarget( kkTexture* );
};


#endif