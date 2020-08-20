// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_ALLOCATOR_H__
#define __KK_ALLOCATOR_H__

class kkDefault_allocator
{
public:
	kkDefault_allocator(){}
	~kkDefault_allocator(){}

	void* allocate( u64 size )
	{
		return kkMemory::allocate(size);
	}
	void free( void * ptr )
	{
		kkMemory::free(ptr);
	}
};

class kkDefault_allocator_stdlib
{
public:
	kkDefault_allocator_stdlib(){}
	~kkDefault_allocator_stdlib(){}

	void* allocate( u64 size )
	{
		return std::malloc(size);
	}
	void free( void * ptr )
	{
		std::free(ptr);
	}
};

template<typename Type, typename allocator = kkDefault_allocator>
class kkAllocator
{
	allocator m_allocator;
public:
	kkAllocator(){}
	kkAllocator(allocator a):m_allocator(a){}

	Type * allocate( u64 size )
	{
		//return static_cast<Type*>( malloc(size * sizeof( Type )) );
		return static_cast<Type*>(m_allocator.allocate(size * sizeof( Type )));
	}

	void construct( Type * new_data, const Type& old_data )
	{
		new(new_data) Type( old_data );
	}

	void deallocate( Type * ptr )
	{
		//free( ptr );
		m_allocator.free(ptr);
	}

	void destruct( Type * ptr )
	{
		ptr->~Type();
	}
};


#endif