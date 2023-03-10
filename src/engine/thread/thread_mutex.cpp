#include "thread_mutex.h"

ThreadMutex::ThreadMutex()
{
	m_Mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_Mutex, &attr);
}

ThreadMutex::~ThreadMutex()
{
	pthread_mutex_destroy(&m_Mutex);
}

//-------------------

void ThreadMutex::Lock()
{
	pthread_mutex_lock(&m_Mutex);
}

//-------------------

void ThreadMutex::Unlock()
{
	pthread_mutex_unlock(&m_Mutex);
}