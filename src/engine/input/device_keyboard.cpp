#include "device_keyboard.h"

DeviceKeyboard::DeviceKeyboard():SUPER()
{
}

 DeviceKeyboard::~DeviceKeyboard()
 {
 }

Bool DeviceKeyboard::Initialize()
{
	memset(m_KeyTable,0,sizeof(Bool)*256);
	memset(m_PreviousKeyTable,0,sizeof(Bool)*256);
	memset(m_NextKeyTable,0,sizeof(Bool)*256);
	return TRUE;
}

void DeviceKeyboard::Update( Float _dTime, const WindowComponent & window )
{
	for(S32 i=0; i<256; i++)
	{
		m_PreviousKeyTable[i] = m_KeyTable[i];
		m_KeyTable[i] = m_NextKeyTable[i];
	}

	// Reset input on lost focus
	/*Bool hasFocus = Application::GetInstance().HasFocus();
	if( !hasFocus )
	{
		memset(m_KeyTable,0,sizeof(Bool)*256);
	}*/
}

void DeviceKeyboard::Finalize()
{
}