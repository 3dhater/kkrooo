/*
 Some code is taken from https://github.com/ocornut/imgui
 Dear ImGui is licensed under the MIT License
*/

#include "KrGui.h"

using namespace Kr;

#ifdef KRGUI_COMPILE_WITH_OPENGL3

#include <cstdio>
#include <GL/gl3w.h>

//static GLuint g_defaultFontTexture = 0;
static GLuint g_shaderProgram = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;                                // Uniforms location
static int          g_AttribLocationVtxPos = 0, g_AttribLocationVtxUV = 0, g_AttribLocationVtxColor = 0; // Vertex attributes location
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

GLuint _createShader(GLenum type, const char * text)
{
	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &text, nullptr );
	glCompileShader(shader);
	return shader;
}

GLuint _createShaderProgram(GLuint v, GLuint f, GLuint g)
{
	auto program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);
	if( g )
		glAttachShader(program, g);
	glLinkProgram(program);
	return program;
}

bool _checkShader(GLuint shader)
{
	GLint compile_result = 0;
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

bool _checkProgram(GLuint program)
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

bool createShader( const char * v, const char * f, const char * g, GLuint& out_program )
{
	auto shader_vert = _createShader(GL_VERTEX_SHADER,v);
	if(!_checkShader(shader_vert))
	{
		return false;
	}
	auto shader_frag = _createShader(GL_FRAGMENT_SHADER,f);
	if(!_checkShader(shader_frag))
	{
		return false;
	}

	GLuint shader_geom = 0;
	if( g )
	{
		shader_geom = _createShader(GL_GEOMETRY_SHADER,g);
		if(!_checkShader(shader_geom))
		{
			return false;
		}
	}

	out_program = _createShaderProgram(shader_vert,shader_frag, shader_geom);
	if(!_checkProgram(out_program))
	{
		return false;
	}

	glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);
    if(shader_geom != 0)
		glDeleteShader(shader_geom);

	return true;
}

bool Gui::GraphicsSystem_OpenGL3Init()
{
	gl3wInit();
	const char * text_v = 
		"#version 130\n"
		"in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
		"uniform mat4 ProjMtx;\n"
		"out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
		"void main(){\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";
	const char * text_f = 
		"#version 130\n" 
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
		"void main(){\n"
        "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
		"}\n";

	if( !createShader( text_v, text_f, nullptr, g_shaderProgram ) )
		return false;

	g_AttribLocationTex      = glGetUniformLocation(g_shaderProgram, "Texture");
    g_AttribLocationProjMtx  = glGetUniformLocation(g_shaderProgram, "ProjMtx");
    g_AttribLocationVtxPos   = glGetAttribLocation(g_shaderProgram, "Position");
    g_AttribLocationVtxUV    = glGetAttribLocation(g_shaderProgram, "UV");
    g_AttribLocationVtxColor = glGetAttribLocation(g_shaderProgram, "Color");

	glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

	return true;
}

void Gui::GraphicsSystem_OpenGL3Draw(Gui::GuiSystem* gui)
{
	// Backup GL state
    GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
    GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
#ifdef GL_POLYGON_MODE
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	glViewport(0, 0, (GLsizei)gui->m_OSWindowClientSize.x, (GLsizei)gui->m_OSWindowClientSize.y);
    float L = 0;
    float R = (float)gui->m_OSWindowClientSize.x;
    float T = 0;
    float B = (float)gui->m_OSWindowClientSize.y;

    float ortho_projection[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };

	glUseProgram(g_shaderProgram);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, 0);
#endif
	GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    glEnableVertexAttribArray(g_AttribLocationVtxPos);
    glEnableVertexAttribArray(g_AttribLocationVtxUV);
    glEnableVertexAttribArray(g_AttribLocationVtxColor);
    glVertexAttribPointer(g_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, sizeof(Gui::Vertex), 0);
    glVertexAttribPointer(g_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, sizeof(Gui::Vertex), (unsigned char*)NULL + (2 * sizeof(float)));
    glVertexAttribPointer(g_AttribLocationVtxColor, 4, GL_FLOAT, GL_FALSE, sizeof(Gui::Vertex), (unsigned char*)NULL + (4 * sizeof(float)));



	auto draw = [&](Gui::DrawCommands* cmd)
	{
		glScissor((GLint)cmd->clipRect.x, (GLint)(gui->m_OSWindowClientSize.y - cmd->clipRect.w), (GLsizei)(cmd->clipRect.z - cmd->clipRect.x), (GLsizei)(cmd->clipRect.w - cmd->clipRect.y));

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd->verts.size() * sizeof(Gui::Vertex), (const GLvoid*)&cmd->verts.data()[0], GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd->inds.size() * sizeof(unsigned short), (const GLvoid*)&cmd->inds.data()[0], GL_STREAM_DRAW);
        
		unsigned long long * t_ptr = (unsigned long long *)cmd->texture.texture_address;

		if( cmd->texture.texture_address == 0 )
			t_ptr = (unsigned long long *)gui->getWhiteTexture()->texture_address;
		
		if( t_ptr )
		{
			auto tid = (GLuint)*t_ptr;
			//printf("tid [%u]\n", tid);
			glBindTexture(GL_TEXTURE_2D, tid);
		}

		glDrawElements(GL_TRIANGLES, (GLsizei)cmd->inds.size(), GL_UNSIGNED_SHORT, 0 );
	};

	for( size_t i = 0, sz = gui->m_drawCommandGroups.size(); i < sz; ++i )
	{
		auto draw_command_group = gui->m_drawCommandGroups[ i ];
		for( size_t o = 0, osz = draw_command_group->m_size; o < osz; ++o )
		{
			draw(draw_command_group->m_commands.data()[ o ]);
		}
		//draw(gui->m_drawCommands.data()[ i ]);
	//		printf( "%i %i %i %i\n", cmd.clipRect.x,cmd.clipRect.y,cmd.clipRect.z,cmd.clipRect.w);
	}
	

    glDeleteVertexArrays(1, &vertex_array_object);

	// ==============
	// Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, last_sampler);
#endif
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void Gui::GraphicsSystem_OpenGL3Release()
{
	//if( g_defaultFontTexture ) glDeleteTextures(1, &g_defaultFontTexture);
	if (g_VboHandle)        { glDeleteBuffers(1, &g_VboHandle); g_VboHandle = 0; }
	if( g_shaderProgram )   { glDeleteProgram(g_shaderProgram); g_shaderProgram = 0; }
}

//Gui::Texture Gui::GraphicsSystem_OpenGL3CreateDefaultFontTexture(unsigned int width, unsigned int height, const unsigned char * data)
//{
//
//	glGenTextures(1, &g_defaultFontTexture);
//    glBindTexture(GL_TEXTURE_2D, g_defaultFontTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	Gui::Texture texture;
//	texture.height  = height;
//	texture.width   = width;
//	texture.texture = (unsigned long long)&g_defaultFontTexture;
//
//	return texture;
//}

//Gui::Texture Gui::GraphicsSystem_OpenGL3CreateTexture(unsigned int width, unsigned int height, const unsigned char * data)
Gui::Texture* Gui::GraphicsSystem_OpenGL3CreateTexture( Gui::Image* image )
{
	assert(image);

	GLuint gltexture;
	glGenTextures(1, &gltexture);
    glBindTexture(GL_TEXTURE_2D, gltexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->m_width, image->m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->m_data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Gui::Texture* texture = new Gui::Texture;
	texture->height  = image->m_height;
	texture->width   = image->m_width;
	//texture.texture = (unsigned long long)&gltexture;
	texture->texture_value = gltexture;
	texture->texture_address = (unsigned long long)&texture->texture_value;

	return texture;
}

void Gui::GraphicsSystem_OpenGL3DestroyTexture( Gui::Texture* t )
{
	assert(t);
	if( t )
	{
		if( t->texture_value )
		{
			GLuint ts[1] = 
			{
				(GLuint)t->texture_value
			};
			glDeleteTextures(1, ts);
		}
		delete t;
	}
}

#endif