//#ifndef __KKLIST_H__
//#define __KKLIST_H__
//
//template<typename T>
//struct kkListNode
//{
//	T* m_data = nullptr;
//
//	kkListNode* m_next = nullptr;
//	kkListNode* m_prev = nullptr;
//};
//
//template<typename T>
//class kkList
//{
//	std::size_t m_size = 0;
//	kkListNode<T>* m_first = nullptr;
//	kkListNode<T>* m_last = nullptr;
//
//	kkAllocator<T> m_allocator;
//public:
//
//	kkList()
//	{
//	}
//
//	kkList(const kkList& other) = delete;
//	kkList(kkList&& other) = delete;
//
//	~kkList()
//	{
//		clear();
//	}
//
//	void clear() noexcept
//	{
//		while(m_first)
//		{
//			auto next = m_first->m_next;
//			m_allocator.destruct(m_first);
//			m_allocator.deallocate(m_first);
//			m_first = next;
//		}
//		m_last = nullptr;
//		m_size = 0;
//	}
//
//	kkList& operator=(const kkList& other)
//	{
//		if(!other->m_size)
//			return;
//
//		clear();
//
//		kkListNode<T>* node = other.m_first;
//		while(node)
//		{
//			push_back(node->m_data);
//			node = node->m_next;
//		}
//	}
//
//	void push_back( const T& data )
//	{
//		kkListNode<T>* node = m_allocator.allocate(1);
//		m_allocator.construct(node, data);
//		++m_size;
//		if(!m_first)
//		{
//			m_first = node;
//		}
//		node->m_prev = m_last;
//		if(m_last)
//		{
//			m_last->m_next = node;
//		}
//		m_last = node;
//	}
//
//	void push_front( const T& data )
//	{
//		kkListNode<T>* node = m_allocator.allocate(1);
//		m_allocator.construct(node, data);
//		++m_size;
//		if(!m_first)
//		{
//			m_first = node;
//			m_last  = node;
//		}
//		else
//		{
//			node->m_next = m_first;
//			m_first->m_prev = node;
//			m_first = node;
//		}
//	}
//
//	T* begin()
//	{
//		return m_data;
//	}
//
//	T* end()
//	{
//		return (m_data+(m_size));
//	}
//
//	T* data() const 
//	{
//		return m_data; 
//	}
//
//	size_t size() const 
//	{
//		return m_size; 
//	}
//
//	bool   empty() const 
//	{
//		return m_size == 0u; 
//	}
//};
//
//#endif