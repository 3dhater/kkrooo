// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_MEMORY_H__
#define __KKROO_MEMORY_H__

#include <new>

class kkMemory
{
public:

	KK_API static void* allocate( u64 size );
	KK_API static void* reallocate( void *, u64 size );
	KK_API static void  free( void * ptr );

	KK_API static void* allocateAligned( u64 size, u64 align );
	KK_API static void  freeAligned( void * ptr );
};

template<typename _type>
class kkObjectCreator
{
public:

	template<typename... Args>
	static _type * create(Args&&... args)
	{
		_type * ptr = (_type*)kkMemory::allocate(sizeof(_type));
		if( ptr )
		{
			new(ptr) _type(std::forward<Args>(args)...);
		}
		return ptr;
	}

	static void destroy( _type * ptr )
	{
		assert(ptr);

		ptr->~_type();
		kkMemory::free( ptr );
	}

};


/// Create object
template<typename _type, typename... Args>
_type * kkCreate(Args&&... args)
{
	return kkObjectCreator<_type>::create(std::forward<Args>(args)...);
}

/// Destroy object
template<typename _type>
void kkDestroy( _type * ptr )
{
	assert(ptr);
	kkObjectCreator<_type>::destroy(ptr);
}

#endif