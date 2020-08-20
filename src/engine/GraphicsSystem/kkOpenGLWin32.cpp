// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#ifdef KK_PLATFORM_WINDOWS

#include <Windows.h>
//#include <gl\gl.h>
#include <GL/gl3w.h>

#pragma comment(lib, "opengl32.lib")

#include "Classes/kkAABB.h"
#include "GraphicsSystem/kkMesh.h"
#include "kkOpenGLWin32.h"
#include "Window/kkWindow.h"
#include "../Window/kkWindowWin32.h"

/// http://www.rastertek.com/gl40tut03.html

#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042
#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SWAP_METHOD_ARB            0x2007
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_SWAP_EXCHANGE_ARB          0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A
#define WGL_TYPE_RGBA_ARB              0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_TEXTURE0                       0x84C0
#define GL_BGRA                           0x80E1
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

kkOpenGLWin32::kkOpenGLWin32()
{
}

kkOpenGLWin32::~kkOpenGLWin32()
{
	if(m_renderingContext)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_renderingContext);
	}

}

void kkOpenGLWin32::setWindow( kkWindow* window )
{
	m_window = window; 
	m_hWnd = (HWND)m_window->getHandle();
	kkWindowWin32 * w = (kkWindowWin32*)window;
	m_dc   = w->m_dc;
}

void kkOpenGLWin32::initWindow(kkWindow* window )
{
	kkWindowWin32 * w = (kkWindowWin32*)window;
	auto dc   = w->m_dc;
	if(!dc)
	{
		KK_PRINT_FAILED;
		return;
	}

	int attributeListInt[] = 
	{
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,     32,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
		WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
		WGL_STENCIL_BITS_ARB,   8,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB,        4,//maxSamples,
		0
	};

	int pixelFormat[1];
	unsigned int formatCount;
	auto result = wglChoosePixelFormatARB(dc, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if(result != 1)
	{
		KK_PRINT_FAILED;
		return;
	}

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	result = SetPixelFormat(dc, pixelFormat[0], &pixelFormatDescriptor);
	if(result != 1)
	{
		KK_PRINT_FAILED;
		return;
	}
}

bool kkOpenGLWin32::init(kkWindow* output_window, const v2i& back_buffer_size, u32 color_depth)
{
	m_window = output_window; 

	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = DefWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(0);
	wc.hIcon         = 0;
	wc.hIconSm       = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"OPENGL";
	wc.cbSize        = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
	auto tmp_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, L"q", WS_POPUP,
				0, 0, 128, 128, NULL, NULL, wc.hInstance, NULL);
	if(tmp_hwnd == NULL)
	{
		KK_PRINT_FAILED;
		return false;
	}
	ShowWindow(tmp_hwnd, SW_HIDE);

	HDC dc = GetDC( tmp_hwnd );
	if(!dc)
	{
		KK_PRINT_FAILED;
		return false;
	}

	PIXELFORMATDESCRIPTOR pf;
	auto error = SetPixelFormat(dc, 1, &pf);
	if(error != 1)
	{
		KK_PRINT_FAILED;
		return false;
	}

	auto rc = wglCreateContext(dc);
	if(!rc)
	{
		KK_PRINT_FAILED;
		return false;
	}

	error = wglMakeCurrent(dc, rc);
	if(error != 1)
	{
		KK_PRINT_FAILED;
		return false;
	}

	if( !load_extensions())
	{
		KK_PRINT_FAILED;
		return false;
	}

	int maxSamples = 0;
	glGetIntegerv( GL_MAX_SAMPLES, &maxSamples );
	//printf("maxSamples = %i\n",maxSamples);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rc);
	ReleaseDC(tmp_hwnd, dc);

	DestroyWindow(tmp_hwnd);

	initWindow(output_window);
	m_hWnd = (HWND)output_window->getHandle();

	kkWindowWin32 * w = (kkWindowWin32*)output_window;
	m_dc   = w->m_dc;

	int v_maj = 3;
	int v_min = 3;
	
	BOOL result = TRUE;

	while(true)
	{
		int attributeList[5];
		attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attributeList[1] = v_maj;
		attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attributeList[3] = v_min;
		attributeList[4] = 0;// Null terminate the attribute list.
		
		m_renderingContext = wglCreateContextAttribsARB(m_dc, 0, attributeList);


		if(m_renderingContext)
		{
			result = TRUE;
			break;
		}
		else
		{
			--v_min;

			if( v_min < 0 )
			{
				if( v_maj == 3 )
				{
					result = FALSE;
					break;
				}

				v_min = 3;
				v_maj = 3;
			}
		}
	}

	if( result == FALSE )
	{
		KK_PRINT_FAILED;
		return false;
	}

	setActive();


	result = wglSwapIntervalEXT(0);
	if(result != 1)
	{
		KK_PRINT_FAILED;
		return false;
	}

	//printf("%s %i.%i\n","Init OpenGL", v_maj, v_min );
	//printf("%s\n",glGetString(GL_RENDERER));

	if( !initShaders() )
	{
		KK_PRINT_FAILED;
		return false;
	}

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	

	auto cr = output_window->getClientRect();
	
	glViewport(0, 0, cr.getWidthAndHeight().x, cr.getWidthAndHeight().y);

	return true;
}

void kkOpenGLWin32::setActive()
{
	//m_dc   = GetDC(m_hWnd);
	wglMakeCurrent(m_dc, m_renderingContext);
}


bool kkOpenGLWin32::load_extensions()
{
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if(!wglChoosePixelFormatARB)
	{
		return false;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if(!wglCreateContextAttribsARB)
	{
		return false;
	}

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(!wglSwapIntervalEXT)
	{
		return false;
	}

	return true;
}

void kkOpenGLWin32::endDraw()
{
	SwapBuffers( m_dc );
//	ReleaseDC( m_hWnd, m_dc );
}

#endif