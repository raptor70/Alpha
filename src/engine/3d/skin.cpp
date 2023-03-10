#include "skin.h"

CSkin::CSkin()
{
}

//----------------------------------

CSkin::~CSkin()
{
}

//----------------------------------

void	CSkin::AddVertexMapping(U32 _submeshId, U32 _ibIdx, U32 _vertexIdx)
{
	while( m_VertexMapping.GetCount() <= _submeshId )
	{
		m_VertexMapping.AddLastItem(U32Array());
		DEBUG_Require(m_VertexMapping.GetCount() <= m_SubMeshs.GetCount());
	}

	if( m_VertexMapping[_submeshId].GetCount() <= _ibIdx )
	{
		DEBUG_Require(m_VertexMapping[_submeshId].GetCount() == _ibIdx);
		m_VertexMapping[_submeshId].AddLastItem(_vertexIdx);
	}
	else
	{
		DEBUG_Require(m_VertexMapping[_submeshId][_ibIdx]== _vertexIdx);
	}
}

//----------------------------------

void	CSkin::AddSkinnedVertex(U32 _submeshId, U32 _vertexId, const Vec3f& _pos, const Name& _bone, Float _weight)
{
	while( m_SkinnedVertices.GetCount() <= _submeshId )
	{
		m_SkinnedVertices.AddLastItem(ArrayOf<SSkinnedVertex>());
		DEBUG_Require(m_SkinnedVertices.GetCount() <= m_SubMeshs.GetCount());
	}

	if( m_SkinnedVertices[_submeshId].GetCount() <= _vertexId )
	{
		DEBUG_Require(m_SkinnedVertices[_submeshId].GetCount() == _vertexId);
		m_SkinnedVertices[_submeshId].AddLastItem(SSkinnedVertex());
		m_SkinnedVertices[_submeshId][_vertexId].m_vPos = _pos;
	}
	else
	{
		DEBUG_Require(m_SkinnedVertices[_submeshId][_vertexId].m_vPos==_pos);
	}

	SSkinnedVertex::SBones& bone = m_SkinnedVertices[_submeshId][_vertexId].m_Bones.AddLastItem(SSkinnedVertex::SBones());
	bone.m_Name = _bone;
	bone.m_Weight = _weight;
}

//----------------------------------

void	CSkin::ComputeSkin()
{
	DEBUG_Require(m_VertexMapping.GetCount() == m_SubMeshs.GetCount());
	DEBUG_Require(m_SkinnedVertices.GetCount() == m_SubMeshs.GetCount());

	for(U32 i=0; i<m_SubMeshs.GetCount(); i++)
	{
		VertexBufferRef vb;
		IndexBufferRef ib;
		GetPrimitive(i)->Lock(vb,ib);
		for(U32 v=0; v<vb->GetVertexCount(); v++)
		{
			U32 vertexIdx = m_VertexMapping[i][v];

			Vec3f pos = m_SkinnedVertices[i][vertexIdx].m_vPos;
			Vec3f result = Vec3f::Zero;
			Float totalW = 0.f;
			for(U32 n=0; n<m_SkinnedVertices[i][vertexIdx].m_Bones.GetCount(); n++)
			{
				CBone* pBone = m_Skel.GetBone(m_SkinnedVertices[i][vertexIdx].m_Bones[n].m_Name);
				if( pBone )
				{
					Float w = m_SkinnedVertices[i][vertexIdx].m_Bones[n].m_Weight;
					result += pBone->GetSkelMatrix() * pBone->m_InvShapeMatrix * m_BindShapeMatrix * pos * w;
					totalW += w;
				}
			}

			result = result * (1.f / totalW );

			//GetVertexBuffer(i)->SetVertexAtIndex(v,m_SkinnedVertices[i][vertexIdx].m_vPos);
			vb->SetVertexPosAtIndex(v,result);
		}

		GetPrimitive(i)->Unlock();
	}
	// for each vertex
	//// for each bone affect vertex
	////// v += v * BSM * IBMi * JMi * JWi
	//// end
	// end
}