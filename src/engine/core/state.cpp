#include "state.h"

CState::CState()
{
	m_Name = m_PendingState = Name::Null;
}

//---------------------------------------------

CState::~CState()
{
}

//---------------------------------------------

CStateMachine::CStateMachine()
{
	m_CurrentState = -1;
}

//---------------------------------------------

CStateMachine::~CStateMachine()
{
}

//---------------------------------------------

void	CStateMachine::Update(Float _dTime)
{
	if( m_CurrentState>= 0 ) 
	{
		m_States[m_CurrentState]->Update(_dTime);

		while(m_States[m_CurrentState]->GetPendingState() != Name::Null)
		{
			Name pendingState = m_States[m_CurrentState]->GetPendingState();
			//m_States[m_CurrentState]->ClearPendingState();
			m_States[m_CurrentState]->m_PendingState = Name::Null;
			if( SetCurrentState(pendingState))
			{
				m_States[m_CurrentState]->Update(_dTime);
			}
		}
	}
}

//---------------------------------------------

Bool	CStateMachine::SetCurrentState(const Name& _name)
{
	if( m_CurrentState >= 0 && m_States[m_CurrentState]->GetName() == _name) 
		return FALSE;

	return ForceCurrentState(_name);
}

//---------------------------------------------

Bool	CStateMachine::ForceCurrentState(const Name& _name)
{
	S32 newStateIdx = GetStateId(_name);
	if( newStateIdx < 0 )
	{
		LOGGER_LogError("Unknown state %s\n",_name.GetStr().GetArray());
		return FALSE;
	}

	Name previous = Name::Null;
	if( m_CurrentState >= 0 )
	{
		previous = m_States[m_CurrentState]->GetName();
		StopCurrentState(_name);
	}
	//LOGGER_Log("(%x)%s Begin\n",m_States[newStateIdx].GetPtr(),m_States[newStateIdx]->GetName().GetStr().GetArray());
	m_States[newStateIdx]->BeginState(previous);
	m_CurrentState = newStateIdx;
	return TRUE;
}

//---------------------------------------------

Bool	CStateMachine::StopCurrentState(const Name& _nextState)
{
	m_States[m_CurrentState]->EndState(_nextState);
	//LOGGER_Log("(%x)%s End\n",m_States[m_CurrentState].GetPtr(),m_States[m_CurrentState]->GetName().GetStr().GetArray());
	m_CurrentState = -1;
	return TRUE;
}

//---------------------------------------------

S32		CStateMachine::GetStateId(const Name& _name)
{
	for(U32 i=0; i<m_States.GetCount(); i++ )
	{
		if( m_States[i]->GetName() == _name )
			return i;
	}

	return -1;
}