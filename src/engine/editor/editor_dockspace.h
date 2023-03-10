#ifndef __EDITOR_DOCKSPACE_H__
#define __EDITOR_DOCKSPACE_H__
#include "editor_include.h"

#ifdef USE_EDITOR_V2
class EditorPanel;

class EditorDockspace
{
public:
	enum class Dir
	{
		None = -1,
		Central,
		Left,
		Right,
		Top,
		Bottom,
		Count
	};

	EditorDockspace(const Char* _sName) 
	{ 
		m_sName = _sName; 
	}

	void Update(Float _dTime);
	void DockPanel(EditorPanel* _pPanel, Dir _dir);

protected:
	Str	m_sName;
	ImGuiID	m_ID = 0;
	Vec2f	m_MainSplitRatio = Vec2f::One;
	ImGuiID m_SplitIDs[(U64)Dir::Count] = { 0 };
};

typedef RefTo<EditorDockspace>	EditorDockspaceRef;
typedef PtrTo<EditorDockspace>	EditorDockspacePtr;
#endif
#endif // __EDITOR_ACTION_H__