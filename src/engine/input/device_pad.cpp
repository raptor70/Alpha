#include "device_pad.h"

#include "device_pad_key.h"

#ifdef WIN32
#include "platform/platform_win32.h"
#include <Xinput.h>
#endif

DevicePad::DevicePad(S32 _id):SUPER()
{
	m_Id = _id;
}

DevicePad::~DevicePad()
 {
 }

Bool DevicePad::Initialize()
{
	m_KeyTable = 0;
	m_PreviousKeyTable = 0;
	return TRUE;
}

void DevicePad::Update( Float _dTime, const WindowComponent & window )
{
	m_PreviousKeyTable = m_KeyTable;
	m_KeyTable = 0;

#ifdef WIN32
	XINPUT_STATE state;
	if (XInputGetState(m_Id,&state) == ERROR_SUCCESS)
	{
		m_KeyTable = 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? PADKEY_Dpad_Up : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? PADKEY_Dpad_Down : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? PADKEY_Dpad_Left : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? PADKEY_Dpad_Right : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? PADKEY_Start : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? PADKEY_Select : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? PADKEY_LeftTrigger : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? PADKEY_RightTrigger : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? PADKEY_LeftButton : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? PADKEY_RightButton : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? PADKEY_A : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? PADKEY_B : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? PADKEY_X : 0;
		m_KeyTable += (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? PADKEY_Y : 0;
	}
#endif
}

void DevicePad::Finalize()
{
}