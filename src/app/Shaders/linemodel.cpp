// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"
#include "linemodel.h"

#include "GraphicsSystem/kkGraphicsSystem.h"


ShaderLineModel::ShaderLineModel(kkGraphicsSystem* gs)
	:
	m_gs(gs)
{
}

ShaderLineModel::~ShaderLineModel()
{
}

void ShaderLineModel::onShader( void * ptr, const kkMatrix4& world )
{
	auto active_camera = m_gs->getActiveCamera();

	auto V = active_camera->getViewMatrix();
	auto P = active_camera->getProjectionMatrix();
	auto VP = P * V * world;

	setActive();

	glUniformMatrix4fv(m_uniforms[ 0 ], 1, GL_FALSE, VP.getPtr() );
	
	glUniform4fv(m_uniforms[ 1 ], 1, edge_color.data() );
}

void ShaderLineModel::onCreate( void * ptr )
{
	m_shaderData = *((kkShaderOpenGLData*)ptr);
	
	gl3wInit();
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "WVP"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "edgeColor"));
}

void ShaderLineModel::setActive()
{
	glUseProgram(m_shaderData.m_program);
}