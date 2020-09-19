#ifndef __KKLIST_H__
#define __KKLIST_H__

template<typename T>
struct kkListNode
{
	T m_data;

	kkListNode* m_left  = nullptr;
	kkListNode* m_right = nullptr;
};

template<typename T>
class kkList
{
	std::size_t m_size = 0;
	kkListNode<T>* m_head = nullptr;
	kkAllocator<kkListNode<T>> m_allocator;
	
public:

	kkList()
	{
	}

	kkList(const kkList& other) = delete;
	kkList(kkList&& other) = delete;

	~kkList()
	{
		clear();
	}

	void clear() noexcept
	{
		if(!m_head)
			return;
		auto last = m_head->m_left;
		while(true)
		{
			auto next = m_head->m_right;
			//m_allocator.destruct(m_head->m_data);
			//m_allocator.deallocate(m_head->m_data);
			m_allocator.destruct(m_head);
			m_allocator.deallocate(m_head);
			if(m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
		m_size = 0;
	}

	void push_back( const T& data )
	{
		kkListNode<T>* node = m_allocator.allocate(1);
		m_allocator.construct(node, kkListNode<T>());
		node->m_data = data;

		++m_size;
		if(!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left  = m_head;
		}
		else
		{
			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
	}

	void push_front( const T& data )
	{
		/*kkListNode<T>* node = m_allocator.allocate(1);
		m_allocator.construct(node, data);
		++m_size;
		if(!m_first)
		{
			m_first = node;
			m_last  = node;
		}
		else
		{
			node->m_next = m_first;
			m_first->m_prev = node;
			m_first = node;
		}*/
	}

	size_t size() const 
	{
		return m_size; 
	}

	T& front()
	{
		return m_head->m_data;
	}
	void pop_front()
	{
		if(!m_size)
			return;
		--m_size;
		auto next = m_head->m_right;
		auto last = m_head->m_left;
		m_allocator.destruct(m_head);
		m_allocator.deallocate(m_head);
		if(next == m_head)
		{
			m_head = nullptr;
			return;
		}
		m_head = next;
		next->m_left = last;
		last->m_right = next;
	}

	bool   empty() const 
	{
		return m_size == 0u; 
	}
};

#endif