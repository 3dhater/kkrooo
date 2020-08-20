#ifndef __KK_OPENGL_WIN32_H__
#define __KK_OPENGL_WIN32_H__

#include "kkOpenGL.h"

#include <GL/gl.h>
#include "Classes/kkColor.h"

typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, 
                                                        int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);


class kkWindow;
class kkOpenGLWin32 : public kkOpenGL
{
	HDC m_dc = 0;
	HWND m_hWnd;
	HGLRC m_renderingContext;

	bool load_extensions();

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	
public:

	kkOpenGLWin32();
	virtual ~kkOpenGLWin32();
	
	void setActive();
	void setWindow( kkWindow* window );
	void initWindow(kkWindow* window );

	bool init( kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth);

	void endDraw();
};


#endif