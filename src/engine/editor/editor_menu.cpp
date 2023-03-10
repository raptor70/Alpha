#include "editor_menu.h"
#ifdef USE_EDITOR

#include "editor.h"

EditorMenuBar::EditorMenuBar(QWidget * _parent) :SUPER(_parent)
{
	EditorMenu* pMenu = AddNewMenu("File");
	pMenu->AddCommandAction("Quit", "Quit");

	pMenu = AddNewMenu("Rendering");
	pMenu->AddDebugFlagAction("Wireframe", DEBUGFLAG_Wireframe);
	pMenu->AddDebugFlagAction("Mesh Normals", DEBUGFLAG_MeshNormal);
	pMenu->AddCommandAction("Dump Textures", "TexturesDump");

	pMenu = AddNewMenu("Stats");
	pMenu->AddDebugFlagAction("FPS", DEBUGFLAG_Fps);
	pMenu->AddDebugFlagAction("Profiler", DEBUGFLAG_Profiler);

	pMenu = AddNewMenu("GUI");
	pMenu->AddDebugFlagAction("Safe Frame", DEBUGFLAG_SafeFrame);
	
	pMenu = AddNewMenu("World");
	pMenu->AddDebugFlagAction("Hierarchy", DEBUGFLAG_Hierarchy);
	pMenu->AddDebugFlagAction("BBox", DEBUGFLAG_BBox);

	pMenu = AddNewMenu("Game");
	pMenu->AddCommandAction("Restart", "Restart");
	pMenu->AddCommandAction("Refresh script(F5)", "RefreshScript");
	pMenu->AddDebugFlagAction("Marketing Mode", DEBUGFLAG_Marketing);

	pMenu = AddNewMenu("Sound");
	pMenu->AddCommandAction("Dump Fmod State", "Dump Sound");

	pMenu = AddNewMenu("Game");
	/*pMenu->AddDebugFlagAction("Agent Path", DEBUGFLAG_AgentPath);
	pMenu->AddDebugFlagAction("Agent State", DEBUGFLAG_AgentState);
	pMenu->AddDebugFlagAction("Global Stats", DEBUGFLAG_GlobalStats);
	pMenu->AddDebugFlagAction("Zone Stats", DEBUGFLAG_ZoneStats);*/
}

//..

EditorMenu* EditorMenuBar::AddNewMenu(const Str& _name)
{
	EditorMenu* pMenu = NEW EditorMenu(_name.GetArray());
	addMenu(pMenu);
	return pMenu;
}

//..

void EditorMenu::AddDebugFlagAction(const Str& _name, const DebugFlag& _flag)
{
	EditorDebugFlagAction* newAction = NEW EditorDebugFlagAction(this, _name, _flag);
	addAction(newAction);
	connect(newAction, SIGNAL(triggered()), newAction, SLOT(OnAction()));
}

//..

void EditorMenu::AddCommandAction(const Str& _name, const Name& _command)
{
	EditorCommandAction* newAction = NEW EditorCommandAction(this, _name, _command);
	addAction(newAction);
	connect(newAction, SIGNAL(triggered()), newAction, SLOT(OnAction()));
}

//..

EditorDebugFlagAction::EditorDebugFlagAction(EditorMenu* _menu, const Char* _name, const DebugFlag& _flag) : 
	SUPER(_name, _menu), 
	m_Flag(_flag)
{
	setCheckable(TRUE);
	setChecked(Editor::g_GameData.m_pDebugFlagList->IsFlagEnable(_flag));
}

//..

void EditorDebugFlagAction::OnAction()
{
	Editor::g_GameData.m_pDebugFlagList->SetFlag(m_Flag, isChecked());
}

//..

EditorCommandAction::EditorCommandAction(EditorMenu* _menu, const Char* _name, const Name& _command) :
	SUPER(_name, _menu),
	m_nCommand(_command)
{
}

//..

void EditorCommandAction::OnAction()
{
	Editor::g_GameData.m_daCommands.AddLastItem(m_nCommand);
}

#endif // USE_EDITOR

#ifdef USE_EDITOR_V2

void EditorMenu::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	if (ImGui::BeginMenu(m_Title.GetArray()))
	{
		for (auto& item : m_Items)
			item->Update(_dTime);

		ImGui::EndMenu();
	}
}

//..

void EditorMenuCheck::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	ImGui::MenuItem(m_Title.GetArray(),"", m_pValue);
}

//..

void EditorMenuBar::Update(Float _dTime)
{
	if (ImGui::BeginMenuBar())
	{
		for (auto& menu : m_Menus)
			menu->Update(_dTime);

		ImGui::EndMenuBar();
	}
}

#endif // USE_EDITOR_V2