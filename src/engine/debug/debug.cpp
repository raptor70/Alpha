#include "debug.h"

#include "core/str.h"

void DEBUG_Message( const char * text, ... )
{
	#if _DEBUG
		Char buffer[4096];
		va_list args;
		va_start(args, text);
		VSPrintf(buffer,text, args);
		va_end( args );

		LOGGER_LogError( "%s",buffer ); 
	#endif
}
