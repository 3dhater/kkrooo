// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKARRAY_H__
#define __KKARRAY_H__

#include "Classes/Common/kkAllocator.h"

template<typename type, typename allocator = kkDefault_allocator>
class kkArray
{

	typedef type* pointer;
	typedef type& reference;
	typedef const type& const_reference;

	pointer m_data;
	u64     m_size;
	u64     m_allocated;
	u64     m_addMemory;
	kkAllocator<type,allocator> m_allocator;

	void reallocate( u64 new_capacity )
	{
		new_capacity += m_addMemory;
		pointer new_data = m_allocator.allocate( new_capacity /** sizeof( type )*/ );

		if( m_data )
		{
			for( u64 i = 0u; i < m_size; ++i )
			{
				m_allocator.construct( &new_data[i], m_data[i] );
				m_allocator.destruct( &m_data[i] );
			}

			m_allocator.deallocate( m_data );
		}

		m_data = new_data;
		m_allocated = new_capacity;
	}

public:

	pointer begin()
	{
		return m_data;
	}

	pointer end()
	{
		return (m_data+(m_size));
	}

	kkArray( u64 addSize = 8u )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
	}

	kkArray( kkAllocator<type,allocator> _allocator, u64 addSize = 8u )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
		m_allocator = _allocator;
	}

	kkArray( u64 addSize, u64 reserve )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
		reallocate( reserve );
	}

	kkArray( const kkArray& other )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( 8u )
	{
		*this = other;
	}

	~kkArray(){ clear(); }
		
	void setData(pointer ptr) { m_data = ptr; }
	pointer data() const     { return m_data; }
	void    setSize( u64 s ) { m_size = s; }
	u64     size() const     { return m_size; }
	u64     capacity() const { return m_allocated; }
		
		
	void    setAddMemoryValue( u64 v ){ m_addMemory = v; }
		
		
	bool    empty() const    { return m_size == 0u; }

	const_reference at( u64 id ) const { return m_data[id]; }
	reference       at( u64 id ){ return m_data[id]; }
	const_reference operator[]( u64 id ) const { return m_data[id]; }
	reference       operator[]( u64 id ){ return m_data[id]; }
	reference       back(){ return m_data[ m_size - 1u ]; }
	const_reference back() const { return m_data[ m_size - 1u ]; }
	const_reference front() const { return m_data[ 0u ]; }

	void reserve( u64 new_capacity )
	{
		if( new_capacity > m_allocated )
			reallocate( new_capacity );
	}

	void push_back( const_reference object )
	{
		u64 new_size = m_size + 1u;
		if( new_size > m_allocated )
			reallocate( new_size );
		m_allocator.construct( &m_data[m_size], object );
		m_size = new_size;
	}

	
	void clear()
	{
		if( m_data )
		{
			for( u64 i = 0u; i < m_size; ++i )
			{
				m_allocator.destruct( &m_data[i] );
			}
			m_allocator.deallocate( m_data );

			m_allocated = m_size = 0u;
			m_data = nullptr;
		}
	}

	void shrink_to_fit()
	{
		reallocate( m_size - m_addMemory );
	}

	void erase( u64 index )
	{
		erase( index, index );
	}

	void erase_first( const_reference ref )
	{
		for( u64 i = 0u; i < m_size; ++i )
		{
			if( m_data[ i ] == ref )
			{
				erase( i );
				return;
			}
		}
	}

	void erase( u64 begin, u64 end )
	{
		if( m_size )
		{
			u64 last = m_size - 1u;
			u64 len = end - begin;
			for( u64 i = begin; i < m_size; ++i )
			{
				m_allocator.destruct( &m_data[ i ] );
				if( i < last )
				{
					u64 next = i + 1u + len;
					if( next < m_size )
					{
						m_allocator.construct( &m_data[ i ], m_data[ next ] );
					}
				}
			}
			m_size = m_size - 1u - len;
		}
	}

	void assign( const kkArray<type, allocator>& other )
	{
		clear();
		m_size = other.size();
		m_allocated = other.capacity();
		reallocate( m_allocated );
		for( u64 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[ i ], other.m_data[ i ] );
		}
	}

	kkArray<type, allocator>& operator=( const kkArray<type, allocator>& other )
	{
		clear();
		m_size = other.size();
		m_allocated = other.capacity();
		reallocate( m_allocated );
		for( u64 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[ i ], other.m_data[ i ] );
		}
		return *this;
	}

};

#endif