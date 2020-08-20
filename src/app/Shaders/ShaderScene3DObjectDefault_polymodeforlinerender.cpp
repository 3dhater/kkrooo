// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "../Application.h"
#include "Material/kkMaterial.h"

#include "ShaderScene3DObjectDefault_polymodeforlinerender.h"


#include "GraphicsSystem/kkGraphicsSystem.h"


ShaderScene3DObjectDefault_polymodeforlinerender::ShaderScene3DObjectDefault_polymodeforlinerender(kkGraphicsSystem* gs)
	:
	m_gs(gs)
{
}

ShaderScene3DObjectDefault_polymodeforlinerender::~ShaderScene3DObjectDefault_polymodeforlinerender()
{
}

void ShaderScene3DObjectDefault_polymodeforlinerender::onShader( void * ptr, const kkMatrix4& world )
{
	auto active_camera = m_gs->getActiveCamera();

	auto V = active_camera->getViewMatrix();
	auto P = active_camera->getProjectionMatrix();
	auto WVP = P * V * world;

	auto 

	setActive();

	glUniformMatrix4fv(m_uniforms[ 0 ], 1, GL_FALSE, WVP.getPtr() );
}

void ShaderScene3DObjectDefault_polymodeforlinerender::onCreate( void * ptr )
{
	m_shaderData = *((kkShaderOpenGLData*)ptr);
	
	gl3wInit();
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "WVP"));
}

void ShaderScene3DObjectDefault_polymodeforlinerender::setActive()
{
	glUseProgram(m_shaderData.m_program);
}