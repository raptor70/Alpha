#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "world/component.h"

BEGIN_COMPONENT_CLASS(Script)
	DECLARE_SCRIPT_CLASS(EntityComponentScript)
public:
	EntityComponentScript();
	~EntityComponentScript();

	virtual Bool IsUpdater() const OVERRIDE { return TRUE; }
	virtual Bool IsDrawer() const OVERRIDE { return FALSE; }
	virtual Bool ReceiveEvent()	const OVERRIDE { return TRUE; }
	
	virtual void	Initialize() OVERRIDE;
	virtual void	OnParamChanged() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;
	virtual void	OnEvent(const EntityEvent& _event) OVERRIDE;

	void LoadEntitiesFromFile();

protected:
	Script	m_Script;
	Bool m_bHasUpdateFunction;

END_COMPONENT_CLASS

#endif
