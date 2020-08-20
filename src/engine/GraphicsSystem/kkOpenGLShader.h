// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_OPENGL_SHADER_H__
#define __KK_OPENGL_SHADER_H__

#include <GL/gl3w.h>

#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"

class kkOpenGLShader : public kkShader
{
	
	GLuint m_program = 0;

	kkArray<kkPair<GLuint,kkStringA>> m_uniforms;

public:
	kkOpenGLShader();
	virtual ~kkOpenGLShader();

	bool create(const char * vert, const char * frag);

	void onShader( void * ptr, const kkMatrix4& world );
	void onCreate( void * ptr );
	void setActive();

	void addUniform(const char*);

	GLuint getUniform(u32);
	GLuint getProgram();


	static GLuint _createShaderProgram(GLuint v, GLuint f, GLuint g = 0);
	static GLuint _createShader(GLenum type, const char * text);
	static bool _checkShader(u32 shader);
	static bool _checkProgram(u32 program);

	GLuint m_VAO = 0;
};

#endif