// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_MESH_H__
#define __KKROO_MESH_H__

// из этой хрени создаются OpenGL буферы
struct kkSMesh
{
	kkSMesh()
	{
	}

	~kkSMesh()
	{
		if( m_vertices ) kkMemory::free(m_vertices);
		if( m_indices )  kkMemory::free(m_indices);
	}

	u8  * m_vertices = nullptr;
	u16 *	m_indices  = nullptr;
	u32   m_vCount   = 0;
	u32   m_iCount   = 0;
	u32   m_stride   = 0;

	kkAabb  m_aabb;
};

class kkMesh
{
public:
	kkMesh(){}
	virtual ~kkMesh(){}

	virtual void mapVerts( u8** v_ptr ) = 0;
	virtual void mapInds( u8** i_ptr ) = 0;
	virtual void unmapVerts() = 0;
	virtual void unmapInds() = 0;
};

#endif