// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_MEMSYS_IMPL_H__
#define __KK_MEMSYS_IMPL_H__

class kkMemorySystemImpl
{
public:

	kkMemorySystemImpl();
	virtual ~kkMemorySystemImpl();

	void* allocate( u64 size );
	void* reallocate( void * p, u64 size );
	void  free( void * ptr );

	void* allocateAligned( u64 size, u64 align );
	void  freeAligned( void * ptr );
};

#endif