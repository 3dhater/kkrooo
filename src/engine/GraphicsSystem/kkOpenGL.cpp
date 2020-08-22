// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"
#include "Classes/kkColor.h"

#include "Window/kkWindow.h"
#include "SceneSystem/kkCamera.h"
#include "GraphicsSystem/kkMesh.h"
#include "GraphicsSystem/kkTexture.h"
#include "GraphicsSystem/kkShader.h"

#include "kkOpenGL.h"
#include "kkOpenGLModel.h"
#include "kkOpenGLShader.h"
#include "kkOpenGLTexture.h"

#include <GL/gl3w.h>

kkOpenGL::kkOpenGL()
{
	update_onSize();
}

kkOpenGL::~kkOpenGL()
{
	if( m_shader_3dline ){ kkDestroy(m_shader_3dline); }
	if( m_shader_2dline ){ kkDestroy(m_shader_2dline); }
	if( m_shader_rectangle ){ kkDestroy(m_shader_rectangle); }
	if( m_shader_polyMesh ){ kkDestroy(m_shader_polyMesh); }
	if( m_shader_polyMesh_lines ){ kkDestroy(m_shader_polyMesh_lines); }

	glDeleteVertexArrays(1,&m_3dline_VAO);
	glDeleteVertexArrays(1,&m_2dline_VAO);
	glDeleteVertexArrays(1,&m_rectangle_VAO);

}

void kkOpenGL::beginDraw( bool clear_canvas )
{
	if( clear_canvas )
	{

		GLbitfield mask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
		
		if( clear_canvas )
		{
			
		}

		glClear(mask);
	}
}

void kkOpenGL::setClearColor( const kkColor& C )
{
	m_clear_color = C;
	auto c = m_clear_color.data();
	glClearColor( c[0], c[1], c[2], c[3] ); 
}


void kkOpenGL::drawRectangle( const v2i& c1, const v2i& c2, const kkColor& color1, const kkColor& color2, kkShader * shader )
{
	//gl3wInit();
    glDisable(GL_CULL_FACE);
	
	if( shader )
	{
		shader->setActive();
		shader->onShader( &m_shaderOpenGLData, m_defaultMatrix );
	}
	else
	{
		m_shader_rectangle->setActive();
		v4f corners((f32)c1.x,(f32)c1.y,(f32)c2.x,(f32)c2.y);
		glUniformMatrix4fv(m_shader_rectangle->getUniform(0), 1, GL_FALSE, m_ortho_projection.getPtr() );
		glUniform4fv(m_shader_rectangle->getUniform(1), 1, corners.data() );
		glUniform4fv(m_shader_rectangle->getUniform(2), 1, color1.data() );
		glUniform4fv(m_shader_rectangle->getUniform(3), 1, color2.data() );
	}

	glBindVertexArray(m_rectangle_VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void kkOpenGL::drawLine2D( const v2i& p1, const v2i& p2, const kkColor& color, kkShader * shader )
{
	glDisable(GL_CULL_FACE);
	if( shader )
	{
		shader->setActive();
		shader->onShader( &m_shaderOpenGLData, m_defaultMatrix );
	}
	else
	{
		m_shader_2dline->setActive();
		v4f positions((f32)p1.x,(f32)p1.y,(f32)p2.x,(f32)p2.y);
		glUniformMatrix4fv(m_shader_2dline->getUniform(0), 1, GL_FALSE, m_ortho_projection.getPtr() );
		glUniform4fv(m_shader_2dline->getUniform(1), 1, positions.data() );
		glUniform4fv(m_shader_2dline->getUniform(2), 1, color.data() );
	}

	glBindVertexArray(m_2dline_VAO);
	glDrawArrays(GL_LINES, 0, 2);
}

void kkOpenGL::drawMesh(kkMesh* m, const kkMatrix4& w, kkShader * shader )
{
	kkOpenGLModel * model = (kkOpenGLModel *)m;
    
	if( shader )
	{
		shader->setActive();
		shader->onShader( &m_shaderOpenGLData, w );
		switch(model->m_type)
		{
		case kkMeshType::Triangles:
		{
			glBindVertexArray(model->m_VAO);
			glDrawElements(GL_TRIANGLES, model->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
		default:
			break;
		case kkMeshType::Lines:
		{
			glBindVertexArray(model->m_VAO);
			glDrawElements(GL_LINES, model->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
			break;
		case kkMeshType::Points:
		{
			glBindVertexArray(model->m_VAO);
			glDrawElements(GL_POINTS, model->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
			break;
		}
	}
	else
	{
		auto V = m_active_camera->getViewMatrix();
		auto P = m_active_camera->getProjectionMatrix();
		auto W = w;
		auto WV  = V * W;
		auto WVP = P * WV;

		auto WVi = WV;
		WVi.invert();

		switch(model->m_type)
		{
		case kkMeshType::Triangles:
		{
			m_shader_polyMesh->setActive();

			glUniformMatrix4fv(m_shader_polyMesh->getUniform(0), 1, GL_FALSE, WVP.getPtr() );
			glUniformMatrix4fv(m_shader_polyMesh->getUniform(1), 1, GL_TRUE, WVi.getPtr() );

			v3f lightDir1(1.f, 0.1f, 0.6f);
			v3f lightDir2(-1.f, 0.3f, 0.6f);

			glUniform3fv(m_shader_polyMesh->getUniform(2), 1, lightDir1.data() );
			glUniform3fv(m_shader_polyMesh->getUniform(3), 1, lightDir2.data() );

			glBindVertexArray(model->m_VAO);
			glDrawElements(GL_TRIANGLES, model->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
		default:
			break;

		case kkMeshType::Lines:
		{
			m_shader_polyMesh_lines->setActive();

			glUniformMatrix4fv(m_shader_polyMesh_lines->getUniform(0), 1, GL_FALSE, WVP.getPtr() );

			glBindVertexArray(model->m_VAO);
			glDrawElements(GL_LINES, model->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
			break;
		}
	}


}

kkMesh* kkOpenGL::createMesh(kkSMesh* sMesh, kkMeshType type )
{
	kkOpenGLModel * mesh = kkCreate<kkOpenGLModel>();
	if( !mesh->init(sMesh, type) )
	{
		kkDestroy(mesh);
		return nullptr;
	}
	return mesh;
}

bool kkOpenGL::initShaders()
{
	gl3wInit();
	if(!_createShaderFor2DRectangle())
		return false;
	if(!_createShaderFor3DLine())
		return false;
	if(!_createShaderFor2DLine())
		return false;
	if(!_createShaderForPolyMesh())
		return false;
	if(!_createShaderForPolyMesh_lines())
		return false;
    return true;
}

bool kkOpenGL::_createShaderFor2DRectangle()
{
	 const char * text_v = 
		"#version 130\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 Corners;\n"
		"uniform vec4 Color1;\n"
		"uniform vec4 Color2;\n"
		"out vec4 out_color;\n"
		"void main(){\n"
		"vec4 vertices[6] = vec4[6](\n"
									"vec4( Corners.z,  Corners.w,  0, 1.0),\n"
                                    "vec4( Corners.x,  Corners.w,  0, 1.0),\n"
                                    "vec4( Corners.z,  Corners.y,  0, 1.0),\n"
									"vec4( Corners.x,  Corners.w,  0, 1.0),\n"
									"vec4( Corners.x,  Corners.y,  0, 1.0),\n"
									"vec4( Corners.z,  Corners.y, 0,  1.0)\n"
										");\n"
		"gl_Position = ProjMtx * vertices[gl_VertexID];\n"
		"switch( gl_VertexID ){\n"
		"case 0:out_color = Color2;break;\n"
		"case 1:out_color = Color2;break;\n"
		"case 3:out_color = Color2;break;\n"
		"default:out_color = Color1;break;\n"
		"}\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec4 out_color;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(out_color.xyz, 1.0);\n"
		"}\n";

	m_shader_rectangle = kkCreate<kkOpenGLShader>();

	if( !m_shader_rectangle->create(text_v, text_f) )
	{
		kkDestroy(m_shader_rectangle); m_shader_rectangle = 0;
		return false;
	}

	m_shader_rectangle->addUniform("ProjMtx");
	m_shader_rectangle->addUniform("Corners");
	m_shader_rectangle->addUniform("Color1");
	m_shader_rectangle->addUniform("Color2");

	glUseProgram(m_shader_rectangle->getProgram()); /// УБРАТЬ?
	glGenVertexArrays(1, &m_rectangle_VAO);

	return true;
}

bool kkOpenGL::_createShaderFor2DLine()
{
	 const char * text_v = 
		"#version 130\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 Positions;\n"
		"uniform vec4 Color;\n"
		"out vec4 out_color;\n"
		"void main(){\n"
		"vec4 vertices[2] = vec4[2](\n"
									"vec4( Positions.x,  Positions.y,  0, 1.0),\n"
                                    "vec4( Positions.z,  Positions.w,  0, 1.0)\n"
										");\n"
		"gl_Position = ProjMtx * vertices[gl_VertexID];\n"
		 "out_color = Color;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec4 out_color;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(out_color.xyz, 1.0);\n"
		"}\n";
	m_shader_2dline = kkCreate<kkOpenGLShader>();

	if( !m_shader_2dline->create(text_v, text_f) )
	{
		kkDestroy(m_shader_2dline); m_shader_2dline = 0;
		return false;
	}

	m_shader_2dline->addUniform("ProjMtx");
	m_shader_2dline->addUniform("Positions");
	m_shader_2dline->addUniform("Color");
	glGenVertexArrays(1, &m_2dline_VAO);
	return true;
}

bool kkOpenGL::_createShaderFor3DLine()
{
	 const char * text_v = 
		"#version 130\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 P1;\n"
		"uniform vec4 P2;\n"
		"uniform vec4 Color;\n"
		"out vec4 out_color;\n"
		"void main(){\n"
		"vec4 vertices[2] = vec4[2](\n"
									"vec4( P1.xyz, 1.0),\n"
									"vec4( P2.xyz, 1.0)\n"
										");\n"
		"gl_Position = ProjMtx * vertices[gl_VertexID];\n"
		"out_color = Color;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec4 out_color;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(out_color.xyz, 1.0);\n"
		"}\n";
	m_shader_3dline = kkCreate<kkOpenGLShader>();

	if( !m_shader_3dline->create(text_v, text_f) )
	{
		kkDestroy(m_shader_3dline); m_shader_3dline = 0;
		return false;
	}

	m_shader_3dline->addUniform("ProjMtx");
	m_shader_3dline->addUniform("P1");
	m_shader_3dline->addUniform("P2");
	m_shader_3dline->addUniform("Color");

	glGenVertexArrays(1, &m_3dline_VAO);
	return true;
}

bool kkOpenGL::_createShaderForPolyMesh()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"out vec2 texCoord;\n"
		"out vec3 normal;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 V;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord = inputTexCoord;\n"
		"normal = mat3(V) * inputNormal;\n"
		"normal = normalize(normal);\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 texCoord;\n"
		"in vec3 normal;\n"
		"out vec4 color;\n"
		"uniform vec3 lightDir1;\n"
		"uniform vec3 lightDir2;\n"
		"void main(){\n"
		  "vec4 diffuseColor = vec4(0.8f,0.8f,0.8f,1.f);\n"
		  "vec4 ambientColor = vec4(0.f,0.,0.f,1.f);\n"
		  "float lightIntensity1 = clamp(dot(normal, lightDir1), 0.0f, 1.0f);\n"
		  "float lightIntensity2 = clamp(dot(normal, lightDir2), 0.0f, 1.0f);\n"
		  "color = clamp((diffuseColor*lightIntensity1), 0.0f, 1.0f);\n"
		  "color += clamp((diffuseColor*lightIntensity2), 0.0f, 1.0f);\n"
		  "color = clamp((color+ambientColor), 0.0f, 1.0f);\n"
		"}\n";
	m_shader_polyMesh = kkCreate<kkOpenGLShader>();

	if( !m_shader_polyMesh->create(text_v, text_f) )
	{
		kkDestroy(m_shader_polyMesh); m_shader_polyMesh = 0;
		return false;
	}

	m_shader_polyMesh->addUniform("WVP");
	m_shader_polyMesh->addUniform("V");
	m_shader_polyMesh->addUniform("lightDir1");
	m_shader_polyMesh->addUniform("lightDir2");

	return true;
}

bool kkOpenGL::_createShaderForPolyMesh_lines()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(1.f,1.f,1.f,1.f);\n"
		"}\n";
	m_shader_polyMesh_lines = kkCreate<kkOpenGLShader>();

	if( !m_shader_polyMesh_lines->create(text_v, text_f) )
	{
		kkDestroy(m_shader_polyMesh_lines); m_shader_polyMesh_lines = 0;
		return false;
	}

	m_shader_polyMesh_lines->addUniform("WVP");
	return true;
}


void kkOpenGL::update_onSize()
{
	gl3wInit();

	if( m_window )
	{
		auto window_client_rect = m_window->getClientRect();
		auto window_client_size = window_client_rect.getWidthAndHeight();


		setViewport( 0, 0, window_client_size.x, window_client_size.y );

		float L = 0;
		float R = (float)window_client_size.x;
		float T = 0;
		float B = (float)window_client_size.y;
	
		m_ortho_projection[0] = kkVector4(2.0f/(R-L),   0.0f,         0.0f,   0.0f);
		m_ortho_projection[1] = kkVector4(0.0f,         2.0f/(T-B),   0.0f,   0.0f);
		m_ortho_projection[2] = kkVector4(0.0f,         0.0f,        -1.0f,   0.0f);
		m_ortho_projection[3] = kkVector4((R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f);

	}
}



void kkOpenGL::drawLine3D( const kkVector4& p1, const kkVector4& p2, const kkColor& color, kkShader * shader )
{
	if( m_active_camera )
	{
		if( shader )
		{
			shader->setActive();
			shader->onShader( &m_shaderOpenGLData, m_defaultMatrix );
		}
		else
		{
			m_shader_3dline->setActive();

			auto P = m_active_camera->getProjectionMatrix();
			auto V = m_active_camera->getViewMatrix();

			auto WVP = P * V * kkMatrix4();

			glUniformMatrix4fv(m_shader_3dline->getUniform(0), 1, GL_FALSE, WVP.getPtr() );
			glUniform4fv(m_shader_3dline->getUniform(1), 1, p1.data() );
			glUniform4fv(m_shader_3dline->getUniform(2), 1, p2.data() );
			glUniform4fv(m_shader_3dline->getUniform(3), 1, color.data() );
		}

		glBindVertexArray(m_3dline_VAO);
		glDrawArrays(GL_LINES, 0, 2);
	}
}

void kkOpenGL::setViewport( s32 x, s32 y, s32 w, s32 h )
{
	glViewport(x, y, w, h);
	m_current_view_port.set(x,y,w,h);
}

void kkOpenGL::useBackFaceCulling( bool v )
{
	if( v )
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void kkOpenGL::useDepth( bool v )
{
	if( v )
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void kkOpenGL::setCompFunc(kkGraphicsSystemCompFunc f)
{
	switch (f)
	{
	case kkGraphicsSystemCompFunc::_NEVER:
		glDepthFunc(GL_NEVER);
		break;
	case kkGraphicsSystemCompFunc::_LESS:
		glDepthFunc(GL_LESS);
		break;
	case kkGraphicsSystemCompFunc::_EQUAL:
		glDepthFunc(GL_EQUAL);
		break;
	case kkGraphicsSystemCompFunc::_LEQUAL:
		glDepthFunc(GL_LEQUAL);
		break;
	case kkGraphicsSystemCompFunc::_GREATER:
		glDepthFunc(GL_GREATER);
		break;
	case kkGraphicsSystemCompFunc::_NOTEQUAL:
		glDepthFunc(GL_NOTEQUAL);
		break;
	case kkGraphicsSystemCompFunc::_GEQUAL:
		glDepthFunc(GL_GEQUAL);
		break;
	case kkGraphicsSystemCompFunc::_ALWAYS:
		glDepthFunc(GL_ALWAYS);
		break;
	default:
		break;
	}
}

void kkOpenGL::useScissor( bool v )
{
	if(v)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

kkTexture* kkOpenGL::createTexture(const v2i& rttSize, bool linear)
{
	kkOpenGLTexture * t = kkCreate<kkOpenGLTexture>();
	if( t->init(rttSize,linear) )
	{
		return t;
	}
	return nullptr;
}

kkTexture* kkOpenGL::createTexture(kkImage* i, bool linear)
{
	kkOpenGLTexture * t = kkCreate<kkOpenGLTexture>();
	if( t->init(i,linear) )
	{
		return t;
	}
	return nullptr;
}

void kkOpenGL::setScissor( int x1, int y1, int x2, int y2 )
{
	glScissor( x1, y1, x2, y2 );
}

bool kkOpenGL::createShader( kkShader* out_shader, const char * v, const char * v_main, const char * f, const char * f_main, const char * g, const char * g_main )
{
	assert(out_shader!=nullptr);

	auto shader_vert = kkOpenGLShader::_createShader(GL_VERTEX_SHADER,v);
	if(!kkOpenGLShader::_checkShader(shader_vert))
	{
		return false;
	}
	auto shader_frag = kkOpenGLShader::_createShader(GL_FRAGMENT_SHADER,f);
	if(!kkOpenGLShader::_checkShader(shader_frag))
	{
		return false;
	}

	GLuint shader_geom = 0;
	if( g )
	{
		shader_geom = kkOpenGLShader::_createShader(GL_GEOMETRY_SHADER,g);
		if(!kkOpenGLShader::_checkShader(shader_geom))
		{
			return false;
		}
	}

	kkShaderOpenGLData sd;

	sd.m_program = kkOpenGLShader::_createShaderProgram(shader_vert,shader_frag, shader_geom);

	if(!kkOpenGLShader::_checkProgram(sd.m_program))
	{
		return false;
	}

	out_shader->onCreate( &sd );
	
	glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);
    if(shader_geom != 0)
		glDeleteShader(shader_geom);

	return true;
}

void kkOpenGL::drawPoint3D( const kkVector4& p, kkShader * shader )
{
	assert(shader!=nullptr);

	glDisable(GL_CULL_FACE);
	
	shader->setActive();
	shader->onShader( &m_shaderOpenGLData, m_defaultMatrix );
	
	kkOpenGLShader * oglshader = (kkOpenGLShader *)shader;
	if( oglshader->m_VAO == 0 )
	{
		glGenVertexArrays(1, &oglshader->m_VAO);
	}

	glBindVertexArray(oglshader->m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
}