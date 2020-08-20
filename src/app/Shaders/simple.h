// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SIMPLE_SHADER__H__
#define __SIMPLE_SHADER__H__

#include "GraphicsSystem/kkShader.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

class kkGraphicsSystem;

class ShaderSimple : public kkShader
{
	kkShaderOpenGLData                m_shaderData;
	kkArray<GLuint>                   m_uniforms;

	kkGraphicsSystem* m_gs = nullptr;

public:
	ShaderSimple(kkGraphicsSystem*);
	virtual ~ShaderSimple();

	void onShader( void * ptr, const kkMatrix4& world );
	void onCreate( void * ptr );
	void setActive();


	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"out vec3 normal;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 texCoord;\n"
		"out vec4 color;\n"
		"uniform vec4 diffuseColor;\n"
		"void main(){\n"
		  "color = diffuseColor;\n"
		"}\n";

	kkColor m_color;
};

#endif