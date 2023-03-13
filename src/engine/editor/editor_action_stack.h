#ifndef __EDITOR_ACTION_STACK_H__
#define __EDITOR_ACTION_STACK_H__
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
#include <editor/editor_action.h>

class EditorActionStack
{
public:
	template<typename T, class... Args>
	void DoAction(Args... _args)
	{
		Node* pNewNode = NEW Node;
		pNewNode->m_pAction = NEW T(_args...);

		if (m_pCurrentNode)
		{
			m_pCurrentNode->m_pNext = pNewNode;
			pNewNode->m_pPrev = m_pCurrentNode;
			m_pCurrentNode = m_pCurrentNode->m_pNext;
		}
		else // first
		{
			m_pFirstNode = pNewNode;
			m_pCurrentNode = m_pFirstNode;
		}
		
		pNewNode->m_pAction->Do();
	}

	void UnDo()
	{
		if (m_pCurrentNode)
		{
			m_pCurrentNode->m_pAction->UnDo();
			m_pCurrentNode = m_pCurrentNode->m_pPrev;
		}
	}

	void ReDo()
	{
		if (m_pCurrentNode)
		{
			if (m_pCurrentNode->m_pNext)
			{
				m_pCurrentNode = m_pCurrentNode->m_pNext;
				m_pCurrentNode->m_pAction->Do();
			}
		}
		else if (m_pFirstNode)
		{
			m_pCurrentNode = m_pFirstNode;
			m_pCurrentNode->m_pAction->Do();
		}
	}

	void Flush()
	{
		m_pCurrentNode = NULL;
		m_pFirstNode = NULL;
	}

	struct Node
	{
		EditorActionRef	m_pAction;
		RefTo<Node>		m_pNext;
		PtrTo<Node>		m_pPrev;
	};

	RefTo<Node>	m_pFirstNode;		// chain
	PtrTo<Node>	m_pCurrentNode;
};

#endif

#endif // __EDITOR_ACTION_H__