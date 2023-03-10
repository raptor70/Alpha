#include "entity.h"

#include "core/name_static.h"

#define SCRIPT_PARAM(Type) \
	SCRIPT_VOID_METHOD2(SetParam##Type,Str,Type); \
	SCRIPT_RESULT_METHOD1(GetParam##Type,Type, Str);

BEGIN_SCRIPT_CLASS(Entity)
	SCRIPT_VOID_METHOD1(SetWorldPos,Vec3f)
	SCRIPT_VOID_METHOD1(SetPos,Vec3f)
	SCRIPT_VOID_METHOD1(SetWorldRot,Quat)
	SCRIPT_VOID_METHOD1(SetWorldRotAroundX,Float)
	SCRIPT_VOID_METHOD1(SetWorldRotAroundY,Float)
	SCRIPT_VOID_METHOD1(SetWorldRotAroundZ,Float)
	SCRIPT_VOID_METHOD1(SetWorldScale,Vec3f)
	SCRIPT_VOID_METHOD1(SetScale,Vec3f)
	SCRIPT_VOID_METHOD1(SetBBox,Box)
	SCRIPT_RESULT_METHOD(GetBBox,Box)
	SCRIPT_RESULT_METHOD1(GetComponent,EntityComponent*,Str)
	SCRIPT_VOID_METHOD1(SetUpdateFactor,Float);
	SCRIPT_PARAM(Bool);
	SCRIPT_PARAM(Float);
	SCRIPT_PARAM(S32);
	SCRIPT_PARAM(Str);
	SCRIPT_PARAM(Vec2f);
	SCRIPT_PARAM(Vec3f);
	SCRIPT_PARAM(Vec4f);
	SCRIPT_PARAM(Color);
	SCRIPT_VOID_METHOD(Hide)
	SCRIPT_VOID_METHOD(Show)
	SCRIPT_RESULT_METHOD(GetParamsTable,ParamsTable)
	SCRIPT_RESULT_METHOD1(GetSon, Entity, S32)
	SCRIPT_RESULT_METHOD(GetNbSons, S32)
	SCRIPT_RESULT_METHOD(GetParent,Entity)
END_SCRIPT_CLASS

Entity::Entity(const Name& _name)
{
	m_pParent = NULL;
	m_nName = _name;
	m_vPos = Vec3f::Zero;
	m_qRot = Quat::Identity;
	m_vScale = Vec3f::One;
	m_Color = Color::White;
	m_iID = 0;
	m_bHidden = FALSE;
	m_bInitialized = FALSE;
	m_bSkipUpdate = FALSE;
	m_bSkipDraw = FALSE;
	m_fUpdateFactor = 1.f;
#ifdef USE_EDITOR_V2
	m_bIsSelected = FALSE;
	m_bToSave = FALSE;
#endif
}

//------------------------------

Entity::~Entity()
{
}

//------------------------------

void	Entity::InitParams(ParamsTable& _table)
{
	static Str currentComponentName = "Entity";
	ParamsTable& m_DefaultParams = _table;
	INIT_ENTITY_PARAMS(Float, BBoxX, 0.f);
	INIT_ENTITY_PARAMS(Float, BBoxY, 0.f);
	INIT_ENTITY_PARAMS(Float, BBoxZ, 0.f);
}

//------------------------------

void	Entity::Initialize()
{
	m_bInitialized = TRUE;

	SetBBox(Vec3f(GET_ENTITY_PARAMS(Float,EntityBBoxX),GET_ENTITY_PARAMS(Float,EntityBBoxY),GET_ENTITY_PARAMS(Float,EntityBBoxZ)));
}

//------------------------------

void	Entity::Finalize(Bool _recurse /*=FALSE*/)
{
	for(U32 i=0; i<m_aComponents.GetCount(); i++)
		m_aComponents[i]->Finalize();

	if( _recurse )
		for(U32 i=0; i<m_aSons.GetCount(); i++)
			m_aSons[i]->Finalize(TRUE);
}

//------------------------------

Bool	Entity::Update(Float _dTime)
{
	if( m_bSkipUpdate || !m_bInitialized )
		return FALSE;

	Float dTime = _dTime * m_fUpdateFactor;

	if (m_Params.HasChanged())
	{
		m_Params.ResetChanged();
		for (U32 i = 0; i < m_aComponents.GetCount(); i++)
			m_aComponents[i]->OnParamChanged();
	}

	for(U32 i=0; i<m_aUpdateComponents.GetCount(); i++)
		m_aUpdateComponents[i]->Update(dTime);

	return TRUE;
}

//------------------------------

Bool	Entity::Draw(Renderer*	_renderer)
{
	if( m_bSkipDraw || !m_bInitialized)
		return FALSE;

	if( !IsHidden() )
	{
#ifdef USE_EDITOR_V2
		if (m_bIsSelected)
			_renderer->SetForceEditorOutline(TRUE);
		_renderer->SetEntityID(m_iID);
#endif

		for(U32 i=0; i<m_aDrawComponents.GetCount(); i++)
		{
			m_aDrawComponents[i]->Draw(_renderer);
		}

#ifdef USE_EDITOR_V2
		if (m_bIsSelected)
			_renderer->SetForceEditorOutline(FALSE);
#endif
	}
	
	return TRUE;
}

//------------------------------

void	Entity::LaunchEvent(const EntityEvent& _event)
{
	if( m_bSkipUpdate || !m_bInitialized )
		return;

	for(U32 i=0; i<m_aEventComponents.GetCount(); i++)
	{
		m_aEventComponents[i]->OnEvent(_event);
	}

	for(U32 i=0; i<m_aSons.GetCount(); i++)
	{
		m_aSons[i]->LaunchEvent(_event);
	}
}

//------------------------------

Entity*	Entity::GetSon(S32 i)
{ 
	return m_aSons[i].GetPtr(); 
}

//------------------------------

Bool	Entity::HasSon(const Entity* _entity, Bool _recurse /*= FALSE*/) const
{
#if 1
	const Entity* pCur = _entity;
	while(pCur)
	{
		if (pCur->GetParent() == this)
			return TRUE;

		if (_recurse)
			pCur = pCur->GetParent();
	}
#else
	for (U32 i = 0; i < m_aSons.GetCount(); i++)
	{
		if (_entity == m_aSons[i].GetPtr())
			return TRUE;

		if (_recurse && m_aSons[i]->HasSon(_entity, TRUE))
			return TRUE;
	}
#endif

	return FALSE;
}

//------------------------------

EntityRef	Entity::RemoveSon(S32 i)
{
	EntityRef ref = m_aSons[i];
	m_aSons.RemoveItems(i,1);
	return ref;
}

//------------------------------

EntityRef	Entity::RemoveSon(Entity* _entity)
{
	for(U32 i=0; i<m_aSons.GetCount(); i++)
	{
		if( _entity == m_aSons[i].GetPtr() )
		{
			EntityRef ref = m_aSons[i];
			m_aSons.RemoveItems(i,1);
			return ref;
		}
	}

	return EntityRef();
}

//------------------------------

void	Entity::RemoveAllSons()
{
	m_aSons.Flush();
}

//------------------------------

U32	Entity::GetNbRecursiveSons() const
{
	U32 count = GetNbSons();
	for(U32 i=0; i<m_aSons.GetCount(); i++)
		count += m_aSons[i]->GetNbRecursiveSons();

	return count;
}

//------------------------------

void	Entity::AddComponent(EntityComponentRef& _newComponent)
{
	m_aComponents.AddLastItem(_newComponent);

	if( _newComponent->IsUpdater() )
		m_aUpdateComponents.AddLastItem(_newComponent.GetPtr());

	if( _newComponent->ReceiveEvent() )
		m_aEventComponents.AddLastItem(_newComponent.GetPtr());

	if( _newComponent->IsDrawer() )
		m_aDrawComponents.AddLastItem(_newComponent.GetPtr());
}

//------------------------------

void	Entity::RemoveComponent(EntityComponent* _component)
{
	U32 idx = -1;
	if (_component->IsUpdater() && m_aUpdateComponents.Find(_component, idx))
		m_aUpdateComponents.RemoveItems(idx, 1);

	if (_component->ReceiveEvent() && m_aEventComponents.Find(_component, idx))
		m_aEventComponents.RemoveItems(idx, 1);

	if (_component->IsDrawer() && m_aDrawComponents.Find(_component, idx))
		m_aDrawComponents.RemoveItems(idx, 1);

	for(U32 i=0; i<m_aComponents.GetCount(); i++)
		if (m_aComponents[i].GetPtr() == _component)
		{
			m_aComponents.RemoveItems(i, 1);
			break;
		}
}

//------------------------------

S32	Entity::GetNbComponents(const Name& _compName)
{
	S32 count = 0;
	for(U32 i=0; i<m_aComponents.GetCount(); i++)
		if( m_aComponents[i]->GetComponentName() == _compName )
		{
			count++;
		}

	return count;
}

//------------------------------

EntityComponent*	Entity::GetComponent(const Name& _compName, S32 _id /*=0*/)
{
	S32 currentId = 0;
	for(U32 i=0; i<m_aComponents.GetCount(); i++)
		if( m_aComponents[i]->GetComponentName() == _compName )
		{
			if( currentId == _id )
				return m_aComponents[i].GetPtr();
			else
				currentId++;
		}

	return NULL;
}

//------------------------------

void	Entity::AddSon(EntityRef& _son)
{
	m_aSons.AddLastItem(_son);
	_son->m_pParent = this;
}

//------------------------------

Vec3f	Entity::GetWorldPos() const
{
	Vec3f result = GetPos();
	Entity* pParent = m_pParent;
	while (pParent)
	{
		result = pParent->GetPos() + pParent->GetRot() * result;
		pParent = pParent->m_pParent;
	}
	return result;
}

//------------------------------

Quat	Entity::GetWorldRot() const
{
	Quat result = GetRot();
	Entity* pParent = m_pParent;
	while (pParent)
	{
		result = pParent->GetRot() * result;
		pParent = pParent->m_pParent;
	}
	return result;
}

//------------------------------

Vec3f	Entity::GetWorldScale() const
{
	Vec3f result = GetScale();
	Entity* pParent = m_pParent;
	while (pParent)
	{
		result = pParent->GetScale().Product(result);
		pParent = pParent->m_pParent;
	}
	return result;
}

//------------------------------

Mat44	Entity::GetWorldMatrix() const
{
	Mat44 result = GetMatrix();
	Entity* pParent = m_pParent;
	while (pParent)
	{
		result = pParent->GetMatrix()*result;
		pParent = pParent->m_pParent;
	}
	return result;
}

//------------------------------

void	Entity::SetWorldMatrix(const Mat44& _matrix)
{
	SetWorldPos(_matrix.GetTranslation());
	SetWorldRot(_matrix.GetRotation());
	SetWorldScale(_matrix.GetScale());
}

//------------------------------

void	Entity::SetWorldPos(const Vec3f& _pos)
{
	if( m_pParent )
	{
		Mat44 worldMat = m_pParent->GetWorldMatrix();
		worldMat.Inverse();
		return SetPos(worldMat*_pos);
	}

	return SetPos(_pos);
}
		
//------------------------------

void	Entity::SetWorldRot(const Quat& _rot)
{
	if( m_pParent )
	{
		return SetRot( m_pParent->GetWorldRot().GetInverse() * _rot);
	}

	return SetRot(_rot);
}

//------------------------------

void	Entity::SetWorldRotAroundX(Float _rot)
{
	Quat qRot;
	qRot.SetFromAxis(_rot,Vec3f::XAxis);
	return SetWorldRot(qRot);
}

//------------------------------

void	Entity::SetWorldRotAroundY(Float _rot)
{
	Quat qRot;
	qRot.SetFromAxis(_rot,Vec3f::YAxis);
	return SetWorldRot(qRot);
}

//------------------------------

void	Entity::SetWorldRotAroundZ(Float _rot)
{
	Quat qRot;
	qRot.SetFromAxis(_rot,Vec3f::ZAxis);
	return SetWorldRot(qRot);

}

//------------------------------

void	Entity::SetWorldScale(const Vec3f& _scale)
{
	if( m_pParent )
	{
		return SetScale(_scale.Divide(m_pParent->GetWorldScale()));
	}

	return SetScale(_scale);
}

//------------------------------

void	Entity::SkipUpdate(Bool _skip)	
{ 
	if( _skip && !m_bSkipUpdate )
		LaunchEvent(EntityEvent(NAME(ON_PAUSE)));
	else if(!_skip && m_bSkipUpdate )
		LaunchEvent(EntityEvent(NAME(ON_RESUME)));

	m_bSkipUpdate = _skip; 
}

//------------------------------

void	Entity::Clone(const Entity* _from)
{
	for(U32 i=0; i<_from->m_aComponents.GetCount(); i++)
	{
		EntityComponent& newComp = World::GetInstance().AddNewComponentToEntity(*this,_from->m_aComponents[i]->GetComponentName());
		newComp.Clone(_from->m_aComponents[i].GetPtr());
	}

	for(U32 i=0; i<_from->m_aSons.GetCount(); i++)
	{
		const Entity* source = _from->m_aSons[i].GetPtr();
		Entity& son = World::GetInstance().CreateNewEntityFromParent(source->GetName(),*this);
		son.Clone(source);
	}

	m_vPos = _from->m_vPos;
	m_BBox = _from->m_BBox;
	m_qRot = _from->m_qRot;
	m_vScale = _from->m_vScale;
	m_Color = _from->m_Color;
	m_bHidden = _from->m_bHidden;
	m_bSkipUpdate = _from->m_bSkipUpdate;
	m_bSkipDraw = _from->m_bSkipDraw;
	m_bInitialized = _from->m_bInitialized;
	m_fUpdateFactor = _from->m_fUpdateFactor;
}