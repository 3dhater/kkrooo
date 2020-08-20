// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_OPENGL_MODEL_H__
#define __KK_OPENGL_MODEL_H__

#include "GraphicsSystem/kkMesh.h"
#include "GraphicsSystem/kkGraphicsSystem.h"

class kkOpenGLModel : public kkMesh
{
	GLuint m_VAO     = 0;
	GLuint m_vBuffer = 0;
	GLuint m_iBuffer = 0;

	GLsizei m_iCount = 0;

	kkMeshType m_type = kkMeshType::Triangles;

	friend class kkOpenGL;

public:
	kkOpenGLModel();
	virtual ~kkOpenGLModel();

	bool init(kkSMesh*, kkMeshType);

	void mapVerts( u8** v_ptr );
	void mapInds( u8** i_ptr );
	void unmapVerts();
	void unmapInds();
};


#endif