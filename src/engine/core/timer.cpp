#include "timer.h"

Timer::Timer( void ):
#ifdef USE_CHRONO
	m_LastTime()
#else
	m_LastTime( 0.0f )
#endif
{
#ifdef WIN32
	m_Frequency = CLOCKS_PER_SEC;
#elif defined(IOS)
	m_Frequency = 1.f;//CLOCKS_PER_SEC;
#elif defined(ANDROID)
	m_Frequency = 1000000.f;
#endif
}

void Timer::Start( void )
{
    Reset();
}

Float Timer::GetSecondCount()
{
#ifdef USE_CHRONO
	BASE_TIMER_TYPE now = GetCurrentTime();
	return Float(duration_cast<microseconds>(now - m_LastTime).count()) / (1000.f * 1000.f);	
#else
    Float second_count, current_time;
    
    current_time = GetCurrentTime();
    second_count = ( current_time - m_LastTime ) / m_Frequency;
    return second_count;
#endif
}

void Timer::Reset()
{
	m_LastTime = GetCurrentTime();
}

BASE_TIMER_TYPE Timer::GetCurrentTime()
{
#ifdef USE_CHRONO
	return steady_clock::now();
#else
#ifdef WIN32
	return ((Float)clock());
#elif defined(ANDROID)
	struct timeval now;
	gettimeofday(&now, NULL);
	return (Float)(now.tv_sec * 1000000 + now.tv_usec);
#elif defined(IOS)
    return (Float)CFAbsoluteTimeGetCurrent();// ((Float)clock());
#else 
    #error "Time not defined !!!"
    return 0;
#endif
#endif
}