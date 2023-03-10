#include "window_whiterect.h"

#include "platform/platform_win32.h"

WindowWhiteRect::WindowWhiteRect(S32 _x, S32 _y, S32 _sizex, S32 _sizey):SUPER()
{
	m_x = _x;
	m_y = _y;
	m_width = _sizex;
	m_height = _sizey;
}

//..

Bool WindowWhiteRect::Create(const Window* _pParentWindow)
{
	if (!SUPER::Create(_pParentWindow))
		return FALSE;

#if defined(WIN32)
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	pData->m_hWnd=CreateWindow(
        "STATIC",
        "",
		WS_CHILD|WS_VISIBLE/*|SS_WHITEFRAME*/,
        m_x,m_y,
        m_width,m_height,
		_pParentWindow->GetPlatformData().GetDynamicCastPtr<WIN32WindowPlatformData>()->m_hWnd,
        NULL,
		NULL,
        NULL);
#endif

	return TRUE;
}

//..

void	WindowWhiteRect::SetSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey)
{
	m_x = _x;
	m_y = _y;
	m_width = _sizex;
	m_height = _sizey;

#if defined(WIN32)
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	::SetWindowPos(pData->m_hWnd,0,m_x,m_y,m_width,m_height, SWP_NOZORDER | SWP_NOACTIVATE);
#endif
}

//..

Bool WindowWhiteRect::Destroy()
{
#if defined(WIN32)
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	DestroyWindow(pData->m_hWnd);
#endif
	return TRUE;
}
