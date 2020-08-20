// SPDX-License-Identifier: GPL-3.0-only
#ifndef __LINEMODEL_SHADER__H__
#define __LINEMODEL_SHADER__H__

#include "GraphicsSystem/kkShader.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

class kkGraphicsSystem;

class ShaderLineModel : public kkShader
{
	kkShaderOpenGLData                m_shaderData;
	kkArray<GLuint>                   m_uniforms;

	kkGraphicsSystem* m_gs = nullptr;

public:
	ShaderLineModel(kkGraphicsSystem*);
	virtual ~ShaderLineModel();

	void onShader( void * ptr, const kkMatrix4& world );
	void onCreate( void * ptr );
	void setActive();


	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec4 inputColor;\n"
		"out vec4 vertexColor;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"gl_Position.z = gl_Position.z - 0.001f;\n"
		"vertexColor = inputColor;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec4 vertexColor;\n"
		"out vec4 color;\n"
		"uniform vec4 edgeColor;\n"
		"void main(){\n"
		  "color = edgeColor * vertexColor;\n"
		"}\n";

	kkColor edge_color;
};

#endif