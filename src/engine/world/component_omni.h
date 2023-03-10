#ifndef __COMPONENT_OMNI_H__
#define __COMPONENT_OMNI_H__

#include "component.h"

BEGIN_COMPONENT_CLASS(Omni)
public:
	EntityComponentOmni();
	~EntityComponentOmni();

	virtual Bool IsUpdater() const OVERRIDE	{ return FALSE; }
	virtual Bool IsDrawer()	const OVERRIDE { return TRUE; }
	virtual Bool ReceiveEvent()	const OVERRIDE{ return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Draw(Renderer*	_renderer) OVERRIDE;

protected:
	RendererOmniDataRef	m_pShaderData;
END_COMPONENT_CLASS

#endif
