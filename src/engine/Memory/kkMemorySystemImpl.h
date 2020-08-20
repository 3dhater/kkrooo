// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_MEMSYS_IMPL_H__
#define __KK_MEMSYS_IMPL_H__

#include <map>
#include <mutex>

class kkMemorySystemImpl
{
	std::mutex m_mutex;

	u64 m_used_memory = 0;
	bool  m_use_memory_counting = false;

	std::map<kkAddressType, u64> m_memory_map;

	void _addUsedMem(void*,u64);
	void _subUsedMem(void*);

public:

	kkMemorySystemImpl();
	virtual ~kkMemorySystemImpl();

	void* allocate( u64 size );
	void* reallocate( void * p, u64 size );
	void  free( void * ptr );

	void* allocateAligned( u64 size, u64 align );
	void  freeAligned( void * ptr );

	u64 getUsedMemory();
	void  addUsedMemory( u64 );
	void  subUsedMemory( u64 );
	void  stopMemoryCounting();
	void  startMemoryCounting();
};

#endif