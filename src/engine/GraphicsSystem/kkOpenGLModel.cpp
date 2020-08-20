// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"

#include <GL/gl3w.h>

#include "Classes/kkAABB.h"

#include "kkOpenGLModel.h"


kkOpenGLModel::kkOpenGLModel()
{
}

kkOpenGLModel::~kkOpenGLModel()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_vBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_iBuffer);

	glBindVertexArray(0);
	glDeleteVertexArrays(1,&m_VAO);
}

bool kkOpenGLModel::init(kkSMesh* smesh, kkMeshType type)
{
	m_type = type;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, smesh->m_vCount * smesh->m_stride, smesh->m_vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, smesh->m_stride, 0); // POSITION

	if( m_type == kkMeshType::Triangles )
	{
	
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
	
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));
	
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));
	}
	else if( m_type == kkMeshType::Points )
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
	}
	else if( m_type == kkMeshType::Lines )
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
	}


	glGenBuffers(1, &m_iBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, smesh->m_iCount * sizeof(u16), smesh->m_indices, GL_DYNAMIC_DRAW);

	m_iCount = smesh->m_iCount;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	return true;
}

void kkOpenGLModel::mapVerts( u8** v_ptr )
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	*v_ptr = (u8*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void kkOpenGLModel::mapInds( u8** i_ptr )
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);
	*i_ptr = (u8*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void kkOpenGLModel::unmapVerts()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void kkOpenGLModel::unmapInds()
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}