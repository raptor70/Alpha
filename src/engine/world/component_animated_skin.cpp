#include "component_animated_skin.h"

#include "resource/resource_manager.h"

#include "3d/skin.h"
#include "3d/animation.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentAnimatedSkin,EntityComponent)
	SCRIPT_VOID_METHOD1(LoadFromFile,Str)
	SCRIPT_VOID_METHOD2(AddAnimation,Str,Str)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(AnimatedSkin)
	EDITOR_CATEGORY(3D)
END_DEFINE_COMPONENT

EntityComponentAnimatedSkin::EntityComponentAnimatedSkin()
{
}

//------------------------------

EntityComponentAnimatedSkin::~EntityComponentAnimatedSkin()
{
}

//------------------------------

void	EntityComponentAnimatedSkin::Update(Float _dTime)
{
	for(U32 i=0; i<m_Animations.GetCount(); i++)
	{
		CAnimation* pAnim = (CAnimation*)m_Animations[i].m_Resource.GetPtr();
		pAnim->Update(_dTime);
	}

	Quat rot;
	rot.SetFromAxis(-MATH_PI_DIV_2,Vec3f::XAxis);
	Quat rot2;
	rot2.SetFromAxis(MATH_PI,Vec3f::YAxis);
	Mat44 worldMat;
	worldMat.SetTRS(GetEntity()->GetPos(),GetEntity()->GetRot()*rot2*rot,Vec3f(10.f));
	CSkin* skin = ((CSkin*)m_Skin.GetPtr());
	CSkel& skel = skin->GetSkel();
	CAnimation* pAnim = (CAnimation*)m_Animations[0].m_Resource.GetPtr();
	ArrayOf<CAnimation::SValue>	vals;
	pAnim->GetValues(vals);
	for(U32 i=0; i<vals.GetCount();i++)
	{
		Mat44& mat = vals[i].m_Value;
		CBone* pBone = skel.GetBone(vals[i].m_Name);
		if( pBone )
			pBone->SetTransformMatrix(mat);

		//Vec3f scale = mat.GetScale();
		//LOGGER_Log("%s %f %f %f\n",vals[i].m_Name.GetStr().GetArray(),scale.x,scale.y,scale.z);
	}
	skin->ComputeSkin();
	skel.DrawDebug(worldMat);
}

//------------------------------

void EntityComponentAnimatedSkin::Draw(Renderer*	_renderer)
{
	if( m_Skin.IsValid() )
	{
		CSkin* pSkin = (CSkin*)m_Skin.GetPtr();
		for(S32 i=0; i<pSkin->GetNbSubMesh(); i++)
		{
			Quat rot;
			rot.SetFromAxis(-MATH_PI_DIV_2,Vec3f::XAxis);
			Quat rot2;
			rot2.SetFromAxis(MATH_PI,Vec3f::YAxis);
			_renderer->SetActiveMaterial(pSkin->GetMaterial(i),GetEntity()->GetColor());
			_renderer->DrawGeometry(
				Geometry_TrianglesList,
				GetEntity()->GetWorldPos(),
				GetEntity()->GetWorldRot()*rot2*rot,
				pSkin->GetPrimitive(i),
				GetEntity()->GetWorldScale()*Vec3f(10.f));
		}

		_renderer->SetActiveMaterial(NULL);
	}
}

//------------------------------

void	EntityComponentAnimatedSkin::LoadFromFile(const Str& _path)
{
	m_Skin = ResourceManager::GetInstance().GetResourceFromFile<CSkin>(_path);
}

//------------------------------

void	EntityComponentAnimatedSkin::AddAnimation(const Str& _name, const Str& _path)
{
	DEBUG_RequireMessage(!GetAnimation(_name),"Animation %s already exist !\n",_name.GetArray());

	Animation newAnim;
	newAnim.m_Name = _name;
	newAnim.m_Resource = ResourceManager::GetInstance().GetResourceFromFile<CAnimation>(_path);
	m_Animations.AddLastItem(newAnim);

	// tmp
	((CAnimation*)newAnim.m_Resource.GetPtr())->Play();
}

//------------------------------

EntityComponentAnimatedSkin::Animation*	EntityComponentAnimatedSkin::GetAnimation(const Name& _name)
{
	for(U32 i=0; i<m_Animations.GetCount(); i++)
	{
		if( m_Animations[i].m_Name == _name )
			return &m_Animations[i];
	}

	return NULL;
}