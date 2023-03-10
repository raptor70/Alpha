#include "skel.h"

CBone::CBone(const Name& _name)
{
	m_TransformMatrix = Mat44::Identity;
	m_InvShapeMatrix = Mat44::Identity;
	m_Name = _name;
}

//----------------------------------

CBone::~CBone()
{
}

//----------------------------------

Mat44 CBone::GetSkelMatrix() const
{
	if( m_pParent )
	{
		return m_pParent->GetSkelMatrix() * m_TransformMatrix;
	}

	return m_TransformMatrix;
}

//----------------------------------

CSkel::CSkel()
{
}

//----------------------------------

CSkel::~CSkel()
{
}

//----------------------------------

CBone*	CSkel::GetBone(const Name& _name, CBone* _parent /*= NULL */)
{
	BoneRefArray* parentArray = &m_Bones;
	if( _parent )
		parentArray = &(_parent->m_aSons);

	for(U32 i=0; i<parentArray->GetCount(); i++)
	{
		if( (*parentArray)[i]->m_Name == _name )
			return (*parentArray)[i];

		CBone* pBone = GetBone(_name,(*parentArray)[i]);
		if( pBone )
			return pBone;
	}

	return NULL;
}

//----------------------------------

CBone* CSkel::AddBone(const Name& _name, const Mat44& _mat, const Name& _parent /*= Name::Null*/)
{
	BoneRefArray* parentArray = &m_Bones;
	CBone*	parentBone = NULL;
	if( _parent != Name::Null )
	{
		parentBone = GetBone(_parent);
		if( parentBone )
		{
			parentArray = &(parentBone->m_aSons);
		}

	}

	CBone* pBone = NEW CBone(_name);
	pBone->SetTransformMatrix(_mat);
	pBone->m_pParent = parentBone;
	parentArray->AddLastItem(pBone);
	return pBone;
}

//----------------------------------

void	CSkel::DrawDebug(const Mat44& _worldMat, const CBone* _pBone /*=NULL*/) const
{
	if( _pBone )
	{
		DEBUG_DrawCross(_worldMat*_pBone->GetSkelMatrix() * Vec3f::Zero,10.f,Color::Green);;
		for(U32 i=0; i<_pBone->m_aSons.GetCount();i++)
			DrawDebug(_worldMat, _pBone->m_aSons[i].GetPtr());
	}
	else
	{
		for(U32 i=0; i<m_Bones.GetCount();i++)
			DrawDebug(_worldMat, m_Bones[i].GetPtr());
	}
}