#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef WIN32
	#define HAVE_STRUCT_TIMESPEC
	#include INCLUDE_LIB(LIB_PTHREAD,pthread.h)
#else
	#include <pthread.h>
#endif

typedef void* (*FThreadRunFunction)(void* _arg);

class Thread
{
public:
	Thread(const Char* _name);
	virtual ~Thread();

	void Run(FThreadRunFunction _runFunction,void * _arg);
	void WaitForCompletion();

	static U32 GetThreadId();

protected:
	static void* ThreadFunc(void* _arg);

	pthread_t			m_Thread;
	pthread_mutex_t		m_Mutex;
	pthread_cond_t		m_Condition;
	FThreadRunFunction	m_pRunFunction;
	void*				m_pRunArg;
};

#endif