// SPDX-License-Identifier: GPL-3.0-only
#ifndef __SCENE3DOBJDEF_SHADER__H__
#define __SCENE3DOBJDEF_SHADER__H__

#include "GraphicsSystem/kkShader.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

class kkGraphicsSystem;

class ShaderScene3DObjectDefault : public kkShader
{
	kkShaderOpenGLData                m_shaderData;
	kkArray<GLuint>                   m_uniforms;

	kkGraphicsSystem* m_gs = nullptr;

	kkTexture * m_whiteTexture = nullptr;
public:
	ShaderScene3DObjectDefault(kkGraphicsSystem*);
	virtual ~ShaderScene3DObjectDefault();

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
		"uniform mat4 W;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = 1.f - inputTexCoord.y;\n"
		"normal = mat3(W) * inputNormal;\n"
		"normal = normalize(normal);\n"
		"vertColor = inputColor;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 texCoord;\n"
		"in vec3 normal;\n"
		"in vec4 vertColor;\n"
		"out vec4 color;\n"
		"uniform sampler2D diffuseTexture;\n"
		"uniform vec4 diffuseColor;\n"
		"uniform vec4 sunDir;\n"
		"void main(){\n"
		  "float lightIntensity = clamp(dot(normal, sunDir.xyz), 0.0f, 1.0f);\n"
		  "color = texture(diffuseTexture,texCoord);\n"
		  "color = color * clamp((diffuseColor*lightIntensity), 0.0f, 1.0f);\n"
		  "if(vertColor.w > 1.f )\n"
		  "color = vertColor;//color * vertColor;\n"
		"}\n";

	kkColor m_diffuseColor;
	kkImageContainerNode * m_diffTex = nullptr;
	//kkVector4 m_eyePosition;
};

#endif