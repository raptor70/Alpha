#include "mesh.h"

Mesh::Mesh()
{
}

//----------------------------------

Mesh::~Mesh()
{
}

//----------------------------------

void	Mesh::PostLoad()
{
	SUPER::PostLoad();

	for (U32 i = 0; i < m_SubMeshs.GetCount(); i++)
	{
		// to force reload of the texture
		Texture* pTexture = m_SubMeshs[i].m_Material->GetTexture();
		if (pTexture)
		{
			Str path = pTexture->GetPath();
			Texture::EWrapping wrap = pTexture->GetWrapping();
			m_SubMeshs[i].m_Material->SetTexture(NULL);
			m_SubMeshs[i].m_Material->LoadTexture(path);
			if (m_SubMeshs[i].m_Material->GetTexture())
				m_SubMeshs[i].m_Material->GetTexture()->SetWrapping(wrap);
		}

		// to force reload of the texture
		pTexture = m_SubMeshs[i].m_Material->GetEmissiveTexture();
		if (pTexture)
		{
			Str path = pTexture->GetPath();
			Texture::EWrapping wrap = pTexture->GetWrapping();
			m_SubMeshs[i].m_Material->SetEmissiveTexture(NULL);
			m_SubMeshs[i].m_Material->LoadEmissiveTexture(path);
			if (m_SubMeshs[i].m_Material->GetEmissiveTexture())
				m_SubMeshs[i].m_Material->GetEmissiveTexture()->SetWrapping(wrap);
		}
	}
}