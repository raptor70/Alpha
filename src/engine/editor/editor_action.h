#ifndef __EDITOR_ACTION_H__
#define __EDITOR_ACTION_H__
#include "editor_include.h"
#ifdef USE_EDITOR

class EditorAction
{
public:
	EditorAction() {}

	virtual void	Do() = 0;
	virtual void	UnDo() = 0;
};

typedef RefTo<EditorAction> EditorActionRef;
typedef ArrayOf<EditorActionRef> EditorActionRefArray;

//---------------------------------------

BEGIN_SUPER_CLASS(EditorActionMoveEntity, EditorAction)
public:
	EditorActionMoveEntity(Entity* _entity, const Vec3f& _sourcePos, const Vec3f& _targetPos);
	virtual void	Do() OVERRIDE;
	virtual void	UnDo() OVERRIDE;

protected:
	Entity*	m_pEntity;
	Vec3f		m_vSourcePos;
	Vec3f		m_vTargetPos;
END_SUPER_CLASS


//---------------------------------------

BEGIN_SUPER_CLASS(EditorActionRotateEntity, EditorAction)
public:
	EditorActionRotateEntity(Entity* _entity, const Quat& _sourceRot, const Quat& _targetRot);
	virtual void	Do() OVERRIDE;
	virtual void	UnDo() OVERRIDE;

protected:
	Entity*	m_pEntity;
	Quat		m_vSourceRot;
	Quat		m_vTargetRot;
	END_SUPER_CLASS

//---------------------------------------

BEGIN_SUPER_CLASS(EditorActionScaleEntity, EditorAction)
public:
	EditorActionScaleEntity(Entity* _entity, const Vec3f& _sourceScale, const Vec3f& _targetScale);
	virtual void	Do() OVERRIDE;
	virtual void	UnDo() OVERRIDE;

protected:
	Entity*	m_pEntity;
	Vec3f		m_vSourceScale;
	Vec3f		m_vTargetScale;
END_SUPER_CLASS

#endif // USE_EDITOR

#ifdef USE_EDITOR_V2
class EditorAction
{
public:
	virtual void	Do() = 0;
	virtual void	UnDo() = 0;
};

typedef RefTo<EditorAction>	EditorActionRef;

class EditorActionDummy : public EditorAction
{
public:
	EditorActionDummy(const Char* _pName) { m_pName = _pName; }
	virtual void	Do() { LOGGER_Log("Do %s\n", m_pName); }
	virtual void	UnDo() { LOGGER_Log("UnDo %s\n", m_pName); }

	const Char* m_pName = NULL;

};

#endif

#endif // __EDITOR_ACTION_H__