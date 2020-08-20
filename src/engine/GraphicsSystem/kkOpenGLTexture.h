// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_OPENGL_TEXTURE_H__
#define __KK_OPENGL_TEXTURE_H__

#include "Classes/Math/kkVector4.h"

#include "GraphicsSystem/kkTexture.h"

class kkOpenGLTexture : public kkTexture
{
	GLuint m_texture   = 0;
	GLuint m_FBO = 0;


	friend class kkOpenGL;
public:
	kkOpenGLTexture();
	virtual ~kkOpenGLTexture();

	bool init(kkImage*,bool linear);
	bool init(const v2i& rttSize, bool linear);

	void* getHandle();
	void fillNewData( u8* ptr );
	//void unmap();
};


#endif