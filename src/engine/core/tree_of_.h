#ifndef __TREE_OF_H__
#define __TREE_OF_H__

template<typename T>
class TreeOf
{
public:
	class Node
	{
	public:
		Node():m_pLeft(NULL), m_pRight(NULL){}
		~Node() {};

		T m_Value;
		Node*	m_pLeft;
		Node*	m_pRight;
	};

	TreeOf() :m_pRoot(NULL), m_iCount(0) {}
	virtual ~TreeOf()	
	{
		Flush();
	}

	TreeOf& operator=(const TreeOf& _other)
	{
		Clone(_other);
		return *this;
	}

	T* AddItem(const T& _newItem);
	Bool RemoveItem(const T& _newItem, T* _outValue =NULL);
	U32 GetCount() const { return m_iCount;  }
	Node* GetNode(const T& _item) const
	{
		Node* pCur = m_pRoot;
		while (pCur)
		{
			if (_item == pCur->m_Value)
				return pCur;

			if (_item < pCur->m_Value) // left
			{
				pCur = pCur->m_pLeft;
			}
			else // right
			{
				pCur = pCur->m_pRight;
			}
		}

		return NULL;
	}

	void GetAllNodes(ArrayOf<Node*>& _outNodes)
	{
		_outNodes.SetEmpty();
		if (!m_pRoot)
			return;

		_outNodes.AddLastItem(m_pRoot);
		U32 i = 0;
		while (i < _outNodes.GetCount())
		{
			Node* pCurrent = _outNodes[i];
			if(pCurrent->m_pLeft)
				_outNodes.AddLastItem(pCurrent->m_pLeft);

			if (pCurrent->m_pRight)
				_outNodes.AddLastItem(pCurrent->m_pRight);

			i++;
		}
	}

	void Clone(const TreeOf& _other);
	void Flush();
	void SetEmpty()
	{
		Flush();
	}

private:
	void	Relink(Node** _pTargetLink, Node* _pLeft, Node* _pRight);

	Node*	m_pRoot;
	U32		m_iCount;
};

//------------------------------------------

template<typename T>
T* TreeOf<T>::AddItem(const T& _newItem)
{
	TreeOf::Node* pNewNode = NEW TreeOf::Node;
	pNewNode->m_Value = _newItem;
	m_iCount++;

	if (!m_pRoot)
	{
		m_pRoot = pNewNode;
	}
	else
	{
		TreeOf::Node* pCur = m_pRoot;
		while (pCur)
		{
			if (_newItem < pCur->m_Value) // left
			{
				if (!pCur->m_pLeft)
				{
					pCur->m_pLeft = pNewNode;
					break;
				}

				pCur = pCur->m_pLeft;
			}
			else // right
			{
				if (!pCur->m_pRight)
				{
					pCur->m_pRight = pNewNode;
					break;
				}
				
				pCur = pCur->m_pRight;
			}
		}
	}

	return &(pNewNode->m_Value);
}

//------------------------------------------

template<typename T>
Bool TreeOf<T>::RemoveItem(const T& _oldItem, T* _outValue /*=NULL*/)
{
	TreeOf::Node* pParent = NULL;
	TreeOf::Node* pCur = m_pRoot;
	while (pCur)
	{
		if (pCur->m_Value == _oldItem)
			break;

		pParent = pCur;
		if (_oldItem < pCur->m_Value) // left
		{
			pCur = pCur->m_pLeft;
		}
		else // right
		{
			pCur = pCur->m_pRight;
		}
	}

	if (!pCur)
		return FALSE;

	if (_outValue)
		(*_outValue) = pCur->m_Value;

	if (!pParent) // root case
	{
		Relink(&(m_pRoot), pCur->m_pLeft, pCur->m_pRight);
	}
	else
	{
		if (pParent->m_pLeft == pCur) // left of parent
		{
			Relink(&(pParent->m_pLeft), pCur->m_pLeft, pCur->m_pRight);
		}
		else // rigth of parent
		{
			Relink(&(pParent->m_pRight), pCur->m_pLeft, pCur->m_pRight);
		}
	}

	DELETE pCur;
	m_iCount--;
	return TRUE;
}

//------------------------------------------

template<typename T>
void	TreeOf<T>::Relink(Node** _pTargetLink, Node* _pLeft, Node* _pRight)
{
	(*_pTargetLink) = _pRight;
	if (!_pRight)
		(*_pTargetLink) = _pLeft;
	else if (_pLeft)
	{
		Node* pLowerLeft = _pRight;
		while (pLowerLeft->m_pLeft)
			pLowerLeft = pLowerLeft->m_pLeft;
		pLowerLeft->m_pLeft = _pLeft;
	}
}

//------------------------------------------

template<typename T>
void TreeOf<T>::Clone(const TreeOf<T>& _other)
{
	m_iCount = _other.m_iCount;
	if (m_iCount == 0)
		return;

	ArrayOf<Node*> alls;
	alls.AddLastItem(_other.m_pRoot);
	U32 i = 0;
	while (i < alls.GetCount())
	{
		Node* pCurrent = alls[i];

		AddItem(pCurrent->m_Value);

		if (pCurrent->m_pLeft)
			alls.AddLastItem(pCurrent->m_pLeft);

		if (pCurrent->m_pRight)
			alls.AddLastItem(pCurrent->m_pRight);

		i++;
	}
}

//------------------------------------------

template<typename T>
void	TreeOf<T>::Flush()
{
	if (!m_pRoot)
		return;

	ArrayOf<TreeOf::Node*> nodes;
	nodes.SetItemCount(m_iCount);
	nodes[0] = m_pRoot;
	U32 nodeCount = 1;
	U32 currentIdx = 0;
	while (currentIdx < nodeCount)
	{
		TreeOf::Node* currentNode = nodes[currentIdx];
		if (currentNode->m_pLeft)
		{
			nodes[nodeCount] = currentNode->m_pLeft;
			nodeCount++;
		}

		if (currentNode->m_pRight)
		{
			nodes[nodeCount] = currentNode->m_pRight;
			nodeCount++;
		}

		DELETE currentNode;
		currentIdx++;
	}

	m_pRoot = NULL;
}

#endif