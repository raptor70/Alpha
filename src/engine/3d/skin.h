#ifndef __SKIN_H__
#define __SKIN_H__

#include "mesh.h"
#include "skel.h"

BEGIN_SUPER_CLASS(CSkin,Mesh)
public:
	CSkin();
	virtual ~CSkin();

	static  const Char* GetResourcePrefix() { return "skin"; }

	CSkel& GetSkel() {return m_Skel;}

	void	AddVertexMapping(U32 _submeshId, U32 _ibIdx, U32 _vertexIdx);
	void	AddSkinnedVertex(U32 _submeshId, U32 _vertexId, const Vec3f& _pos, const Name& _bone, Float _weight);
	void	SetBindShapeMatrix(const Mat44& _mat) { m_BindShapeMatrix = _mat; }
	void	ComputeSkin();

	virtual	Bool IsDynamic() const OVERRIDE { return TRUE; }

protected:
	CSkel			m_Skel;

	struct	SSkinnedVertex
	{
		Vec3f m_vPos;

		struct SBones
		{
			Name	m_Name;
			Float	m_Weight;
		};
		ArrayOf<SBones>	m_Bones;	
	};
	ArrayOf< ArrayOf<SSkinnedVertex> >	m_SkinnedVertices;	// vertices per submesh
	ArrayOf<U32Array>					m_VertexMapping;	// size of index buffer
	Mat44								m_BindShapeMatrix;
END_SUPER_CLASS

#endif