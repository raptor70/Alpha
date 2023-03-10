#ifndef __EDITOR_PANEL_H__
#define __EDITOR_PANEL_H__
#include "editor_include.h"

#ifdef USE_EDITOR_V2
#include <editor/editor_dockspace.h>

//--------

class EditorPanel
{
public:
	EditorPanel(const Char* _title) : m_Title(_title) {}

	virtual void Update(Float _dTime);
	virtual void UpdatePanelContent(Float _dTime) = 0;
	void DockTo(EditorDockspacePtr _pDockspace, EditorDockspace::Dir _dir) { m_pDockspace = _pDockspace; m_DockDir = _dir; }

	const Str& GetTitle() const { return m_Title; }

protected:
	Str	m_Title;

	Bool					m_bUpdateDocking = TRUE;
	EditorDockspacePtr		m_pDockspace = NULL;
	EditorDockspace::Dir	m_DockDir = EditorDockspace::Dir::None;
};

typedef RefTo<EditorPanel> EditorPanelRef;
typedef ArrayOf<EditorPanelRef> EditorPanelRefArray;


#endif // USE_EDITOR_V2

#endif // __EDITOR_PANEL_H__