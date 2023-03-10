#ifndef __HASH_MAP_OF_H__
#define __HASH_MAP_OF_H__

#define DECLARE_TEMPLATE template<typename K, typename V, U32 _sizePOW2 = 256>
#define DEFINE_TEMPLATE template<typename K, typename V, U32 _sizePOW2 /*= 256*/>
#define TEMPLATE_ARG	K,V,_sizePOW2

DECLARE_TEMPLATE
class HashOf
{
protected:

	struct Node
	{
		Node() { m_pNext = NULL; }
		~Node()
		{
		}
		K	m_Key;
		V	m_Value;
		Node*	m_pNext;
	};
public:
	HashOf()
	{
		DEBUG_Require(_sizePOW2 > 0 && (_sizePOW2 & (_sizePOW2 - 1)) == 0);
		memset(m_Nodes, 0, _sizePOW2*sizeof(Node*));
		m_iCount = 0;
	}

	~HashOf()
	{
		RemoveAllItems();
	}

	HashOf& operator=(const HashOf& _other);

	V* AddItem(const K& _key, const V& _value);
	const V* GetItem(const K& _key) const;
	V* GetItem(const K& _key);
	V* GetOrAddItem(const K& _key);
	Bool RemoveItem(const K& _key, V* _outCopy = NULL);
	void RemoveAllItems();

	const V&	operator[](const K& _key) const { return GetItem(_key); }
	V&	operator[](const K& _key) { return *(GetItem(_key)); }

	// ITERATOR
	class Iterator
	{
	public:
		Iterator(const HashOf<TEMPLATE_ARG>* _hashMap, U32 _index, Node* _current)
		{
			m_HashMap = _hashMap;
			m_Index = _index;
			m_pNode = _current;
			AdvanceToNextValid();
		}

		Bool operator==(const Iterator& _other) { return m_Index == _other.m_Index && m_pNode == _other.m_pNode; }
		Bool operator!=(const Iterator& _other) { return !((*this) == _other); }
		Bool IsValid() const { return m_pNode; }
		K&	GetKey() { DEBUG_Require(m_pNode);  return m_pNode->m_Key; }
		V&	GetValue() { DEBUG_Require(m_pNode); return m_pNode->m_Value; }
		Iterator& operator++()
		{
			AdvanceToNextValid();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

	protected:
		Iterator();
		void AdvanceToNextValid()
		{
			if (m_Index < _sizePOW2)
			{
				// just advance in list
				if (m_pNode)
				{
					m_pNode = m_pNode->m_pNext;
				}

				// 
				if (!m_pNode)
				{
					++m_Index;
					while (m_Index < _sizePOW2)
					{
						m_pNode = m_HashMap->m_Nodes[m_Index];
						if (m_pNode)
							break;
						++m_Index;
					}
				}
			}
		}

		const HashOf<TEMPLATE_ARG>* m_HashMap;
		Node*	m_pNode;
		U32 m_Index;
	};

	Iterator GetFirst() const { return Iterator(this, 0, m_Nodes[0]); }
	Iterator GetLast() const { return Iterator(this, _sizePOW2,NULL); }

	void CheckConsistency() const
	{
		for (U32 i = 0; i < _sizePOW2; i++)
		{
			Node* pNode = m_Nodes[i];
			while (pNode)
			{
				pNode = pNode->m_pNext;
			}
		}
	}

protected:
	Node*	m_Nodes[_sizePOW2];
	U32		m_iCount;
};

//------------------------------------------------------

DEFINE_TEMPLATE
HashOf<TEMPLATE_ARG>& HashOf<TEMPLATE_ARG>::operator=(const HashOf<TEMPLATE_ARG>& _other)
{
	for(HashOf<TEMPLATE_ARG>::Iterator it = _other.GetFirst(); it != _other.GetLast(); it++)
	{
		AddItem(it.GetKey(), it.GetValue());
	}

	return *this;
}

//------------------------------------------------------

DEFINE_TEMPLATE
V* HashOf<TEMPLATE_ARG>::AddItem(const K& _key, const V& _value)
{
	U32 hash = ComputeHash(_key,_sizePOW2);
	DEBUG_Require(hash < _sizePOW2);

	// already exists, replace
	HashOf::Node* pCurrent = m_Nodes[hash];
	while (pCurrent)
	{
		if (pCurrent->m_Key == _key)
		{
			pCurrent->m_Value = _value;
			return &(pCurrent->m_Value);
		}

		pCurrent = pCurrent->m_pNext;
	}

	// add new one
	HashOf::Node* pNew = NEW HashOf::Node;
	pNew->m_Key = _key;
	pNew->m_Value = _value;
	pNew->m_pNext = m_Nodes[hash];
	m_Nodes[hash] = pNew;
	m_iCount++;
	return &(pNew->m_Value);
}

//------------------------------------------------------

DEFINE_TEMPLATE
const V* HashOf<TEMPLATE_ARG>::GetItem(const K& _key) const
{
	U32 hash = ComputeHash(_key, _sizePOW2);
	DEBUG_Require(hash < _sizePOW2);

	HashOf::Node* pCurrent = m_Nodes[hash];
	while (pCurrent)
	{
		if (pCurrent->m_Key == _key)
		{
			return &(pCurrent->m_Value);
		}

		pCurrent = pCurrent->m_pNext;
	}
	return NULL;
}

//------------------------------------------------------

DEFINE_TEMPLATE
V* HashOf<TEMPLATE_ARG>::GetItem(const K& _key)
{
	U32 hash = ComputeHash(_key, _sizePOW2);
	DEBUG_Require(hash < _sizePOW2);

	HashOf::Node* pCurrent = m_Nodes[hash];
	while (pCurrent)
	{
		if (pCurrent->m_Key == _key)
		{
			return &(pCurrent->m_Value);
		}

		pCurrent = pCurrent->m_pNext;
	}
	return NULL;
}

//------------------------------------------------------

DEFINE_TEMPLATE
V* HashOf<TEMPLATE_ARG>::GetOrAddItem(const K& _key)
{
	U32 hash = ComputeHash(_key, _sizePOW2);
	DEBUG_Require(hash < _sizePOW2);

	// find it
	HashOf::Node* pCurrent = m_Nodes[hash];
	while (pCurrent)
	{
		if (pCurrent->m_Key == _key)
		{
			return &(pCurrent->m_Value);
		}

		pCurrent = pCurrent->m_pNext;
	}

	// add it
	HashOf::Node* pNew = NEW HashOf::Node;
	pNew->m_Key = _key;
	pNew->m_pNext = m_Nodes[hash];
	m_Nodes[hash] = pNew;
	m_iCount++;
	return &(pNew->m_Value);
}

//------------------------------------------------------

DEFINE_TEMPLATE
Bool HashOf<TEMPLATE_ARG>::RemoveItem(const K& _key, V*	_outCopy /*= NULL*/)
{
	U32 hash = ComputeHash(_key, _sizePOW2);
	DEBUG_Require(hash < _sizePOW2);

	// first of the list
	HashOf::Node* pCurrent = m_Nodes[hash];
	if (pCurrent->m_Key == _key)
	{
		m_Nodes[hash] = pCurrent->m_pNext;
		if (_outCopy)
			*_outCopy = pCurrent->m_Value;
		DELETE pCurrent;
		m_iCount--;
		return TRUE;
	}

	HashOf::Node* pPrevious = pCurrent;
	pCurrent = pCurrent->m_pNext;
	while (pCurrent)
	{
		if (pCurrent->m_Key == _key)
		{
			pPrevious->m_pNext = pCurrent->m_pNext;
			if (_outCopy)
				*_outCopy = pCurrent->m_Value;
			DELETE pCurrent;
			m_iCount--;
			return TRUE;
		}

		pPrevious = pCurrent;
		pCurrent = pCurrent->m_pNext;
	}

	return FALSE;
}

//------------------------------------------------------

DEFINE_TEMPLATE
void HashOf<TEMPLATE_ARG>::RemoveAllItems()
{
	for (U32 i = 0; i < _sizePOW2; i++)
	{
		HashOf::Node* pCurrent = m_Nodes[i];
		while (pCurrent)
		{
			HashOf::Node* pNext = pCurrent->m_pNext;
			DELETE pCurrent;
			m_iCount--;
			pCurrent = pNext;
		}

		m_Nodes[i] = 0;
	}
}

//------------------------------------------------------

template<typename K>
static U32		ComputeHash(const K& _value, U32 _maxSize)
{
	return (GetHash(_value) & (_maxSize - 1));
}

//------------------------------------------------------

template<typename K>
static U32		GetHash(const K& _value)
{
	DEBUG_Forbidden("No GetHash defined for this type");
	return 0;
}

//------------------------------------------------------

static U32 GetHash(void* _ptr)
{
	return U32(U64(_ptr) >> 3);
}

#undef DECLARE_TEMPLATE
#undef DEFINE_TEMPLATE
#undef TEMPLATE_ARG

#endif
