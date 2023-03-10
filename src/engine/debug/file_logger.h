#ifndef __OldFileLogger_H__
    #define __OldFileLogger_H__

#include "debug/main_logger.h"
#include "file/base_file.h"

class OldFileLogger : public BaseLogger
{
public :    
	OldFileLogger( void );
	OldFileLogger( const Bool _overwrite );
	OldFileLogger( const Str & _path, const Bool _overwrite );
	virtual ~OldFileLogger( void ) ;

	virtual void Write( const LogType _type, const Str & _message ) const;

private :
	OldFileLogger & operator=( const OldFileLogger & _other ) { return *this; }
	OldFileLogger( const OldFileLogger & _other );

	mutable OldFile m_out;
};

#endif