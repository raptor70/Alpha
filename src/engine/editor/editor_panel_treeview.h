#ifndef __EDITOR_PANEL_TREEVIEW_H__
#define __EDITOR_PANEL_TREEVIEW_H__
#include "editor_include.h"

#ifdef USE_EDITOR_V2

#include <editor/editor_panel.h>

//--------

BEGIN_SUPER_CLASS(EditorPanelTreeView,EditorPanel)
public:
	EditorPanelTreeView() : SUPER("Treeview") {}

	virtual void UpdatePanelContent(Float _dTime) OVERRIDE;

	const EntityPtr& GetSelectedEntity() const { return m_pSelected; }
	void SelectEntity(EntityPtr _pEntity) { ChangeSelectEntity(_pEntity);  m_bScroll = TRUE; }

protected:
	void CreateTreeForEntity(EntityPtr& _pEntity, Bool _bParentOpen);
	void DisplayRenameTextInput(EntityPtr& _pEntity);

	struct EntityAction
	{
		enum Type
		{
			None,
			Add,
			Delete,
			Copy,
			Paste,
			PasteHierarchy
		};

		Type	m_Type = None;
		EntityPtr	m_Entity;
	};

	void ChangeSelectEntity(EntityPtr _pNewEntity);

	void AddChild(EntityPtr _pParent);
	void DeleteEntity(EntityPtr _pParent);

	// copy/paste
	void CopyEntity(EntityPtr _pEntity);
	void PasteEntity(EntityPtr _pEntity);
	void PasteHierarchy(EntityPtr _pEntity);
	Bool HasEntityInClipboard() const;

	ImGuiTextFilter	m_Filter;
	EntityPtr	m_pSelected;
	EntityPtr	m_pRenamed;
	EntityPtr	m_pPrevious;
	Char		m_RenameBuffer[256];
	Bool		m_bOpenCurrent = FALSE;
	Bool		m_bCloseCurrent = FALSE;
	Bool		m_bGoPrev = FALSE;
	Bool		m_bGoNext = FALSE;
	Bool		m_bScroll = FALSE;

	EntityAction	m_NextAction;

END_SUPER_CLASS

#endif // USE_EDITOR_V2

#endif // __EDITOR_PANEL_H__