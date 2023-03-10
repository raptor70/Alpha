#include "component_camera.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentCamera,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Camera)
	INIT_ENTITY_PARAMS(Bool, IsOrtho, FALSE);
	INIT_ENTITY_PARAMS(Float, Near, 0.01f);
	INIT_ENTITY_PARAMS(Float, Far, 1000.f);
	INIT_ENTITY_PARAMS(Float, OrthoY, 20.f);
	INIT_ENTITY_PARAMS(Float, Fov, MATH_PI_DIV_4);
END_DEFINE_COMPONENT

EntityComponentCamera::EntityComponentCamera()
{
}

//------------------------------

EntityComponentCamera::~EntityComponentCamera()
{
}

//------------------------------

void	EntityComponentCamera::Initialize()
{
	SUPER::Initialize();

	m_pRendererCamera = NEW RendererCamera;
}

//------------------------------

void	EntityComponentCamera::OnParamChanged()
{
	m_pRendererCamera->SetNearAndFar(GET_ENTITY_PARAMS(Float, CameraNear), GET_ENTITY_PARAMS(Float, CameraFar));

	if (GET_ENTITY_PARAMS(Bool, CameraIsOrtho))
	{
		m_pRendererCamera->SetOrtho(GET_ENTITY_PARAMS(Float, CameraOrthoY));
	}
	else
	{
		m_pRendererCamera->SetPerspective(GET_ENTITY_PARAMS(Float, CameraFov));
	}
}

//------------------------------

void	EntityComponentCamera::Update(Float _dTime) 
{
	// X left
	// Y up
	// Z Front
	m_pRendererCamera->SetPosAndDir(GetEntity()->GetWorldPos(),GetEntity()->GetWorldRot() * Vec3f::ZAxis);
}