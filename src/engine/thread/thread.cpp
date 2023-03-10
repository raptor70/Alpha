#include "thread.h"

void* Thread::ThreadFunc(void* _arg)
{
	PROFILER_OPTICK_THREAD("Worker");
	Thread* pThread = (Thread*)_arg;
	while(1)
	{
		pthread_mutex_lock(&(pThread->m_Mutex));
		if(pThread->m_pRunFunction) 
		{
			(*(pThread->m_pRunFunction))(pThread->m_pRunArg);
			pThread->m_pRunFunction = NULL;
		}
		else
		{
			pthread_cond_wait(&(pThread->m_Condition), &(pThread->m_Mutex));
		}
		pthread_mutex_unlock(&(pThread->m_Mutex));
	}
	pthread_exit(NULL);
}

//------------------------------

Thread::Thread(const Char* _name)
{
	m_pRunArg = NULL;
	m_pRunFunction = NULL;
	m_Mutex = PTHREAD_MUTEX_INITIALIZER;
	m_Condition = PTHREAD_COND_INITIALIZER;
	pthread_mutex_init(&m_Mutex, NULL);
    pthread_cond_init(&m_Condition, NULL);
	pthread_create(&m_Thread,NULL,ThreadFunc,this);
	static U32 g_GlobalIdx = 0;
	m_Thread.x = g_GlobalIdx;
	g_GlobalIdx++;
}

//------------------------------

Thread::~Thread()
{
	pthread_cancel(m_Thread);
	pthread_join(m_Thread, NULL);

    pthread_cond_destroy(&m_Condition);
    pthread_mutex_destroy(&m_Mutex);
}

//------------------------------

void Thread::Run(FThreadRunFunction _runFunction,void* _arg)
{
	pthread_mutex_lock(&m_Mutex);
	m_pRunFunction = _runFunction;
	m_pRunArg = _arg;
	pthread_cond_signal(&m_Condition);
	pthread_mutex_unlock(&m_Mutex);
}

//------------------------------

void Thread::WaitForCompletion()
{
	while(1)
	{
		pthread_mutex_lock(&m_Mutex);
		if(!m_pRunFunction) 
		{
			pthread_mutex_unlock(&m_Mutex);
			return;
		}
		pthread_mutex_unlock(&m_Mutex);
	}
}

//------------------------------

U32 Thread::GetThreadId()
{
	pthread_t thread = pthread_self();
	return thread.x;
}