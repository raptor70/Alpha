#ifndef __DEVICE_MOUSE_H__
#define __DEVICE_MOUSE_H__

#include "device.h"
#include "device_keyboard.h"

enum DeviceMouseCursor
{
	DeviceMouseCursor_None = -1,
	DeviceMouseCursor_Arrow = 0,
	DeviceMouseCursor_TextInput,    
	DeviceMouseCursor_ResizeAll,    
	DeviceMouseCursor_ResizeNS,     
	DeviceMouseCursor_ResizeEW,     
	DeviceMouseCursor_ResizeNESW,   
	DeviceMouseCursor_ResizeNWSE,   
	DeviceMouseCursor_Hand,         
	DeviceMouseCursor_NotAllowed,   
	DeviceMouseCursor_COUNT
};

BEGIN_SUPER_CLASS(DeviceMouse,DeviceKeyboard)
public:
	DeviceMouse();
	virtual ~DeviceMouse();
		
	virtual Bool Initialize();
	virtual void Update( Float _dTime, const WindowComponent & window);
	virtual void Finalize();

	const Vec2i&   GetPos() const		{ return m_Pos; }
	const Vec2i&   GetScreenPos() const		{ return m_ScreenPos; }
	const Vec2f&   GetPosRatio() const		{ return m_vPosRatio; }
	const Vec2f&   GetPosRatioDelta() const		{ return m_vPosRatioDelta; }
	const Segment&   GetWorldSegment() const		{ return m_WorldSegment; }

	void	UpdateValue( Vec2i _pos );
	void	SetWheelDelta(Float _delta)	{ m_fNextWheelDelta = _delta; }
	Float	GetWheelDelta() const		{ return m_fWheelDelta; }
	
	Bool IsInside() const { return m_vPosRatio.x >= 0.f && m_vPosRatio.y >= 0.f && m_vPosRatio.x <= 1.f && m_vPosRatio.y <= 1.f; }

	void SetCursor(const DeviceMouseCursor& _cursor);
protected:
	KEY		GetKeyFromId(S32 _id) const;

	Vec2i	m_Pos;
	Vec2i	m_ScreenPos;
	Vec2f	m_vPosRatio;
	Vec2f	m_vPosRatioDelta;
	Vec2i	m_NextScreenPos;
	Vec2i	m_NextPos;
	Segment	m_WorldSegment;
	Float m_fWheelDelta;
	Float m_fNextWheelDelta;
	Bool	m_bNextDownLeft;
	Bool	m_bChanged;
END_SUPER_CLASS

#endif