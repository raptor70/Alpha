#ifndef __DebugLogger_H__
#define __DebugLogger_H__

#include "main_logger.h"

class DebugLogger : public BaseLogger
{
public :    
	DebugLogger( void ): BaseLogger() {}
	virtual ~DebugLogger( void ) {}

	virtual void Write( const LogType _type, const Str & _message ) const;

private :
	DebugLogger & operator=( const DebugLogger & ) { return *this; }
	DebugLogger( const DebugLogger & _other ): BaseLogger( _other ) {}
};

#endif