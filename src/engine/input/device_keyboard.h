#ifndef __DEVICE_KEYBOARD_H__
#define __DEVICE_KEYBOARD_H__

#include "device.h"
#include "device_key.h"

BEGIN_SUPER_CLASS(DeviceKeyboard,Device)
public:
	DeviceKeyboard();
	virtual ~DeviceKeyboard();
		
	virtual Bool Initialize();
	virtual void Update( Float _dTime, const WindowComponent & window );
	virtual void Finalize();

	const Bool IsPressed( KEY _key ) const { return (m_KeyTable[_key]); }
	const Bool IsJustPressed( KEY _key ) const 
	{ 
		return (m_KeyTable[_key]) && !(m_PreviousKeyTable[_key]); 
	}
	const Bool IsReleased( KEY _key ) const { return (!m_KeyTable[_key]); }
	const Bool IsJustReleased( KEY _key ) const { return (!m_KeyTable[_key]) && (m_PreviousKeyTable[_key]); }
	
	void	SetKeyState(KEY _key, Bool _pressed)	
	{ 
		m_NextKeyTable[_key] = _pressed; 
	}

protected:
	Bool		m_KeyTable[256];
	Bool		m_PreviousKeyTable[256];
	Bool		m_NextKeyTable[256];

END_SUPER_CLASS

#endif