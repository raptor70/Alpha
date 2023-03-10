#ifndef __COMPONENT_INPUT_DRAG_AND_DROP_H__
#define __COMPONENT_INPUT_DRAG_AND_DROP_H__

#include "world/component.h"

BEGIN_COMPONENT_CLASS(InputDragAndDrop)
	DECLARE_SCRIPT_CLASS(EntityComponentInputDragAndDrop)
public:
	EntityComponentInputDragAndDrop();
	~EntityComponentInputDragAndDrop();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer() const	{ return FALSE; }
	virtual Bool ReceiveEvent()	const { return TRUE; }

	virtual void	Initialize();
	virtual void	Update(Float _dTime);
	virtual void	OnEvent(const EntityEvent& _event) OVERRIDE;

protected:
	Bool	m_bDrag;
	Vec3f	m_vSpeed;
END_COMPONENT_CLASS

#endif
