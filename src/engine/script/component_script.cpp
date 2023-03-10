#include "component_script.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentScript,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Script)
	INIT_ENTITY_PARAMS(Str, Path, "");
	INIT_ENTITY_PARAMS(Bool, DefaultHide,FALSE);
	EDITOR_FILE_VALUE(Path,Script)
END_DEFINE_COMPONENT

EntityComponentScript::EntityComponentScript():SUPER()
{
}

//------------------------------

EntityComponentScript::~EntityComponentScript()
{
}

//------------------------------

void	EntityComponentScript::Initialize()
{
	SUPER::Initialize();

	OnParamChanged();

	if (m_Script.HasFunction("Init"))
	{
		if (!m_Script.Call("Init", *this))
		{
			LOGGER_LogError("Unable to call Update in component script %s\n", GET_ENTITY_PARAMS(Str, ScriptPath));
		}
	}

	LoadEntitiesFromFile();
}

//------------------------------

void	EntityComponentScript::OnParamChanged()
{
	SUPER::OnParamChanged();

	Str path = GET_ENTITY_PARAMS(Str, ScriptPath);
	if (!path.IsEmpty() && m_Script.GetPath() != path)
	{
		if (!m_Script.LoadFromFile(path) || !m_Script.Execute())
		{
			LOGGER_LogError("Unable to create component script %s\n", path.GetArray());
		}
		else
		{
			m_bHasUpdateFunction = m_Script.HasFunction("Update");
		}
	}
}

//------------------------------

void	EntityComponentScript::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	if(m_bHasUpdateFunction)
	{
		if( !m_Script.Call("Update",*this,_dTime) )
		{
			LOGGER_LogError("Unable to call Update in component script %s\n",GET_ENTITY_PARAMS(Str,ScriptPath));
		}
	}
}

//------------------------------

void	EntityComponentScript::OnEvent(const EntityEvent& _event)
{
	SUPER::OnEvent(_event);

	if( m_Script.HasFunction("OnEvent") )
	{
		if( !m_Script.Call("OnEvent",*this,_event.m_nName.GetStr()) )
		{
			LOGGER_LogError("Unable to call Update in component script %s\n",GET_ENTITY_PARAMS(Str,ScriptPath));
		}
	}
}

//------------------------------

void	EntityComponentScript::LoadEntitiesFromFile()
{
	Str path = m_Script.GetPath().GetTextInsideRange(0, m_Script.GetPath().GetLength()-4);
	path += "ent";
	World::GetInstance().LoadEntitiesFromFile(path, GetEntity(), FALSE);
	if(GET_ENTITY_PARAMS(Bool,ScriptDefaultHide))
		GetEntity()->Hide();
}
