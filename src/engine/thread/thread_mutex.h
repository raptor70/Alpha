#ifndef __THREAD_MUTEX_H__
#define __THREAD_MUTEX_H__

class ThreadMutex
{
public:
	ThreadMutex();
	virtual ~ThreadMutex();

	void Lock();
	void Unlock();

protected:
	pthread_mutex_t		m_Mutex;
};

//-------------------------------------

class ThreadMutexScope
{
public:
	ThreadMutexScope(ThreadMutex* _mutex) 
	{ 
		m_pMutex = _mutex; m_pMutex->Lock(); 
	}
	virtual ~ThreadMutexScope() 
	{ 
		m_pMutex->Unlock(); 
	}

	ThreadMutex* m_pMutex;
};

#define THREAD_MUTEX_PROTECT(_mutex)	ThreadMutexScope scopeMutex(&_mutex);

#endif