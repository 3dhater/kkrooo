#ifndef __KKROO_PTR_H__
#define __KKROO_PTR_H__

// It is necessary to ensure that objects are destroyed before the call `kkStopEngine`
template<typename _type>
class kkPtr
{
	_type * m_ptr = nullptr;
public:

	kkPtr()
	{
	}

	~kkPtr()
	{
		if( m_ptr )
		{
			kkDestroy( m_ptr );
		}
	}

	kkPtr( _type * p )
	:
		m_ptr( p )
	{
	}

	_type * ptr()
	{
		return m_ptr;
	}

	_type* operator->() const
	{
		return m_ptr;
	}

	void operator=( _type* p )
	{
		m_ptr = p;
	}

};

template<typename _type>
kkPtr<_type> kkCreatePtr( _type * ptr )
{
	return kkPtr<_type>( ptr );
}


#endif