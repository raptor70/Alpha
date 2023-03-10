#include "world.h"

#include "editor\editor.h"
#include "file/base_file.h"
#include "core/name_static.h"
#include "file/node_stream.h"

SINGLETON_Define(World)

BEGIN_SCRIPT_CLASS(World)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,World)
	SCRIPT_RESULT_METHOD1(CreateNewEntity,Entity, Name)
	SCRIPT_RESULT_METHOD2(CreateNewEntityFromParent,Entity,Name,Entity)
	SCRIPT_RESULT_METHOD2(AddNewComponentToEntity,EntityComponent,Entity,Str);
	SCRIPT_RESULT_METHOD1(GetEntityWithComponent, Entity*, Str)
	SCRIPT_RESULT_METHOD1(GetComponent,EntityComponent*,Str)
	SCRIPT_RESULT_METHOD(GetRoot,Entity)
	SCRIPT_RESULT_METHOD1(GetEntity, Entity, Name)
END_SCRIPT_CLASS

World::World()
{
	m_iEntityCount = 1;
	m_Root = NEW Entity("ROOT");
	m_Root->InitParams();
	m_Root->Initialize();
	m_fTimeFactor = 1.f;
	m_iNextEntityUID = 1;
#ifdef USE_EDITOR_V2
	m_bEditorTreeDone = TRUE;
#endif
}

//------------------------------

World::~World()
{
}

//------------------------------

void	World::Update(Float _dTime)
{
	//PROFILER_Begin("WORLD_Init");
	EntityRefArray tmpentities = m_EntitiesToInit;
	m_EntitiesToInit.SetEmpty();
	S32 count = tmpentities.GetCount();
	do
	{
		for(S32 i=0; i<count;i++)
		{
			tmpentities[i]->InitParams();
			tmpentities[i]->Initialize();
		}
		tmpentities = m_EntitiesToInit;
		m_EntitiesToInit.SetEmpty();
		count = tmpentities.GetCount();
	}
	while(count);

	// init components
	EntityComponentPtrArray tmpComp = m_ComponentToInit;
	m_ComponentToInit.SetEmpty();
	count = tmpComp.GetCount();
	do
	{
		for (S32 i = 0; i<count; i++)
		{
			tmpComp[i]->Initialize();
		}
		tmpComp = m_ComponentToInit;
		m_ComponentToInit.SetEmpty();
		count = tmpComp.GetCount();
	} while (count);
	//PROFILER_End();

	//PROFILER_Begin("WORLD_Remove");
	for(U32 i=0; i<m_EntitesToRemove.GetCount();i++) // keep ascending due to doublon test
	{
		m_EntitesToRemove[i]->Finalize(TRUE);
		m_iEntityCount-=m_EntitesToRemove[i]->GetNbRecursiveSons() + 1;
		m_EntitesToRemove[i]->GetParent()->RemoveSon(m_EntitesToRemove[i]);

#ifdef USE_EDITOR_V2
		m_bEditorTreeDone = FALSE;
#endif
	}
	m_EntitesToRemove.Flush();
	//PROFILER_End();

	PROFILER_Begin("WORLD_RootUpdate");
	static EntityPtrArray entities;
	entities.AddLastItem(m_Root);
	while (entities.GetCount())
	{
		Entity* pCurrent = entities[entities.GetCount() - 1];
		entities.RemoveItems(entities.GetCount() - 1, 1);
		if (pCurrent->Update(_dTime*m_fTimeFactor))
		{
			S32 nbSon = pCurrent->GetNbSons();
			for (S32 i = nbSon - 1; i >= 0; i--)
				entities.AddLastItem(pCurrent->GetSon(i));
		}
	}
	PROFILER_End();
	
#ifdef USE_EDITOR_V2
	/*if (!m_bEditorTreeDone)
	{
		if(Application::GetInstance().HasEditor())
			Application::GetInstance().GetEditor().UpdateEntitiesTree();
	}*/
#endif
}

//------------------------------

void	World::Draw(Renderer*	_renderer)
{
	static EntityPtrArray entities;
	entities.AddLastItem(m_Root);
	while (entities.GetCount())
	{
		Entity* pCurrent = entities[entities.GetCount() - 1];
		entities.RemoveItems(entities.GetCount() - 1, 1);
		if (pCurrent->Draw(_renderer))
		{
			S32 nbSon = pCurrent->GetNbSons();
			for (S32 i = nbSon - 1; i >= 0; i--)
				entities.AddLastItem(pCurrent->GetSon(i));
		}
	}
}

//------------------------------

void	World::DrawDebugHierarchie(const Entity* _parent /*= NULL*/) const
{
	const Entity* parent = m_Root;
	if( _parent )
		parent = _parent;

	DEBUG_DrawCross(parent->GetWorldPos(),10.f,Color::Cyan);
	if( parent->m_pParent )
	{
		DEBUG_DrawLine(parent->GetWorldPos(),parent->m_pParent->GetWorldPos(),Color::Cyan);
	}

	for(U32 i=0; i<parent->m_aSons.GetCount(); i++)
		DrawDebugHierarchie(parent->m_aSons[i].GetPtr());
}

//------------------------------

void		World::DrawDebugBBox(const Entity* _parent /*= NULL*/) const
{
	const Entity* parent = m_Root;
	if( _parent )
		parent = _parent;

	if( parent->GetBBox().m_vSize != Vec3f::Zero )
	{
		const Box& bbox = parent->GetBBox();
		Vec3f center  = parent->GetWorldPos() + parent->GetWorldRot() * bbox.m_vCenter; 
		Vec3f size  = parent->GetWorldScale().Product(bbox.m_vSize); 
		Quat rot = parent->GetWorldRot() * bbox.m_qRot;
		DEBUG_DrawBox(center,rot,size,Color::Cyan);
	}
	
	for(U32 i=0; i<parent->m_aSons.GetCount(); i++)
		DrawDebugBBox(parent->m_aSons[i].GetPtr());
}

//------------------------------

void	World::SetParent(Entity* _pEntity, Entity* _pParent)
{
	EntityRef entity = _pEntity->GetParent()->RemoveSon(_pEntity);
	_pParent->AddSon(entity);
	
#ifdef USE_EDITOR_V2
	m_bEditorTreeDone = FALSE;
#endif
}

//------------------------------

EntityComponent&		World::AddNewComponentToEntity(Entity& _entity, const Name& _component)
{
	EntityComponentRef new_component = EntityComponentFactory::GetInstance().GetNewComponent(_component);
	DEBUG_RequireMessage(new_component.IsValid(),"Unable to find component %s\n",_component.GetStr().GetArray() );
	
	new_component->SetEntity(&_entity);
	_entity.AddComponent(new_component);
	new_component->InitParams(_entity.GetParamsTable());
	m_ComponentToInit.AddLastItem(new_component.GetPtr());
	return *new_component;
}

//------------------------------

Entity&	World::CreateNewEntity(const Name& _name, Entity* _parent /*= NULL*/)
{
	Entity* parent = m_Root;
	if (_parent)
		parent = _parent;

	EntityRef new_entity = NEW Entity(_name);
	new_entity->SetID(m_iNextEntityUID);
	m_iNextEntityUID++;
	parent->AddSon(new_entity);

	m_EntitiesToInit.AddLastItem(new_entity);

	m_iEntityCount++;

#ifdef USE_EDITOR_V2
	m_bEditorTreeDone = FALSE;
#endif

	return *new_entity;
}

//------------------------------

void	World::RemoveEntity(Entity* _entity,Entity* _parent /*= NULL*/)
{
	if( !IsEntityWaitingToRemove(_entity) )
	{
		m_EntitesToRemove.AddLastItem(_entity);
	}
}

//------------------------------

Bool	World::IsEntityWaitingToRemove(const Entity* _pEntity) const 
{
	EntityPtrArray entities = m_EntitesToRemove;
	for (U32 i = 0; i < entities.GetCount(); i++)
	{
		if (entities[i] == _pEntity)
			return TRUE;

		for (U32 c = 0; c < entities[i]->GetNbSons(); c++)
			entities.AddLastItem(entities[i]->GetSon(c));
	}

	return FALSE;
}

//------------------------------

void World::RemoveAll()
{
	m_Root->RemoveAllSons();
	m_iEntityCount = 1;
#ifdef USE_EDITOR_V2
	m_bEditorTreeDone = FALSE;
#endif
}

//------------------------------

void World::GetEntitiesInBox(const Vec3f& _pos, const Quat& _rot, const Vec3f& _size, ArrayOf<Entity*>& _outEntities, Entity* _pParent /*= NULL*/)
{
	VisitAllEntities([&](Entity* _pEntity)
	{
		Vec3f pos = _pEntity->GetWorldPos();
		pos -= _pos;
		pos = (_rot.GetInverse()) * pos;
		Vec3f halfBbox = _pEntity->GetBBox().m_vSize * .5f;
		Vec3f halfSize = _size * .5f;
		if (pos.x >= -halfSize.x && pos.x <= halfSize.x
			&& pos.y >= -halfSize.y && pos.y <= halfSize.y
			&& pos.z >= -halfSize.z && pos.z <= halfSize.z)
		{
			_outEntities.AddLastItem(_pEntity);
		}

		return VisitorReturn::Continue;
	});
}

//------------------------------

EntityComponent* World::GetComponent(const Name& _component, Entity* _pParent /*= NULL*/)
{
	EntityComponent* pResult = NULL;
	VisitAllEntities([&](Entity* _pEntity)
	{
		EntityComponent* pComp = _pEntity->GetComponent(_component);
		if (pComp)
		{
			pResult = pComp;
			return VisitorReturn::Stop;
		}

		return VisitorReturn::Continue;
	});

	return pResult;
}

//------------------------------

EntityPtr World::GetEntityWithComponent(const Name& _component, Entity* _pParent /*= NULL*/)
{
	EntityPtr pResult = NULL;
	VisitAllEntities([&](EntityPtr _pEntity)
	{
		EntityComponent* pComp = _pEntity->GetComponent(_component);
		if (pComp)
		{
			pResult = _pEntity;
			return VisitorReturn::Stop;
		}

		return VisitorReturn::Continue;
	});

	return pResult;
}

//------------------------------

EntityPtr World::GetEntity(const Name& _name)
{
	EntityPtr pResult = NULL;
	VisitAllEntities([&](EntityPtr _pEntity)
	{
		if (_pEntity->GetName() == _name)
		{
			pResult = _pEntity;
			return VisitorReturn::Stop;
		}

		return VisitorReturn::Continue;
	});

	return pResult;
}

//------------------------------

EntityPtr World::GetEntity(U32& _id)
{
	EntityPtr pResult = NULL;
	VisitAllEntities([&](EntityPtr _pEntity)
		{
			if (_pEntity->GetID() == _id)
			{
				pResult = _pEntity;
				return VisitorReturn::Stop;
			}

			return VisitorReturn::Continue;
		});

	return pResult;
}

//------------------------------

void	World::SaveEntitiesToFile(const Str& _path, Entity* _pRoot, Bool _saveRoot)
{
#ifdef USE_EDITOR_V2
	auto addUserLeaf = [&](NodeStream::Leaf& _leaf, Object* _pUserNode)
	{
		//ENTITY
		if (Entity* pEntity = dynamic_cast<Entity*>(_pUserNode))
		{
			_leaf.AddLeaf("Name", pEntity->GetName().GetStr().GetArray());
			_leaf.AddLeaf("Pos", pEntity->GetPos());
			_leaf.AddLeaf("Rot", pEntity->GetRot());
			_leaf.AddLeaf("Scale", pEntity->GetScale());
			_leaf.AddLeaf("Hide", pEntity->IsHidden());
			_leaf.AddLeaf("Color", pEntity->GetColor());

			// components
			for (U32 c = 0; c < pEntity->GetNbComponents(); c++)
			{
				EntityComponent* pComp = pEntity->GetComponent(c);
				_leaf.AddUserLeaf("Component", pComp);
			}

			// children
			for (U32 s = 0; s < pEntity->GetNbSons(); s++)
			{
				Entity* pSon = pEntity->GetSon(s);
				if (!pSon->ToSave())
					continue;

				_leaf.AddUserLeaf("Entity", pSon);
			}
		}
		else if (EntityComponent* pComp = dynamic_cast<EntityComponent*>(_pUserNode))
		{
			Name compName = pComp->GetComponentName();
			if (compName != Name::Null)
			{
				_leaf.AddLeaf("Type", compName.GetStr().GetArray());

				const EntityComponentFactory::Register* pFactory = EntityComponentFactory::GetInstance().GetComponentRegister(compName);
				// params
				for (U32 p = 0; p < pFactory->m_DefaultParams.GetNbValues(); p++)
				{
					const Params* defaultParam = pFactory->m_DefaultParams.GetValue(p);
					const Params* param = pComp->GetEntity()->GetParamsTable().GetValue(defaultParam->GetName());
					switch (param->GetType())
					{
#define PARAM_TYPE(_type)	case Params::TYPE_##_type : \
							if(param->Get##_type() != defaultParam->Get##_type())	\
								_leaf.AddLeaf(param->GetName().GetStr().GetArray(), param->Get##_type()); \
							break;
#include "params/params_type.h"
#undef PARAM_TYPE
					}
				}
			}
		}
	};

	NodeStream stream;
	stream.Save(_path, "Entity", addUserLeaf, _pRoot);
#endif
}

//----------------------------------

void	World::LoadEntitiesFromFile(const Str& _path, Entity* _root, Bool _editable)
{
	auto readUserLeaf = [&](NodeStream::Leaf& _leaf, const Char* _leafID, Object* _pParent)
	{
		Name nLeafID = _leafID;
		Entity* pEntity = dynamic_cast<Entity*>(_pParent);
		if (nLeafID == NAME(Entity))
		{
			Name entityName = _leaf.GetLeaf<Str>("Name");
			if (entityName != _root->GetName())
			{
				pEntity = &CreateNewEntity(_leaf.GetLeaf<Str>("Name"), pEntity);
#ifdef USE_EDITOR_V2
				if (_editable)
					pEntity->SetToSave(TRUE);
#endif
			}

			pEntity->SetPos(_leaf.GetLeaf<Vec3f>("Pos"));
			pEntity->SetRot(_leaf.GetLeaf<Quat>("Rot"));
			pEntity->SetScale(_leaf.GetLeaf<Vec3f>("Scale"));
			if (_leaf.GetLeaf<Bool>("Hide"))
				pEntity->Hide();
			else
				pEntity->Show();
			pEntity->SetColor(_leaf.GetLeaf<Color>("Color"));

			_leaf.ReadLeaves("Component", pEntity);
			_leaf.ReadLeaves("Entity", pEntity);
		}
		else if (nLeafID == NAME(Component))
		{
			// create component
			Name compType = _leaf.GetLeaf<Str>("Type");
			World::GetInstance().AddNewComponentToEntity(*pEntity, compType);

			const EntityComponentFactory::Register* pFactory = EntityComponentFactory::GetInstance().GetComponentRegister(compType);
			if (pFactory)
			{
				for (U32 p = 0; p < pFactory->m_DefaultParams.GetNbValues(); p++)
				{
					const Params* pFactoryParam = pFactory->m_DefaultParams.GetValue(p);
					switch (pFactoryParam->GetType())
					{
#define PARAM_TYPE(_type)	case Params::TYPE_##_type : 																  \
							{																							  \
								_type v;																				  \
								if (_leaf.FindLeaf(pFactoryParam->GetName().GetStr().GetArray(), v))						  \
								{																						  \
									Params* pNewParam = pEntity->GetParamsTable().GetOrAddValue(pFactoryParam->GetName());				  \
									pNewParam->Set##_type(v);															\
								}																						  \
								break;																					  \
							}
#include "params/params_type.h"
#undef PARAM_TYPE
					}
				}
			}
		}
	};

	NodeStream stream;
	stream.Load(_path, "Entity", readUserLeaf, _root);
}
