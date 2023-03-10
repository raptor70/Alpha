#ifndef __COMPONENT_TEXT_H__
#define __COMPONENT_TEXT_H__

#include "world/component.h"
#include "3d/mesh.h"

BEGIN_COMPONENT_CLASS(Text)
	DECLARE_SCRIPT_CLASS(EntityComponentText)
public:
	EntityComponentText();
	~EntityComponentText();

	virtual Bool IsUpdater() const	{ return FALSE; }
	virtual Bool IsDrawer()	const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const	{ return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Draw(Renderer*	_renderer) OVERRIDE;
	virtual void	OnParamChanged() OVERRIDE;

protected:
	void	ComputeGeometry();

	Str	m_Text;
	Bool m_bChanged;
	Mesh	m_Mesh;
END_COMPONENT_CLASS

#endif
