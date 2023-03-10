#include "editor_panel.h"

#ifdef USE_EDITOR_V2

void EditorPanel::Update(Float _dTime)
{
	if (m_bUpdateDocking && m_pDockspace.IsValid())
	{
		m_bUpdateDocking = FALSE;
		m_pDockspace->DockPanel(this, m_DockDir);
	}

	if (ImGui::Begin(m_Title.GetArray()))
	{
		UpdatePanelContent(_dTime);
	}
	ImGui::End();
}

#endif // USE_EDITOR_V2