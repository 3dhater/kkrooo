// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_OPENGL_H__
#define __KK_OPENGL_H__

#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMatrix.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

#include "GraphicsSystem/kkGraphicsSystem.h"
#include "GraphicsSystem/kkShader.h"

struct kkImage;
class kkTexture;

class kkWindow;
class kkOpenGLShader;

/// Base class
class kkOpenGL
{
	friend class kkGraphicsSystemImpl;

	kkShaderOpenGLData m_shaderOpenGLData;

	v4i m_current_view_port;

	kkMatrix4 m_defaultMatrix;

protected:
	kkColor m_clear_color = kkColorRed;

	bool initShaders();

	kkWindow* m_window = nullptr;
	kkCamera * m_active_camera = nullptr;

	/// if client rect <= 0 then do not draw gui
	bool m_skip_render = false;

	kkOpenGLShader* m_shader_3dline = nullptr;
	kkOpenGLShader* m_shader_2dline = nullptr;
	kkOpenGLShader* m_shader_rectangle = nullptr;
	kkOpenGLShader* m_shader_polyMesh  = nullptr;
	kkOpenGLShader* m_shader_polyMesh_lines  = nullptr;

	GLuint m_3dline_VAO = 0;
	GLuint m_2dline_VAO = 0;
	GLuint m_rectangle_VAO = 0;

	kkMatrix4 m_ortho_projection;

	bool _createShaderFor2DRectangle();
	bool _createShaderFor2DLine();
	bool _createShaderFor3DLine();
	bool _createShaderForPolyMesh();
	bool _createShaderForPolyMesh_lines();
	

public:

	kkOpenGL();
	virtual ~kkOpenGL();

	virtual bool init(kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth) = 0;
	virtual void endDraw() = 0;
	virtual void setActive() = 0;

	virtual void setWindow( kkWindow* window ) = 0;
	virtual void initWindow(kkWindow* window ) = 0;

	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void drawPoint3D( const kkVector4& p, kkShader * shader );
	void beginDraw( bool clear_canvas );
	void setClearColor( const kkColor& c );
	void drawRectangle( const v2i& c1, const v2i& c2, const kkColor& color1, const kkColor& color2, kkShader * shader = nullptr );
	void drawLine2D( const v2i& p1, const v2i& p2, const kkColor& color, kkShader * shader = nullptr );
	void drawLine3D( const kkVector4& p1, const kkVector4& p2, const kkColor& color, kkShader * shader = nullptr );
	void drawMesh(kkMesh*, const kkMatrix4& W, kkShader * shader = nullptr);
	kkMesh* createMesh(kkSMesh*, kkMeshType );
	kkTexture* createTexture(kkImage* i, bool linear);
	kkTexture* createTexture(const v2i& rttSize, bool linear);
	bool createShader( kkShader* out_shader, const char * v, const char * v_main, const char * f, const char * f_main, const char * g = nullptr, const char * g_main = nullptr );
	void update_onSize();
	void setViewport( s32 x, s32 y, s32 w, s32 h );
	void useDepth( bool );
	void useBackFaceCulling( bool v );

	void useScissor( bool v );
	void setScissor( int x1, int y1, int x2, int y2 );

	void setCompFunc(kkGraphicsSystemCompFunc f);
};


#endif