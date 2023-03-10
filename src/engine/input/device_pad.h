#ifndef __DEVICE_PAD_H__
#define __DEVICE_PAD_H__

#include "device.h"
#include "device_pad_key.h"

BEGIN_SUPER_CLASS(DevicePad,Device)
public:
	DevicePad(S32 _id);
	virtual ~DevicePad();
		
	virtual Bool Initialize();
	virtual void Update( Float _dTime, const WindowComponent & window );
	virtual void Finalize();

	const Bool IsPressed( PADKEY _key ) const { return ((m_KeyTable&_key) != 0); }
	const Bool IsJustPressed( PADKEY _key ) const { return ((m_KeyTable&_key) != 0) && ((m_PreviousKeyTable&_key) == 0);  }
	const Bool IsReleased( PADKEY _key ) const { return ((m_KeyTable& _key) == 0); }
	const Bool IsJustReleased( PADKEY _key ) const { return ((m_KeyTable&_key) == 0) && ((m_PreviousKeyTable&_key) != 0); }

protected:
	S32			m_Id;
	U16			m_KeyTable;
	U16			m_PreviousKeyTable;

END_SUPER_CLASS

#endif