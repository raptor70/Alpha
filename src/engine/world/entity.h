#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "component.h"
#include "math/box.h"

class Entity;
typedef ArrayOf<Entity*>	EntityPtrArray;
typedef RefTo<Entity>		EntityRef;
typedef PtrTo<Entity>		EntityPtr;
typedef ArrayOf<EntityRef>	EntityRefArray;

//------------------------------

class EntityEvent
{
public:
	EntityEvent()
	{
		m_nName = Name::Null;
	}
	EntityEvent(const Name& _name)
	{
		m_nName = _name;
	}
	~EntityEvent(){}

	Name	m_nName;
};

//------------------------------

#define GET_ENTITY_PARAMS(_type,_name)	GetEntity()->GetParamsTable().GetValue(#_name)->Get##_type()

#define SET_ENTITY_PARAMS(_type,_name,_value)	GetEntity()->GetParamsTable().GetOrAddValue(#_name)->Set##_type(_value)

//------------------------------

class Entity : public Object
{
	friend class World;
	DECLARE_SCRIPT_CLASS_FINAL(Entity)
public:
	Entity():Entity(Name::Null) {}
	Entity(const Name& _name);
	~Entity();

	void	InitParams() { InitParams(m_Params);  }
	void	InitParams(ParamsTable& _table);
	void	Initialize();
	void	Finalize(Bool _recurse =FALSE);
	Bool	Update(Float _dTime);
	Bool	Draw(Renderer*	_renderer);
	void	LaunchEvent(const EntityEvent& _event);

	const Name&		GetName() const		{ return m_nName; }
	const Vec3f&	GetPos() const		{ return m_vPos; }
	const Quat&		GetRot() const		{ return m_qRot; }
	const Vec3f&	GetScale() const	{ return m_vScale; }
	const Color&	GetColor() const	{ return m_Color; }
	U32				GetID() const		{ return m_iID; }
	Mat44			GetMatrix() const { return Mat44(m_vPos, m_qRot, m_vScale); }

	void			SetName(const Name& _name)		{ m_nName = _name;}
	void			SetPos(const Vec3f& _pos)		{ m_vPos = _pos; }
	void			SetRot(const Quat& _rot)		{ m_qRot = _rot; }
	void			SetScale(const Vec3f& _scale)	{ m_vScale = _scale; }
	void			SetColor(const Color& _color)	{  m_Color = _color; }
	void			SetID(U32& _id) { m_iID = _id; }

	Vec3f			GetWorldPos() const;
	Quat			GetWorldRot() const;
	Vec3f			GetWorldScale() const;
	Mat44			GetWorldMatrix() const;

	void			SetWorldPos(const Vec3f& _pos);
	void			SetWorldRot(const Quat& _rot);
	void			SetWorldRotAroundX(Float _rot);
	void			SetWorldRotAroundY(Float _rot);
	void			SetWorldRotAroundZ(Float _rot);
	void			SetWorldScale(const Vec3f& _scale);
	void			SetWorldMatrix(const Mat44& _matrix);

	void			SetBBox(const Box& _box)			{ m_BBox = _box; }
	const Box&		GetBBox()const						{ return m_BBox; }

	void			SetUpdateFactor(Float _factor)	{ m_fUpdateFactor = _factor; }
	Float			GetUpdateFactor() const			{ return m_fUpdateFactor; }

	Entity*		GetParent() const { return m_pParent; }
	U32				GetNbSons() const { return m_aSons.GetCount(); }
	U32				GetNbRecursiveSons() const;
	Entity*		GetSon(S32 i);
	Bool			HasSon(const Entity* _entity, Bool _recurse = FALSE) const;

	template<class Visitor>
	VisitorReturn	VisitAllSons(Bool _bRecurse, Visitor&& _visitor);

#ifdef USE_EDITOR_V2
	Bool			IsSelected() const { return m_bIsSelected; }
	void			SetIsSelected(Bool _selected) { m_bIsSelected = _selected; }
	Bool			ToSave() const { return m_bToSave; }
	void			SetToSave(Bool _selected) { m_bToSave = _selected; }
#endif

	EntityComponent*	GetComponent(S32 _id = 0) { return m_aComponents[_id]; }
	U32					GetNbComponents() { return m_aComponents.GetCount(); }

	template<typename T>
	S32					GetNbComponents();
	template<typename T>
	T*					GetComponent(S32 _id = 0);
	S32					GetNbComponents(const Name& _name);
	EntityComponent*	GetComponent(const Name& _name,S32 _id = 0);

	void			Hide(Bool _bRecurse = TRUE) 
	{ 
		m_bHidden = TRUE; 
		if( _bRecurse )
			for(U32 i=0; i<m_aSons.GetCount(); i++)
				m_aSons[i]->Hide();
	}
	void			Show(Bool _bRecurse = TRUE) 
	{ 
		m_bHidden = FALSE;
		if( _bRecurse )
			for(U32 i=0; i<m_aSons.GetCount(); i++)
				m_aSons[i]->Show(); 
	}
	Bool			IsHidden() const { return m_bHidden; }

	void			SkipUpdate(Bool _skip);
	void			SkipDraw(Bool _skip)	{ m_bSkipDraw = _skip; }

	ParamsTable&	GetParamsTable() { return m_Params; }

#define SUPPORT_PARAM(Type)	\
	const Type&		GetParam##Type(const Name& _name) { return GetParamsTable().GetValue(_name)->Get##Type(); } \
	void			SetParam##Type(const Name& _name, const Type& _value)	{ GetParamsTable().GetOrAddValue(_name)->Set##Type(_value); }

#define SUPPORT_PARAM_COPY(Type)	\
	Type			GetParam##Type(const Name& _name) { return GetParamsTable().GetValue(_name)->Get##Type(); } \
	void			SetParam##Type(const Name& _name, const Type& _value)	{ GetParamsTable().GetOrAddValue(_name)->Set##Type(_value); }

	SUPPORT_PARAM_COPY(Bool)
	SUPPORT_PARAM_COPY(Float)
	SUPPORT_PARAM_COPY(S32)
	SUPPORT_PARAM(Str)
	SUPPORT_PARAM_COPY(Vec2f)
	SUPPORT_PARAM_COPY(Vec3f)
	SUPPORT_PARAM(Vec4f)
	SUPPORT_PARAM_COPY(Color)

	void			Clone(const Entity* _from);
	void			RemoveComponent(EntityComponent* _component);

private:
	Entity*		GetEntity() const { return (Entity*)this; } // tricks for ENTITY_PARAMS macro

protected:
	// protected, used only by World  !
	void			AddComponent(EntityComponentRef& _newComponent);
	void			AddSon(EntityRef& _son);
	EntityRef		RemoveSon(S32 i);
	EntityRef		RemoveSon(Entity* _entity);
	void			RemoveAllSons();

	Name					m_nName;
	Entity*				m_pParent;
	EntityRefArray			m_aSons;
	Vec3f					m_vPos;
	Quat					m_qRot;
	Vec3f					m_vScale;
	Color					m_Color;
	U32						m_iID;
	Bool					m_bHidden;
	EntityComponentRefArray	m_aComponents;
	Box						m_BBox;	
	Bool					m_bSkipUpdate;
	Bool					m_bSkipDraw;
	Bool					m_bInitialized;
	Float					m_fUpdateFactor;
	ParamsTable			m_Params;
	EntityComponentPtrArray m_aUpdateComponents;
	EntityComponentPtrArray m_aDrawComponents;
	EntityComponentPtrArray m_aEventComponents;

#ifdef USE_EDITOR_V2
	Bool	m_bIsSelected;
	Bool	m_bToSave;
#endif

};

//------------------------------

template<typename T>
S32	Entity::GetNbComponents()
{
	return GetNbComponents(T::GetStaticComponentName());
}

//------------------------------

template<typename T>
T*	Entity::GetComponent(S32 _id /*= 0*/)
{
	return (T*)GetComponent(T::GetStaticComponentName(),_id);
}

//------------------------------

template<class Visitor>
VisitorReturn	Entity::VisitAllSons(Bool _bRecurse, Visitor&& _visitor)
{
	for (EntityRef& pSon : m_aSons)
	{
		if (_visitor(pSon) == VisitorReturn::Stop)
			return VisitorReturn::Stop;

		if (_bRecurse)
			if (pSon->VisitAllSons(_bRecurse, FWD(_visitor)) == VisitorReturn::Stop)
				return VisitorReturn::Stop;
	}

	return VisitorReturn::Continue;
}

#endif