#ifndef __PTRTO_H__
    #define __PTRTO_H__


#if 1
template<typename T>
struct _PtrInfo
{
	_PtrInfo(T* _ptr) { m_ptr = _ptr; }
	_PtrInfo(T* _ptr, S32 refcount, S32 ptrcount) { m_ptr = _ptr; m_refcount = 1; m_ptrcount = ptrcount; }
		
	T* m_ptr = NULL;
	S32	m_refcount = 0;
	S32 m_ptrcount = 0;


	static _PtrInfo g_Null;
};

#define PTRINLINE INLINE

template<typename T, Bool _REFCOUNT>
class _PtrTo
{
public:
	template<typename T, Bool _OTHERREFCOUNT>
	friend class _PtrTo;

	PTRINLINE _PtrTo() { SetInfoFromPtr(NULL); }
	PTRINLINE _PtrTo(T* _ptr) { SetInfoFromPtr(_ptr); }
	PTRINLINE _PtrTo(const _PtrTo<T, _REFCOUNT>& _other) { SetInfoFromPtr(NULL); CopyFrom(_other); }
	template<Bool _OTHERREFCOUNT>
	PTRINLINE _PtrTo(const _PtrTo<T, _OTHERREFCOUNT>& _other)	{ SetInfoFromPtr(NULL); CopyFrom(_other); }
	template<class S, Bool _OTHERREFCOUNT>
	PTRINLINE _PtrTo(_PtrTo<S, _OTHERREFCOUNT>& _other) { SetInfoFromPtr(NULL); CopyFrom(_other);  }
	PTRINLINE ~_PtrTo(void) { Release(); }
	PTRINLINE _PtrTo& operator=(const _PtrTo<T, _REFCOUNT>& _other) { CopyFrom(_other); return *this;}
	template<Bool _OTHERREFCOUNT>
	PTRINLINE _PtrTo& operator=(const _PtrTo<T, _OTHERREFCOUNT>& _other) { CopyFrom(_other); return *this;}

	PTRINLINE _PtrTo& operator=(T* _ptr) { Release();  SetInfoFromPtr(_ptr); return *this; }
	PTRINLINE T& operator*(void) { return *GetRawPtr(); }
	PTRINLINE T* operator->(void) { return GetRawPtr(); }
	PTRINLINE const T& operator*(void) const { return *GetRawPtr(); }
	PTRINLINE const T* operator->(void) const { return GetRawPtr(); }
	PTRINLINE operator T* () { return GetRawPtr(); }
	PTRINLINE operator const T* () const { return GetRawPtr(); }
	PTRINLINE operator bool() { return GetRawPtr() != NULL; }

	PTRINLINE Bool operator==(const T* _ptr) { return m_pInfo->m_ptr == _ptr; }
	PTRINLINE Bool operator==(_PtrTo<T, _REFCOUNT>& _other) { return m_pInfo == _other.m_pInfo; }
	PTRINLINE Bool operator==(const _PtrTo<T, _REFCOUNT>& _other) const { return m_pInfo == _other.m_pInfo; }
	PTRINLINE Bool operator==(const _PtrTo<T, _REFCOUNT>& _other) { return m_pInfo == _other.m_pInfo; }

	template<Bool _OTHERREFCOUNT>
	PTRINLINE Bool operator==(_PtrTo<T, _OTHERREFCOUNT>& _other) { return m_pInfo == _other.m_pInfo; }
	template<Bool _OTHERREFCOUNT>
	PTRINLINE Bool operator==(const _PtrTo<T, _OTHERREFCOUNT>& _other) const { return m_pInfo == _other.m_pInfo; }
	template<Bool _OTHERREFCOUNT>
	PTRINLINE Bool operator!=(_PtrTo<T, _OTHERREFCOUNT>& _other) { return !((*this) == _other); }
	template<Bool _OTHERREFCOUNT>
	PTRINLINE Bool operator!=(const _PtrTo<T, _OTHERREFCOUNT>& _other) const { return !((*this) == _other); }

	PTRINLINE const T* GetPtr(void) const { return GetRawPtr(); }
	PTRINLINE const void* GetVoidPtr(void) const { return GetRawPtr(); }
	PTRINLINE T* GetPtr(void) { return GetRawPtr(); }

	template<class S>
	PTRINLINE S* GetCastPtr() { return (S*)(m_pInfo->m_ptr); }
	template<class S>
	PTRINLINE const S* GetCastPtr() const { return (const S*)(m_pInfo->m_ptr); }
	template<class S>
	PTRINLINE S* GetDynamicCastPtr() { return dynamic_cast<S*>(m_pInfo->m_ptr); }
	template<class S>
	PTRINLINE const S* GetDynamicCastPtr() const { return dynamic_cast<const S*>(m_pInfo->m_ptr); }

	template<class S>
	PTRINLINE _PtrTo<S, FALSE> GetCastedPtrTo() { return _PtrTo<S, FALSE>(*this); }

	PTRINLINE  S32 GetRefCount() const { return m_pInfo->m_refcount; }

	PTRINLINE void SetNull(void) { Release(); SetInfoFromPtr(NULL);}
	PTRINLINE Bool IsNull(void) const { return !GetRawPtr(); }
	PTRINLINE Bool IsValid(void) const { return GetRawPtr(); }

private:
	template<class S, Bool _OTHERREFCOUNT>
	PTRINLINE void CopyFrom(const _PtrTo<S, _OTHERREFCOUNT>& _other);
	PTRINLINE void Release();
	PTRINLINE T* GetRawPtr() { return (T*)m_pInfo->m_ptr; }
	PTRINLINE const T* GetRawPtr() const { return (const T*)m_pInfo->m_ptr; }
	PTRINLINE void SetInfoFromPtr(T* _ptr);

	_PtrInfo<T>* m_pInfo = NULL;
};

#undef PTRINLINE

//..

template<typename T, Bool _REFCOUNT>
void _PtrTo<T, _REFCOUNT>::Release(void)
{
	DEBUG_Require(m_pInfo->m_ptrcount > 0);
	DEBUG_Require(m_pInfo->m_refcount >= 0);

	if (_REFCOUNT && m_pInfo->m_refcount > 0)
	{
		m_pInfo->m_refcount--;
		if (m_pInfo->m_refcount == 0)
		{
			DELETE (T*)m_pInfo->m_ptr;
			m_pInfo->m_ptr = 0;
		}
	}

	if (m_pInfo->m_ptrcount > 0)
	{
		m_pInfo->m_ptrcount--;
		if ((m_pInfo->m_ptrcount) == 0)
		{
			DEBUG_Require(m_pInfo);
			DELETE m_pInfo;
		}
	}

	m_pInfo = NULL;
}

template<typename T, Bool _REFCOUNT>
template<class S, Bool _OTHERREFCOUNT>
void _PtrTo<T, _REFCOUNT>::CopyFrom(const _PtrTo<S, _OTHERREFCOUNT>& _other)
{
	Release();

	m_pInfo = (_PtrInfo<T>*)_other.m_pInfo;
	m_pInfo->m_ptrcount++;
	if (_REFCOUNT)
		m_pInfo->m_refcount++;
}

template<typename T, Bool _REFCOUNT>
void _PtrTo<T, _REFCOUNT>::SetInfoFromPtr(T* _ptr)
{
	DEBUG_Require(!m_pInfo);

	if (_ptr)
	{
		DEBUG_RequireMessage(_REFCOUNT, "Use RefTo instead of PtrTo");
		m_pInfo = NEW _PtrInfo<T>(_ptr);
	}
	else
	{
		m_pInfo = (_PtrInfo<T>*)&_PtrInfo<void>::g_Null;
	}

	m_pInfo->m_ptrcount++;
	if (_REFCOUNT)
		m_pInfo->m_refcount++;
}

template <typename T>
using RefTo = _PtrTo<T, TRUE>;

template <typename T>
using PtrTo = _PtrTo<T, FALSE>;

#else

template<typename T>
class PtrTo
{
public:
	PtrTo( void ): m_ptr(NULL),m_ptrCount(NULL) {}
	PtrTo( T * _ptr ): m_ptr( _ptr ) { m_ptrCount = NEW S32( 1 ); }
	PtrTo( const PtrTo & _other );
	~PtrTo( void ) { Release(); m_ptr = NULL; m_ptrCount = NULL;  }    
	PtrTo & operator=( const PtrTo & _other );

	PtrTo & operator=( T * _ptr );
	PtrTo & operator=( const T * _ptr );
	T& operator*( void ) { return *m_ptr; }
	INLINE T* operator->( void ) { return m_ptr; }
	const T& operator*( void ) const { return *m_ptr; }
	INLINE const T* operator->( void ) const { return m_ptr; }
	operator T* () {return m_ptr;}
	operator const T* () {return m_ptr;}

	Bool operator==(const T * _ptr ){ return m_ptr == _ptr; }
	Bool operator==(const PtrTo<T>& _other ){ return m_ptr == _other.m_ptr; }

	const T* GetPtr( void ) const { return m_ptr; }
	const void* GetVoidPtr( void ) const { return m_ptr; }
	T* GetPtr( void ) { return m_ptr; }

	template<class S>
	S* GetCastPtr() { return (S*)m_ptr; }

	S32 GetRefCount() const { return *m_ptrCount; }

	void SetNull( void ) { Release(); }
	Bool IsNull(void) const {return m_ptr == NULL;}
	Bool IsValid(void) const {return m_ptr != NULL;}

private:
	void Release( void );

	T *		m_ptr;
	S32 *	m_ptrCount;
};

template <typename T>
PtrTo<T>::PtrTo( const PtrTo<T> & _other ):
	m_ptr( _other.m_ptr ),
	m_ptrCount( _other.m_ptrCount )
{
	if(m_ptrCount && (*m_ptrCount) != 0 )
	{
		(*m_ptrCount)++;
	}
}

template <typename T>
PtrTo<T> & PtrTo<T>::operator=( const PtrTo<T> & _other )
{
	if( m_ptr != _other.m_ptr )
	{
		Release();
		m_ptr = _other.m_ptr;
		m_ptrCount = _other.m_ptrCount;

		if( m_ptrCount && (*m_ptrCount) != 0 )
		{
			(*m_ptrCount)++;
		}
	}

	return *this;
}

template <typename T>
PtrTo<T> & PtrTo<T>::operator=( T * _ptr )
{
	if( m_ptr != _ptr )
	{
		Release();
		m_ptr = _ptr;
		m_ptrCount = NEW S32(1);
	}
	return *this;
}

template <typename T>
PtrTo<T> & PtrTo<T>::operator=( const T * _ptr )
{
	if( m_ptr != _ptr )
	{
		Release();
		m_ptr = _ptr;
		m_ptrCount = NEW S32(1);
	}
	return *this;
}

template <typename T>
void PtrTo<T>::Release( void )
{
	if( m_ptrCount && (*m_ptrCount) != 0 )
	{
		(*m_ptrCount)--;
		if( (*m_ptrCount) == 0 )
		{
			DELETE m_ptr;
			DELETE m_ptrCount;
		}
	}

	m_ptr = NULL;
	m_ptrCount = NULL;
}
#endif

#endif