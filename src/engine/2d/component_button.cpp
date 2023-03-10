#include "component_button.h"

#include "input/input_manager.h"
#include "core/name_static.h"
#include "application.h"
#include "script/flow_script_manager.h"
#include "2d/component_text.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentButton, EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Button)
	INIT_ENTITY_PARAMS(Color, HoverColor, Color::White);
	INIT_ENTITY_PARAMS(Str, HoverTexture, DEFAULT_TEXTURE_PATH);
	INIT_ENTITY_PARAMS(Color, PressedColor, Color::White);	
	INIT_ENTITY_PARAMS(Str, PressedTexture, DEFAULT_TEXTURE_PATH);
	INIT_ENTITY_PARAMS(Str, Event, "EVENT");
	INIT_ENTITY_PARAMS(Str, PressedEvent, "");
	INIT_ENTITY_PARAMS(Str, JustPressedSound, "");
	INIT_ENTITY_PARAMS(Str, JustReleasedSound, "");
	EDITOR_CATEGORY(2D)
	EDITOR_SOUND_VALUE(JustPressedSound);
	EDITOR_SOUND_VALUE(JustReleasedSound);
	EDITOR_NEED_COMPONENT(InputClick);
	EDITOR_NEED_COMPONENT(Quad);
	EDITOR_FILE_VALUE(HoverTexture, Texture);
	EDITOR_FILE_VALUE(PressedTexture, Texture);
END_DEFINE_COMPONENT

EntityComponentButton::EntityComponentButton() :SUPER()
{

}

//------------------------------

EntityComponentButton::~EntityComponentButton()
{
}

//------------------------------

void	EntityComponentButton::Initialize()
{
	SUPER::Initialize();

	SET_ENTITY_PARAMS(Bool, InputClick2D, TRUE);

	m_bIsPressed = FALSE;
	m_bPressed = FALSE;
	m_bWasPressed = FALSE;
	m_bValidate = FALSE;
	m_bHovered = FALSE;
	m_bIsDefault = TRUE;
}

//------------------------------

void	EntityComponentButton::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	if (IsDefault())
	{
		m_sDefaultTexture = GET_ENTITY_PARAMS(Str, QuadTexture);
		m_DefaultColor = GET_ENTITY_PARAMS(Color, QuadColor);
	}

	m_bWasPressed = m_bIsPressed;
	m_bIsPressed = m_bPressed;
	m_bJustValidate = m_bValidate;
	m_bValidate = FALSE;

	
	EntityComponentQuad* pQuad = GetEntity()->GetComponent<EntityComponentQuad>();
	Vec2f size = pQuad ? pQuad->GetSize() : Vec2f::Zero;
	GetEntity()->SetBBox(Box(Vec3f(size.x, size.y, 0.01f), GetEntity()->GetWorldPos()));

	Name wantedStyle = Name::Null;
	Bool bWasDefault = m_bIsDefault;
	if (m_bIsPressed)
	{
		SetPressedStyle();
		m_bIsDefault = FALSE;
	}
	else if (m_bHovered)
	{
		SetHoverStyle();
		m_bIsDefault = FALSE;
	}
	else
	{
		SetDefaultStyle();
		m_bIsDefault = TRUE;
	}
}

//------------------------------

void	EntityComponentButton::OnEvent(const EntityEvent& _event)
{
	if (_event.m_nName == NAME(ON_PRESS))
	{
		if (!m_bPressed)
		{
			m_bPressed = TRUE;
			OnPressed();
		}
	}
	else if (_event.m_nName == NAME(ON_RELEASE))
	{
		if (m_bPressed)
		{
			m_bPressed = FALSE;
			OnRelease();
		}
	}
	else if (_event.m_nName == NAME(ON_VALIDATE))
	{
		if (m_bPressed)
		{
			m_bPressed = FALSE;
			m_bValidate = TRUE;
			OnValidate();
		}
	}
	else if (_event.m_nName == NAME(ON_HOVER))
	{
		m_bHovered = TRUE;
	}
	else if (_event.m_nName == NAME(ON_END_HOVER))
	{
		m_bHovered = FALSE;
	}
}

//------------------------------

void EntityComponentButton::OnPressed()
{
	SoundManager::GetInstance().PlaySound(GET_ENTITY_PARAMS(Str, ButtonJustPressedSound));
	const Str& pressEvent = GET_ENTITY_PARAMS(Str, ButtonPressedEvent);
	if (pressEvent.GetLength())
		Application::GetInstance().GetFlowScriptManager()->LaunchEvent(pressEvent);
}

//------------------------------

void EntityComponentButton::OnRelease()
{
}

//------------------------------

void EntityComponentButton::OnValidate()
{
	Application::GetInstance().GetFlowScriptManager()->LaunchEvent(GET_ENTITY_PARAMS(Str, ButtonEvent));
	SoundManager::GetInstance().PlaySound(GET_ENTITY_PARAMS(Str, ButtonJustReleasedSound));
}

//------------------------------

void EntityComponentButton::SetDefaultStyle() const
{
	SET_ENTITY_PARAMS(Color, QuadColor, m_DefaultColor);
	SET_ENTITY_PARAMS(Str, QuadTexture, m_sDefaultTexture);
}

//------------------------------

void EntityComponentButton::SetPressedStyle() const
{
	SET_ENTITY_PARAMS(Color, QuadColor, GET_ENTITY_PARAMS(Color, ButtonPressedColor));
	SET_ENTITY_PARAMS(Str, QuadTexture, GET_ENTITY_PARAMS(Str, ButtonPressedTexture));
}

//------------------------------

void EntityComponentButton::SetHoverStyle() const
{
	SET_ENTITY_PARAMS(Color, QuadColor, GET_ENTITY_PARAMS(Color, ButtonHoverColor));
	SET_ENTITY_PARAMS(Str, QuadTexture, GET_ENTITY_PARAMS(Str, ButtonHoverTexture));
}
