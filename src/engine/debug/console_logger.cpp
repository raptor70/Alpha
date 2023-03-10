#include "console_logger.h"

#include <iostream>

#include "platform/platform_win32.h"

enum EConsoleColor { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE };

ConsoleLogger::ConsoleLogger( void ): BaseLogger() 
{
#ifdef WIN32
	m_PlatformData = NEW WIN32ConsolePlatformData;
	m_PlatformData.GetDynamicCastPtr<WIN32ConsolePlatformData>()->m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

void ConsoleLogger::Write( const LogType _type, const Str & _message ) const
{
#ifdef WIN32
	switch(_type)
	{
		case eLOG_Default : SetConsoleTextAttribute(m_PlatformData.GetDynamicCastPtr<WIN32ConsolePlatformData>()->m_hConsole, GRAY); break;
		case eLOG_Warning : SetConsoleTextAttribute(m_PlatformData.GetDynamicCastPtr<WIN32ConsolePlatformData>()->m_hConsole, YELLOW); break;
		case eLOG_Error :	SetConsoleTextAttribute(m_PlatformData.GetDynamicCastPtr<WIN32ConsolePlatformData>()->m_hConsole, RED); break;
	}
	std::cout << _message.GetArray();
#endif
}