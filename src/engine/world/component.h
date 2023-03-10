#ifndef __ENTITY_COMPONENT_H__
#define __ENTITY_COMPONENT_H__

#include "component_factory.h"
class Entity;
class EntityEvent;

class EntityComponent : public Object
{
	friend class World;
	DECLARE_SCRIPT_CLASS(EntityComponent)
public:
	EntityComponent();
	virtual ~EntityComponent();

	virtual Bool	IsUpdater() const = 0;
	virtual Bool	IsDrawer() const = 0;
	virtual Bool	ReceiveEvent() const = 0;

	virtual void	InitParams(ParamsTable& _table) {}
	virtual void	InitRegister() {}
	virtual void	Initialize(){}
	virtual void	Finalize(){}
	virtual void	Update(Float _dTime){}
	virtual void	Draw(Renderer*	_renderer){}

	virtual void	OnParamChanged() {}
	virtual void	OnEvent(const EntityEvent&	_event){}
	virtual void	OnPause(){}
	virtual void	OnResume(){}

	virtual void	Clone(const EntityComponent* _from);

	Entity*		GetEntity() const { return m_pEntity; }

	static const Name&		GetStaticComponentName() { return EntityComponentFactory::GetInstance().GetComponentName(); }
	virtual const Name&	GetComponentName() const { return EntityComponent::GetStaticComponentName(); }

private:
	void			SetEntity(Entity* _entity) { m_pEntity = _entity; }
	Entity*		m_pEntity;
};

typedef ArrayOf<EntityComponent*>	EntityComponentPtrArray;
typedef RefTo<EntityComponent>		EntityComponentRef;
typedef ArrayOf<EntityComponentRef>	EntityComponentRefArray;

#endif