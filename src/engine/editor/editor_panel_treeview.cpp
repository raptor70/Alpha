#include "editor_panel_treeview.h"

#ifdef USE_EDITOR_V2
#include <input/device_key.h>
#include <core/name_static.h>

void EditorPanelTreeView::UpdatePanelContent(Float _dTime)
{
	// search
	Bool bDisplaySearchX = m_Filter.IsActive();
	ImGui::SetNextItemWidth(bDisplaySearchX ? -24.f : -1.f);
	if(ImGui::InputTextWithHint("##tree_search", "Search", m_Filter.InputBuf, IM_ARRAYSIZE(m_Filter.InputBuf)))
		m_Filter.Build();

	if (bDisplaySearchX)
	{
		ImGui::SameLine();
		if (ImGui::Button("X"))
		{
			m_Filter.Clear();
		}
	}

	m_pPrevious = NULL;
	m_bOpenCurrent = FALSE;
	m_bCloseCurrent = FALSE;
	m_bGoPrev = FALSE;
	m_bGoNext = FALSE;

	if (ImGui::IsWindowFocused())
	{
		if (m_pRenamed.IsValid())
		{
			if (m_pRenamed != m_pSelected || ImGui::IsKeyPressed(KEY_Escape))
			{
				m_pRenamed = NULL;
			}
		}
		else
		{
			if (m_pSelected.IsValid() && ImGui::IsKeyPressed(KEY_F2))
			{
				m_pRenamed = m_pSelected;
				memcpy(m_RenameBuffer, m_pSelected->GetName().GetStr().GetArray(), m_pSelected->GetName().GetStr().GetLength() + 1);
			}
		}

		if (m_pSelected.IsValid())
		{
			m_bOpenCurrent = ImGui::IsKeyPressed(KEY_Right);
			m_bCloseCurrent = ImGui::IsKeyPressed(KEY_Left);
			m_bGoPrev = ImGui::IsKeyPressed(KEY_Up);
			m_bGoNext = ImGui::IsKeyPressed(KEY_Down);
		}
	}
	else
	{
		m_pRenamed = NULL;
	}

	// 
	EntityPtr pRoot = World::GetInstance().GetRoot();
	CreateTreeForEntity(pRoot,TRUE);

	// ACtion
	if (m_NextAction.m_Type != EntityAction::None)
	{
		switch (m_NextAction.m_Type)
		{
		case EntityAction::Add: 
			AddChild(m_NextAction.m_Entity); break;
		case EntityAction::Delete:
			DeleteEntity(m_NextAction.m_Entity); break;
		case EntityAction::Copy:
			CopyEntity(m_NextAction.m_Entity); break;
		case EntityAction::Paste:
			PasteEntity(m_NextAction.m_Entity); break;
		case EntityAction::PasteHierarchy:
			PasteHierarchy(m_NextAction.m_Entity); break;
		}
	}

	m_NextAction.m_Type = EntityAction::None;
}

//..

void EditorPanelTreeView::CreateTreeForEntity(EntityPtr& _pEntity, Bool _bParentOpen)
{
	const Char* pTitle = _pEntity->GetName().GetStr().GetArray();
	Bool bDisplayRenameInput = FALSE;
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_SpanFullWidth;
	if (_pEntity == m_pRenamed)
	{
		bDisplayRenameInput = TRUE;
	}
	else if (_pEntity == m_pSelected)
	{
		flag |= ImGuiTreeNodeFlags_Selected;
		if (m_bOpenCurrent)
			ImGui::SetNextItemOpen(TRUE);
		if (m_bCloseCurrent)
			ImGui::SetNextItemOpen(FALSE);
		if (m_pPrevious && m_bGoPrev)
			ChangeSelectEntity(m_pPrevious);
		if (m_bScroll)
		{
			ImGui::SetScrollHereY();
			m_bScroll = FALSE;
		}
	}
	else if (m_pSelected == m_pPrevious)
	{
		if (m_pPrevious && m_bGoNext && _bParentOpen)
		{
			m_bGoNext = FALSE;
			ChangeSelectEntity(_pEntity);
		}
	}
	Bool bOpened = FALSE;
	Bool bAddNode = FALSE;
	Bool bForceOpen = FALSE;
	if (m_Filter.IsActive())
	{
		if (m_Filter.PassFilter(pTitle))
		{
			flag |= ImGuiTreeNodeFlags_Leaf;
			bAddNode = TRUE;
		}
	}
	else if (m_pSelected && _pEntity->HasSon(m_pSelected,TRUE))
	{
		flag |= ImGuiTreeNodeFlags_OpenOnArrow;
		bAddNode = TRUE;
		bForceOpen = TRUE;
	}
	else
	{
		flag |= ImGuiTreeNodeFlags_OpenOnArrow;
		if (_pEntity->GetNbSons() == 0)
			flag |= ImGuiTreeNodeFlags_Leaf;

		bAddNode = _bParentOpen;
	}

	if (bAddNode)
	{
		m_pPrevious = _pEntity;
		if (m_pRenamed)
			ImGuiEx::PushDisabled();
		if (bForceOpen)
			ImGui::SetNextItemOpen(TRUE);
		if(!_pEntity->ToSave())
			ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.f,0.f,0.f,1.0f));
		bOpened = ImGui::TreeNodeEx(_pEntity, flag, bDisplayRenameInput ? "" : pTitle);
		if (!_pEntity->ToSave())
			ImGui::PopStyleColor();
		if (m_pRenamed)
			ImGuiEx::PopDisabled();

		if (ImGui::IsItemClicked())
			ChangeSelectEntity(_pEntity);

		// option
		if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight))
		{
			ChangeSelectEntity(_pEntity);
			Bool bIsRootScript = Application::GetInstance().GetFlowScriptManager()->IsRootScriptEntity(_pEntity);
			Bool bIsSavable = _pEntity->ToSave();
			Bool bHasEntityInClipBoard = HasEntityInClipboard();
			Bool bIsValid = bIsRootScript || bIsSavable;
			if (ImGui::MenuItem("Add child",NULL,FALSE, bIsValid))
				m_NextAction = { EntityAction::Add, _pEntity };
			if (ImGui::MenuItem("Delete", NULL, FALSE, bIsSavable))
				m_NextAction = { EntityAction::Delete, _pEntity };
			if(ImGui::MenuItem("Copy", NULL, FALSE, bIsSavable))
				m_NextAction = { EntityAction::Copy, _pEntity };
			if (ImGui::MenuItem("Paste Entity", NULL, FALSE, bIsValid && bHasEntityInClipBoard))
				m_NextAction = { EntityAction::Paste, _pEntity };
			if(ImGui::MenuItem("Paste Hierarchy", NULL, FALSE, bIsValid && bHasEntityInClipBoard))
				m_NextAction = { EntityAction::PasteHierarchy, _pEntity };
			ImGui::EndPopup();
		}
		
		if (bDisplayRenameInput)
			DisplayRenameTextInput(_pEntity);
	}

	_pEntity->VisitAllSons(FALSE, [this, bOpened](EntityRef& _pSon)
		{
			CreateTreeForEntity(EntityPtr(_pSon), bOpened);
			return VisitorReturn::Continue;
		});



	if (bOpened)
		ImGui::TreePop();
}

//..

void EditorPanelTreeView::DisplayRenameTextInput(EntityPtr& _pEntity)
{
	ImGui::PushID(_pEntity);
	ImGui::SameLine();
	ImGui::SetKeyboardFocusHere();
	if (ImGuiEx::SmallInputText("##RenameEntity", m_RenameBuffer, sizeof(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		m_pRenamed->SetName(m_RenameBuffer);
		m_pRenamed = NULL;
	}
	ImGui::PopID();
}

//..

void EditorPanelTreeView::ChangeSelectEntity(EntityPtr _pNewEntity)
{
	if (m_pSelected.IsValid())
		m_pSelected->SetIsSelected(FALSE);
	m_pSelected = _pNewEntity;
	if (m_pSelected.IsValid())
		m_pSelected->SetIsSelected(TRUE);
}

//..

void EditorPanelTreeView::AddChild(EntityPtr _pParent)
{
	FlowScript* pScript = Application::GetInstance().GetFlowScriptManager()->GetScriptForEntity(_pParent);
	Entity& entity = pScript->CreateNewEntityFromParent(NAME(NO_NAME), *_pParent);
	entity.SetToSave(TRUE);
}

//..

void EditorPanelTreeView::DeleteEntity(EntityPtr _pEntity)
{
	World::GetInstance().RemoveEntity(_pEntity);
}

//..

void EditorPanelTreeView::CopyEntity(EntityPtr _pEntity)
{
	// entities
	rapidxml::xml_document<> doc;
	//World::GetInstance().SaveEntitiesToXml(&doc, &doc, _pEntity, TRUE);
	std::string s;
	print(std::back_inserter(s), doc, 0);
	ImGui::SetClipboardText(s.c_str());
}

//..

void EditorPanelTreeView::PasteEntity(EntityPtr _pEntity)
{
	rapidxml::xml_document<> doc;
	doc.parse<0>((char*)ImGui::GetClipboardText());
	
	//World::GetInstance().LoadEntitiesFromXml(&doc, _pEntity, TRUE, FALSE);
}

//..

void EditorPanelTreeView::PasteHierarchy(EntityPtr _pEntity)
{
	rapidxml::xml_document<> doc;
	doc.parse<0>((char*)ImGui::GetClipboardText());

	//World::GetInstance().LoadEntitiesFromXml(&doc, _pEntity, TRUE, TRUE);
}

//..

Bool EditorPanelTreeView::HasEntityInClipboard() const
{
	rapidxml::xml_document<> doc;

	try
	{
		doc.parse<0>((char*)ImGui::GetClipboardText());
	}
	catch (...)
	{
		return FALSE;
	}

	rapidxml::xml_node<>* pComponent = doc.first_node();
	if (pComponent && Name(pComponent->name()) == NAME(Entity))
	{
		return TRUE;
	}

	return FALSE;
}

#endif // USE_EDITOR_V2