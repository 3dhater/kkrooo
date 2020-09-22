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
		//"out vec4 viewPosition;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 W;\n"
		"void main(){\n"
		//"viewPosition = WVP * vec4(inputPosition.xyz,1.0f);\n"
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
		//"in vec4 viewPosition;\n"
		"out vec4 color;\n"
		"uniform sampler2D diffuseTexture;\n"
		//"uniform sampler2D projectionTexture;\n"
		"uniform vec4 diffuseColor;\n"
		"uniform vec4 sunDir;\n"
		//"uniform int fragmentFlags;\n"
		"void main(){\n"
		  "float lightIntensity = clamp(dot(normal, sunDir.xyz), 0.0f, 1.0f);\n"
		  "vec4 textureColor = texture(diffuseTexture,texCoord);\n"
		  "color = textureColor * clamp((diffuseColor*lightIntensity), 0.0f, 1.0f);\n"
		//  "vec2 projectTexCoord;\n"
		//"projectTexCoord.x =  viewPosition.x / viewPosition.w / 2.0f + 0.5f;\n"
		//"projectTexCoord.y = -viewPosition.y / viewPosition.w / 2.0f + 0.5f;"
		//"projectTexCoord.x = projectTexCoord.x * 50.f;\n"
		//"projectTexCoord.y = projectTexCoord.y * 50.f;\n"
		//"if((fragmentFlags & 1)==1){\n"
		//  "vec4 projectionColor = texture(projectionTexture,projectTexCoord);\n"
		//  "color = clamp(color + projectionColor, 0.f, 1.f);"
		//  "}\n"
		  "if(vertColor.w > 1.f ){\n"
		  "color = clamp(vertColor+(textureColor*0.2f), 0.f, 1.f);//color * vertColor;\n"
		  "}\n"
		"}\n";

	kkColor m_diffuseColor;
	kkImageContainerNode* m_diffTex = nullptr;
	kkTexture * m_projTexture = nullptr;
	//kkVector4 m_eyePosition;

	//s32 m_fragmentFlags = 0;
	void thisSelectedObject(bool v)
	{
	//	if(v)
	//		m_fragmentFlags = 1;
	//	else 
	//		m_fragmentFlags = 0;
	}
};

#endif