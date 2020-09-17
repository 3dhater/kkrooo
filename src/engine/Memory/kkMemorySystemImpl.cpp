#include "kkrooo.engine.h"
#include "kkMemorySystemImpl.h"
#include <cstdlib>

kkMemorySystemImpl::kkMemorySystemImpl()
{
}

kkMemorySystemImpl::~kkMemorySystemImpl()
{
}

void* kkMemorySystemImpl::allocate( u64 size )
{
	auto ptr = std::malloc(size);
	return ptr;
}

void* kkMemorySystemImpl::reallocate( void * p, u64 size )
{
	auto ptr = std::realloc(p,size);

	return ptr;
}

void  kkMemorySystemImpl::free( void * ptr )
{
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
	return ptr;
}

void  kkMemorySystemImpl::freeAligned( void * ptr )
{
#ifdef _MSC_VER
	return _aligned_free( ptr );
#else
	return _aligned_free( ptr );
#endif
}

