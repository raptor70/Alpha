#ifndef __Logger_H__
    #define __Logger_H__

#include "main_logger.h"
#include "core/singleton.h"

#ifdef USE_LOGGER

#define FL_LOG_CONSOLE	(1<<0)
#define FL_LOG_FILE		(1<<1)
#define FL_LOG_DEBUG	(1<<2)

class Logger
{
	SINGLETON_Declare( Logger )

public :
	Logger(): m_loggerArray() {}
	virtual ~Logger() { m_loggerArray.SetEmpty(); }

	void Init(U32 _flag);

	void Log(const LogType _type, const Char * file, const S64 _line, const Char * _message, ... );

	void AddNewLogger(const BaseLoggerRef& _newLogger);

private :
	Logger( const Logger & ){};
	Logger & operator=( const Logger & ) { return *this; }

	void LogAllLogger( const LogType _type, const char * _message ) const;

	BaseLoggerRefArray m_loggerArray;
};

// LOGGER
#define LOGGER_LogMessage(__TYPE__,...)		Logger::GetInstance().Log(__TYPE__,__FILE__, __LINE__,__VA_ARGS__)
#define LOGGER_Log(...)						LOGGER_LogMessage(eLOG_Default,__VA_ARGS__)
#define LOGGER_LogWarning(...)				LOGGER_LogMessage(eLOG_Warning,__VA_ARGS__)
#define LOGGER_LogError(...) 				LOGGER_LogMessage(eLOG_Error,__VA_ARGS__)

#define LOGGER_Init							Logger::GetInstance().Init

#else
#define LOGGER_LogMessage(__TYPE__,...)		DEBUG_DoNothing()
#define LOGGER_Log(...)						DEBUG_DoNothing()					
#define LOGGER_LogWarning(...)				DEBUG_DoNothing()
#define LOGGER_LogError(...) 				DEBUG_DoNothing()

#define LOGGER_Init(a)						
#endif

#endif
