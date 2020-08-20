// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"


#include "kkMemorySystemImpl.h"

#include <cstdlib>

kkMemorySystemImpl::kkMemorySystemImpl()
{
}

kkMemorySystemImpl::~kkMemorySystemImpl()
{
}

void kkMemorySystemImpl::_addUsedMem( void* ptr, u64 sz )
{
	std::lock_guard<std::mutex> lck( m_mutex );

	m_memory_map[(kkAddressType)ptr] = sz;
	m_used_memory += sz;
}

void kkMemorySystemImpl::_subUsedMem( void* ptr )
{
	std::lock_guard<std::mutex> lck( m_mutex );

	m_used_memory -= m_memory_map[(kkAddressType)ptr];

	m_memory_map.erase((kkAddressType)ptr);
}

void* kkMemorySystemImpl::allocate( u64 size )
{
	auto ptr = std::malloc(size);

	if( m_use_memory_counting )
		_addUsedMem(ptr,size);

	return ptr;
}

void* kkMemorySystemImpl::reallocate( void * p, u64 size )
{
	auto ptr = std::realloc(p,size);

	return ptr;
}

void  kkMemorySystemImpl::free( void * ptr )
{
	if( m_use_memory_counting )
		_subUsedMem(ptr);

	std::free(ptr);
}

void* kkMemorySystemImpl::allocateAligned( u64 size, u64 align )
{
	void * ptr = nullptr;
#ifdef _MSC_VER
	ptr = _aligned_malloc( size, align );
#else
	ptr = _aligned_malloc( size, align );
#endif

	if( m_use_memory_counting )
		_addUsedMem(ptr,size);

	return ptr;
}

void  kkMemorySystemImpl::freeAligned( void * ptr )
{
	if( m_use_memory_counting )
		_subUsedMem(ptr);

#ifdef _MSC_VER
	return _aligned_free( ptr );
#else
	return _aligned_free( ptr );
#endif
}

u64 kkMemorySystemImpl::getUsedMemory()
{
	return m_used_memory;
}

void  kkMemorySystemImpl::addUsedMemory( u64 v )
{
	m_used_memory += v;
}

void  kkMemorySystemImpl::subUsedMemory( u64 v )
{
	m_used_memory -= v;
}

void  kkMemorySystemImpl::stopMemoryCounting()
{
	m_use_memory_counting = false;
}

void  kkMemorySystemImpl::startMemoryCounting()
{
	m_use_memory_counting = true;
}