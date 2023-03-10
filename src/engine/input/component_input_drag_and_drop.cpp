#include "component_input_drag_and_drop.h"

#include "input/input_manager.h"
#include "core/name_static.h"
#include "component_input_click.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentInputDragAndDrop,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(InputDragAndDrop)
	INIT_ENTITY_PARAMS(Float, Speed, 100.f);
	INIT_ENTITY_PARAMS(Float, InertiaSpeedFactor, 0.f);
	INIT_ENTITY_PARAMS(Float, InertiaFrictionFactor, 1.f);
END_DEFINE_COMPONENT

EntityComponentInputDragAndDrop::EntityComponentInputDragAndDrop():SUPER()
{
}

//------------------------------

EntityComponentInputDragAndDrop::~EntityComponentInputDragAndDrop()
{
}

//------------------------------

void	EntityComponentInputDragAndDrop::Initialize()
{
	SUPER::Initialize();

	DEBUG_RequireMessage(GetEntity()->GetComponent<EntityComponentInputClick>(),"Component DragAndDrop need ComponentInputClick\n");

	m_bDrag = FALSE;
	m_vSpeed = Vec3f::Zero;
}

//------------------------------

void	EntityComponentInputDragAndDrop::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	if( m_bDrag ) 
	{
		EntityComponentInputClick* pClick = GetEntity()->GetComponent<EntityComponentInputClick>();
		const Segment& seg = InputManager::GetInstance().GetMouse(pClick->GetTouchId()).GetWorldSegment();
		Vec3f inter = Vec3f::Zero;
		if( GetEntity()->GetBBox().IntersectSegment(seg,inter) )
		{
			Vec3f dest = inter;
			LinearSmooth(GetEntity()->GetWorldPos(),dest,GET_ENTITY_PARAMS(Float,DragSpeed),_dTime,dest);
			m_vSpeed = GET_ENTITY_PARAMS(Float,DragInertiaSpeedFactor) * (dest - GetEntity()->GetWorldPos()) / _dTime;
			GetEntity()->SetWorldPos(dest);
		}
	}
	else
	{
		Vec3f curPos = GetEntity()->GetWorldPos();
		Vec3f accel = - m_vSpeed * GET_ENTITY_PARAMS(Float,DragInertiaFrictionFactor);
		m_vSpeed += accel * _dTime;
		curPos += m_vSpeed * _dTime;
		GetEntity()->SetWorldPos(curPos);
	}
}

//------------------------------

void	EntityComponentInputDragAndDrop::OnEvent(const EntityEvent& _event)
{
	if( _event.m_nName == NAME(ON_PRESS) )
	{
		m_bDrag = TRUE;
		GetEntity()->LaunchEvent(EntityEvent(NAME(ON_DRAG)));
	}
	else if( _event.m_nName == NAME(ON_RELEASE) )
	{
		m_bDrag = FALSE;
		GetEntity()->LaunchEvent(EntityEvent(NAME(ON_DROP)));
	}
}
