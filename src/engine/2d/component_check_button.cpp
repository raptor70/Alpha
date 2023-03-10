#include "component_check_button.h"

#include "input/input_manager.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentCheckButton,EntityComponentButton)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS_INHERITED(CheckButton,Button)
	INIT_ENTITY_PARAMS(Bool, Checked, FALSE);
	INIT_ENTITY_PARAMS(Color, CheckedColor, Color::White);
	INIT_ENTITY_PARAMS(Str, CheckedTexture, DEFAULT_TEXTURE_PATH);
	INIT_ENTITY_PARAMS(Color, CheckedHoverColor, Color::White);
	INIT_ENTITY_PARAMS(Str, CheckedHoverTexture, DEFAULT_TEXTURE_PATH);
	INIT_ENTITY_PARAMS(Color, CheckedPressedColor, Color::White);
	INIT_ENTITY_PARAMS(Str, CheckedPressedTexture, DEFAULT_TEXTURE_PATH);
	EDITOR_CATEGORY(2D)
END_DEFINE_COMPONENT

//------------------------------

EntityComponentCheckButton::EntityComponentCheckButton():SUPER()
{
}

//------------------------------

EntityComponentCheckButton::~EntityComponentCheckButton()
{
}

//------------------------------

void	EntityComponentCheckButton::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void EntityComponentCheckButton::OnValidate()
{ 
	SUPER::OnValidate();

	SET_ENTITY_PARAMS(Bool, CheckButtonChecked, !GET_ENTITY_PARAMS(Bool, CheckButtonChecked)); 
}

//------------------------------

void EntityComponentCheckButton::SetDefaultStyle() const
{
	if (GET_ENTITY_PARAMS(Bool, CheckButtonChecked))
	{
		SET_ENTITY_PARAMS(Color, QuadColor, GET_ENTITY_PARAMS(Color, CheckButtonCheckedColor));
		SET_ENTITY_PARAMS(Str, QuadTexture, GET_ENTITY_PARAMS(Str, CheckButtonCheckedTexture));
	}
	else
	{
		SUPER::SetDefaultStyle();
	}
}

//------------------------------

void EntityComponentCheckButton::SetPressedStyle() const
{
	if (GET_ENTITY_PARAMS(Bool, CheckButtonChecked))
	{
		SET_ENTITY_PARAMS(Color, QuadColor, GET_ENTITY_PARAMS(Color, CheckButtonCheckedPressedColor));
		SET_ENTITY_PARAMS(Str, QuadTexture, GET_ENTITY_PARAMS(Str, CheckButtonCheckedPressedTexture));
	}
	else
	{
		SUPER::SetPressedStyle();
	}
}

//------------------------------

void EntityComponentCheckButton::SetHoverStyle() const
{
	if (GET_ENTITY_PARAMS(Bool, CheckButtonChecked))
	{
		SET_ENTITY_PARAMS(Color, QuadColor, GET_ENTITY_PARAMS(Color, CheckButtonCheckedHoverColor));
		SET_ENTITY_PARAMS(Str, QuadTexture, GET_ENTITY_PARAMS(Str, CheckButtonCheckedHoverTexture));
	}
	else
	{
		SUPER::SetHoverStyle();
	}
}