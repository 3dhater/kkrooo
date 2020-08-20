// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_REF_OBJECT_H__
#define __KKROO_REF_OBJECT_H__

class kkRefObject : public kkBaseObject
{
	mutable u32 m_count = 1;
public:
	kkRefObject(){}
	virtual ~kkRefObject(){}

	u32 GetReferenceCount() const 
	{
		return m_count;
	}

	void AddRef() const 
	{
		++m_count;
	}

	void Release() 
	{
		--m_count;
		if( !m_count )
		{
			//delete this;
			this->~kkRefObject();
			kkMemory::free(this);
		}
	}
};

#endif