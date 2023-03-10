#include "component_omni.h"

#define DEFAULT_MESH_PATH ""

DEFINE_COMPONENT_CLASS(Omni)
	INIT_ENTITY_PARAMS(Float, Intensity, 1.f);
END_DEFINE_COMPONENT

EntityComponentOmni::EntityComponentOmni()
{
	Str name("OmniData_%x", this);
	m_pShaderData = ResourceManager::GetInstance().GetVirtualResource<RendererOmniData>(name, TRUE);
}

//------------------------------

EntityComponentOmni::~EntityComponentOmni()
{
}

//------------------------------

void	EntityComponentOmni::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void EntityComponentOmni::Draw(Renderer*	_renderer)
{
	//Vec3f worldPos = GetEntity()->GetWorldPos();
	Float worldRadius = GetEntity()->GetWorldScale().x;
	//Vec4f color = GET_ENTITY_PARAMS(Color, Color).vec4 * GET_ENTITY_PARAMS(Float, Intensity);
	m_pShaderData->UpdateData([&](auto& _data)
		{
			/*_data.m_vEmissiveColor = Vec4f(m_EmissiveColor.r, m_EmissiveColor.g, m_EmissiveColor.b, m_EmissiveColor.a);
			_data.m_bIsFont = m_bIsFont;*/
		});
	_renderer->DrawOmni(GetEntity()->GetWorldPos(), worldRadius, GetEntity()->GetColor(), m_pShaderData);
}
