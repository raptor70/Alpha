#pragma once
#ifndef __WINDOW_W32_H__
#define __WINDOW_W32_H__

#ifdef WIN32
	#define MEMORY_UNDEF
	#include "memory/memory.h"
	#include <windows.h>
	#include "memory/memory.h"

	struct WIN32MenuPlatformData : public Object
	{
		HMENU	m_hMenu = NULL;
	};

	struct WIN32ComponentPlatformData : public Object
	{
		HWND	m_hWnd = NULL;
	};

	struct WIN32WindowPlatformData : public Object
	{
		HWND					 m_hWnd = NULL;
		WIN32MenuPlatformData    m_MenuData;
	};

	struct WIN32ConsolePlatformData : public Object
	{
		HANDLE m_hConsole;
	};
#endif

#endif
