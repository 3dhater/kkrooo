// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "Classes/Patterns/kkSingleton.h"
#include "kkMemorySystemImpl.h"

void*  kkMemory::allocate( u64 size )
{
	return kkSingleton<kkMemorySystemImpl>::s_instance->allocate( size );
}

void*  kkMemory::reallocate( void * p, u64 size )
{
	return kkSingleton<kkMemorySystemImpl>::s_instance->reallocate( p, size );
}

void   kkMemory::free( void * ptr )
{
	assert(ptr);
	kkSingleton<kkMemorySystemImpl>::s_instance->free( ptr );
}

void*  kkMemory::allocateAligned( u64 size, u64 align )
{
	return kkSingleton<kkMemorySystemImpl>::s_instance->allocateAligned( size, align );
}

void   kkMemory::freeAligned( void * ptr )
{
	assert(ptr);
	kkSingleton<kkMemorySystemImpl>::s_instance->freeAligned( ptr );
}

