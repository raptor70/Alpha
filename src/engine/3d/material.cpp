#include "material.h"

#include "3d/texture_manager.h"

BEGIN_SCRIPT_CLASS(Material)
	SCRIPT_VOID_METHOD1(SetDiffuseColor,Color)
	SCRIPT_VOID_METHOD1(SetIsAdditive,Bool)
	SCRIPT_VOID_METHOD1(LoadTexture,Str)
END_SCRIPT_CLASS

//..

Material::Material()
{
	m_DiffuseColor = Color::White;
	m_EmissiveColor = Color::Black;
	m_bIsAdditive = FALSE;
	m_vUvMin = Vec2f::Zero;
	m_vUvMax = Vec2f::One;
	m_bIsFont = FALSE;

	Str name("MaterialData_%x", this);
	m_pShaderData = ResourceManager::GetInstance().GetVirtualResource<RendererMaterialData>(name, TRUE);
}

//..

Material::~Material(void)
{
}

//..

void Material::LoadTexture(const Str& _path)
{
	if(m_texture.IsNull() || !(m_texture->GetPath() == TextureManager::GetInstance().GetRealTexturePath(_path)))
		TextureManager::GetInstance().GetTexture(_path,m_texture,m_vUvMin,m_vUvMax);
}

//..

void Material::LoadEmissiveTexture(const Str& _path)
{
	if (m_EmissiveTexture.IsNull() || !(m_EmissiveTexture->GetPath() == TextureManager::GetInstance().GetRealTexturePath(_path)))
		TextureManager::GetInstance().GetTexture(_path, m_EmissiveTexture, m_vUvMin, m_vUvMax);
}

//..

void	Material::UpdateShaderData()
{
	m_pShaderData->UpdateData([&](auto& _data)
		{
			_data.m_vEmissiveColor = Vec4f(m_EmissiveColor.r, m_EmissiveColor.g, m_EmissiveColor.b, m_EmissiveColor.a);
			_data.m_bIsFont = m_bIsFont;
		});
}