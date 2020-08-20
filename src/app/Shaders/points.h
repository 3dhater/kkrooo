// SPDX-License-Identifier: GPL-3.0-only
#ifndef __POINT_SHADER__H__
#define __POINT_SHADER__H__

#include "GraphicsSystem/kkShader.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Containers/kkPair.h"
#include "Classes/Strings/kkString.h"
#include "Classes/kkColor.h"

#include <GL/gl3w.h>

class kkGraphicsSystem;

class ShaderPoint : public kkShader
{
	kkShaderOpenGLData                m_shaderData;
	kkArray<GLuint>                   m_uniforms;

	kkGraphicsSystem* m_gs = nullptr;
	kkMatrix4 m_world;

public:
	ShaderPoint(kkGraphicsSystem*);
	virtual ~ShaderPoint();

	void onShader( void * ptr, const kkMatrix4& world );
	void onCreate( void * ptr );
	void setActive();

	const char * text_v = 
		"#version 330\n"
		"in vec3 inputPosition;\n"
		"in vec4 inputColor;\n"
		"out vec4 vertexColor;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_PointSize = 4.0;\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"gl_Position.z = gl_Position.z - 0.002f;\n"
		"vertexColor = inputColor;\n"
		"}\n";
	const char * text_f = 
		"#version 330\n" 
		"in vec4 vertexColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vertexColor;\n"
		"}\n";
	/*const char * text_v = 
		"#version 330\n"
		"void main(){\n"
		"gl_Position = vec4(0.f,0.f,0.f,1.0f);\n"
		"}\n";*/
	//const char * text_g = 
	//	"#version 330\n" 
	//	"layout (points) in;\n"
	//	"layout (points, max_vertices = 250) out;\n"
	//	"uniform mat4 WVP;\n"
	//	"uniform vec4 p[250];\n"
	//	"uniform int num;\n"
	//	"void createPoint(in float offset_x, in float offset_y, in float offset_z){\n"
	//	  "gl_Position = WVP * vec4(offset_x,offset_y,offset_z,1.f); \n"
	//	  "gl_PointSize = 4.0;\n"
	//	  "EmitVertex();\n"
	//	"}\n"
	//	"void main(){\n"
	//	"int N = num;\n"
	//	"for(int i = 0; i < N; ++i){createPoint( p[i].x, p[i].y, p[i].z );}"
	//	  "EndPrimitive();\n"
	//	"}\n";
	/*const char * text_f = 
		"#version 330\n" 
		"in vec2 texCoord;\n"
		"out vec4 color;\n"
		"uniform vec4 diffuseColor;\n"
		"uniform int is_selected;\n"
		"void main(){\n"
		  "color = vec4(0.f,0.f,1.f,1.f);\n"
		"if(is_selected==1)\n"
		  "color = vec4(1.f,0.f,0.f,1.f);\n"
		"}\n";*/

	kkColor   m_color;
	kkVector4 m_pos[250];
	
	int m_num = 0;
	int m_isSelected = 0;

	void setWorld( const kkMatrix4& );
};

#endif