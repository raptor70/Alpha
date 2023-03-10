#include "logger.h"

#include "main_logger.h"
#include "debug_logger.h"
#include "file_logger.h"
#include "console_logger.h"

#ifdef USE_LOGGER

SINGLETON_Define( Logger )

// ..

void Logger::Init(U32 _flag)
{
	if( _flag & FL_LOG_CONSOLE )
		m_loggerArray.AddLastItem( NEW ConsoleLogger() );

	if( _flag & FL_LOG_FILE )
		m_loggerArray.AddLastItem( NEW OldFileLogger(TRUE) );

	if( _flag & FL_LOG_DEBUG )
		m_loggerArray.AddLastItem( NEW DebugLogger() );
}

// ..

void Logger::AddNewLogger(const BaseLoggerRef& _newLogger)
{
	m_loggerArray.AddLastItem(_newLogger);
}

// ..

void Logger::Log( const LogType _type, const Char * _file, const S64 _line, const Char * _message, ...  )
{
    Str base_message;
	switch( _type )
    {
        case eLOG_Default: // normal
        {
			//base_message = _message;
            break;
        }

        case eLOG_Warning: // warning
        {
			base_message.SetFrom("%s(%d):warning: ", _file, _line);
            break;
        }

        case eLOG_Error: // error
        {
			base_message.SetFrom("%s(%d):ERROR: ", _file, _line);
            break;
        }

        default:
        {
			base_message.SetFrom("Unsupported type of message : ");
            break;
        }
    }

	Char buffer[4096];
	va_list args;
	va_start(args, _message);
	VSPrintf(buffer,_message, args);
    va_end( args );

	base_message += buffer;

    LogAllLogger( _type, base_message.GetArray() );
}

// ..

void Logger::LogAllLogger(const LogType _type, const Char * _message) const 
{
    for( U32 i=0; i < m_loggerArray.GetCount(); ++i )
    {
        m_loggerArray[i]->Write(_type,_message);
    }
}

#endif