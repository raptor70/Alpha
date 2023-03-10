#ifndef __DEBUG_H__
#define __DEBUG_H__

	void DEBUG_Message( const char * text, ... );

#if _DEBUG
    #define DEBUG_DoNothing() {}

	#if WIN32
		#define DEBUG_Break()  __debugbreak();
	#elif LINUX
		#define DEBUG_Break()  __asm__("int3");
	#else
		#define DEBUG_Break()  DEBUG_DoNothing()
	#endif

	#define DEBUG_RequireMessage( _CONDITION_, ... ) if( !(_CONDITION_) ){ DEBUG_Message(__VA_ARGS__); DEBUG_Break();}
	#define DEBUG_Require( _CONDITION_ ) DEBUG_RequireMessage(_CONDITION_,"%s\n",#_CONDITION_)
	#define DEBUG_Forbidden(...)	DEBUG_RequireMessage(FALSE,__VA_ARGS__)

#else
    #define DEBUG_DoNothing()							{}
	#define DEBUG_Break()								DEBUG_DoNothing()
    #define DEBUG_Require( _CONDITION_ )				DEBUG_DoNothing()
	#define DEBUG_RequireMessage( _CONDITION_, ... )	DEBUG_DoNothing()
	#define DEBUG_Forbidden(...)						DEBUG_DoNothing()
#endif

#endif