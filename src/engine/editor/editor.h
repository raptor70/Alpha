#ifndef __EDITOR_H__
#define __EDITOR_H__
#include "editor_include.h"
#ifdef USE_EDITOR
#include "ui/ui_main.h"
#include "ui/ui_entities_tree.h"
#include "ui/ui_top_bar.h"
#include "editor_camera.h"
#include "editor_gizmo.h"
#include "editor_action.h"

class Editor3DView;
class EditorMenuBar; 
class EditorMenu;
class Application;
class EditorEntitiesProperties;
 
BEGIN_EDITOR_CLASS(EditorEventFilter,QObject)
public:
	virtual Bool eventFilter(QObject *_target, QEvent *_event) OVERRIDE;
END_EDITOR_CLASS

BEGIN_EDITOR_CLASS(Editor, QMainWindow)
public:
	static Editor*	CreateEditor();

	void	Start(Application* _app, S32 _iWidth, S32 _iHeight, const Str& _sTitle, Bool _bFullscreen);
	void	Update(Float _dTime);
	void	Draw();

	const NameArray&	GetCommands() const { return g_GameData.m_daCommands; }
	void			FlushCommands() { g_GameData.m_daCommands.SetEmpty(); }

	EditorMenuBar*	GetMenuBar() { return m_UiMainWindow.menubar; }
	Editor3DView* Get3DView() { return m_UiMainWindow.openGLWidget; }

	void		UpdateEntitiesTree() { emit NeedEntitiesTree(); }

	// camera
	EditorCamera&	GetCamera() { return m_Camera; }

	//gizmo
	EditorGizmo& GetGizmo() { return m_Gizmo; }

	struct SGameData
	{
		NameArray		m_daCommands;
		DebugFlagList*	m_pDebugFlagList;
	};
	static SGameData	g_GameData;

	Ui_MainWindow			m_UiMainWindow;
	Ui_EntitiesTree			m_UiEntitiesTree;
	Ui_TopBar				m_UiTopBar;
	
signals:
	void NeedEntitiesTree();
	void OnSelectEntity(Entity* _pEntity);

public slots:
	void		onNew();
	void		onOpen();
	void		onSave();
	void		onRefreshButton();
	void		onPlay();
	void		onPause();
	void		onForward();
	void		onBackward();
	void		onTranslate();
	void		onRotate();
	void		onScale();
	void		onSelectedEntityTranslate(const Vec3f& _vfromPos, const Vec3f& _vtoPos);
	void		onSelectedEntityRotate(const Quat& _vFromRot, const Quat& _vtoRot);
	void		onSelectedEntityScale(const Vec3f& _vfromScale, const Vec3f& _vtoScale);
	
protected:
	virtual void resizeEvent(QResizeEvent * _event) OVERRIDE;
	virtual void keyPressEvent(QKeyEvent * _event) OVERRIDE;
	virtual void keyReleaseEvent(QKeyEvent * _event) OVERRIDE;
	virtual void mousePressEvent(QMouseEvent *_event) OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *_event) OVERRIDE;
	virtual void wheelEvent(QWheelEvent * _event) OVERRIDE;
	virtual void closeEvent(QCloseEvent *_event) OVERRIDE;

	void RefreshFromTimeFactor();
	void RefreshPlayButtons(Bool _bIsPaused);
	void RefreshModifyButtons();

	void DoAction(EditorActionRef& _newAction);
	void UnDoLastAction();
	void ReDoLastAction();

	Application*	m_pApplication;

	EditorEntitiesProperties* m_EntityProperties;
	EditorCamera	m_Camera;
	EditorGizmo	m_Gizmo;
	EditorActionRefArray m_UndoStack;
	EditorActionRefArray m_RedoStack;
	Bool	m_bPaused;

	END_EDITOR_CLASS

#endif // USE_EDITOR

#ifdef USE_EDITOR_V2
#include <editor/editor_menu.h>
#include <editor/editor_panel.h>
#include <editor/editor_dockspace.h>
#include <editor/editor_camera.h>
#include <editor/editor_gizmo.h>

class EditorPanelTreeView;
class EditorPanelProperties;

class Editor
{
public:
	Editor();

	void Update(Float _dTime);
	void Draw();

	// tools
	EditorCamera& GetCamera() { return m_Camera; }
	EditorGizmo& GetGizmo() { return m_Gizmo; }

	template<typename Visitor>
	void VisitAndFlushCommands(Visitor&& _visitor)
	{
		for (const Name& n : m_daCommands)
		{
			if (_visitor(n) == VisitorReturn::Stop)
				break;
		}

		m_daCommands.Flush();
	}

protected:
	void Prepare();

private:
	EditorMenuBar				m_MenuBar;
	EditorPanelTreeView*		m_pTreeView = NULL;
	EditorPanelProperties*		m_pProperties = NULL;
	EditorDockspaceRef			m_pDockspace = NULL;

	EditorPanelRefArray	m_Panels;

	NameArray		m_daCommands;
	EditorCamera	m_Camera;
	EditorGizmo	m_Gizmo;
	Bool	m_bPaused;
};

#endif // USE_EDITOR_V2


#endif // __EDITOR_H__