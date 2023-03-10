#ifndef __FLOW_SCRIPT_H__
#define __FLOW_SCRIPT_H__

class FlowScript;
typedef RefTo<FlowScript> FlowScriptRef;

class FlowScript
{
	DECLARE_SCRIPT_CLASS(FlowScript)
public:
	FlowScript() {}
	FlowScript(const Str& _scriptPath);
	virtual ~FlowScript();

	void Init();

	const Str&			GetScriptPath() const		{ if( m_pScript.IsValid() ) return m_pScript->GetPath(); return Str::Void; }

	Entity&			CreateNewEntityFromParent(const Name& _name, Entity& _parent);
	Entity&			CreateNewEntity(const Name& _name, Entity* _parent = NULL);
	Entity&			GetRootEntity() const		{ return *m_pScriptRoot; }

	void				Update(Float _dTime);
	void				PrepareDraw(Renderer*	_renderer);
	void				LaunchEvent(const Name& _nEvent);

	void				PushScript(const Str& _scriptPath);
	void				PopScript()				{ LOGGER_Log("Flow - pop script \n"); Close(); }
	Bool				HasTopScript() const	{ return m_pTopScript.IsValid(); }
	FlowScriptRef		GetTopScript()			{ return m_pTopScript; }

	void				Close()					{ m_bMustClose = TRUE; }
	Bool				MustClose() const		{ return m_bMustClose; }

	void				GoToScript(const Str& _scriptPath, Bool _bUseTransition = FALSE);
	void				GoToScriptWithClean(const Str& _scriptPath, Bool _bUseTransition = FALSE);
	Bool				HasNextScript() const	{ return !m_sNextScript.IsEmpty(); }
	const Str&			GetNextScript() const	{ return m_sNextScript; }
	Bool				NeedTransition() const	{ return m_bNeedTranstion; }
	Bool				NeedCleanDuringTransition() const	{ return m_bCleanResourceTranstion; }

	void				ShowAllEntities();
	void				HideAllEntities();

	ParamsTable&		GetParams()												{ return WORLDPARAMS; }
	void				AddParamFloat(const Name& _name, const Float _value)	{ GetParams().GetOrAddValue(_name)->SetFloat(_value); }
	void				AddParamStr(const Name& _name, const Str& _value)		{ GetParams().GetOrAddValue(_name)->SetStr(_value); }
	Float				GetParamFloat(const Name& _name)						{ return GetParams().GetOrAddValue(_name)->GetFloat(); }
	const Str&			GetParamStr(const Name& _name)							{ return GetParams().GetOrAddValue(_name)->GetStr(); }
	void				ResetParams()											{ GetParams().Reset(); }
    void                SaveParams()                                            { GetParams().SaveToFile(); }

	void				SetAlwaysDraw(Bool	_enable)			{ m_bAlwaysDraw = _enable; }
	Bool				AlwaysDraw() const						{ return m_bAlwaysDraw; }
	void				SetAlwaysUpdate(Bool	_enable)		{ m_bAlwaysUpdate = _enable; }

	void				DoTransition()							{ m_bDoTransition = TRUE; }
	Bool				MustDoTransition() const				{ return m_bDoTransition; }

	void				OnPause();

	void				SaveEntitiesToFile();
	void				LoadEntitiesFromFile();
	Str					ComputeEntitiesFilePath() const;

protected:
	FlowScriptRef		m_pTopScript;
	Entity*			m_pScriptRoot;
	RefTo<Script>		m_pScript;
	Str					m_sScriptPath;
	Bool				m_bMustClose;
	Str					m_sNextScript;
	Bool				m_bAlwaysDraw;
	Bool				m_bAlwaysUpdate;
	Bool				m_bDoTransition;
	Bool				m_bNeedTranstion;
	Bool				m_bCleanResourceTranstion;
};

#endif
