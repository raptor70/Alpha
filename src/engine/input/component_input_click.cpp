#include "component_input_click.h"

#include "input/input_manager.h"
#include "core/name_static.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentInputClick,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(InputClick)
	INIT_ENTITY_PARAMS(Bool, 2D, FALSE);
END_DEFINE_COMPONENT

EntityComponentInputClick::EntityComponentInputClick():SUPER()
{
	m_iTouchId = -1;
	m_vTouchPos = Vec3f::Zero;
	m_bIsIn = FALSE;
}

//------------------------------

EntityComponentInputClick::~EntityComponentInputClick()
{
}

//------------------------------

void	EntityComponentInputClick::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

Bool	EntityComponentInputClick::ComputeIsIn()
{
	Bool bWasIn = m_bIsIn;
	m_bIsIn = FALSE;
	m_IsInIndex.SetEmpty();
	for (S32 i = 0; i < MAX_MULTITOUCH_COUNT; i++)
	{
		Segment seg = InputManager::GetInstance().GetMouse(i).GetWorldSegment();
		if (GET_ENTITY_PARAMS(Bool, InputClick2D))
		{
			seg.m_vDirection = Vec3f::ZAxis;
			Vec2i pos = InputManager::GetInstance().GetMouse(i).GetPos();
			seg.m_vOrigin = Vec3f(Float(pos.x), Float(pos.y), -0.01f);
		}
		Vec3f inter;
		if (GetEntity()->GetBBox().IntersectSegment(seg, inter))
		{
			m_IsInIndex.AddLastItem(i);
			m_IsInPos.AddLastItem(inter);
			m_bIsIn = TRUE;
		}
	}

	if (m_bIsIn && !bWasIn)
		GetEntity()->LaunchEvent(EntityEvent(NAME(ON_HOVER)));

	if (!m_bIsIn && bWasIn)
		GetEntity()->LaunchEvent(EntityEvent(NAME(ON_END_HOVER)));

	return m_bIsIn;
}

//------------------------------

void	EntityComponentInputClick::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	if( m_iTouchId != -1 )
	{
		if( GetEntity()->IsHidden() )
		{
			m_iTouchId = -1;
		}
		else
		{
			Bool bIsIn = m_IsInIndex.Find(m_iTouchId);
			Bool isReleased = InputManager::GetInstance().GetMouse(m_iTouchId).IsReleased(m_TouchKey);
			if( isReleased && bIsIn)
			{
				GetEntity()->LaunchEvent(EntityEvent(NAME(ON_VALIDATE)));
				m_iTouchId = -1;
			}
			else if( isReleased )
			{	
				m_iTouchId = -1;
			}
		}

		if (m_iTouchId == -1)
		{
			GetEntity()->LaunchEvent(EntityEvent(NAME(ON_RELEASE)));
		}
	}
	else
	{
		if( !GetEntity()->IsHidden() )
		{
			S32Array ids;
			if( InputManager::GetInstance().GetAllMouseJustPressed(MOUSE_Left, ids) )
			{
				m_TouchKey = MOUSE_Left;
			}
			else if( InputManager::GetInstance().GetAllMouseJustPressed(MOUSE_Right, ids) )
			{
				m_TouchKey = MOUSE_Right;
			}

			for(U32 i=0; i<ids.GetCount(); i++)
			{
				S32 touchId = ids[i];
				U32 isinIdx = 0;
				Bool bIsIn = m_IsInIndex.Find(touchId, isinIdx);
				if(bIsIn)
				{
					m_iTouchId = touchId;
					m_vTouchPos = m_IsInPos[isinIdx];
					GetEntity()->LaunchEvent(EntityEvent(NAME(ON_PRESS)));
				}
			}
		}
	}
}
