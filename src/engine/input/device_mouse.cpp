#include "device_mouse.h"

#include "input_manager.h"
#include "window/window_3dscreen.h"
#include "platform/platform_win32.h"
#include "renderer/renderer.h"

DeviceMouse::DeviceMouse():SUPER()
{
	m_Pos = Vec2i::Zero;
	m_NextPos = Vec2i::Zero;
	m_vPosRatio = Vec2f::Zero;
	m_WorldSegment = Segment(Vec3f::Zero,Vec3f::ZAxis);
	m_bChanged = FALSE;
	m_fWheelDelta = 0.f;
	m_fNextWheelDelta = 0.f;
}

 DeviceMouse::~DeviceMouse()
 {
 }

Bool DeviceMouse::Initialize()
{
	SUPER::Initialize();
	return TRUE;
}

void DeviceMouse::Update( Float _dTime, const WindowComponent & window)
{
	SUPER::Update(_dTime,window);
	
#ifdef IOS
	if( !m_bChanged )
		return;
	m_bChanged = FALSE;
#endif
	
	Window3DScreen* pScreen = (Window3DScreen*)&window;
#if defined(WIN32)
	if (!pScreen->UseEditor())
	{
		POINT point;
		GetCursorPos(&point);
		m_NextScreenPos.x = point.x;
		m_NextScreenPos.y = point.y;
		ScreenToClient(window.GetPlatformData().GetDynamicCastPtr<WIN32ComponentPlatformData>()->m_hWnd, &point);
		m_NextPos.x = point.x;
		m_NextPos.y = point.y;
	}
#endif

	// set wheel
	m_fWheelDelta = m_fNextWheelDelta;
	m_fNextWheelDelta = 0.f;

	// set pos
	m_Pos = m_NextPos;
	m_ScreenPos = m_NextScreenPos;

	// compute world segment
	Vec3f cameraWorld = pScreen->GetRenderer()->GetCameraInvViewMatrix() * Vec3f::Zero;
	Vec3f worldDir = 2.f * Vec3f((Float)m_Pos.x/(Float)pScreen->GetWidth(),(Float)m_Pos.y/(Float)pScreen->GetHeight(),0.5f) - Vec3f::One;		
	worldDir.y = -worldDir.y;
	worldDir = pScreen->GetRenderer()->GetCameraInvViewProjMatrix() * worldDir - cameraWorld;
	worldDir.Normalize();
	m_WorldSegment = Segment(cameraWorld,worldDir,1e7f);

	// compute ratio
	Vec2f newRatio  = Vec2f((Float)m_Pos.x/(Float)pScreen->GetWidth(),(Float)m_Pos.y/(Float)pScreen->GetHeight());
	m_vPosRatioDelta = newRatio - m_vPosRatio;
	m_vPosRatio = newRatio;

	// reset button if out of screen
	// commented for imgui multiviewport
	/*if( m_vPosRatio.x < 0.f  || m_vPosRatio.x > 1.f
		|| m_vPosRatio.y < 0.f  || m_vPosRatio.y > 1.f )
	{
		SetKeyState(MOUSE_Left,FALSE);
		SetKeyState(MOUSE_Middle,FALSE);
		SetKeyState(MOUSE_Right,FALSE);
	}*/

	//LOGGER_Log("%s %d %d\n",m_KeyTable[MOUSE_Left]?"PRESSED":"RELEASED",m_Pos.x,m_Pos.y);
	//DEBUG_DrawSphere(m_WorldSegment.m_vOrigin+ m_WorldSegment.m_vDirection, 0.005f,Color::Red);
}

void	DeviceMouse::UpdateValue( Vec2i _pos )
{
	m_NextPos = _pos;
	m_bChanged = TRUE;
}

void DeviceMouse::Finalize()
{
	SUPER::Finalize();
}

void DeviceMouse::SetCursor(const DeviceMouseCursor& _cursor)
{
#ifdef WIN32
	LPTSTR win32_cursor = IDC_ARROW;
	switch (_cursor)
	{
	case DeviceMouseCursor_None:		::SetCursor(NULL); return;
	case DeviceMouseCursor_Arrow:       win32_cursor = IDC_ARROW; break;
	case DeviceMouseCursor_TextInput:   win32_cursor = IDC_IBEAM; break;
	case DeviceMouseCursor_ResizeAll:   win32_cursor = IDC_SIZEALL; break;
	case DeviceMouseCursor_ResizeEW:    win32_cursor = IDC_SIZEWE; break;
	case DeviceMouseCursor_ResizeNS:    win32_cursor = IDC_SIZENS; break;
	case DeviceMouseCursor_ResizeNESW:  win32_cursor = IDC_SIZENESW; break;
	case DeviceMouseCursor_ResizeNWSE:  win32_cursor = IDC_SIZENWSE; break;
	case DeviceMouseCursor_Hand:        win32_cursor = IDC_HAND; break;
	case DeviceMouseCursor_NotAllowed:  win32_cursor = IDC_NO; break;	
	}	
	::SetCursor(::LoadCursor(NULL, win32_cursor));
#endif
}