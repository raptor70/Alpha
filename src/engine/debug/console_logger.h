#ifndef __ConsoleLogger_H__
    #define __ConsoleLogger_H__

#include "main_logger.h"

class ConsoleLogger : public BaseLogger
{
public :    
	ConsoleLogger( void );
	virtual ~ConsoleLogger( void ) {}

	virtual void Write( const LogType _type, const Str & _message ) const;

private :
	ConsoleLogger & operator=( const ConsoleLogger & ) { return *this; }
	ConsoleLogger( const ConsoleLogger & _other ): BaseLogger( _other ) {}

#ifdef WIN32
	RefTo<Object>	m_PlatformData;
#endif
};

#endif