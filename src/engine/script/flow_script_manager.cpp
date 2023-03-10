#include "flow_script_manager.h"

#include "2d/component_button.h"

//----------------------------------

FlowScriptManager::FlowScriptManager(const Str& _firstScript)
{
	m_pFlowScript = NEW FlowScript(_firstScript);
	m_pFlowScript->Init();
	//m_pFlowScript->GetParams().LoadFromFile();
}

//----------------------------------

FlowScriptManager::~FlowScriptManager()
{
}

//----------------------------------

void	FlowScriptManager::Update(Float _dTime)
{
	if( m_pTransitionScript.IsValid() )
	{
		PROFILER_Begin("FLOW_Update");
		m_pTransitionScript->Update(_dTime);
		PROFILER_End();
		if( !m_bTransitionDone && m_pTransitionScript->MustDoTransition() )
		{
			PROFILER_Begin("FLOW_CreateScript");
			//m_pFlowScript->GetParams().SaveToFile();
			m_pFlowScript.SetNull();
			World::GetInstance().Update(0.f);
			if( m_bTransitionWithClean )
				ResourceManager::GetInstance().CleanResources();
			m_pFlowScript = NEW FlowScript(m_sTransitionScriptPath);
			m_pFlowScript->Init();
			if( m_pFlowScript->HasTopScript() )
			{
				m_bTranstionAlwaysDraw = m_pFlowScript->GetTopScript()->AlwaysDraw();
				m_pFlowScript->GetTopScript()->SetAlwaysDraw(TRUE);
				m_pFlowScript->GetTopScript()->HideAllEntities();
			}
			else
			{
				m_bTranstionAlwaysDraw = m_pFlowScript->AlwaysDraw();
				m_pFlowScript->SetAlwaysDraw(TRUE);
				m_pFlowScript->HideAllEntities();
			}
			m_bTransitionDone = TRUE;
			PROFILER_End();
		}
		else if( m_pTransitionScript->MustClose() )
		{
			if( m_pFlowScript->HasTopScript() )
			{
				m_pFlowScript->GetTopScript()->ShowAllEntities();
				m_pFlowScript->GetTopScript()->SetAlwaysDraw(m_bTranstionAlwaysDraw);
			}
			else
			{
				m_pFlowScript->ShowAllEntities();
				m_pFlowScript->SetAlwaysDraw(m_bTranstionAlwaysDraw);
			}
			m_pTransitionScript.SetNull();
		}
	}
	else if( m_pFlowScript.IsValid() )
	{
		m_pFlowScript->Update(_dTime);

		if( m_pFlowScript->MustClose() )
		{
			if( m_pFlowScript->HasNextScript() )
			{
				if( m_pFlowScript->NeedTransition() )
				{
					PROFILER_Begin("FLOW_CreateTransition");
					m_pTransitionScript = NEW FlowScript("script/transition.lua");
					m_pTransitionScript->Init();
					m_sTransitionScriptPath = m_pFlowScript->GetNextScript();
					m_bTranstionAlwaysDraw = m_pFlowScript->AlwaysDraw();
					m_bTransitionDone = FALSE;
					m_bTransitionWithClean = m_pFlowScript->NeedCleanDuringTransition();
					m_pFlowScript->SetAlwaysDraw(TRUE);
					m_pFlowScript->HideAllEntities();
					PROFILER_End();
				}
				else
				{
					PROFILER_Begin("FLOW_CreateScriptNoTransition");
					//m_pFlowScript->GetParams().SaveToFile();
					Str path = m_pFlowScript->GetNextScript();
					m_pFlowScript.SetNull();
					World::GetInstance().Update(0.f);
					m_pFlowScript = NEW FlowScript(path);
					m_pFlowScript->Init();
					PROFILER_End();
				}
			}
			else
			{
				m_pFlowScript.SetNull();
			}
		}
	}
}

//----------------------------------

void	FlowScriptManager::PrepareDraw(Renderer*	_renderer)
{
	if (m_pTransitionScript.IsValid())
	{
		m_pTransitionScript->PrepareDraw(_renderer);
	}
	else if (m_pFlowScript.IsValid())
	{
		m_pFlowScript->PrepareDraw(_renderer);
	}
}

//----------------------------------

void	FlowScriptManager::ForceNextScript(const Str& scriptPath)
{
	if( m_pFlowScript.IsValid() )
	{
		m_pFlowScript->GoToScript(scriptPath,FALSE);
	}
	else
	{
		m_pFlowScript = NEW FlowScript(scriptPath);
		m_pFlowScript->Init();
	}
}

//----------------------------------

void	FlowScriptManager::LaunchEvent(const Name& _nEvent)
{
	if( m_pFlowScript.IsValid() )
	{
		m_pFlowScript->LaunchEvent(_nEvent);
	}
}

//----------------------------------

FlowScript*	FlowScriptManager::GetScriptForEntity(const Entity* _entity)
{
	const Entity* pCurrent = _entity;
	while (pCurrent)
	{
		FlowScript* pCurrentScript = m_pFlowScript;
		while (pCurrentScript)
		{
			if (&pCurrentScript->GetRootEntity() == pCurrent)
				return pCurrentScript;

			pCurrentScript = pCurrentScript->GetTopScript();
		}

		pCurrent = pCurrent->GetParent();
	}

	return NULL;
}

//----------------------------------

Bool		FlowScriptManager::IsRootScriptEntity(const Entity* _entity)
{
	FlowScript* pCurrentScript = m_pFlowScript;
	while (pCurrentScript)
	{
		if (&pCurrentScript->GetRootEntity() == _entity)
			return TRUE;

		pCurrentScript = pCurrentScript->GetTopScript();
	}

	return FALSE;
}

//----------------------------------

void	FlowScriptManager::OnPause()
{
	if( m_pFlowScript.IsValid() )
	{
		m_pFlowScript->OnPause();
	}
}

//----------------------------------

void	FlowScriptManager::SaveAll()
{
	FlowScript* pCurrentScript = m_pFlowScript;
	while (pCurrentScript)
	{
		pCurrentScript->SaveEntitiesToFile();
		pCurrentScript = pCurrentScript->GetTopScript();
	}
}