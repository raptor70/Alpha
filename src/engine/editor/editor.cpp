#include "editor.h"
#ifdef USE_EDITOR

#include "editor_3dview.h"
#include "editor_menu.h"
#include "input/input_manager.h"
#include "application.h"
#include "debug/debug_flag_list.h"
#include "memory\memory_allocator.h"
#include "editor_entities_properties.h"
#include "file/base_file.h"

Editor::SGameData Editor::g_GameData;

//..

Bool EditorEventFilter::eventFilter(QObject *_target, QEvent *_event)
{
	QEvent::Type type = _event->type();
	if (type == QEvent::Shortcut)
		return TRUE;

	return SUPER::eventFilter(_target, _event);
}

//..

Editor*	Editor::CreateEditor()
{
	Editor::g_GameData.m_pDebugFlagList = &DebugFlagList::GetInstance();
	QResource::registerResource("editor.rcc");
	
	Editor* pEditor = NEW Editor();
	
	// style
	QFile f(QString(":/styles/dark.qss"));
	if (f.open(QIODevice::ReadOnly))
	{
		// style sheets should only have latin1 chars
		QString styleSheet = QLatin1String(f.readAll());
		qApp->setStyleSheet(styleSheet);
	}

	pEditor->m_UiMainWindow.setupUi(pEditor);
	return pEditor;
}

//..

void	Editor::Start(Application* _app, S32 _iWidth, S32 _iHeight, const Str& _sTitle, Bool _bFullscreen)
{
	m_pApplication = _app;

	//default local
	QLocale locale(QLocale::English); 
	locale.setNumberOptions(QLocale::OmitGroupSeparator);
	QLocale::setDefault(locale); 

	// default values 
	QCoreApplication::setOrganizationName("CoffeeTime");
	QCoreApplication::setOrganizationDomain("CoffeeTime.com");
	QCoreApplication::setApplicationName("Editor");
	
	// customize window
	setWindowTitle(_sTitle.GetArray());
	move(0, 0);

	// event filter
	EditorEventFilter* eventFilter = NEW EditorEventFilter;
	this->installEventFilter(eventFilter);
	m_UiMainWindow.menubar->installEventFilter(eventFilter);

	// TOP BAR
	QDockWidget* topbar = NEW QDockWidget();
	m_UiTopBar.setupUi(topbar);
	addDockWidget(Qt::TopDockWidgetArea, topbar);
	QWidget* lTitleBar = topbar->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	topbar->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;
	connect(m_UiTopBar.newscript, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(m_UiTopBar.open, SIGNAL(clicked()), this, SLOT(onOpen()));
	connect(m_UiTopBar.save, SIGNAL(clicked()), this, SLOT(onSave()));
	connect(m_UiTopBar.refresh, SIGNAL(clicked()), this, SLOT(onRefreshButton()));
	connect(m_UiTopBar.play, SIGNAL(clicked()), this, SLOT(onPlay()));
	connect(m_UiTopBar.pause, SIGNAL(clicked()), this, SLOT(onPause()));
	connect(m_UiTopBar.forward, SIGNAL(clicked()), this, SLOT(onForward()));
	connect(m_UiTopBar.rewind, SIGNAL(clicked()), this, SLOT(onBackward()));
	connect(m_UiTopBar.translate, SIGNAL(clicked()), this, SLOT(onTranslate()));
	connect(m_UiTopBar.rotate, SIGNAL(clicked()), this, SLOT(onRotate()));
	connect(m_UiTopBar.scale, SIGNAL(clicked()), this, SLOT(onScale()));

	// ENTITY TREE
	QDockWidget* entitiesTree = NEW QDockWidget();
	m_UiEntitiesTree.setupUi(entitiesTree);
	addDockWidget(Qt::LeftDockWidgetArea, entitiesTree);
	connect(this, SIGNAL(NeedEntitiesTree()), m_UiEntitiesTree.treeView, SLOT(updateTree()));
	connect(m_UiEntitiesTree.search, SIGNAL(textChanged(const QString&)), m_UiEntitiesTree.treeView, SLOT(onSearchChanged(const QString&)));

	// ENTITY PROPERTIES
	m_EntityProperties = NEW EditorEntitiesProperties;
	addDockWidget(Qt::RightDockWidgetArea, m_EntityProperties);
	connect(m_UiEntitiesTree.treeView, SIGNAL(selectedEntityChanged(Entity*)), m_EntityProperties, SLOT(entitiesTreeSelectedEntityChanged(Entity*)));
	connect(this, SIGNAL(OnSelectEntity(Entity*)), m_EntityProperties, SLOT(entitiesTreeSelectedEntityChanged(Entity*)));
	connect(&m_Gizmo, SIGNAL(OnEndTranslation(const Vec3f&, const Vec3f&)), this, SLOT(onSelectedEntityTranslate(const Vec3f&, const Vec3f&)));
	connect(&m_Gizmo, SIGNAL(OnEndRotation(const Quat&, const Quat&)), this, SLOT(onSelectedEntityRotate(const Quat&, const Quat&)));
	connect(&m_Gizmo, SIGNAL(OnEndScale(const Vec3f&, const Vec3f&)), this, SLOT(onSelectedEntityScale(const Vec3f&, const Vec3f&)));

	// update size
	m_UiMainWindow.openGLWidget->setFixedSize(_iWidth, _iHeight);
	adjustSize();
	m_UiMainWindow.openGLWidget->setMinimumSize(0, 0);
	m_UiMainWindow.openGLWidget->setMaximumSize(100000, 1000000);

	show();

	// Refresh play/pause state
	RefreshFromTimeFactor();

	// Refresh modifier
	RefreshModifyButtons();
	
	// LOAD SETTINGS
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

//..

void	Editor::Update(Float _dTime)
{
	// pause/resume
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	if (keyboard.IsJustPressed(KEY_Space))
	{
		if (m_bPaused)
		{
			onPlay();
		}
		else
		{
			onPause();
		}
	}

	// undo/redo
	if (keyboard.IsPressed(KEY_Control) )
	{
		if(keyboard.IsJustPressed(KEY_Z))
			UnDoLastAction();
		if (keyboard.IsJustPressed(KEY_Y))
			ReDoLastAction();
	}

	m_Gizmo.SetEntity(m_EntityProperties->GetEntity());
	m_Gizmo.Update(_dTime);
	m_Camera.Update(_dTime);
	m_Camera.SetCanMove(!m_Gizmo.IsMoving());	

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	if (m_bPaused && !m_Gizmo.IsMoving() && !m_Camera.IsMoving())
	{
		if(mouse.IsJustReleased(MOUSE_Left))
			Renderer::GetInstance()->RequestPicking(mouse.GetPos());

		U32 id = 0;
		if (Renderer::GetInstance()->IsPickingReady(id))
		{
			Entity* pEntity = World::GetInstance().GetEntity(id);
			if (pEntity)
			{
				emit OnSelectEntity(pEntity);
			}
		}
	}

	if (m_bPaused)
	{
		Draw();
	}
}

//..

void	Editor::Draw()
{
	// GRID
	U32 gridSize = 20;
	for (U32 x = 0; x <= gridSize; x++)
		DEBUG_DrawLine(Vec3f(Float(x),0.f,x==0?1.f:0.f), Vec3f(Float(x),0.f,Float(gridSize)), Color::Grey);
	for (U32 z = 0; z <= gridSize; z++)
		DEBUG_DrawLine(Vec3f(z==0?1.f:0.f, 0.f, Float(z)), Vec3f(Float(gridSize), 0.f, Float(z)), Color::Grey);

	// ROOT
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::XAxis, Color::Red);
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::YAxis, Color::Green);
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::ZAxis, Color::Blue);
}

//..

void Editor::onNew()
{
	QString fileName = QFileDialog::getSaveFileName(
		this,
		"New Script",
		"./",
		"Scripts (*.lua)");
	if (!fileName.isEmpty())
	{
		QDir dir;
		fileName = dir.relativeFilePath(fileName);
		QByteArray ba = fileName.toLatin1();
		const char *cba = ba.data();
		Str path(cba);

		// script
		OldFile scriptfile(path, OldFile::USE_TYPE_Write);
		scriptfile.Open(TRUE);
		scriptfile.WriteText(
			"function Init(self)\n"
			"end\n"
			"\n"
			"function Update(self,dtime)\n"
			"end\n"
			"\n"
			"function OnEvent(self,event)\n"
			"end\n"
		);
		scriptfile.Close();

		// entities
		Str entPath = path.GetTextInsideRange(0, path.GetLength() - 5);
		entPath += ".ent";
		OldFile entfile(entPath, OldFile::USE_TYPE_Write);
		entfile.Open(TRUE);
		entfile.WriteText(
			"<World/>"
		);
		entfile.Close();

		Application::GetInstance().GetFlowScriptManager()->ForceNextScript(path);
	}
}

//..

void Editor::onOpen()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open Script",
		"./",
		"Scripts (*.lua)");
	if (!fileName.isEmpty())
	{
		QDir dir;
		fileName = dir.relativeFilePath(fileName);
		QByteArray ba = fileName.toLatin1();
		const char *cba = ba.data();
		Str path(cba);
		Application::GetInstance().GetFlowScriptManager()->ForceNextScript(path);
	}
}

//..

void Editor::onSave()
{
	Application::GetInstance().GetFlowScriptManager()->SaveAll();
}

//..

void Editor::onRefreshButton()
{
	Application::GetInstance().LaunchCommand("RefreshScript");
}

//..

void		Editor::onPlay()
{
	World::GetInstance().StartTimeFactor();
	RefreshFromTimeFactor();
}

//..

void		Editor::onPause()
{
	World::GetInstance().StopTimeFactor();
	RefreshFromTimeFactor();
}

//..

void		Editor::onForward()
{
	World::GetInstance().ForwardTimeFactor();
	RefreshFromTimeFactor();
}

//..

void		Editor::onBackward()
{
	World::GetInstance().BackwardTimeFactor();
	RefreshFromTimeFactor();
}

//..

void		Editor::onTranslate()
{
	m_Gizmo.SetMode(EditorGizmo::MODE_Translation);
	RefreshModifyButtons();
}

//..

void		Editor::onRotate()
{
	m_Gizmo.SetMode(EditorGizmo::MODE_Rotation);
	RefreshModifyButtons();
}

//..

void		Editor::onScale()
{
	m_Gizmo.SetMode(EditorGizmo::MODE_Scale);
	RefreshModifyButtons();
}

//..

void		Editor::onSelectedEntityTranslate(const Vec3f& _vfromPos, const Vec3f& _vtoPos)
{
	EditorActionRef newAction = NEW EditorActionMoveEntity(m_EntityProperties->GetEntity(), _vfromPos, _vtoPos);
	DoAction(newAction);
}

//..

void		Editor::onSelectedEntityRotate(const Quat& _vFromRot, const Quat& _vtoRot)
{
	EditorActionRef newAction = NEW EditorActionRotateEntity(m_EntityProperties->GetEntity(), _vFromRot, _vtoRot);
	DoAction(newAction);
}

//..

void		Editor::onSelectedEntityScale(const Vec3f& _vfromScale, const Vec3f& _vtoScale)
{
	EditorActionRef newAction = NEW EditorActionScaleEntity(m_EntityProperties->GetEntity(), _vfromScale, _vtoScale);
	DoAction(newAction);
}

//..

void Editor::RefreshFromTimeFactor()
{
	Float fTimeFactor = World::GetInstance().GetTimeFactor();
	m_bPaused = (fTimeFactor < Float_Eps);
	RefreshPlayButtons(m_bPaused);
	m_Camera.SetEnabled(m_bPaused);
	m_Gizmo.SetEnabled(m_bPaused);
}

//..

void Editor::RefreshPlayButtons(Bool _bIsPaused)
{
	if (_bIsPaused)
	{
		m_UiTopBar.play->setChecked(FALSE);
		m_UiTopBar.pause->setChecked(TRUE);
	}
	else
	{
		m_UiTopBar.play->setChecked(TRUE);
		m_UiTopBar.pause->setChecked(FALSE);
	}

	QLocale locale;
	QString text = "x";
	text.append(locale.toString(World::GetInstance().GetTimeFactor()));
	m_UiTopBar.factor->setText(text);
}

void Editor::RefreshModifyButtons()
{
	switch (m_Gizmo.GetMode())
	{
	case EditorGizmo::MODE_Translation:
	{
		m_UiTopBar.translate->setChecked(TRUE);
		m_UiTopBar.rotate->setChecked(FALSE);
		m_UiTopBar.scale->setChecked(FALSE);
		break;
	}
	case EditorGizmo::MODE_Rotation:
	{
		m_UiTopBar.translate->setChecked(FALSE);
		m_UiTopBar.rotate->setChecked(TRUE);
		m_UiTopBar.scale->setChecked(FALSE);
		break;
	}
	case EditorGizmo::MODE_Scale:
	{
		m_UiTopBar.translate->setChecked(FALSE);
		m_UiTopBar.rotate->setChecked(FALSE);
		m_UiTopBar.scale->setChecked(TRUE);
		break;
	}
	}
}

void Editor::DoAction(EditorActionRef& _newAction)
{
	_newAction->Do();

	m_UndoStack.AddLastItem(_newAction);
	m_RedoStack.SetEmpty();
}

//..

void Editor::UnDoLastAction()
{
	if (m_UndoStack.GetCount() > 0)
	{
		U32 idx = m_UndoStack.GetCount() - 1;
		EditorActionRef lastAction = m_UndoStack[idx];
		lastAction->UnDo();

		m_UndoStack.RemoveItems(idx);
		m_RedoStack.AddLastItem(lastAction);
	}
}

//..

void Editor::ReDoLastAction()
{
	if (m_RedoStack.GetCount() > 0)
	{
		U32 idx = m_RedoStack.GetCount() - 1;
		EditorActionRef lastAction = m_RedoStack[idx];
		lastAction->Do();

		m_RedoStack.RemoveItems(idx);
		m_UndoStack.AddLastItem(lastAction);
	}
}

//..

void Editor::resizeEvent( QResizeEvent *_event ) 
{
	SUPER::resizeEvent(_event);
}

//..

void Editor::keyPressEvent(QKeyEvent * _event) 
{
	SUPER::keyPressEvent(_event);

	if( !_event->isAutoRepeat() )
	{
		U32 key = _event->nativeVirtualKey();
		InputManager::GetInstance().GetKeyboard().SetKeyState((KEY)key,TRUE);
	}
}

//..

void Editor::keyReleaseEvent(QKeyEvent * _event) 
{
	SUPER::keyReleaseEvent(_event);

	if( !_event->isAutoRepeat() )
	{
		U32 key = _event->nativeVirtualKey();
		InputManager::GetInstance().GetKeyboard().SetKeyState((KEY)key,FALSE);
	}
}

//..

void Editor::mousePressEvent(QMouseEvent *_event) 
{
	SUPER::mousePressEvent(_event);
	InputManager::GetInstance().GetMouse().SetKeyState((KEY)_event->button(),TRUE);
}
	
//..

void Editor::mouseReleaseEvent(QMouseEvent *_event) 
{
	SUPER::mouseReleaseEvent(_event);
	InputManager::GetInstance().GetMouse().SetKeyState((KEY)_event->button(),FALSE);
}
	
//..

void Editor::wheelEvent(QWheelEvent *_event)
{
	SUPER::wheelEvent(_event);
	InputManager::GetInstance().GetMouse().SetWheelDelta((Float)_event->delta()/120.f);
}

//..

void Editor::closeEvent(QCloseEvent *_event)
{
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	QMainWindow::closeEvent(_event);
}
#endif // USE_EDITOR

#ifdef USE_EDITOR_V2

#include <editor/editor_panel_treeview.h>
#include <editor/editor_panel_properties.h>
#include <input/input_manager.h>
#include <imgui\imgui_manager.h>

Editor::Editor()
{
#define ADD_MENUDEBUGFLAG(__TITLE__,__FLAG__) pMenu->AddItem<EditorMenuCheck>(#__TITLE__, DebugFlagList::GetInstance().GetFlagPtr(DEBUGFLAG_##__FLAG__))
#define ADD_MENUCOMMAND(__TITLE__,__COMMAND__) do{auto lambda = [this]() { m_daCommands.AddLastItem(#__COMMAND__); }; pMenu->AddItem<EditorMenuAction<decltype(lambda)>>(#__TITLE__,lambda);}while(0)
	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("File");
		ADD_MENUCOMMAND(Save, Save);
		ADD_MENUCOMMAND(Quit, Quit);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Rendering");
		ADD_MENUDEBUGFLAG(Wireframe, Wireframe);
		ADD_MENUDEBUGFLAG(Mesh Normals, MeshNormal);
		ADD_MENUCOMMAND(Dump Textures, TexturesDump);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Stats");
		ADD_MENUDEBUGFLAG(FPS, Fps);
		ADD_MENUDEBUGFLAG(Profiler, Profiler);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("GUI");
		ADD_MENUDEBUGFLAG(Safe Frame, SafeFrame);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("World");
		ADD_MENUDEBUGFLAG(Hierarchy, Hierarchy);
		ADD_MENUDEBUGFLAG(BBox, BBox);
	}
	
	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Game");
		ADD_MENUCOMMAND(Restart, Restart);
		ADD_MENUCOMMAND(Refresh script, RefreshScript); // shortcut F5
		ADD_MENUDEBUGFLAG(Marketing Mode, Marketing);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Sound");
		ADD_MENUCOMMAND(Dump Fmod State, Dump Sound);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Tools");
		ADD_MENUCOMMAND(Profiler, LaunchProfiler);
		ADD_MENUCOMMAND(Frame Debbuger, LaunchFrameDebugger);
	}

	{
		EditorMenu* pMenu = m_MenuBar.AddMenu("Help");
		ADD_MENUDEBUGFLAG(ImGui Demo, ImGuiDemo);
	}

	m_MenuBar.AddStats();
	
	m_pDockspace = NEW  EditorDockspace("Dockspace");

#define ADD_PANEL(_ptr, _class) _ptr = m_Panels.AddLastItem(NEW _class).GetCastPtr<_class>();

	ADD_PANEL(m_pTreeView, EditorPanelTreeView);
	m_pTreeView->DockTo(m_pDockspace, EditorDockspace::Dir::Left);
	ADD_PANEL(m_pProperties, EditorPanelProperties);
	m_pProperties->DockTo(m_pDockspace, EditorDockspace::Dir::Right);

	m_bPaused = FALSE;
}

//-----------------------------------

void Editor::Update(Float _dTime)
{
	Prepare();
	
	m_Camera.SetEnabled(m_bPaused); // tmp
	m_Gizmo.SetEnabled(m_bPaused); // tmp

	// tmp : TAB
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	if (keyboard.IsJustPressed(KEY_Tab))
	{
		EditorGizmo::Modes curMode = m_Gizmo.GetMode();
		curMode = EditorGizmo::Modes((S32(curMode)+1)% S32(EditorGizmo::MODE_Count));
		m_Gizmo.SetMode(curMode);
	}
	else if (keyboard.IsJustPressed(KEY_Space))
	{
		m_bPaused = !m_bPaused;
	}

	m_Gizmo.SetEntity(m_pTreeView->GetSelectedEntity());
	m_Gizmo.Update(_dTime);
	m_Camera.Update(_dTime);
	m_Camera.SetCanMove(!m_Gizmo.IsMoving());

	Bool bHoverImgui = ImGui::GetIO().WantCaptureMouse;
	Window3DScreen* p3DScreen = ImguiManager::GetInstance().GetMain3DScreen();
	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	if (m_bPaused && !m_Gizmo.IsMoving() && !m_Camera.IsMoving() && !bHoverImgui)
	{
		if (mouse.IsJustReleased(MOUSE_Left))
			p3DScreen->GetRenderer()->RequestPicking(mouse.GetPos());

		U32 id = 0;
		if (p3DScreen->GetRenderer()->IsPickingReady(id))
		{
			EntityPtr pEntity = World::GetInstance().GetEntity(id);
			if (pEntity)
			{
				m_pTreeView->SelectEntity(pEntity);
			}
		}
	}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;
	
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Editor", NULL, window_flags);

	ImGui::PopStyleVar(2);

	m_pDockspace->Update(_dTime);

	m_MenuBar.Update(_dTime);

	for (auto& panel : m_Panels)
		panel->Update(_dTime);
	
	ImGui::End();

	if(m_bPaused)
		Draw();
}

//-----------------------------------

void	Editor::Draw()
{
	// GRID
	U32 gridSize = 20;
	for (U32 x = 0; x <= gridSize; x++)
		DEBUG_DrawLine(Vec3f(Float(x), 0.f, x == 0 ? 1.f : 0.f), Vec3f(Float(x), 0.f, Float(gridSize)), Color::Grey);
	for (U32 z = 0; z <= gridSize; z++)
		DEBUG_DrawLine(Vec3f(z == 0 ? 1.f : 0.f, 0.f, Float(z)), Vec3f(Float(gridSize), 0.f, Float(z)), Color::Grey);

	// ROOT
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::XAxis, Color::Red);
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::YAxis, Color::Green);
	DEBUG_DrawLine(Vec3f::Zero, Vec3f::ZAxis, Color::Blue);
}

//-----------------------------------

void Editor::Prepare()
{
	// TODO Common structure ?
	m_pProperties->SetSelectedEntity(m_pTreeView->GetSelectedEntity());
}

#endif // USE_EDITOR_V2
