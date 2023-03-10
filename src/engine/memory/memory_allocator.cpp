#include "memory_allocator.h"

#ifdef MEMORY_ALLOCATOR

#include "debug/debug.h"
#include "debug/logger.h"

#include <stdlib.h>

MemoryAllocator::MemoryAllocator()
{
	StopTracking();
	StopSafe();

	m_TotalAllocSize = 0;
	m_MaxTotalAllocSize = 0;
	m_TotalAllocCount = 0;
	
#ifdef MEMORY_CHECK
	StartTracking();
#endif
}

//-----------------------------------

MemoryAllocator::~MemoryAllocator()
{
	StopTracking();
}

//-----------------------------------

MemoryAllocator& MemoryAllocator::GetInstance()
{
	static MemoryAllocator instance;
	return instance;
}

//-----------------------------------

void	MemoryAllocator::StartTracking()
{
	m_pNewFunction = &MemoryAllocator::RegisteredNew;
	m_pDeleteFunction = &MemoryAllocator::RegisteredDelete;
}

//-----------------------------------

void	MemoryAllocator::StopTracking()
{
	m_pNewFunction = &MemoryAllocator::UnregisteredNew;
	m_pDeleteFunction = &MemoryAllocator::UnregisteredDelete;
}

//-----------------------------------

void	MemoryAllocator::StartSafe()
{
	m_pNewSafe = &MemoryAllocator::NewSafe;
	m_pDeleteSafe = &MemoryAllocator::DeleteSafe;
}

//-----------------------------------

void	MemoryAllocator::StopSafe()
{
	m_pNewSafe = &MemoryAllocator::NewUnsafe;
	m_pDeleteSafe = &MemoryAllocator::DeleteUnsafe;
}

//-----------------------------------

void*	MemoryAllocator::New(U32 _size, const Char* _file, S32 _line, const Char* _name)
{
	return (this->*m_pNewSafe)(_size, _file, _line, _name);
}

//-----------------------------------

void	MemoryAllocator::Delete(void* _ptr)
{
	(this->*m_pDeleteSafe)(_ptr);
}

//-----------------------------------

void*	MemoryAllocator::NewSafe(U32 _size, const Char* _file, S32 _line, const char* _name)
{
	THREAD_MUTEX_PROTECT(m_Mutex);
	return (this->*m_pNewFunction)(_size,_file,_line,_name);
}

//-----------------------------------

void	MemoryAllocator::DeleteSafe(void* _ptr)
{
	THREAD_MUTEX_PROTECT(m_Mutex);
	(this->*m_pDeleteFunction)(_ptr);
}

//-----------------------------------

void*	MemoryAllocator::NewUnsafe(U32 _size, const Char* _file, S32 _line, const char* _name)
{
	void* newptr = (this->*m_pNewFunction)(_size, _file, _line, _name);
	return newptr;
}

//-----------------------------------

void	MemoryAllocator::DeleteUnsafe(void* _ptr)
{
	(this->*m_pDeleteFunction)(_ptr);
}
	
//-----------------------------------

void*	MemoryAllocator::RegisteredNew(U32 _size, const Char* _file, S32 _line, const char* _name)
{
	void* new_ptr = malloc(_size);
	
	StopTracking();

	SAlloc* new_alloc = m_Allocs.AddItem(new_ptr, SAlloc());
	new_alloc->m_ptr = new_ptr;
	new_alloc->m_size = _size;
	new_alloc->m_file = _file;
	new_alloc->m_line = _line;
	new_alloc->m_name = _name;
	
	m_TotalAllocCount++;
	m_TotalAllocSize += _size;
	m_MaxTotalAllocSize = Max(m_TotalAllocSize,m_MaxTotalAllocSize);

	StartTracking();

	return new_ptr;
}

//-----------------------------------

void	MemoryAllocator::RegisteredDelete(void* _ptr)
{
	void* old_ptr = _ptr;
	if( old_ptr )
	{
		StopTracking();
		Bool bFound = FALSE;

		SAlloc oldAlloc;
		if (m_Allocs.RemoveItem(old_ptr, &oldAlloc))
		{
			m_TotalAllocCount--;
			m_TotalAllocSize -= oldAlloc.m_size;
			bFound = TRUE;
		}

		if (!bFound)
		{
			LOGGER_LogError("Memory Allocation problem !\n");
		}
		else
			free(_ptr);

		StartTracking();
	}
}

//-----------------------------------

void*	MemoryAllocator::UnregisteredNew(U32 _size, const Char* _file, S32 _line, const Char* _name)
{
	return malloc(_size);
}

//-----------------------------------

void	MemoryAllocator::UnregisteredDelete(void* _ptr)
{
	free(_ptr);
}

//-----------------------------------

void	MemoryAllocator::Flush()
{
	StopTracking();
	if( m_TotalAllocSize == 0 )
		LOGGER_Log("----- MEMORY_ALLOCATOR : NO LEAK -------\n");
	else
	{
		LOGGER_Log("----- MEMORY_ALLOCATOR -------\n");

		for (AllocHashMap::Iterator it = m_Allocs.GetFirst(); it != m_Allocs.GetLast(); it++)
		{
			SAlloc* current = &(it.GetValue());
			if( current->m_name )
				LOGGER_Log("(%x)(%s) %dB - %s(%d)\n",current->m_ptr,current->m_name,current->m_size,current->m_file,current->m_line);
			else
				LOGGER_Log("(%x) %dB - %s(%d)\n",current->m_ptr,current->m_size,current->m_file,current->m_line);
		}
		LOGGER_Log("----- MEMORY_ALLOCATOR (LEAK = %dB)-------\n",m_TotalAllocSize);
	}
	StartTracking();
}

//-----------------------------------

void* operator new(size_t _size, const char* _file, int _line, const char* _name)
{
	return MemoryAllocator::GetInstance().New(U32(_size),_file,_line,_name);
}

//-----------------------------------

void* operator new[](size_t _size, const char* _file, int _line, const char* _name)
{
	return MemoryAllocator::GetInstance().New(U32(_size),_file,_line,_name);
}

//-----------------------------------

void operator delete(void* Ptr)
{
	MemoryAllocator::GetInstance().Delete(Ptr);
}

//-----------------------------------

void operator delete[](void* Ptr)
{
	MemoryAllocator::GetInstance().Delete(Ptr);
}

//-----------------------------------

void operator delete(void* Ptr, const char* File, int Line, const char* _name)
{
	MemoryAllocator::GetInstance().Delete(Ptr);
}

//-----------------------------------

void operator delete[](void* Ptr, const char* File, int Line, const char* _name)
{
	MemoryAllocator::GetInstance().Delete(Ptr);
}

#endif // MEMORY_ALLOCATOR
