#ifndef __BaseLogger_H__
    #define __BaseLogger_H__

#include "debug/debug.h"
#include "core/array_of_.h"
#include "core/ptr_to_.h"
#include "core/str.h"

enum LogType
{
	eLOG_Default,
	eLOG_Warning,
	eLOG_Error
};

class BaseLogger
{
public :
	BaseLogger( void ){}; 
	BaseLogger( const BaseLogger & ){};
	virtual ~BaseLogger( void ){};

	virtual void Write( const LogType _type, const Str & message ) const = 0;

private :
	BaseLogger & operator=( const BaseLogger & ) { return *this; } 
};

typedef RefTo<BaseLogger> BaseLoggerRef;
typedef ArrayOf<BaseLoggerRef> BaseLoggerRefArray;

#endif