#ifndef __COMPONENT_BUTTON_H__
#define __COMPONENT_BUTTON_H__

#include "world/component.h"
#include "2d/component_quad.h"

BEGIN_COMPONENT_CLASS(Button)
DECLARE_SCRIPT_CLASS(EntityComponentButton)
public:
	EntityComponentButton();
	~EntityComponentButton();

	virtual Bool IsUpdater() const { return TRUE; }
	virtual Bool ReceiveEvent() const { return TRUE; }
	virtual Bool IsDrawer() const { return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;
	virtual void	OnEvent(const EntityEvent& _event) OVERRIDE;

	Bool	IsPressed() const { return m_bIsPressed; }
	Bool	IsJustPressed() const { return m_bIsPressed && !m_bWasPressed; }
	Bool	IsJustReleased() const { return !m_bIsPressed && m_bWasPressed; }
	Bool	IsJustValidate() const { return m_bJustValidate; }

	virtual Bool IsDefault() const { return m_bIsDefault; }
	virtual void OnPressed();
	virtual void OnRelease();
	virtual void OnValidate();

protected:
	virtual void SetDefaultStyle() const;
	virtual void SetPressedStyle() const;
	virtual void SetHoverStyle() const;

		Str			m_sDefaultTexture;
		Color		m_DefaultColor;

		Bool		m_bIsPressed;
		Bool		m_bWasPressed;
		Bool		m_bJustValidate;
		Bool		m_bValidate;
		Bool		m_bPressed;
		Bool		m_bHovered;
		Bool		m_bIsDefault;
END_COMPONENT_CLASS


#endif
