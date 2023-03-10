	
#ifndef __ArrayOf_H__
	#define __ArrayOf_H__

#include "debug/debug.h"

template<typename T>
class ArrayOf
{
public:
	ArrayOf()
	{
		m_iCount = m_iSize = 0;
		m_pArray = NULL;

	}
	ArrayOf( const ArrayOf& _other );
	ArrayOf( const U32 _count )
	{ 
		m_iCount = m_iSize = _count;
		m_pArray = (T*)NEW U8[m_iSize*sizeof(T)];
		for (U32 i = 0; i < m_iCount; i++)
			NEW(m_pArray + i) T();
	}        
	~ArrayOf( void ) 
	{ 
		Flush();
	}
	ArrayOf& operator=( const ArrayOf& _other );
	ArrayOf& operator+=( const ArrayOf& _other );

	INLINE T & operator[](U32 _index)  { DEBUG_Require(_index<m_iCount); return m_pArray[_index]; }
	INLINE const T & operator[](U32 _index ) const { DEBUG_Require(_index<m_iCount); return m_pArray[_index]; }
	INLINE T& Get(U32 _index) { DEBUG_Require(_index<m_iCount); return m_pArray[_index]; }
	INLINE const T& Get(U32 _index) const	{ DEBUG_Require(_index<m_iCount); return m_pArray[_index]; }

	INLINE const T * GetPointer() const { return m_pArray; }
	INLINE T * GetPointer() { return m_pArray; }

	T& AddLastItem() { return AddLastItem(T()); }
	T& AddLastItem( const T& _item ) 
	{ 
		Resize( m_iCount + 1);
		NEW(m_pArray+m_iCount-1) T(_item); 
		return m_pArray[m_iCount-1];
	}
	T& InsertItemAtIndex( const T& _item, U32 _index )
	{
		DEBUG_Require(_index>=0&&_index<=m_iCount);
		Resize( m_iCount + 1);
		memmove(m_pArray + _index + 1, m_pArray + _index, (m_iCount - _index - 1) * sizeof(T));
		/*for(U32 i=m_iCount-1;i>_index;i--)
			m_pArray[i] = m_pArray[i-1];*/
		NEW(m_pArray + _index) T(_item);
		return m_pArray[_index];
	}
	T& AddUniqueItem( const T& _item )
	{
		U32 index = -1;
		if( !Find(_item,index) )
		{
			return AddLastItem(_item);
		}

		return m_pArray[index];
	}
	void RemoveItems( U32 _first_index, U32 _count = 1 );

	void SetItemCount( U32 _new_item_count ) 
	{ 
		U32 previousCount = m_iCount;
		Resize( _new_item_count );
		for (U32 i = previousCount; i < m_iCount; i++)
			NEW(m_pArray + i) T();

	}
	U32 GetCount() const { return m_iCount; }
	U32 GetItemSize() const { return m_iCount * sizeof(T); }
	void SetEmpty()
	{ 
		RemoveItems(0,m_iCount); 
	};
	void ReserveCount(const U32 _size)
	{
		U32 previousCount = m_iCount;
		SetItemCount(m_iCount+_size);
		SetItemCount(previousCount);
	}
	void SetAllToValue(const T& _value);
	void Flush();

	Bool Find( const T &_item ) const;
	Bool Find( const T &_item, U32& _out_index  ) const;

	Bool HasItem( const T& _item ) const;

	void Reverse();

	T*	begin() { return m_pArray; }
	const T*	begin() const { return m_pArray; }
	T*	end() { return m_pArray + m_iCount; }
	const T*	end() const { return m_pArray + m_iCount; }

private:  
	void Resize( U32 _new_item_count );

	U32 m_iSize;
	U32 m_iCount;
	T * m_pArray;
};

template<typename T>
ArrayOf<T>::ArrayOf( const ArrayOf<T> & _other )
{
	m_iSize =  _other.m_iSize;
	m_iCount = _other.m_iCount;
	m_pArray = (T*)NEW U8[m_iSize * sizeof(T)];
	for( U32 i = 0; i < m_iCount; ++i )
	{
		NEW(m_pArray + i) T(_other.m_pArray[i]);
	}
}

template<typename T>
ArrayOf<T> & ArrayOf<T>::operator=( const ArrayOf<T> & _other )
{        
	Resize( _other.m_iCount );
	for( U32 i = 0; i < m_iCount; ++i )
	{
		m_pArray[i] = _other.m_pArray[i];
	}

	return *this;
}

template<typename T>
ArrayOf<T>& ArrayOf<T>::operator+=( const ArrayOf<T>& _other )
{
	U32 firstIdx = m_iCount;
	Resize(m_iCount + _other.m_iCount);
	for( U32 i = 0; i < _other.m_iCount; ++i )
	{
		m_pArray[firstIdx+i] = _other.m_pArray[i];
	}

	return *this;
}

template<typename T>
void ArrayOf<T>::RemoveItems( U32 _first_index, U32 _count )
{
	DEBUG_Require((_first_index+_count)<=m_iCount);
	U32 firstIndexKeep = _first_index + _count;
	for(U32 i= _first_index; i< firstIndexKeep; i++ )
	{
		m_pArray[i].~T();
	}
	
#if 1
	memmove(m_pArray + _first_index, m_pArray+ firstIndexKeep, (m_iCount - firstIndexKeep) * sizeof(T));
#else
	U32 new_count = m_iCount-_count;
	for(U32 i=_first_index; i<new_count; i++ )
	{
		m_pArray[i] = m_pArray[i + _count];
	}

	for(U32 i=new_count; i<m_iCount; i++ )
	{
		m_pArray[i].~T();
	}
#endif

	m_iCount -= _count;
}

template<typename T>
void ArrayOf<T>::Flush()
{
	SetEmpty();
	if( m_pArray )
	{
		DELETE[] (U8*)m_pArray; 
		m_pArray = NULL;
		m_iSize = 0;
	}
}

template<typename T>
Bool ArrayOf<T>::Find( const T &_item ) const
{
	U32 idx = 0;
	return Find(_item,idx);
}

template<typename T>
Bool ArrayOf<T>::Find( const T & _item, U32& _out_index ) const
{
	for( _out_index=0; _out_index<m_iCount; ++_out_index )
	{
		if( m_pArray[_out_index] == _item )
		{
			return TRUE;
		}
	}

	return FALSE;
}

template<typename T>
Bool ArrayOf<T>::HasItem( const T& _item ) const
{
	for(U32 i=0; i<m_iCount; ++i )
	{
		if( m_pArray[i] == _item )
		{
			return TRUE;
		}
	}

	return FALSE;
}

template<typename T>
void ArrayOf<T>::SetAllToValue(const T& _value)
{
	for (U32 i = 0; i<m_iCount; ++i)
	{
		m_pArray[i] = _value;
	}
}

template<typename T>
void ArrayOf<T>::Reverse()
{
	U32 mid = m_iCount / 2;
	for(U32 i=0;i<mid;i++)
	{
		Swap(m_pArray[i],m_pArray[m_iCount-1-i]);
	}
}

template<typename T>
void ArrayOf<T>::Resize( U32 _new_item_count )
{
	while( _new_item_count > m_iSize )
	{
		if( m_iSize == 0 ) m_iSize = 1;
		m_iSize*=2;
		T * new_array = (T*)NEW U8[m_iSize * sizeof(T)];
		memmove(new_array, m_pArray, m_iCount * sizeof(T));
		/*for(U32 i=0; i<m_iCount; i++ )
		{
			new_array[i] = m_pArray[i];
		}*/

		DELETE[] (U8*)m_pArray;
		m_pArray = new_array;
	}

	if(_new_item_count < m_iCount)
		RemoveItems(_new_item_count, m_iCount - _new_item_count);

	m_iCount = _new_item_count;
}

typedef ArrayOf<Bool>	BoolArray;
typedef ArrayOf<S32>	S32Array;
typedef ArrayOf<U32>	U32Array;
typedef ArrayOf<Float>	FloatArray;
typedef ArrayOf<S8>		S8Array;
typedef ArrayOf<U8>		U8Array;
typedef ArrayOf<U16>	U16Array;

#endif