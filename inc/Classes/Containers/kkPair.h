// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PAIR_H__
#define __KK_PAIR_H__

template<typename Type1, typename Type2>
class kkPair
{
public:

	using reference = kkPair<Type1,Type2>&;
	using const_reference = const kkPair<Type1,Type2>&;
	using move = kkPair<Type1,Type2>&&;

	kkPair()
		:
		m_first(),
		m_second(),
		compareFunc(nullptr)
	{
	}
		
	kkPair( const Type1& first, const Type2& second ):
		m_first( first ), 
		m_second( second ),
		compareFunc(nullptr)
	{
	}

	kkPair( Type1&& first, Type2&& second ):
		m_first( std::forward<Type1>(first) ), 
		m_second( std::forward<Type2>(second) ),
		compareFunc(nullptr)
	{
	}
		
	kkPair( const Type1& first, const Type2& second, bool(*pred)(const_reference This, const_reference Other) )
		:
		m_first( first ),
		m_second( second ),
		compareFunc(pred)
	{
	}

	Type1 m_first;
	Type2 m_second;

	kkPair(const_reference) = default;
    kkPair(kkPair&&)      = default;

	reference operator=( const_reference o )
	{
		m_first = o.m_first;
		m_second = o.m_second;
		return *this;
	}

	reference& operator=(move o)
	{
        m_first  = std::forward<Type1>(o.m_first);
        m_second = std::forward<Type2>(o.m_second);
        return *this;
    }

	bool operator<=( const_reference o ) const 
	{
		if( compareFunc ) 
			return compareFunc( *this, o );
		return false;
	}
		
	bool operator>=( const_reference o ) const 
	{
		if( compareFunc ) 
			return compareFunc( *this, o );
		return false;
	}

	bool operator>( const_reference o ) const 
	{
		if( compareFunc ) 
			return compareFunc( *this, o );
		return false;
	}

	bool operator<( const_reference o ) const 
	{
		if( compareFunc ) 
			return compareFunc( *this, o );
		return false;
	}

	bool(*compareFunc)(const_reference This, const_reference Other);
};

#endif