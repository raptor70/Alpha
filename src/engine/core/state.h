#ifndef __STATE_H__
#define __STATE_H__

class CState
{
	friend class CStateMachine;
public:
	CState();
	virtual ~CState();

	virtual		void	BeginState(const Name& _previousState) = 0;
	virtual		void	Update(Float _dTime) = 0;
	virtual		void	EndState(const Name& _nextState) = 0;

	void	GoToState(const Name& _newState)	
	{ 
		DEBUG_Require(m_PendingState == Name::Null);
		m_PendingState = _newState; 
	}
	const Name&		GetName() const				{ return m_Name; }

private:
	void			SetName(const Name& _name)	{ m_Name = _name; }

	const Name&		GetPendingState() const				{ return m_PendingState; }
	void			ClearPendingState()					{ m_PendingState = Name::Null; }

	Name	m_Name;
	Name	m_PendingState;
};

typedef RefTo<CState>		StateRef;
typedef ArrayOf<StateRef>	StateRefArray;

//---------------------------------------------

BEGIN_SUPER_CLASS(CEmptyState,CState)
public:
	virtual		void	BeginState(const Name& _previousState) OVERRIDE {}
	virtual		void	Update(Float _dTime) OVERRIDE {}
	virtual		void	EndState(const Name& _nextState) OVERRIDE {}
END_SUPER_CLASS

//---------------------------------------------

class CStateMachine
{
public:
	CStateMachine();
	virtual ~CStateMachine();

	template<class T>
	void	Register(const Name& _name);
	void	Update(Float _dTime);

	Bool	SetCurrentState(const Name& _name);
	Bool	ForceCurrentState(const Name& _name);
	Bool	StopCurrentState(const Name& _nextState = Name::Null);
	const Name& GetCurrentState() const	{ if( m_CurrentState >= 0 ) return m_States[m_CurrentState]->GetName(); return Name::Null; }

protected:
	void	Register(const Name& _name, StateRef& _newState);
	S32		GetStateId(const Name& _name);

	StateRefArray	m_States;
	S32				m_CurrentState;
};

//---------------------------------------------

template<class T>
void	CStateMachine::Register(const Name& _name)
{
	StateRef newState = NEW T;
	newState->SetName(_name);
	m_States.AddLastItem(newState);
}

#endif