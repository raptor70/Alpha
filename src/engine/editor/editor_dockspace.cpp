#include "editor_dockspace.h"
#ifdef USE_EDITOR_V2
#include <editor/editor_panel.h>

void EditorDockspace::Update(Float _dTime)
{
	if (m_ID == 0)
	{
		m_ID = ImGui::GetID(m_sName.GetArray());
		m_SplitIDs[(U32)Dir::Central] = m_ID;
	}
	ImGui::DockSpace(m_ID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
}


void EditorDockspace::DockPanel(EditorPanel* _pPanel, Dir _dir)
{
	if (m_SplitIDs[(U32)_dir] == 0)
	{
		ImGuiDir imguiDir = ImGuiDir_None;
		Float* pfMainSplitRatio = NULL;
		switch (_dir)
		{
		case Dir::Left: imguiDir = ImGuiDir_Left; pfMainSplitRatio = &m_MainSplitRatio.x;  break;
		case Dir::Right: imguiDir = ImGuiDir_Right; pfMainSplitRatio = &m_MainSplitRatio.x;  break;
		case Dir::Top: imguiDir = ImGuiDir_Up; pfMainSplitRatio = &m_MainSplitRatio.y;  break;
		case Dir::Bottom: imguiDir = ImGuiDir_Down; pfMainSplitRatio = &m_MainSplitRatio.y;  break;
		}

		Float fRatioFromDockspace = .2f;
		Float fRatioFromMainSplit = .2f / *pfMainSplitRatio;
		m_SplitIDs[(U32)_dir] = ImGuiEx::SplitDockspace(m_SplitIDs[(U32)Dir::Central], imguiDir, fRatioFromMainSplit, &m_SplitIDs[(U32)Dir::Central]);
		*pfMainSplitRatio -= fRatioFromDockspace;
	}

	ImGuiEx::DockWindow(m_SplitIDs[(U32)_dir], _pPanel->GetTitle().GetArray());
}

#endif // USE_EDITOR_V2