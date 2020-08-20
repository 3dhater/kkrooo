// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SCENE3DOBJDEF_SHADER_POLYLINE__H__
#define __SCENE3DOBJDEF_SHADER_POLYLINE__H__

#include "GraphicsSystem/kkShader.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

class kkGraphicsSystem;

class ShaderScene3DObjectDefault_polymodeforlinerender : public kkShader
{
	kkShaderOpenGLData                m_shaderData;
	kkArray<GLuint>                   m_uniforms;

	kkGraphicsSystem* m_gs = nullptr;
public:
	ShaderScene3DObjectDefault_polymodeforlinerender(kkGraphicsSystem*);
	virtual ~ShaderScene3DObjectDefault_polymodeforlinerender();

	void onShader( void * ptr, const kkMatrix4& world );
	void onCreate( void * ptr );
	void setActive();


	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"in vec4 inputColor;\n"
		"out vec2 texCoord;\n"
		"out vec3 normal;\n"
		"out vec4 vertColor;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = 1.f - inputTexCoord.y;\n"
		"normal = inputNormal;\n"
		"vertColor = inputColor;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 texCoord;\n"
		"in vec3 normal;\n"
		"in vec4 vertColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(1.f,1.f,1.f,1.f);\n"
		  "if(vertColor.w > 1.f )\n"
		  "color = vertColor;//color * vertColor;\n"
		  "else discard;\n"
		"}\n";

};

#endif