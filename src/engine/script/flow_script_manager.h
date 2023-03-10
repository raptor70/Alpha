#ifndef __FLOW_SCRIPT_MANAGER_H__
#define __FLOW_SCRIPT_MANAGER_H__

#include "flow_script.h"

class FlowScriptManager
{
public:
	FlowScriptManager(const Str& _firstScript);
	virtual ~FlowScriptManager();

	void				Update(Float _dTime);
	void				PrepareDraw(Renderer*	_renderer);
	Bool				IsDone() const	{ return m_pFlowScript.IsNull(); }

	void				ForceNextScript(const Str& scriptPath);

	const Str&			GetCurrentScriptPath() const		{ if( m_pFlowScript.IsValid() ) return m_pFlowScript->GetScriptPath(); return Str::Void; }
	ParamsTable*		GetCurrentScriptParams()		{ if( m_pFlowScript.IsValid() ) return &(m_pFlowScript->GetParams()); return NULL; }

	void				LaunchEvent(const Name& _nEvent);

	FlowScript*		GetScriptForEntity(const Entity* _entity);
	Bool				IsRootScriptEntity(const Entity* _entity);

	void				OnPause();
	void				SaveAll();

protected:
	FlowScriptRef			m_pFlowScript;
	FlowScriptRef			m_pTransitionScript;
	Bool					m_bTranstionAlwaysDraw;
	Str						m_sTransitionScriptPath;
	Bool					m_bTransitionDone;
	Bool					m_bTransitionWithClean;
};

#endif
