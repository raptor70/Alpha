#include "input_manager.h"

#include "device.h"
#include "device_keyboard.h"

SINGLETON_Define( InputManager );

InputManager::InputManager()
{
	m_pKeyboard = NULL;
	for(S32 i=0; i<MAX_MULTITOUCH_COUNT; i++)
		m_pMouse[i] = NULL;
	for (S32 i = 0; i < MAX_PAD_COUNT; i++)
		m_pPad[i] = NULL;
}

Bool InputManager::Initialize()
{
#ifdef WIN32
	m_pKeyboard = NEW DeviceKeyboard();
	m_DeviceTable.AddLastItem(m_pKeyboard);
#endif

	for(S32 i=0; i<MAX_MULTITOUCH_COUNT; i++)
	{
		m_pMouse[i] = NEW DeviceMouse();
		m_DeviceTable.AddLastItem(m_pMouse[i]);
	}

	for(S32 i = 0; i < MAX_PAD_COUNT; i++)
	{
			m_pPad[i] = NEW DevicePad(i);
			m_DeviceTable.AddLastItem(m_pPad[i]);
	}

	for( U32 i = 0; i < m_DeviceTable.GetCount(); ++i )
	{
		m_DeviceTable[ i ]->Initialize();
	}

	m_bIsOn2D = FALSE;

	return TRUE;
}

void InputManager::Update(Float _dTime, const WindowComponent & window)
{
	for( U32 i = 0; i < m_DeviceTable.GetCount(); ++i )
	{
		m_DeviceTable[ i ]->Update(_dTime,window);
	}
}

void InputManager::Finalize()
{
	for( U32 i = 0; i < m_DeviceTable.GetCount(); ++i )
	{
		m_DeviceTable[ i ]->Finalize();
	}
}

Bool InputManager::GetAllMouseJustPressed(KEY _key, S32Array& _outIds)
{
	Bool found = FALSE;
	for(S32 i=0; i<MAX_MULTITOUCH_COUNT; i++)
	{
		if( m_pMouse[i]->IsJustPressed(_key) )
		{
			_outIds.AddLastItem(i);
			found = TRUE; 
		}
	}

	return found;
}