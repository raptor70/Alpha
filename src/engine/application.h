#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "window/window.h"
#include "window/window_3dscreen.h"
#include "core/state.h"
#include "core/timer.h"
#include "script/flow_script_manager.h"
#include "thread/thread.h"
class Editor;
class Application;

//----------------------------------------

typedef void (Application::*CommandCallback)();

struct SApplicationCommand
{
	Name	m_nName;
	CommandCallback	m_pFunction;

	SApplicationCommand() {}
};

//----------------------------------------

class Application
{
	DECLARE_SCRIPT_CLASS(Application)
public:
	Application();
	virtual ~Application();

	virtual S32 Run(S32 _argc,Char ** _argv);
	virtual Bool Init(S32 _argc,Char ** _argv);
	virtual Bool Release(Bool _expectStatic = false);

	virtual void Pause();
	virtual void Resume();

    virtual void DoFrame();
	virtual void DoFrame(Float _dTime);

	static Application&	GetInstance() { return *g_pInstance; }

	void	UseSteam(Bool _use) { m_bUseSteam = _use; }
	Bool	MustUseSteam() const { return m_bUseSteam; }

	void	UseEditor(Bool _use) { m_bUseEditor = _use; }
	Bool	MustUseEditor() const { return m_bUseEditor; }

	void	SetWindowParams(S32 _iWidth, S32 _iHeight, const Str& _sTitle, Bool _bFullscreen);
	void	SetWindowSize(S32 _iWidth, S32 _iHeight);
	
	void		SetResultStr(const Str& _str)		{ m_sResult = _str; }
	const Str&	GetResultStr() const				{ return m_sResult; }

	S32			GetFrameCount() const				{ return m_iFrameCount; }

	void		SetStartScript(const Str& _str)		{ m_sStartScript = _str; }
	void		SetMainThemeSoundEvent(const Str& _str)	{ m_sMainThemeEvent = _str; }
	void		SetMainThemeParam(const Str& _name, Float _value)		{ if( m_MainTheme.IsValid()) m_MainTheme->SetParam(_name.GetArray(),_value); }

	void		AddPreloadTexture(const Str& _path);
	void		StartPreload();
	Bool		PreloadDone() const;

	RefTo<FlowScriptManager>&	GetFlowScriptManager() { return m_pFlow; }
	Thread&		GetRenderThread() { return m_RenderThread; }
	
	void		UseMetal() { m_p3DScreen->GetRenderer()->UseMetal(); }
	Bool		HasFocus() const;

	// commands
	void	LaunchCommand(const Name& _command);
	void	RegisterCommand(const Name& _command, const CommandCallback& _function);
#define COMMAND(_name) void COMMAND_##_name()
#include "application_commands.h"
#undef COMMAND

	//thread
	static void* ThreadRedraw(void* _arg);
	static void* DestroyDraw(void* _arg);

protected:
	virtual	void InitWorld();

	float ComputeDeltaTime();
	void UpdateFrame(Float _dTime);
	void ManageEvents();
	void UpdateDisplayDebug(Float _dTime);
	void UpdateDisplayFPS(Float _dTime);
	void RenderFrame();
	void EndFrame();

	void Loop();
	static void Preload(void* _game);

	void		UpdateComputeIsIn();

	ArrayOf<Str>		m_PreloadTextures;	
	Timer				m_Timer;
	WindowRef			m_Window;
	PtrTo<Window3DScreen>	m_p3DScreen;
	RefTo<RendererDriver>	m_p3DDriver;
	S32					m_iReferenceResX;
	S32					m_iReferenceResY;
	S32					m_iResolutionX;
	S32					m_iResolutionY;
	S32					m_iFrameCount;
	Str					m_sWindowTitle;
	Bool				m_bFullscreen;
	RefTo<FlowScriptManager>	m_pFlow;
	SoundRef			m_MainTheme;
	Str					m_sMainThemeEvent;
	Str					m_sResult;
	Str					m_sStartScript;
	Float				m_fTimeFactor;
	Bool				m_bFirstFrameAfterInit;
	Bool				m_bOnResumeNextFrame;
	Bool				m_bPaused;
	Bool				m_bAskRefresh;
	Bool				m_bUseEditor;
	Bool				m_bUseSteam;
	Thread				m_RenderThread;
	Float				m_fUpdateDuration;
	Timer				m_UpdateTimer;
	Float				m_fDrawDuration;
	Timer				m_DrawTimer;

#ifdef USE_QT
	QApplication		m_QTApp;
#endif
		
	static Application*	g_pInstance;
	HashOf<Name, SApplicationCommand>	m_Commands;

public:
	Float				m_fRenderDuration;
	Timer				m_RenderTimer;

#ifdef USE_EDITOR
public:
	Bool				HasEditor() const { return m_Editor.IsValid(); }
	Editor&			GetEditor() { return *(m_Editor.GetPtr()); }
protected:
	RefTo<Editor>		m_Editor;
#endif

#ifdef USE_EDITOR_V2
public:
	Bool				HasEditor() const { return m_Editor.IsValid(); }
	Editor& GetEditor() { return *(m_Editor.GetPtr()); }
protected:
	RefTo<Editor>		m_Editor;
#endif
};

#endif
