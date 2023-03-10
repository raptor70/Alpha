#include "flow_script.h"

#include "2d/component_button.h"
#include "2d/component_check_button.h"
#include "core/name_static.h"
#include "file/base_file.h"

BEGIN_SCRIPT_CLASS(FlowScript)
	SCRIPT_RESULT_METHOD1(CreateNewEntity,Entity, Name)
	SCRIPT_RESULT_METHOD2(CreateNewEntityFromParent,Entity, Name,Entity)
	SCRIPT_RESULT_METHOD(GetRootEntity,Entity)
	SCRIPT_VOID_METHOD1(PushScript,Str)
	SCRIPT_VOID_METHOD(PopScript)
	SCRIPT_VOID_METHOD1(GoToScript,Str)
	SCRIPT_VOID_METHOD1(GoToScriptWithClean,Str)
	SCRIPT_VOID_METHOD2(AddParamFloat,Str,Float)
	SCRIPT_VOID_METHOD2(AddParamStr,Str,Str)
	SCRIPT_RESULT_METHOD1(GetParamFloat,Float,Str)
	SCRIPT_RESULT_METHOD1(GetParamStr,Str,Str)
	SCRIPT_VOID_METHOD1(SetAlwaysDraw,Bool)
	SCRIPT_VOID_METHOD1(SetAlwaysUpdate,Bool)
	SCRIPT_VOID_METHOD(DoTransition)
	SCRIPT_VOID_METHOD(ResetParams)
	SCRIPT_VOID_METHOD(SaveParams)
END_SCRIPT_CLASS

FlowScript::FlowScript(const Str& _scriptPath)
{
	m_pScriptRoot = &(World::GetInstance().CreateNewEntity(_scriptPath.GetArray()));

	m_bAlwaysDraw = FALSE;
	m_bAlwaysUpdate = FALSE;
	m_bDoTransition = FALSE;
	m_bCleanResourceTranstion = FALSE;

	m_bMustClose = FALSE;
	m_pScript = NEW Script();
	m_sScriptPath = _scriptPath;
	
}

//----------------------------------

FlowScript::~FlowScript()
{
	World::GetInstance().RemoveEntity(m_pScriptRoot);
}

//----------------------------------

void FlowScript::Init()
{
	if (!m_pScript->LoadFromFile(m_sScriptPath) || !m_pScript->Execute())
	{
		LOGGER_LogError("Unable to load menu script %s !!\n", m_sScriptPath.GetArray());
		m_pScript.SetNull();
		Close();
	}
	else
	{
		PROFILER_Begin("FLOWSCRIPT_Init");
		m_pScript->Call("Init", *this);
		PROFILER_End();

		LoadEntitiesFromFile();

		World::GetInstance().Update(0.f);
	}
}

//----------------------------------

void	FlowScript::Update(Float _dTime)
{
	if( m_pTopScript.IsValid() )
	{
		if( m_pTopScript->MustClose() )
		{
			if( m_pTopScript->HasNextScript() )
			{
				if( m_pTopScript->m_bCleanResourceTranstion )
					GoToScriptWithClean(m_pTopScript->GetNextScript());
				else
					GoToScript(m_pTopScript->GetNextScript());
			}
			
			m_pTopScript.SetNull();
			ShowAllEntities();
		}
		else
			m_pTopScript->Update(_dTime);
	}
	else
	{
		if (m_pScript.IsValid())
		{
			if (m_pScript->HasFunction("Update"))
				m_pScript->Call("Update", *this, _dTime);
		}
	}
}

//----------------------------------

void		FlowScript::PrepareDraw(Renderer*	_renderer)
{
	if (m_pTopScript.IsValid())
	{
		m_pTopScript->PrepareDraw(_renderer);
	}
	else
	{
		// todo
	}
}

//----------------------------------

void		FlowScript::LaunchEvent(const Name& _nEvent)
{
	if( m_pScript.IsValid() )
	{
		m_pScript->Call("OnEvent",*this,_nEvent.GetStr());
	}

	if (m_pTopScript.IsValid())
	{
		m_pTopScript->LaunchEvent(_nEvent);
	}
}

//----------------------------------

Entity&	FlowScript::CreateNewEntityFromParent(const Name& _name, Entity& _parent)
{ 
	// todo check parent is script root (assert)

	return CreateNewEntity(_name,&_parent);
}

//----------------------------------

Entity&	FlowScript::CreateNewEntity(const Name& _name, Entity* _parent /*= NULL*/)
{
	Entity* parent = _parent;
	if( !parent ) 
		parent = m_pScriptRoot;

	Entity& entity = World::GetInstance().CreateNewEntity(_name,parent);
	return entity;
}

//----------------------------------

void	FlowScript::PushScript(const Str& _scriptPath)
{
	if (m_pTopScript.IsNull())
	{
		LOGGER_Log("Flow - push script %s\n", _scriptPath.GetArray());
		m_pTopScript = NEW FlowScript(_scriptPath);
		m_pTopScript->Init();
		HideAllEntities();
	}
	else
	{
		m_pTopScript->PushScript(_scriptPath);
	}
}

//----------------------------------

void	FlowScript::GoToScript(const Str& _scriptPath, Bool _bUseTransition /*= FALSE*/)
{
	LOGGER_Log("Flow - go to script %s\n",_scriptPath.GetArray());
	Close();
	m_sNextScript = _scriptPath;
	m_bNeedTranstion = _bUseTransition;
	m_bCleanResourceTranstion = FALSE;
}

//----------------------------------

void	FlowScript::GoToScriptWithClean(const Str& _scriptPath, Bool _bUseTransition /*= FALSE*/)
{
	GoToScript(_scriptPath,_bUseTransition);
	m_bCleanResourceTranstion = TRUE;
}

//----------------------------------

void	FlowScript::ShowAllEntities()
{
	m_pScriptRoot->SkipUpdate(FALSE);
	m_pScriptRoot->SkipDraw(FALSE);
}

//----------------------------------

void	FlowScript::HideAllEntities()
{
	m_pScriptRoot->SkipUpdate(!m_bAlwaysUpdate);
	m_pScriptRoot->SkipDraw(!m_bAlwaysDraw);
}

//----------------------------------

void	FlowScript::OnPause()
{
	if( m_pTopScript.IsValid() )
	{
		m_pTopScript->OnPause();
	}
	else if( m_pScript.IsValid() && m_pScript->HasFunction("OnButtonJustReleased") )
	{
		m_pScript->Call("OnButtonJustReleased",*this,Str("PAUSE"),TRUE);
	}
}

//----------------------------------

struct EntityNode
{
	Entity* m_pWorldEntity;
	rapidxml::xml_node<>* m_Node;
};

//----------------------------------

void	FlowScript::SaveEntitiesToFile()
{
	World::GetInstance().SaveEntitiesToFile(ComputeEntitiesFilePath(), m_pScriptRoot, FALSE);
}

//----------------------------------

void	FlowScript::LoadEntitiesFromFile()
{
	World::GetInstance().LoadEntitiesFromFile(ComputeEntitiesFilePath(), m_pScriptRoot, TRUE);
}

//----------------------------------

Str		FlowScript::ComputeEntitiesFilePath() const
{
	Str currentPath = m_pScript->GetPath();
	currentPath = currentPath.GetTextInsideRange(0, currentPath.GetLength() - 5);
	currentPath += ".ent";
	return currentPath;
}