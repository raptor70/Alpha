#ifndef __COMPONENT_CHECK_BUTTON_H__
#define __COMPONENT_CHECK_BUTTON_H__

#include "2d/component_button.h"

BEGIN_COMPONENT_CLASS_INHERITED(CheckButton,Button)
	DECLARE_SCRIPT_CLASS(EntityComponentCheckButton)
public:
	EntityComponentCheckButton();
	~EntityComponentCheckButton();

	virtual void	Initialize() OVERRIDE;

	virtual void OnValidate() OVERRIDE;

	virtual Bool IsDefault() const OVERRIDE  { return SUPER::IsDefault() && !GET_ENTITY_PARAMS(Bool, CheckButtonChecked); }
	
protected:
	virtual void SetDefaultStyle() const OVERRIDE;
	virtual void SetPressedStyle() const OVERRIDE;
	virtual void SetHoverStyle() const OVERRIDE;

END_COMPONENT_CLASS

#endif
