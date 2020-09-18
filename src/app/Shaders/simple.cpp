#include "kkrooo.engine.h"
#include "../Common.h"
#include "simple.h"

#include "GraphicsSystem/kkGraphicsSystem.h"


ShaderSimple::ShaderSimple(kkGraphicsSystem* gs)
	:
	m_gs(gs)
{
	m_color = kkColorWhite;
}

ShaderSimple::~ShaderSimple()
{
}

void ShaderSimple::onShader( void * ptr, const kkMatrix4& world )
{
	auto active_camera = m_gs->getActiveCamera();

	auto V = active_camera->getViewMatrix();
	auto P = active_camera->getProjectionMatrix();
	auto VP = P * V * world;

	setActive();

	glUniformMatrix4fv(m_uniforms[ 0 ], 1, GL_FALSE, VP.getPtr() );
	glUniform4fv(m_uniforms[ 1 ], 1, m_color.data() );
}

void ShaderSimple::onCreate( void * ptr )
{
	m_shaderData = *((kkShaderOpenGLData*)ptr);
	
	gl3wInit();
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "WVP"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "diffuseColor"));
}

void ShaderSimple::setActive()
{
	glUseProgram(m_shaderData.m_program);
}