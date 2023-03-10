#ifndef __LIST_OF_H__
#define __LIST_OF_H__

#define DECLARE_USE_IN_LIST(_class)						\
public:													\
	_class*		GetPrev() const	{ return m_pPrev; }		\
	_class*		GetNext() const	{ return m_pNext; }		\
private:												\
	friend class ListOf<_class>;						\
	_class*		m_pPrev;								\
	_class*		m_pNext;

//------------------------------------------

template<typename T>
class ListOf
{
public:
	ListOf(): m_pHead(NULL),m_pTail(NULL){}
	virtual ~ListOf( void ) 
	{ 
		RemoveAll();
	}

	T*		GetHead() const		{ return m_pHead; }
	T*		GetTail() const		{ return m_pTail; }

	void	AddFirst(T* _newElement);
	void	AddLast(T* _newElement);
	void	InsertAfter(T* _newElement,T* _referenceElement);
	void	Remove(T* _element);
	void	RemoveAll();
	void	Switch(T* _a, T* _b);
	S32		GetCount() const;

private:  
	T * m_pHead;
	T * m_pTail;
};

//------------------------------------------

template<typename T>
void	ListOf<T>::AddFirst(T* _newElement)
{
	if( m_pHead )
		m_pHead->m_pPrev = _newElement;

	_newElement->m_pNext = m_pHead;
	_newElement->m_pPrev = NULL;

	m_pHead = _newElement;

	if( !m_pTail )
		m_pTail = m_pHead;
}

//------------------------------------------

template<typename T>
void	ListOf<T>::AddLast(T* _newElement)
{
	if( m_pTail )
		m_pTail->m_pNext = _newElement;

	_newElement->m_pPrev = m_pTail;
	_newElement->m_pNext = NULL;

	m_pTail = _newElement;

	if( !m_pHead )
		m_pHead = m_pTail;
}

//------------------------------------------

template<typename T>
void	ListOf<T>::InsertAfter(T* _newElement,T* _referenceElement)
{
	T* pNextUnit = _referenceElement->m_pNext;

	_referenceElement->m_pNext = _newElement;
	_newElement->m_pNext = pNextUnit;

	_newElement->m_pPrev = _referenceElement;
	if( pNextUnit )
		pNextUnit->m_pPrev = _newElement;

	if( m_pTail == _referenceElement )
		m_pTail = _newElement;
}

//------------------------------------------

template<typename T>
void	ListOf<T>::Remove(T* _element)
{
	if( _element->m_pPrev )
		_element->m_pPrev->m_pNext = _element->m_pNext;

	if( _element->m_pNext )
		_element->m_pNext->m_pPrev = _element->m_pPrev;

	if( m_pHead == _element )
		m_pHead = _element->m_pNext;

	if( m_pTail == _element )
		m_pTail = _element->m_pPrev;
}

//------------------------------------------

template<typename T>
void	ListOf<T>::RemoveAll()
{
	while(m_pHead)
	{
		T* tmp = m_pHead;
		m_pHead = m_pHead->m_pNext;
		tmp->m_pNext = NULL;
		tmp->m_pPrev = NULL;
	}

	m_pHead = m_pTail = NULL;
}

//------------------------------------------

template<typename T>
void	ListOf<T>::Switch(T* _a, T* _b)
{
	if( _a == _b )
		return;

	Swap(_a->m_pNext,_b->m_pNext);

	if( _a->m_pNext )
		_a->m_pNext->m_pPrev = _a;
	if( _b->m_pNext )
		_b->m_pNext->m_pPrev = _b;

	Swap(_a->m_pPrev,_b->m_pPrev);

	if( _a->m_pPrev )
		_a->m_pPrev->m_pNext = _a;
	if( _b->m_pPrev )
		_b->m_pPrev->m_pNext = _b;

	if( m_pHead == _a ) m_pHead = _b;
	else if( m_pHead == _b ) m_pHead = _a;
	if( m_pTail == _a ) m_pTail = _b;
	else if( m_pTail == _b ) m_pTail = _a;
}

//------------------------------------------

template<typename T>
S32		ListOf<T>::GetCount() const
{
	S32 count = 0;
	T* it = m_pHead;
	while(it)
	{
		count++;
		it = it->m_pNext;
	}
	return count;
}

#endif