// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_SHADER_H__
#define __KKROO_SHADER_H__

#include "Classes/Math/kkMatrix.h"

class kkShaderData
{
public:
	kkShaderData(){}
	virtual ~kkShaderData(){}
};

class kkShaderOpenGLData : public kkShaderData
{
public:
	kkShaderOpenGLData(){}
	virtual ~kkShaderOpenGLData(){}

	u32 m_program = 0;
};

class kkShader
{
protected:

	kkShaderData * m_shaderData = nullptr;

public:
	kkShader()
	{
	}

	virtual ~kkShader()
	{
	}

	// если работает OpenGL то ptr = kkShaderOpenGLData
	virtual void onShader( void * ptr, const kkMatrix4& world ) = 0;

	// вызывается при создании шейдера
	// место где нужно получить uniforms
	virtual void onCreate( void * ptr ) = 0;

	// вызывается когда начинает рисовать.
	// вызывает перед onShader
	virtual void setActive() = 0;

	kkShaderData * getShaderData()
	{
		return m_shaderData;
	}
};

#endif