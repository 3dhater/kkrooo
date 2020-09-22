#include "kkrooo.engine.h"
#include "../Common.h"
#include "../Application.h"
#include "Material/kkMaterial.h"

#include "scene3DobjectDefault.h"


#include "GraphicsSystem/kkGraphicsSystem.h"


ShaderScene3DObjectDefault::ShaderScene3DObjectDefault(kkGraphicsSystem* gs)
	:
	m_gs(gs)
{
	m_diffuseColor = kkColorWhite;
	m_whiteTexture = kkSingleton<Application>::s_instance->getWhiteTexture();
}

ShaderScene3DObjectDefault::~ShaderScene3DObjectDefault()
{
}

void ShaderScene3DObjectDefault::onShader( void * ptr, const kkMatrix4& world )
{
	auto active_camera = m_gs->getActiveCamera();

	auto W = world;
	auto V = active_camera->getViewMatrix();
	auto P = active_camera->getProjectionMatrix();
	auto WVP = P * V * world;

	setActive();

	auto sunDir = -active_camera->getDirection();

	glUniformMatrix4fv(m_uniforms[ 0 ], 1, GL_FALSE, WVP.getPtr() );
	glUniformMatrix4fv(m_uniforms[ 1 ], 1, GL_FALSE, W.getPtr() );
	glUniform4fv(m_uniforms[ 2 ], 1, m_diffuseColor.data() );
	glUniform4fv(m_uniforms[ 3 ], 1, sunDir.data() );

	unsigned long long * t_ptr = (unsigned long long *)m_whiteTexture->getHandle();
	if( m_diffTex )
	{
		if( m_diffTex->m_texture )
		{
			t_ptr = (unsigned long long *)m_diffTex->m_texture->getHandle();
			glBindTexture(GL_TEXTURE_2D,(GLuint)*t_ptr);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,(GLuint)*t_ptr);
		}
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,(GLuint)*t_ptr);
	}
	glActiveTexture(GL_TEXTURE0);

//	t_ptr = (unsigned long long *)m_projTexture->getHandle();
//	glBindTexture(GL_TEXTURE_2D,(GLuint)*t_ptr);
//	glActiveTexture(GL_TEXTURE1);
	//m_fragmentFlags |= 1;
	//glUniform1i(m_uniforms[ 6 ], m_fragmentFlags );
}

void ShaderScene3DObjectDefault::onCreate( void * ptr )
{
	m_shaderData = *((kkShaderOpenGLData*)ptr);
	
	gl3wInit();
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "WVP"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "W"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "diffuseColor"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "sunDir"));
	m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "diffuseTexture"));
	//m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "projectionTexture"));
	//m_uniforms.push_back( glGetUniformLocation(m_shaderData.m_program, "fragmentFlags"));
}

void ShaderScene3DObjectDefault::setActive()
{
	glUseProgram(m_shaderData.m_program);
}