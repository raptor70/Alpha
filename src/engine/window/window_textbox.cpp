#include "window_textbox.h"
#include "platform/platform_win32.h"

WindowTextBox::WindowTextBox(S32 _x, S32 _y, S32 _sizex, S32 _sizey):SUPER()
{
	m_x = _x;
	m_y = _y;
	m_width = _sizex;
	m_height = _sizey;
}

//..

Bool WindowTextBox::Create(const Window* _pParentWindow)
{
	if (!SUPER::Create(_pParentWindow))
		return FALSE;

#ifdef WIN32
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	pData->m_hWnd=CreateWindow(
        "EDIT",
        "",
		WS_CHILD|WS_VISIBLE|WS_BORDER,
        m_x,m_y,
        m_width,m_height,
		_pParentWindow->GetPlatformData().GetDynamicCastPtr<WIN32ComponentPlatformData>()->m_hWnd,
        NULL,
		NULL,
        NULL);
#endif

	return TRUE;
}

Bool WindowTextBox::Destroy()
{
	#ifdef WIN32
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	DestroyWindow(pData->m_hWnd);
#endif
	return TRUE;
}

const Char* WindowTextBox::GetText()
{
	#ifdef WIN32
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	GetWindowText(pData->m_hWnd,m_sText,256);
#endif
	return m_sText;
}

void	WindowTextBox::ClearText()
{
	#ifdef WIN32
	WIN32ComponentPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32ComponentPlatformData>();
	SetWindowText(pData->m_hWnd,NULL);
	memset(m_sText,0,256);
#endif
}
