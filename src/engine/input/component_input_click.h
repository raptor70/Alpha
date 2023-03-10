#ifndef __COMPONENT_INPUT_CLICK_H__
#define __COMPONENT_INPUT_CLICK_H__

#include "world/component.h"
#include "2d/component_quad.h"
#include "input/device_key.h"

BEGIN_COMPONENT_CLASS(InputClick)
	DECLARE_SCRIPT_CLASS(EntityComponentInputClick)
public:
	EntityComponentInputClick();
	~EntityComponentInputClick();

	virtual Bool IsUpdater() const OVERRIDE { return TRUE; }
	virtual Bool IsDrawer() const OVERRIDE { return FALSE; }
	virtual Bool ReceiveEvent()	const OVERRIDE { return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;

	Bool	Is2D() const { return GET_ENTITY_PARAMS(Bool, InputClick2D); }
	Bool	ComputeIsIn();

	S32 GetTouchId() const	{ return m_iTouchId; }
	const Vec3f& GetTouchPos() const {return m_vTouchPos; }

	KEY GetTouchKey() const { return m_TouchKey; }

protected:
	S32			m_iTouchId; 
	Vec3f		m_vTouchPos;
	KEY			m_TouchKey;
	Bool		m_bIsIn;
	S32Array	m_IsInIndex;
	Vec3fArray	m_IsInPos;

END_COMPONENT_CLASS

#endif
