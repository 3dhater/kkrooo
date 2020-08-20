// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS
#include "kkrooo.engine.h"

#include <GL/gl3w.h>

#include "kkOpenGLTexture.h"

kkOpenGLTexture::kkOpenGLTexture()
{
}


kkOpenGLTexture::~kkOpenGLTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if( m_FBO )
		glDeleteFramebuffers(1, &m_FBO);

	if( m_texture )
		glDeleteTextures(1, &m_texture);
}

bool kkOpenGLTexture::init(const v2i& rttSize, bool linear)
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rttSize.x, rttSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST );
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool kkOpenGLTexture::init(kkImage* i, bool linear)
{
	auto & info = this->getInfo();
	info = *i;

	glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.m_width, info.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info.m_data8);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    
	// info содержит только информацию
	info.m_data8  = nullptr;
	info.m_data16 = nullptr;
	info.m_data32 = nullptr;
	info.m_data64 = nullptr;

	return true;
}

void* kkOpenGLTexture::getHandle()
{
	return (void*)&m_texture;
}

void kkOpenGLTexture::fillNewData( u8* ptr )
{
	auto & info = this->getInfo();
    glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, info.m_width, info.m_height, GL_RGBA, GL_UNSIGNED_BYTE, ptr );
	//glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	//*v_ptr = (u8*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_TEXTURE_2D,0);
}

//void kkOpenGLTexture::unmap()
//{
//}