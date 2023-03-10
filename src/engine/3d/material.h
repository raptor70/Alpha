#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "texture.h"
#include "renderer/shader_data.h"

class Material
{
	DECLARE_SCRIPT_CLASS(Material)
public:
	Material();
	virtual ~Material(void);
	
	void SetDiffuseColor(const Color& color)	{ m_DiffuseColor =color; }
	const Color& GetDiffuseColor() const		{return m_DiffuseColor; }

	void SetEmissiveColor(const Color& color) { m_EmissiveColor = color; }
	const Color& GetEmissiveColor() const { return m_EmissiveColor; }

	void LoadTexture(const Str& _path);
	void SetTexture(const ResourceRef& _texture)	{ m_texture = _texture;}
	Texture* GetTexture() const	{ return (Texture*)m_texture.GetPtr(); }
	const Vec2f& GetUvMin() const	{ return m_vUvMin; }
	const Vec2f& GetUvMax() const	{ return m_vUvMax; }

	void LoadEmissiveTexture(const Str& _path);
	void SetEmissiveTexture(const ResourceRef& _texture) { m_EmissiveTexture = _texture; }
	Texture* GetEmissiveTexture() const { return (Texture*)m_EmissiveTexture.GetPtr(); }

	void SetIsFont(Bool _isFont) { m_bIsFont = _isFont; }
	Bool IsFont() const { return m_bIsFont; }

	void SetIsAdditive(Bool _isAdditive)	{ m_bIsAdditive = _isAdditive; }
	Bool IsAdditive() const					{ return m_bIsAdditive; }
	Bool IsTransparent() const				{ return FALSE /*TODO FLAG*/; }

	void	UpdateShaderData();
	RendererMaterialDataRef		GetShaderData() const { return m_pShaderData; }

private:
	Color			m_DiffuseColor;
	Color			m_EmissiveColor;
	Vec2f			m_vUvMin;
	Vec2f			m_vUvMax;
	ResourceRef		m_texture;
	ResourceRef		m_EmissiveTexture;
	RendererMaterialDataRef	m_pShaderData;
	Bool			m_bIsAdditive;
	Bool			m_bIsFont;

};

typedef RefTo<Material> MaterialRef;
typedef ArrayOf<MaterialRef> MaterialRefArray;

#endif