#ifndef __EDITOR_PANEL_PROPERTIES_H__
#define __EDITOR_PANEL_PROPERTIES_H__
#include "editor_include.h"

#ifdef USE_EDITOR_V2

#include <editor/editor_panel.h>

//--------

BEGIN_SUPER_CLASS(EditorPanelProperties,EditorPanel)
public:
	EditorPanelProperties() : SUPER("Properties") {}

	virtual void UpdatePanelContent(Float _dTime) OVERRIDE;

	void SetSelectedEntity(const EntityPtr& _pEntity) { m_pSelected = _pEntity; OnSeletedChanged(); }

protected:
	void OnSeletedChanged();

	EntityPtr m_pSelected;
	Char	m_RenameBuffer[256]	= "";
END_SUPER_CLASS

#endif // USE_EDITOR_V2

#endif // __EDITOR_PANEL_PROPERTIES_H__