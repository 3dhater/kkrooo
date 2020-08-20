// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "GraphicsSystem/kkShader.h"
#include "kkOpenGLShader.h"


kkOpenGLShader::kkOpenGLShader()
{
}

kkOpenGLShader::~kkOpenGLShader()
{
	if( m_VAO )
	{
		glDeleteVertexArrays(1,&m_VAO);
	}

	if( m_program )
	{
		glDeleteProgram(m_program);
	}
}

GLuint kkOpenGLShader::_createShaderProgram(GLuint v, GLuint f, GLuint g)
{
	auto program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);
	if( g )
		glAttachShader(program, g);
	glLinkProgram(program);
	return program;
}

GLuint kkOpenGLShader::_createShader(GLenum type, const char * text)
{
	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &text, nullptr );
	glCompileShader(shader);
	return shader;
}

bool kkOpenGLShader::_checkShader(u32 shader)
{
	s32 compile_result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
	if(compile_result == GL_FALSE)
	{
		GLint loglen=0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
		char * buf = new char[loglen+1];
		buf[loglen]=0;
		glGetShaderInfoLog(shader, loglen, NULL, buf);
		std::fprintf(stderr,"Shader error: %s\n", buf);
		delete[] buf;
		return false;
	}
	return true;
}

bool kkOpenGLShader::_checkProgram(u32 program)
{
	GLint link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		GLint loglen=0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
		char * buf = new char[loglen+1];
		buf[loglen]=0;
		glGetProgramInfoLog(program, loglen, NULL, buf);
		std::fprintf(stderr,"Shader error: %s\n", buf);
		delete[] buf;
		return false;
	}
	return true;
}

bool kkOpenGLShader::create(const char * vert, const char * frag)
{
	auto shader_vert = _createShader(GL_VERTEX_SHADER,vert);
	if(!_checkShader(shader_vert))
	{
		return false;
	}
	auto shader_frag = _createShader(GL_FRAGMENT_SHADER,frag);
	if(!_checkShader(shader_frag))
	{
		return false;
	}
	m_program = _createShaderProgram(shader_vert,shader_frag);
	if(!_checkProgram(m_program))
	{
		return false;
	}

	return true;
}

void kkOpenGLShader::addUniform(const char* n)
{
	setActive(); /// убрать?
	m_uniforms.push_back( kkPair<GLuint,kkStringA>( glGetUniformLocation(m_program, n), kkStringA(n) ) );
}

void kkOpenGLShader::onShader( void * ptr, const kkMatrix4& world )
{
}

GLuint kkOpenGLShader::getUniform(u32 i)
{
	return m_uniforms[i].m_first;
}

GLuint kkOpenGLShader::getProgram()
{
	return m_program;
}

void kkOpenGLShader::onCreate( void * ptr )
{
}

void kkOpenGLShader::setActive()
{
	glUseProgram(m_program);
}