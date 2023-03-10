#ifndef __WORLD_H__
#define __WORLD_H__

#include "core/singleton.h"
#include "entity.h"

class World
{
	DECLARE_SCRIPT_CLASS(World)
	SINGLETON_Declare(World)

	~World();

public:
	World();
	void				Update(Float _dTime);
	void				Draw(Renderer*	_renderer);
	void				DrawDebugHierarchie(const Entity* _parent = NULL) const;
	void				DrawDebugBBox(const Entity* _parent = NULL) const;

	Entity&			CreateNewEntityFromParent(const Name& _name, Entity& _parent)	{ return CreateNewEntity(_name, &_parent); }
	Entity&			CreateNewEntity(const Name& _name, Entity* _parent = NULL);
	EntityComponent&	AddNewComponentToEntity(Entity& _entity, const Name& _component);
	void				SetParent(Entity* _pEntity, Entity* _pParent);
	void				RemoveEntity(Entity* _entity, Entity* _parent = NULL);
	void				RemoveAll();

	EntityRef&			GetRoot()	{ return m_Root; }

	template <class Visitor> // [&](Entity* _pEntity)
	void				VisitAllEntities(Visitor&& _visitor);
	template <typename ComponentType, class Visitor> // [&](Entity* _pEntity, EntityComponentCamera* _pCamera)
	void				VisitComponents(Visitor&& _visitor);

	void				GetEntitiesInBox(const Vec3f& _pos, const Quat& _rot, const Vec3f& _size, EntityPtrArray& _outEntities, Entity* _pParent = NULL);
	EntityComponent*	GetComponent(const Name& _component, Entity* _pParent = NULL);
	EntityPtr			GetEntityWithComponent(const Name& _component, Entity* _pParent = NULL);
	template <typename T>
	T*	GetComponent(Entity* _pParent = NULL);
	EntityPtr			GetEntity(const Name& _name);
	EntityPtr			GetEntity(U32& _id);

	Float				GetTimeFactor() const { return m_fTimeFactor; }
	void				StopTimeFactor() { m_fTimeFactor = 0.f; }
	void				StartTimeFactor() { m_fTimeFactor = 1.f; }
	void				ForwardTimeFactor() { m_fTimeFactor = (m_fTimeFactor > Float_Eps) ? m_fTimeFactor * 2.f : 1.f; }
	void				BackwardTimeFactor() { m_fTimeFactor = (m_fTimeFactor > Float_Eps) ? m_fTimeFactor * 0.5f : 1.f; }

	void				SaveEntitiesToFile(const Str& _path, Entity* _root, Bool _saveRoot);
	void				LoadEntitiesFromFile(const Str& _path, Entity* _root, Bool _editable);

protected:
	Bool				IsEntityWaitingToRemove(const Entity* _pEntity) const;

	EntityRef		m_Root;
	U32			m_iEntityCount;
	EntityRefArray		m_EntitiesToInit;
	EntityPtrArray		m_EntitesToRemove;
	EntityComponentPtrArray m_ComponentToInit;
	Float				m_fTimeFactor;
	U32			m_iNextEntityUID;

#ifdef USE_EDITOR_V2
public:
	void SetEditorTreeDone(Bool _done) { m_bEditorTreeDone = _done; }
protected:
	Bool m_bEditorTreeDone;
#endif
};

//---------------------------------------

template <typename T>
T* World::GetComponent(Entity* _pParent /*= NULL*/)
{
	return (T*)GetComponent(T::GetStaticComponentName(),_pParent);
}

//---------------------------------------

template <class Visitor>
void	World::VisitAllEntities(Visitor&& _visitor)
{
	if (_visitor(m_Root) == VisitorReturn::Stop)
		return;

	m_Root->VisitAllSons(TRUE, FWD(_visitor));
}

//---------------------------------------

template <typename ComponentType, class Visitor>
void	World::VisitComponents(Visitor&& _visitor)
{
	VisitAllEntities([&](Entity* _pEntity)
	{
		ComponentType* pComp = _pEntity->GetComponent<ComponentType>();
		if (pComp)
		{
			if (_visitor(_pEntity, pComp) == VisitorReturn::Stop)
				return VisitorReturn::Stop;
		}

		return VisitorReturn::Continue;
	});
}

//---------------------------------------

#define WORLDPARAMS World::GetInstance().GetRoot()->GetParamsTable()
#define WORLDPARAMFLOAT(_value)	WORLDPARAMS.GetValue(NAME(_value))->GetFloat()

#endif