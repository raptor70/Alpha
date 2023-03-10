#ifndef __MESH_H__
#define __MESH_H__

#include "resource/resource.h"

BEGIN_SUPER_CLASS(Mesh,Resource)
public:
	Mesh();
	virtual ~Mesh();

	static  const Char* GetResourcePrefix() { return "mesh"; }

	S32				AddSubMesh()				{ m_SubMeshs.AddLastItem(SubMesh(Str("%s_%d",GetBaseName().GetStr().GetArray(), m_SubMeshs.GetCount()),IsDynamic())); return m_SubMeshs.GetCount()-1; }
	S32				GetNbSubMesh() const		{ return m_SubMeshs.GetCount(); }
	MaterialRef&	GetMaterial(S32 _idx)		{ return m_SubMeshs[_idx].m_Material; }
	Primitive3DRef&	GetPrimitive(S32 _idx)		{ return m_SubMeshs[_idx].m_Primitive; }

	virtual	Bool IsDynamic() const { return FALSE; }
	virtual void	PostLoad() OVERRIDE;
	void Clean() { m_SubMeshs.SetEmpty(); }

protected:
	struct SubMesh
	{
		SubMesh()
		{
			m_Material = RendererDatas::GetInstance().CreateMaterial();
		}

		SubMesh(Str& _name, Bool _isdyn)
		{
			m_Primitive = Primitive3D::Create(_name.GetArray(), _isdyn);
			m_Material = RendererDatas::GetInstance().CreateMaterial();
		}
		MaterialRef			m_Material;
		Primitive3DRef		m_Primitive;
	};
	ArrayOf<SubMesh>	m_SubMeshs;
END_SUPER_CLASS

#endif