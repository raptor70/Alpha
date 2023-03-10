#include "debug_logger.h"

#include "platform/platform_win32.h"

void DebugLogger::Write( const LogType _type, const Str & _message ) const
{
	#if WIN32
		OutputDebugString( _message.GetArray() );
	#elif LINUX
		openlog("ENGINE",LOG_PID,LOG_USER);
		syslog(LOG_DEBUG, "%s", _message.GetArray());
		closelog();
	#elif ANDROID
		android_LogPriority logType = ANDROID_LOG_INFO;
		if( _type == eLOG_Warning )
			logType = ANDROID_LOG_WARN;
		else if( _type == eLOG_Error )
			logType = ANDROID_LOG_ERROR;
		__android_log_print(logType,"Game","%s",_message.GetArray());
    #elif IOS
        fprintf(stderr,"%s",_message.GetArray());
    #endif
}