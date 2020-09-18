#include "kkrooo.engine.h"
#include "../Common.h"
#include "points.h"

#include "GraphicsSystem/kkGraphicsSystem.h"


ShaderPoint::ShaderPoint(kkGraphicsSystem* gs)
	:
	m_gs(gs)
{
	m_color = kkColorRed;
}

ShaderPoint::~ShaderPoint()
{
}

void ShaderPoint::onShader( void * ptr, const kkMatrix4& world )
{
	auto active_camera = m_gs->getActiveCamera();

	auto V = active_camera->getViewMatrix();
	auto P = active_camera->getProjectionMatrix();
	auto W = m_world;
	

	auto VP = P * V * W;

	setActive();

	glUniformMatrix4fv(m_uniforms[ 0 ], 1, GL_FALSE, VP.getPtr() );
	//glUniform4fv(m_uniforms[ 1 ], 250, m_pos[0].data() );
	//glUniform1i( m_uniforms[ 2 ], m_num );
	//glUniform4fv(m_uniforms[ 1 ], 1, m_color.data() );
	//glUniform1i( m_uniforms[ 2 ], m_isSelected );
}

void ShaderPoint::onCreate( void * ptr )
{
	glEnable(GL_PROGRAM_POINT_SIZE);

	m_shaderData = *((kkShaderOpenGLData*)ptr);
	
	gl3wInit();
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "WVP"));
	//m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "p"));
	//m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "num"));
//	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "diffuseColor"));
//	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "is_selected"));
}

void ShaderPoint::setActive()
{
	glUseProgram(m_shaderData.m_program);
}

void ShaderPoint::setWorld( const kkMatrix4& m )
{
	m_world = m;
}