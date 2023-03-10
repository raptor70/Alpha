#include "window_component.h"

#include "platform/platform_win32.h"

WindowComponent::WindowComponent()
{
#ifdef WIN32
	m_pPlatformData = NEW WIN32ComponentPlatformData;
#endif
}