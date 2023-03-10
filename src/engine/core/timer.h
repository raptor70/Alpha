#ifndef __TIMER_H__
    #define __TIMER_H__

#include "debug/logger.h"

#define USE_CHRONO
#ifdef USE_CHRONO
using namespace std::chrono;
#define BASE_TIMER_TYPE	steady_clock::time_point
#else
#define BASE_TIMER_TYPE Float
#endif

class Timer
{
public :    
	Timer( void );
	virtual ~Timer( void ) {}

	void Start( void );

	Float GetSecondCount();
	void Reset();

private :
	Timer( const Timer & ) { LOGGER_LogError( "Copy construtor must not be used\n" ); }
	Timer & operator=( const Timer & ) { LOGGER_LogError( "Affectation operator must not be used\n" );return *this;}

	BASE_TIMER_TYPE GetCurrentTime();

	Float m_Frequency;
	BASE_TIMER_TYPE m_LastTime;
};

#endif