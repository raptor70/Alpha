#ifndef __MEMORY_ALLOCATOR_H__
#define __MEMORY_ALLOCATOR_H__

#ifdef MEMORY_ALLOCATOR

#define HASHMAP_ALLOCS

#include "thread/thread_mutex.h"

struct SAlloc
{
	void*		m_ptr;
	U32			m_size;
	const Char*	m_file;
	S32			m_line;
	const Char* m_name;

	SAlloc()
	{
		m_ptr = NULL;
	}
};

class MemoryAllocator;
typedef void*	(MemoryAllocator::*FNewFunction)(U32, const Char*, S32, const Char*);
typedef void	(MemoryAllocator::*FDeleteFunction)(void*);

class MemoryAllocator
{
	
public:
	MemoryAllocator();
	~MemoryAllocator();

	static MemoryAllocator& GetInstance();

	void	StartTracking();
	void	StopTracking();
	void	StartSafe();
	void	StopSafe();

	void*	New(U32 _size, const Char* _file, S32 _line, const Char* _name);
	void	Delete(void* _ptr);
	void*	NewUnsafe(U32 _size, const Char* _file, S32 _line, const Char* _name); // during static initialization (no new thread during static)
	void	DeleteUnsafe(void* _ptr);
	void*	NewSafe(U32 _size, const Char* _file, S32 _line, const Char* _name); // after static
	void	DeleteSafe(void* _ptr);

	void*	RegisteredNew(U32 _size, const Char* _file, S32 _line, const Char* _name);
	void	RegisteredDelete(void* _ptr);

	void*	UnregisteredNew(U32 _size, const Char* _file, S32 _line, const Char* _name);
	void	UnregisteredDelete(void* _ptr);

	void	Flush();

	U64		GetTotalAllocSize() const { return m_TotalAllocSize; }
	U64		GetMaxTotalAllocSize() const { return m_MaxTotalAllocSize; }

private:
	U64				m_TotalAllocSize;	
	U64				m_MaxTotalAllocSize;	
	U64				m_TotalAllocCount;
	typedef		HashOf<void*, SAlloc,1024> AllocHashMap;
	AllocHashMap	m_Allocs;

	FNewFunction	m_pNewSafe;
	FDeleteFunction m_pDeleteSafe;
	FNewFunction	m_pNewFunction;
	FDeleteFunction m_pDeleteFunction;
	ThreadMutex	m_Mutex;
};

#endif //MEMORY_ALLOCATOR

#endif