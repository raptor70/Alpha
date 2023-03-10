#include "editor_action.h"
#ifdef USE_EDITOR

EditorActionMoveEntity::EditorActionMoveEntity(Entity* _entity, const Vec3f& _sourcePos, const Vec3f& _targetPos):SUPER()
{
	m_pEntity = _entity;
	m_vSourcePos = _sourcePos;
	m_vTargetPos = _targetPos;
}

//---------------------------------------

void EditorActionMoveEntity::Do()
{
	m_pEntity->SetWorldPos(m_vTargetPos);
}

//---------------------------------------

void EditorActionMoveEntity::UnDo()
{
	m_pEntity->SetWorldPos(m_vSourcePos);
}

//---------------------------------------

EditorActionRotateEntity::EditorActionRotateEntity(Entity* _entity, const Quat& _sourceRot, const Quat& _targetRot) :SUPER()
{
	m_pEntity = _entity;
	m_vSourceRot = _sourceRot;
	m_vTargetRot = _targetRot;
}

//---------------------------------------

void EditorActionRotateEntity::Do()
{
	m_pEntity->SetRot(m_vTargetRot);
}

//---------------------------------------

void EditorActionRotateEntity::UnDo()
{
	m_pEntity->SetRot(m_vSourceRot);
}

//---------------------------------------

EditorActionScaleEntity::EditorActionScaleEntity(Entity* _entity, const Vec3f& _sourceScale, const Vec3f& _targetScale) :SUPER()
{
	m_pEntity = _entity;
	m_vSourceScale = _sourceScale;
	m_vTargetScale = _targetScale;
}

//---------------------------------------

void EditorActionScaleEntity::Do()
{
	m_pEntity->SetScale(m_vTargetScale);
}

//---------------------------------------

void EditorActionScaleEntity::UnDo()
{
	m_pEntity->SetScale(m_vSourceScale);
}

//---------------------------------------


#endif