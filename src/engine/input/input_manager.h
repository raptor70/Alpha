#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "device.h"
#include "device_key.h"
#include "device_keyboard.h"
#include "device_mouse.h"
#include "device_pad.h"

#if defined(IOS) ||defined(ANDROID)
#define MAX_MULTITOUCH_COUNT	3
#else
#define MAX_MULTITOUCH_COUNT	1
#endif

#ifdef WIN32
#define MAX_PAD_COUNT	1
#else
#define MAX_PAD_COUNT	0
#endif

class InputManager
{
	SINGLETON_Declare( InputManager );

public:
	InputManager();
	virtual ~InputManager(){};

	Bool Initialize();
	void Update( Float _dTime, const WindowComponent & window );
	void Finalize( void );

	DeviceKeyboard& GetKeyboard() const { return *m_pKeyboard; }
	DeviceMouse& GetMouse(S32 _id = 0) const { return *(m_pMouse[_id]); }
	Bool HasPad(S32 _id = 0)	const { return m_pPad[_id] != NULL; }
	DevicePad& GetPad(S32 _id = 0) const { return *(m_pPad[_id]); }

	Bool GetAllMouseJustPressed(KEY _key, S32Array& _outIds);
	void SetOn2D(Bool _isOn2D) { m_bIsOn2D = _isOn2D; }
	Bool IsOn2D() const { return m_bIsOn2D;  }

private:
	DeviceRefArray		m_DeviceTable;
	DeviceKeyboard*	m_pKeyboard;
	DeviceMouse*		m_pMouse[MAX_MULTITOUCH_COUNT];
	DevicePad*			m_pPad[MAX_PAD_COUNT];
	Bool				m_bIsOn2D;
};

#endif