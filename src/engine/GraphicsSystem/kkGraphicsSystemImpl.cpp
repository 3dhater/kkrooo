// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"
#include "kkGraphicsSystemImpl.h"
#include "Classes/Math/kkMath.h"

#include <GL/gl3w.h>

#ifdef KK_PLATFORM_WINDOWS
#include <windows.h>
#include <GL/GL.h>
#include "kkOpenGLWin32.h"
#else
#error here
#endif

#include <string>

kkGraphicsSystemImpl::kkGraphicsSystemImpl()
{
}


kkGraphicsSystemImpl::~kkGraphicsSystemImpl()
{
	if( m_open_gl )
	{
		kkDestroy( m_open_gl );
	}
}

void kkGraphicsSystemImpl::setActive(kkWindow* window)
{
	setViewport(0,0,window->getClientRect().getWidthAndHeight().x,window->getClientRect().getWidthAndHeight().y);
	_setAssociatedWindow(window);
	m_open_gl->setWindow(window);
	m_open_gl->setActive();
}

void kkGraphicsSystemImpl::initWindow(kkWindow* window )
{
	m_open_gl->initWindow(window);
}

bool kkGraphicsSystemImpl::initGS( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth )
{
	if( !output_window )
	{
		KK_PRINT_FAILED;
		return false;
	}

	m_back_buffer_size = back_buffer_size;
	m_color_depth      = color_depth;

#ifdef KK_PLATFORM_WINDOWS
	//m_open_gl = kkCreate(kkOpenGLWin32());
	m_open_gl = kkCreate<kkOpenGLWin32>();
#else
#error Need to implement
#endif

	if( !m_open_gl )
	{
		KK_PRINT_FAILED;
		return false;
	}
    
    gl3wInit();

	if( !m_open_gl->init(output_window, back_buffer_size, color_depth) )
	{
		KK_PRINT_FAILED;
		return false;
	}

	setActive(output_window);

	setCompFunc(m_compFunc);

	return true;
}

void kkGraphicsSystemImpl::setViewport( s32 x, s32 y, s32 w, s32 h )
{
	m_open_gl->setViewport( x,y,w,h );
}

void kkGraphicsSystemImpl::drawRectangle( const v2i& c1, const v2i& c2, const kkColor& color1, const kkColor& color2, kkShader * shader )
{
	m_open_gl->drawRectangle( c1, c2, color1, color2, shader );
}

void kkGraphicsSystemImpl::drawLine3D( const kkVector4& p1, const kkVector4& p2, const kkColor& color, kkShader * shader )
{
	m_open_gl->drawLine3D( p1, p2, color, shader );
}

void kkGraphicsSystemImpl::drawLine2D( const v2i& p1, const v2i& p2, const kkColor& color, kkShader * shader )
{
	m_open_gl->drawLine2D( p1, p2, color, shader );
}

void kkGraphicsSystemImpl::drawCircle2D( const v2i& position, s32 radius, s32 smoothLevel, const kkColor& color, kkShader * shader )
{
	static std::basic_string<v2i> points;

	points.clear();

	s32 q = smoothLevel;

	if( !q ) 
		q = 1u;

	f32 x, y;

	s32 s = 9 * q + 1u;
	f32 m = 40.f / q;
	for( s32 i = 0; i < s; ++i )
	{
		f32 a = (f32)i * m / 180.f * math::PI;
		x = std::sin( a );
		y = std::cos( a );
		points.push_back( v2i( (int)(x * radius), (int)(y * radius) ) + position );
	}

	size_t sz = points.size();
	for( size_t i = 1u; i < sz; ++i )
	{
		m_open_gl->drawLine2D( points[ i ], points[ i - 1 ], color, shader );
	}
}

void kkGraphicsSystemImpl::beginDraw( bool clear_canvas )
{
	m_open_gl->beginDraw(clear_canvas);
}

void kkGraphicsSystemImpl::endDraw()
{
	m_open_gl->endDraw();
}

void kkGraphicsSystemImpl::setClearColor( const kkColor& c )
{
	m_open_gl->setClearColor( c );
}

void kkGraphicsSystemImpl::update()
{
	m_open_gl->update_onSize();
}

void kkGraphicsSystemImpl::setActiveCamera( kkCamera * c )
{
	m_active_camera = c;
	m_open_gl->m_active_camera = c;
}

void kkGraphicsSystemImpl::useDepth( bool v )
{
	m_open_gl->useDepth(v);
}

void kkGraphicsSystemImpl::useBackFaceCulling( bool v )
{
	m_open_gl->useBackFaceCulling(v);
}

void kkGraphicsSystemImpl::drawMesh(kkMesh* m, const kkMatrix4& W, kkShader * shader)
{
	if(!m) return;
	m_open_gl->drawMesh(m,W,shader);
}

kkMesh* kkGraphicsSystemImpl::createMesh(kkSMesh* sMesh, kkMeshType t)
{
	return m_open_gl->createMesh(sMesh, t);
}

kkTexture* kkGraphicsSystemImpl::createTexture(kkImage* i)
{
	return m_open_gl->createTexture(i, m_useLinearFilter );
}

kkTexture* kkGraphicsSystemImpl::createTexture(const v2i& rttSize,bool linear)
{
	return m_open_gl->createTexture(rttSize,linear);
}

void kkGraphicsSystemImpl::setScissor( int x1, int y1, int x2, int y2 )
{
	return m_open_gl->setScissor( x1, y1, x2, y2 );
}

void kkGraphicsSystemImpl::useScissor( bool v )
{
	return m_open_gl->useScissor( v );
}

bool kkGraphicsSystemImpl::createShader( kkShader* out_shader, const char * v, const char * v_main, const char * f, const char * f_main,
	const char * g, const char * g_main)
{
	return m_open_gl->createShader( out_shader, v, v_main, f, f_main, g, g_main );
}

void kkGraphicsSystemImpl::drawPoint3D( const kkVector4& p, kkShader * shader )
{
	m_open_gl->drawPoint3D( p, shader );
}

void kkGraphicsSystemImpl::setCompFunc(kkGraphicsSystemCompFunc f)
{
	m_compFunc = f;
	m_open_gl->setCompFunc(f);
}