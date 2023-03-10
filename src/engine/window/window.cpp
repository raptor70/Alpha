#include "window.h"

#include "input/input_manager.h"
#include "imgui/imgui_manager.h"
#include "platform/platform_win32.h"

//..

#ifdef WIN32

static struct KEYFromWChar
{
	KEYFromWChar()
	{
		m_Keys[VK_LBUTTON] = MOUSE_Left;
		m_Keys[VK_MBUTTON] = MOUSE_Middle;
		m_Keys[VK_RBUTTON] = MOUSE_Right;

		m_Keys['A'] = KEY_A;
		m_Keys['B'] = KEY_B;
		m_Keys['C'] = KEY_C;
		m_Keys['D'] = KEY_D;
		m_Keys['E'] = KEY_E;
		m_Keys['F'] = KEY_F;
		m_Keys['G'] = KEY_G;
		m_Keys['H'] = KEY_H;
		m_Keys['I'] = KEY_I;
		m_Keys['J'] = KEY_J;
		m_Keys['K'] = KEY_K;
		m_Keys['L'] = KEY_L;
		m_Keys['M'] = KEY_M;
		m_Keys['N'] = KEY_N;
		m_Keys['O'] = KEY_O;
		m_Keys['P'] = KEY_P;
		m_Keys['Q'] = KEY_Q;
		m_Keys['R'] = KEY_R;
		m_Keys['S'] = KEY_S;
		m_Keys['T'] = KEY_T;
		m_Keys['U'] = KEY_U;
		m_Keys['V'] = KEY_V;
		m_Keys['W'] = KEY_W;
		m_Keys['X'] = KEY_X;
		m_Keys['Y'] = KEY_Y;
		m_Keys['Z'] = KEY_Z;
		m_Keys['0'] = KEY_Num0;
		m_Keys['1'] = KEY_Num1;
		m_Keys['2'] = KEY_Num2;
		m_Keys['3'] = KEY_Num3;
		m_Keys['4'] = KEY_Num4;
		m_Keys['5'] = KEY_Num5;
		m_Keys['6'] = KEY_Num6;
		m_Keys['7'] = KEY_Num7;
		m_Keys['8'] = KEY_Num8;
		m_Keys['9'] = KEY_Num9;
		m_Keys[VK_ESCAPE] = KEY_Escape;
		m_Keys[VK_CONTROL] = KEY_Control;
		m_Keys[VK_SHIFT] = KEY_Shift;
		m_Keys[VK_MENU] = KEY_Alt;
		m_Keys[VK_LCONTROL] = KEY_LControl;
		m_Keys[VK_LSHIFT] = KEY_LShift;
		m_Keys[VK_LMENU] = KEY_LAlt;
		m_Keys[VK_LWIN] = KEY_LSystem;
		m_Keys[VK_RCONTROL] = KEY_RControl;
		m_Keys[VK_RSHIFT] = KEY_RShift;
		m_Keys[VK_RMENU] = KEY_RAlt;
		m_Keys[VK_RWIN] = KEY_RSystem;
		m_Keys[VK_APPS] = KEY_Menu;
		m_Keys[VK_OEM_4] = KEY_LBracket;
		m_Keys[VK_OEM_6] = KEY_RBracket;
		m_Keys[VK_OEM_1] = KEY_SemiColon;
		m_Keys[VK_OEM_COMMA] = KEY_Comma;
		m_Keys[VK_OEM_PERIOD] = KEY_Period;
		m_Keys[VK_OEM_7] = KEY_Quote;
		m_Keys[VK_OEM_2] = KEY_Slash;
		m_Keys[VK_OEM_5] = KEY_BackSlash;
		m_Keys[VK_OEM_3] = KEY_Tilde;
		m_Keys[VK_OEM_PLUS] = KEY_Equal;
		m_Keys[VK_OEM_MINUS] = KEY_Dash;
		m_Keys[VK_SPACE] = KEY_Space;
		m_Keys[VK_RETURN] = KEY_Enter;
		m_Keys[VK_BACK] = KEY_BackSpace;
		m_Keys[VK_TAB] = KEY_Tab;
		m_Keys[VK_PRIOR] = KEY_PageUp;
		m_Keys[VK_NEXT] = KEY_PageDown;
		m_Keys[VK_END] = KEY_End;
		m_Keys[VK_HOME] = KEY_Home;
		m_Keys[VK_INSERT] = KEY_Insert;
		m_Keys[VK_DELETE] = KEY_Delete;
		m_Keys[VK_ADD] = KEY_Add;
		m_Keys[VK_SUBTRACT] = KEY_Subtract;
		m_Keys[VK_MULTIPLY] = KEY_Multiply;
		m_Keys[VK_DIVIDE] = KEY_Divide;
		m_Keys[VK_LEFT] = KEY_Left;
		m_Keys[VK_RIGHT] = KEY_Right;
		m_Keys[VK_UP] = KEY_Up;
		m_Keys[VK_DOWN] = KEY_Down;
		m_Keys[VK_NUMPAD0] = KEY_Numpad0;
		m_Keys[VK_NUMPAD1] = KEY_Numpad1;
		m_Keys[VK_NUMPAD2] = KEY_Numpad2;
		m_Keys[VK_NUMPAD3] = KEY_Numpad3;
		m_Keys[VK_NUMPAD4] = KEY_Numpad4;
		m_Keys[VK_NUMPAD5] = KEY_Numpad5;
		m_Keys[VK_NUMPAD6] = KEY_Numpad6;
		m_Keys[VK_NUMPAD7] = KEY_Numpad7;
		m_Keys[VK_NUMPAD8] = KEY_Numpad8;
		m_Keys[VK_NUMPAD9] = KEY_Numpad9;
		m_Keys[VK_F1] = KEY_F1;
		m_Keys[VK_F2] = KEY_F2;
		m_Keys[VK_F3] = KEY_F3;
		m_Keys[VK_F4] = KEY_F4;
		m_Keys[VK_F5] = KEY_F5;
		m_Keys[VK_F6] = KEY_F6;
		m_Keys[VK_F7] = KEY_F7;
		m_Keys[VK_F8] = KEY_F8;
		m_Keys[VK_F9] = KEY_F9;
		m_Keys[VK_F10] = KEY_F10;
		m_Keys[VK_F11] = KEY_F11;
		m_Keys[VK_F12] = KEY_F12;
		m_Keys[VK_F13] = KEY_F13;
		m_Keys[VK_F14] = KEY_F14;
		m_Keys[VK_F16] = KEY_F15;
		m_Keys[VK_PAUSE] = KEY_Pause;
	}

	KEY m_Keys[256];
} g_ToKey;


LRESULT CALLBACK WndProc(HWND _hWnd, U32 _iMsg, WPARAM _wParam, LPARAM _lParam)
{
	Window* pWin = (Window*)GetWindowLongPtr(_hWnd, GWLP_USERDATA);

	switch (_iMsg)
	{
	case WM_SYSCOMMAND:
	{
		switch (_wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_MOVING:
	case WM_MOVE:
	{
		if (_hWnd)
		{
			POINT pos = { 0, 0 };
			::ClientToScreen(_hWnd, &pos);

			if (pWin)
			{
				pWin->SetPos(pos.x,pos.y);
			}
		}
		return 0;
	}

	case WM_SIZING:
	case WM_SIZE:
	{
		if (_hWnd)
		{
			RECT rect;
			::GetClientRect(_hWnd, &rect);

			if (pWin)
			{
				pWin->SetSize(rect.right - rect.left, rect.bottom - rect.top);
			}
		}
		return 0;
	}

	case WM_CHAR:
	{
		IMGUIMGR_AddInputCharacter((U32)_wParam);
		break;
	}

	case WM_SETCURSOR:
	{
		if (LOWORD(_lParam) == HTCLIENT && IMGUIMGR_UpdateMouseCursor())
		{
			// on client space & manage by imgui
			return 1;
		}
		break;
	}

	if (pWin->IsManagingInput())
	{
	case WM_KEYDOWN:
	{
		InputManager::GetInstance().GetKeyboard().SetKeyState(g_ToKey.m_Keys[_wParam], TRUE);
		return 0;
	}

	case WM_KEYUP:
	{
		InputManager::GetInstance().GetKeyboard().SetKeyState(g_ToKey.m_Keys[_wParam], FALSE);
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		InputManager::GetInstance().GetMouse().SetWheelDelta((Float)(GET_WHEEL_DELTA_WPARAM(_wParam) / (Float)WHEEL_DELTA));
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Left, TRUE);
		return 0;
	}

	case WM_LBUTTONUP:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Left, FALSE);
		return 0;
	}

	case WM_MBUTTONDOWN:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Middle, TRUE);
		return 0;
	}

	case WM_MBUTTONUP:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Middle, FALSE);
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Right, TRUE);
		return 0;
	}

	case WM_RBUTTONUP:
	{
		InputManager::GetInstance().GetMouse().SetKeyState(MOUSE_Right, FALSE);
		return 0;
	}
	}
	}

	return DefWindowProc(_hWnd, _iMsg, _wParam, _lParam);
}

void ComputeWin32Styles(Window::Params& _params, DWORD& _wndStyle, DWORD& _extStyle)
{
	_wndStyle = _extStyle = 0;

	if ((_params.m_Style & WindowStyle_Fullscreen) != 0)
	{
		_wndStyle |= WS_POPUP;
	}
	else if ((_params.m_Style & WindowStyle_Borderless) != 0)
	{
		_wndStyle |= WS_POPUP;
	}
	else
	{
		_wndStyle |= WS_OVERLAPPEDWINDOW;
	}

	if ((_params.m_Style & WindowStyle_InTaskBar) != 0)
		_extStyle |= WS_EX_APPWINDOW;
	else
		_extStyle |= WS_EX_TOOLWINDOW;

	if ((_params.m_Style & WindowStyle_TopMost) != 0)
		_extStyle |= WS_EX_TOPMOST;
}

RECT ComputeWin32FullWindowRect(Window::Params& _params, DWORD& _wndStyle, DWORD& _extStyle, Bool _bHasMenu)
{
	RECT wndRect;
	wndRect.left = _params.m_x;
	wndRect.right = _params.m_x + _params.m_iWidth;
	wndRect.top = _params.m_y;
	wndRect.bottom = _params.m_y + _params.m_iHeight;
	::AdjustWindowRectEx(&wndRect, _wndStyle, _bHasMenu, _extStyle);
	return wndRect;
}	
#endif

//..

void Window::InitPlatform()
{
	// Create class (todo : static InitPlatform)
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = ::GetModuleHandle(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Default";
	wndclass.hIconSm = NULL;
	if (!RegisterClassEx(&wndclass))
	{
		LOGGER_LogError("Unable to register class of the windows\n");
	}
}

//..

void Window::DestroyPlatform()
{
#ifdef WIN32	
	if (!UnregisterClass("Default", ::GetModuleHandle(NULL)))				// Are We Able To Unregister Class
	{
		LOGGER_LogError("Could Not Unregister Class\n");
	}
#endif
}

//..

Window::Window()
{  
	m_bMustKill = FALSE;
	m_bManageInput= FALSE;

#ifdef WIN32
	m_pPlatformData = NEW WIN32WindowPlatformData;
#endif
}

//..

Window::Window(Params& _params)
{ 
	m_Params = _params;
	m_bMustKill = FALSE;
	m_bManageInput= FALSE;
#ifdef WIN32
	m_pPlatformData = NEW WIN32WindowPlatformData;
#endif
}

//..

Bool Window::Create(const Window* _pParentWindow)
{    
#ifdef WIN32
	WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();

	// Style
	DWORD extendedStyle = 0;
	DWORD	wndStyle = 0;
	ComputeWin32Styles(m_Params, wndStyle, extendedStyle);
    if ((m_Params.m_Style & WindowStyle_Fullscreen) != 0)
    {
        DEVMODE  dmScreenSettings;
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth  = m_Params.m_iWidth;
        dmScreenSettings.dmPelsHeight = m_Params.m_iHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        if ( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL )
        {
            LOGGER_LogError( "The Requested Fullscreen Mode Is Not Supported\n" );
            return FALSE;
        }

		ShowCursor(FALSE);
    }

	pData->m_MenuData.m_hMenu = NULL;
	if( m_aMenus.GetCount()>0)
	{
		pData->m_MenuData.m_hMenu = CreateMenu();
		for(U32 i=0; i < m_aMenus.GetCount() ; ++i )
		{
			m_aMenus[i]->Create(&pData->m_MenuData);
		}
	}

	// Rect
	RECT wndRect = ComputeWin32FullWindowRect(m_Params, wndStyle, extendedStyle, (pData->m_MenuData.m_hMenu != NULL));
	
	// Create
	pData->m_hWnd=CreateWindowEx(
		extendedStyle,
		"Default",
		m_Params.m_sTitle.GetArray(),
		wndStyle,
		wndRect.left, wndRect.top,
		wndRect.right-wndRect.left, 
		wndRect.bottom-wndRect.top,
		_pParentWindow ? _pParentWindow->m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>()->m_hWnd : NULL,
		pData->m_MenuData.m_hMenu,
		::GetModuleHandle(NULL), 
		NULL);

    if (!pData->m_hWnd)
    {	
		//DEBUG
		DWORD errorMessageID = GetLastError();
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

        Destroy();
		LOGGER_LogError("Window Creation Error\n");
		return FALSE;
	}    

    SetWindowLongPtr(pData->m_hWnd, GWLP_USERDATA, (LONG_PTR)(this));

#endif

	for (U32 i = 0; i < m_aComponents.GetCount(); i++)
	{
		m_aComponents[i]->Create(this);
	}

	return TRUE;
}

//..

Bool Window::Destroy()
{ 
	for(U32 i=0; i<m_aComponents.GetCount();i++)
	{
		m_aComponents[i]->Destroy();
	}

#ifdef WIN32
	WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
    if (m_Params.m_Style & WindowStyle_Fullscreen)
    {
        ChangeDisplaySettings( NULL, 0 );
        ShowCursor( TRUE );              
    }

    if (pData->m_hWnd && !DestroyWindow(pData->m_hWnd))
	{
		LOGGER_LogError("Could Not Release hWnd\n");
		pData->m_hWnd=NULL;							// Set hWnd To NULL        
	}
#endif

    return TRUE;
}

//..

void Window::ManageMessages(U32Array& _commands)
{
#ifdef WIN32
	MSG	 msg;
	while( PeekMessage(&msg,0,0,0,PM_REMOVE))			// Is There A Message Waiting?
	{
		if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
		{
			m_bMustKill = TRUE;
		}
		else if( msg.message==WM_COMMAND)
		{
			_commands.AddLastItem(U32(msg.wParam));
		}
		else
		{
            TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
#endif
}

//..

void Window::AddMenu( const WindowMenuRef& _Menu)
{
	m_aMenus.AddLastItem(_Menu);
}

//..

void Window::AddComponent( const WindowComponentRef& _Component)
{
	m_aComponents.AddLastItem(_Component);
}

//..

void    Window::SetPos(S32 _x, S32 _y)
{
	SetPosAndSize(_x, _y, m_Params.m_iWidth, m_Params.m_iHeight);
}

//..

void	Window::SetSize(S32 _sizex, S32 _sizey)
{
	SetPosAndSize(m_Params.m_x, m_Params.m_y, _sizex, _sizey);
}

//..

void	Window::SetPosAndSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey)
{
	m_Params.m_x = _x;
	m_Params.m_y = _y;
	m_Params.m_iWidth = _sizex;
	m_Params.m_iHeight = _sizey;

	for (U32 i = 0; i < m_aComponents.GetCount(); i++)
	{
		m_aComponents[i]->OnResize(m_Params.m_iWidth, m_Params.m_iHeight);
	}

#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	DWORD extendedStyle = 0;
	DWORD	wndStyle = 0;
	ComputeWin32Styles(m_Params, wndStyle, extendedStyle);
	RECT wndRect = ComputeWin32FullWindowRect(m_Params, wndStyle, extendedStyle, (pData->m_MenuData.m_hMenu != NULL));
	::SetWindowPos(pData->m_hWnd, NULL, wndRect.left, wndRect.top, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, SWP_NOZORDER | SWP_NOACTIVATE);
#endif
}

//..

void Window::SetTitle(const Char* _pTitle)
{
	m_Params.m_sTitle = _pTitle;
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	// use SetWindowTextW for UTF8
	::SetWindowTextA(pData->m_hWnd, m_Params.m_sTitle.GetArray());
#endif
}

//..

void Window::SetStyle(WindowStyle _style)
{
	if (m_Params.m_Style != _style)
	{
		m_Params.m_Style = _style;

#ifdef WIN32
		const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
		DWORD extendedStyle = 0;
		DWORD	wndStyle = 0;
		ComputeWin32Styles(m_Params, wndStyle, extendedStyle);
		::SetWindowLong(pData->m_hWnd, GWL_STYLE, wndStyle);
		::SetWindowLong(pData->m_hWnd, GWL_EXSTYLE, extendedStyle);
		SetPosAndSize(m_Params.m_x, m_Params.m_y, m_Params.m_iWidth, m_Params.m_iHeight);
		Show();
#endif
	}
}

//..

void    Window::Show()
{
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	::ShowWindow(pData->m_hWnd, SW_SHOW);
#endif
}

//..

void    Window::SetFocus()
{
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	::BringWindowToTop(pData->m_hWnd);
	::SetForegroundWindow(pData->m_hWnd);
	::SetFocus(pData->m_hWnd);
#endif
}

//..

Bool    Window::HasFocus() const
{
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	return (GetForegroundWindow() == pData->m_hWnd);
#endif

	return FALSE;
}

//..

Bool    Window::IsMinimized() const
{
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	return (::IsIconic(pData->m_hWnd) != 0);
#endif

	return FALSE;
}

//..

Bool    Window::HasMouseCapture() const
{
#ifdef WIN32
	const WIN32WindowPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>();
	return (::GetCapture() == pData->m_hWnd);
#endif

	return FALSE;
}

//..

void    Window::TransferMouseCapture(Window* _pTargetWindow)
{
#ifdef WIN32
	::ReleaseCapture();
	::SetCapture(_pTargetWindow->m_pPlatformData.GetDynamicCastPtr<WIN32WindowPlatformData>()->m_hWnd);
#endif
}

//..