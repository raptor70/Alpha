#include "application.h"

#include "core/tree_of_.h"
#include "core/name_static.h"
#include "core/sort.h"
#include "memory/memory_allocator.h"
#include "debug/file_logger.h"
#include "world/component_mesh.h"
#include "world/component_spline.h"
#include "world/component_camera.h"
#include "resource/resource_manager.h"
#include "3d/texture.h"
#include "3d/texture_manager.h"
#include "3d/skin.h"
#include "renderer/renderer_driver.h"
#include "renderer/renderer_driver_gl.h"
#include "renderer/renderer_driver_vulkan.h"
#include "renderer/renderer_driver_metal.h"
#include "2d/font.h"
#include "2d/component_quad.h"
#include "input/input_manager.h"
#include "input/component_input_click.h"
#include "platform/platform_manager.h"
#include "editor/editor.h"
#include "database/database.h"
#include "imgui/imgui_manager.h"
#include "game_name_static.h"

//---------------------------------

BEGIN_SCRIPT_CLASS(Application)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,Game)
	SCRIPT_VOID_METHOD4(SetWindowParams,S32,S32,Str,Bool)
	SCRIPT_VOID_METHOD1(SetStartScript,Str)
	SCRIPT_VOID_METHOD1(SetMainThemeSoundEvent,Str)
	SCRIPT_VOID_METHOD2(SetMainThemeParam,Str,Float)
	SCRIPT_VOID_METHOD1(AddPreloadTexture,Str)
	SCRIPT_VOID_METHOD(StartPreload)
	SCRIPT_VOID_METHOD1(UseSteam, Bool)
	SCRIPT_VOID_METHOD1(UseEditor,Bool)
	SCRIPT_RESULT_METHOD(PreloadDone,Bool)
END_SCRIPT_CLASS

//---------------------------------

void SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	if (nSeverity >= 1)
	{
		LOGGER_Log("Steam - %s", pchDebugText);
	}
	else
	{
		LOGGER_LogWarning("Steam - %s", pchDebugText);
	}
}

//---------------------------------

void* Application::ThreadRedraw(void* _arg)
{
	PROFILER_Begin("RenderThread");
	Application* pApp = (Application*)_arg;

	Timer& timer = pApp->m_RenderTimer;
	timer.Reset();

	// todo : all viewport
	pApp->m_p3DScreen->Redraw(pApp->m_p3DDriver);	
	IMGUIMGR_ReDraw(pApp->m_p3DDriver);	

	pApp->m_p3DScreen->Swap(pApp->m_p3DDriver);
	IMGUIMGR_Swap(pApp->m_p3DDriver);
	
	pApp->m_fRenderDuration = timer.GetSecondCount();

	PROFILER_End();

	return 0;
}

//---------------------------------

void* Application::DestroyDraw(void* _arg)
{
	Application* pApp = (Application*)_arg;
	pApp->m_p3DScreen->Finalize(pApp->m_p3DDriver);

	return 0;
}

//---------------------------------

Application* Application::g_pInstance = NULL;

//---------------------------------

#ifdef USE_QT
	int g_nbArg = 0;
#endif

Application::Application():m_RenderThread("Render Thread")
#ifdef USE_QT
	,m_QTApp(g_nbArg,NULL)
#endif
{
	DEBUG_Require(g_pInstance==NULL);
	g_pInstance = this;

	m_iFrameCount = 0;
	m_iResolutionX = 320;
	m_iResolutionY = 240;
	m_iReferenceResX = m_iResolutionX;
	m_iReferenceResY = m_iResolutionY;
	m_sWindowTitle = "New window";
	m_bFullscreen = FALSE;
	m_p3DScreen = NULL;
	m_sStartScript = "script/logo.lua";
	m_fTimeFactor = 1.f;
	m_bOnResumeNextFrame = FALSE;
	m_bPaused = FALSE;
	m_fUpdateDuration=0.f;
	m_fDrawDuration = 0.f;
	m_fRenderDuration=0.f;
	m_bAskRefresh = FALSE;
	m_bUseSteam = FALSE;
	m_bUseEditor = FALSE;
}

//---------------------------------

Application::~Application()
{
	DEBUG_Require(g_pInstance==this);
	g_pInstance = NULL;
}

//---------------------------------

void	Application::SetWindowParams(S32 _iWidth, S32 _iHeight, const Str& _sTitle, Bool _bFullscreen)
{
	m_sWindowTitle = _sTitle;
	m_bFullscreen = _bFullscreen;
	SetWindowSize(_iWidth,_iHeight);
}

//---------------------------------

void	Application::SetWindowSize(S32 _iWidth, S32 _iHeight)
{
	m_iResolutionX = _iWidth;
	m_iResolutionY = _iHeight;

	if( m_Window.IsValid() )
	{
		m_Window->SetSize(_iWidth,_iHeight);
	}

	if( m_p3DScreen )
	{
		m_p3DScreen->SetSize(0,0,_iWidth,_iHeight);
	}
}

//---------------------------------

S32 Application::Run(S32 _argc,Char ** _argv)
{
#ifdef MEMORY_CHECK
	MemoryAllocator::GetInstance().StartSafe();
#endif
#ifdef WIN32
	if( _argc == 3 )
	{
		Str param1 = _argv[1];
		param1.ToUpper();
		if( param1 == "CACHE" )
		{
			ResourceRef resource = ResourceManager::GetInstance().GetResourceFromFile<Texture>(_argv[2]);
			return 0;
		}
	}
#endif

	if( Init(_argc,_argv) )
	{
		if (m_bUseEditor)
		{
#ifdef USE_QT
			qApp->exec();
#endif
		}
		else
		{
			Loop();
		}
	}

	Release();

	return 0;
}

//---------------------------------

Bool Application::Init(S32 _argc,Char ** _argv)
{
	Timer initTimer;
	initTimer.Start();
#ifdef WIN32
	#if !defined(MASTER) // enable floating point exception
		unsigned int x86_cw;
		_controlfp_s(&x86_cw, 0, _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_INVALID | _EM_DENORMAL);
	#endif

	#ifdef MEMORY_CHECK
		MemoryAllocator::GetInstance().StopTracking();
	#endif

	#ifdef USE_LOGGER
		LOGGER_Init(FL_LOG_CONSOLE|FL_LOG_DEBUG);
		Logger::GetInstance().AddNewLogger(NEW OldFileLogger("game.log",TRUE));
	#endif

	#ifdef MEMORY_CHECK
		MemoryAllocator::GetInstance().StartTracking();
	#endif
#elif defined(IOS)
	LOGGER_Init(FL_LOG_DEBUG);
#elif defined(ANDROID)
	LOGGER_Init(FL_LOG_DEBUG);
#endif

	// init components
	EntityComponentFactory::GetInstance().InitAll();

	LOGGER_Log("Init - params.lua\n");
	Script params;
	if( !params.LoadFromFile("script/params.lua") )
	{
		DEBUG_Forbidden("Unable to load params.lua\n");
		return FALSE;
	}
	params.Execute();
	
	LOGGER_Log("Init - init.lua\n");
	Script init;
	if( !init.LoadFromFile("script/init.lua") )
	{
		DEBUG_Forbidden("Unable to load init.lua\n");
		return FALSE;
	}
	init.Execute();

	if (m_bUseSteam)
	{
#ifdef USE_STEAM
		LOGGER_Log("Init - Steam\n");
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
		{
			DEBUG_Forbidden("Need restart within Steam\n");
			return FALSE;
		}
		if (!SteamAPI_Init())
		{
			DEBUG_Forbidden("Unable to init Steam\n");
			return FALSE;
		}

		// set our debug handler
		SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);
#endif
	}

	// input
	LOGGER_Log("Init - inputs\n");
	InputManager::GetInstance().Initialize();

	// sound
	LOGGER_Log("Init - sounds\n");
	SoundManager::GetInstance().Initialize();
	//m_MainTheme = SoundManager::GetInstance().PlaySound(m_sMainThemeEvent);

	// add global entities
	InitWorld();

	// 3D driver
#ifdef USE_METAL
	if (m_bUseMetal)
		m_p3DDriver = NEW RendererDriverMetal();
	else
#endif
	{
		Bool bUseGL = FALSE;
		for (S32 a = 1; a < _argc; a++)
		{
			Str sArg = _argv[a];
			if (sArg == "-gl")
				bUseGL = TRUE;
		}

		if (bUseGL)
			m_p3DDriver = NEW RendererDriverGL();

		// default
		if (!m_p3DDriver)
		{
#ifdef USE_VULKAN
			m_p3DDriver = NEW RendererDriverVulkan();
#else
			m_p3DDriver = NEW RendererDriverGL();
#endif
		}
	}

#ifdef USE_GPU_PROFILER
	RendererDriver::g_pProfilerDriver = m_p3DDriver;
#endif

	Window::InitPlatform();

	m_p3DDriver->Initialize();

	RendererDatas::GetInstance().Initialize();

	// create screen
	LOGGER_Log("Init - create window 3d screen\n");
	RefTo<Window3DScreen> p3DScreenRef = NEW Window3DScreen(0, 0, m_iResolutionX, m_iResolutionY);
	m_p3DScreen = p3DScreenRef;
	m_p3DScreen->GetRenderer()->SetPipeline(RendererPipeline::Full);

	// window
#ifndef USE_EDITOR // force no editor if bad exe
	m_bUseEditor = FALSE;
#endif

#ifdef WIN32
	if (m_bUseEditor)
	{
#ifdef USE_EDITOR
		// editor
		LOGGER_Log("Init - editor\n");
		m_Editor = Editor::CreateEditor();
		m_p3DScreen->SetUseEditor(TRUE);
		if (!m_p3DScreen->Create(NULL))
		{
			DEBUG_Forbidden("Unable to create 3d screen\n");
			return FALSE;
		}
#endif
	}
	else
	{
		Window::Params params;
		params.m_x = 50;
		params.m_y = 50;
		params.m_iWidth = m_iResolutionX;
		params.m_iHeight = m_iResolutionY;
		params.m_sTitle = m_sWindowTitle;
		if (m_bFullscreen)
			params.m_Style |= WindowStyle_Fullscreen;
		m_Window = NEW Window(params);
		m_Window->AddComponent(p3DScreenRef);
		m_Window->ManageInput(TRUE);

		if (!m_Window->Create(NULL))
		{
			DEBUG_Forbidden("Unable to create window\n");
			return FALSE;
		}
		m_Window->Show();
		m_Window->SetFocus();
	}
#else
	if (!m_p3DScreen->Create(NULL))
	{
		DEBUG_Forbidden("Unable to create 3d screen\n");
		return FALSE;
	}
#endif

	// imgui
	IMGUIMGR_Init(WindowPtr(m_Window), m_p3DScreen);

	// flow
	LOGGER_Log("Init - flow\n");
	m_pFlow = NEW FlowScriptManager(m_sStartScript);
	
	// fonts
	FontManager::GetInstance().LoadFontResources();

	// start editor
#ifdef USE_EDITOR
	if(m_Editor.IsValid())
		m_Editor->Start(this, m_iResolutionX, m_iResolutionY, m_sWindowTitle, m_bFullscreen);
#endif
	
#ifdef USE_EDITOR_V2
	m_Editor = NEW Editor();
#endif

	LOGGER_Log("Init - timer\n");
	m_Timer.Reset();

	// commands register
#define COMMAND(_name) RegisterCommand(#_name,&Application::COMMAND_##_name); if(false)
#include "application_commands.h"
#undef COMMAND

	//_CrtCheckMemory();

#if _DEBUG
	// generate doc
	EntityComponentFactory::GetInstance().GenerateDoc(SCRIPT_DOC_FILE);
#endif

	LOGGER_Log("Init - Duration : %.2fs\n", initTimer.GetSecondCount());

	return TRUE;  	
}

//---------------------------------

void Application::InitWorld()
{
	World::GetInstance().AddNewComponentToEntity(*World::GetInstance().GetRoot(), "SpriteManager");
}

//---------------------------------

Bool Application::Release(Bool _expectStatic /*= false*/)
{
#ifdef USE_MULTITHREAD_RENDERING
	m_RenderThread.WaitForCompletion();
#endif
	m_pFlow.SetNull();
	World::GetInstance().Update(0.f); // remove entities from flow
	World::DestroyInstance();
	m_MainTheme.SetNull();
	IMGUIMGR_Release();
	DebugRenderer::DestroyInstance();
	SoundManager::DestroyInstance();
	TextureManager::DestroyInstance();
	InputManager::GetInstance().Finalize();
	InputManager::GetInstance().DestroyInstance();
	ResourceManager::DestroyInstance();
	m_Editor.SetNull();
#ifdef USE_MULTITHREAD_RENDERING
	m_RenderThread.Run(DestroyDraw,this);
	m_RenderThread.WaitForCompletion();
#else
	DestroyDraw(this);
#endif
	FontManager::DestroyInstance();

#ifdef USE_QT
	m_p3DScreen->Destroy();
	DELETE m_p3DScreen;
#else
	if( m_Window.IsValid() )
	{
		m_Window->Destroy();
		m_Window.SetNull(); 
	}
#endif
	RendererDatas::GetInstance().Finalize();
	RendererDatas::GetInstance().ManageDatas(m_p3DDriver);
	RendererDatas::DestroyInstance();

	m_p3DDriver->Finalize();

	m_p3DScreen.SetNull();

	Window::DestroyPlatform();
	
	m_p3DDriver.SetNull();

	if(!_expectStatic )
	{
		ScriptRegisterManager::DestroyInstance();
		LoaderFactory::DestroyInstance();
		EntityComponentFactory::DestroyInstance();
		PlatformManager::DestroyInstance();
#ifdef USE_PROFILER
		Profiler::DestroyInstance();
#endif
	}
	
	m_sWindowTitle = "";
	m_MainTheme.SetNull();
	SoundManager::DestroyInstance();
#ifdef USE_DRAWDEBUG
	DebugRenderer::DestroyInstance();
#endif
	m_PreloadTextures.Flush();
	m_sMainThemeEvent = "";
	m_sStartScript = "";
	NAME_RemoveAll();
	GAMENAME_RemoveAll();

#ifdef USE_STEAM
	SteamAPI_Shutdown();
#endif

#ifdef MEMORY_CHECK
	MemoryAllocator::GetInstance().Flush();
	MemoryAllocator::GetInstance().StopTracking();
	MemoryAllocator::GetInstance().StopSafe();
#endif
#ifdef USE_LOGGER
	Logger::DestroyInstance();
#endif

	PROFILER_OPTICK_SHUTDOWN();

	return TRUE;
}

//---------------------------------

void Application::Loop()
{
	do
	{
		DoFrame();
	}
#ifdef IOS
    while(TRUE);
#else
	while(!m_Window->MustKill());
#endif
}

//---------------------------------

Float Application::ComputeDeltaTime()
{
	Float fTime = m_Timer.GetSecondCount();
	PROFILER_Begin("ComputeTime");
	while (fTime < 0.001f)
	{
		fTime = m_Timer.GetSecondCount();
	} 
	
	m_Timer.Reset();

	if (fTime>0.1f)
		fTime = 0.1f;

	PROFILER_End();

	return fTime;
}

//---------------------------------

void Application::DoFrame()
{
	PROFILER_OPTICK_FRAME("FullFrame");
#ifdef USE_PROFILER
	Profiler::GetInstance().Flush();
#endif
	DoFrame(ComputeDeltaTime());
}

//---------------------------------

void Application::DoFrame(Float _dTime)
{
	if( m_bPaused )
		return;

	if(m_bOnResumeNextFrame)
	{
		m_pFlow->OnPause();

		//ResourceManager::GetInstance().OnResume();
		//SoundManager::GetInstance().Resume();
		m_bOnResumeNextFrame = FALSE;
	}

	PROFILER_Begin("DoFrame"); // FIRST PROFILER EVENT
	ManageEvents();
	UpdateFrame(_dTime);
	RenderFrame();
	EndFrame();
	PROFILER_End();
}

//---------------------------------

void Application::ManageEvents()
{
#ifdef WIN32
	PROFILER_Begin("Window");
#ifdef USE_EDITOR
	if (m_Editor.IsValid())
	{
		const NameArray&	commands = m_Editor->GetCommands();
		for (U32 i = 0; i < commands.GetCount(); i++)
		{
			LaunchCommand(commands[i]);
		}
		m_Editor->FlushCommands();
	}
	
#endif
#ifdef USE_EDITOR_V2
	m_Editor->VisitAndFlushCommands([this](const Name& _cmd)
		{
			LaunchCommand(_cmd);
			return VisitorReturn::Continue;
		});
#endif

	if(m_Window.IsValid())
	{
		U32Array	commands;
		m_Window->ManageMessages(commands);
		//for (U32 i = 0; i<commands.GetCount(); i++)
		//{
		//	LaunchMenuEvent((MenuEvent)commands[i]);
		//}
	}

	PROFILER_End();
#endif

#ifndef MASTER
	m_bAskRefresh = m_bAskRefresh || InputManager::GetInstance().GetKeyboard().IsJustPressed(KEY_F5);
	if (m_bAskRefresh)
	{
		m_bAskRefresh = FALSE;
#ifdef USE_MULTITHREAD_RENDERING
		m_RenderThread.WaitForCompletion();
#endif
		if (m_pFlow->GetCurrentScriptPath().GetLength()) 
		{
			m_pFlow->ForceNextScript(m_pFlow->GetCurrentScriptPath());			
			//m_p3DScreen->GetRenderer()->DestroyShaders();
			//m_p3DScreen->GetRenderer()->ReloadParams(m_p3DDriver);
			//m_p3DScreen->GetRenderer()->CreateShaders();
		}
	}
#endif
}

//---------------------------------

void Application::UpdateFrame(Float _dTime)
{
	PROFILER_Begin("Update");
	m_UpdateTimer.Reset();
	_dTime *= m_fTimeFactor;

#ifdef USE_STEAM
	SteamAPI_RunCallbacks();
#endif

	PROFILER_Begin("Input");
	InputManager::GetInstance().Update(_dTime, *m_p3DScreen);
	UpdateComputeIsIn();
	PROFILER_End();

	PROFILER_Begin("ImguiMgr");
	IMGUIMGR_Update(_dTime);
	PROFILER_End();

	// check resources validity for autorefresh
	PROFILER_Begin("ResourceManager");
	ResourceManager::GetInstance().Update(_dTime);
	PROFILER_End();

	PROFILER_Begin("Flow");
	m_pFlow->Update(_dTime);
	PROFILER_End();

	PROFILER_Begin("World");
	World::GetInstance().Update(_dTime);
	PROFILER_End();

	PROFILER_Begin("Sound");
	SoundManager::GetInstance().Update();
	PROFILER_End();
	
	PROFILER_Begin("Plateform");
	PlatformManager::GetInstance().Update(_dTime);
	PROFILER_End();

	PROFILER_Begin("Debug");
	UpdateDisplayDebug(_dTime);
	PROFILER_End();

	PROFILER_Begin("Editor");
#ifdef USE_EDITOR
	if(m_Editor.IsValid())
		m_Editor->Update(_dTime);
#endif

#ifdef USE_EDITOR_V2
	PROFILER_Begin("Editor");
	m_Editor->Update(_dTime);
	PROFILER_End();
#endif
	PROFILER_End();

	m_fUpdateDuration = m_UpdateTimer.GetSecondCount();
	PROFILER_End();
}

//---------------------------------

void Application::RenderFrame()
{
	PROFILER_Begin("Draw");

	// world draw (add to draw tables)
	m_DrawTimer.Reset();
	PROFILER_Begin("World_Draw");
	World::GetInstance().Draw(m_p3DScreen->GetRenderer());
	PROFILER_End();
	m_fDrawDuration = m_DrawTimer.GetSecondCount();

	PROFILER_Begin("ImguiMgr_Draw");
	IMGUIMGR_Draw(m_p3DScreen->GetRenderer());
	PROFILER_End();

#ifdef USE_EDITOR_V2
	PROFILER_Begin("Editor_Draw");
	m_Editor->GetGizmo().Draw(m_p3DScreen->GetRenderer());
	PROFILER_End();
#endif

#ifdef USE_DRAWDEBUG
	PROFILER_Begin("DrawDebug_Draw");
	DebugRenderer::GetInstance().Draw3D(m_p3DScreen->GetRenderer());
	PROFILER_End();
#endif

	PROFILER_End();
	   
#ifdef USE_MULTITHREAD_RENDERING
	PROFILER_Begin("WaitRenderThread");
	m_RenderThread.WaitForCompletion();
	PROFILER_End();
#endif

	PROFILER_Begin("PrepareDraw");
	m_DrawTimer.Reset();
	PROFILER_Begin("Flow_PrepareDraw");
	m_pFlow->PrepareDraw(m_p3DScreen->GetRenderer());
	PROFILER_End();

#ifdef USE_DRAWDEBUG
	PROFILER_Begin("DrawDebug_PrepareDraw");
	DebugRenderer::GetInstance().PrepareDraw(m_p3DScreen->GetRenderer());
	PROFILER_End();
#endif

	PROFILER_Begin("Renderer_PrepareDraw");
	m_p3DScreen->GetRenderer()->PrepareDraw(m_p3DDriver);
	PROFILER_End();

	PROFILER_Begin("ImguiMgr_PrepareDraw");
	IMGUIMGR_PrepareDraw(m_p3DDriver);
	PROFILER_End();

#ifdef USE_EDITOR_V2
	PROFILER_Begin("Editor_PrepareDraw");
	m_Editor->GetGizmo().PrepareDraw(m_p3DDriver);
	PROFILER_End();
#endif

	PROFILER_Begin("RendererDatas_PrepareDraw");
	RendererDatas::GetInstance().PrepareDraw();
	PROFILER_End();

#ifdef USE_DRAWDEBUG
	PROFILER_Begin("DebugRenderer_Reset");
	DebugRenderer::GetInstance().Reset();
	PROFILER_End();
#endif

	PROFILER_Begin("SetCamera");
	// setup camera (todo : camera priority ? activation/desactivation ?)
	Bool bFound = FALSE;
	RendererCameraRef camera;
	World::GetInstance().VisitComponents<EntityComponentCamera>([&](Entity* _pEntity, EntityComponentCamera* _pCamera)
	{
		camera = _pCamera->GetRendererCamera();
		bFound = TRUE;
		return VisitorReturn::Stop;
	});
	m_p3DScreen->GetRenderer()->SetCamera(camera);

#ifdef USE_EDITOR_V2
	if (m_Editor.IsValid())
	{
		if (m_Editor->GetCamera().IsEnabled())
			m_p3DScreen->GetRenderer()->SetCamera(m_Editor->GetCamera().GetRendererCamera());
		else if (m_p3DScreen->GetRenderer()->GetCamera())
			m_Editor->GetCamera().CopyFromCamera(m_p3DScreen->GetRenderer()->GetCamera());
	}
#endif
	PROFILER_End();

	m_fDrawDuration += m_DrawTimer.GetSecondCount();
	
#ifdef USE_MULTITHREAD_RENDERING
	if(m_bUseEditor)
		ThreadRedraw(this);
	else
		m_RenderThread.Run(ThreadRedraw, this);
#else
	ThreadRedraw(this);
#endif
	PROFILER_End();
}

//---------------------------------

void Application::EndFrame()
{
	PROFILER_Begin("EndFrame");
	ResourceManager::GetInstance().CleanResources();
	PROFILER_End();
}

//---------------------------------

void Application::UpdateDisplayDebug(Float _dTime)
{
	if (DEBUGFLAG(Marketing))
		return;

	if(DEBUGFLAG(Fps))
		UpdateDisplayFPS(_dTime/ m_fTimeFactor);
	if (DEBUGFLAG(Hierarchy))
		World::GetInstance().DrawDebugHierarchie();
	if (DEBUGFLAG(BBox))
		World::GetInstance().DrawDebugBBox();
	if (DEBUGFLAG(SafeFrame))
		DEBUG_DrawQuad(Vec3f::Zero, Vec3f((Float)m_iReferenceResX, (Float)m_iReferenceResY, 0.f), Color::Red);
#ifdef USE_PROFILER
	if(DEBUGFLAG(Profiler))
		Profiler::GetInstance().DrawDebug();
#endif
}

//---------------------------------

void Application::UpdateDisplayFPS(Float _dTime)
{
	S32 curY = 20;
	Str value;

	static Float prevFps = 0.f;
	Float curFps = (1.f / _dTime);
	prevFps = (prevFps * 29.f + curFps) / 30.f;
	S32 fps = (S32)prevFps;
	value.SetFrom("%d FPS", (S32)prevFps);
	DEBUG_DrawText(value.GetArray(), 10, curY, fps >= 30 ? Color::Green : Color::Red);
	curY += 20;

	value.SetFrom("U:%.2fms / D: %.2fms / R:%.2fms", m_fUpdateDuration*1000.f, m_fDrawDuration*1000.f, m_fRenderDuration*1000.f);
	DEBUG_DrawText(value.GetArray(), 10, curY, fps >= 30 ? Color::Green : Color::Red);
	curY += 20;

#ifdef MEMORY_ALLOCATOR
	{
		U64 memory = MemoryAllocator::GetInstance().GetTotalAllocSize();
		U64 maxmemory = MemoryAllocator::GetInstance().GetMaxTotalAllocSize();
		value.SetFrom("RAM : %.2f/%.2f Mo", ((Float)memory) / (1024.f*1024.f), ((Float)maxmemory) / (1024.f*1024.f));
		DEBUG_DrawText(value.GetArray(), 10, curY, Color::Grey);
		curY += 20;
	}
#endif
	U64 memory = RendererDatas::GetInstance().GetCurTextureMemory();
	U64 maxmemory = RendererDatas::GetInstance().GetMaxTextureMemory();
	value.SetFrom("VRAM: %.2f/%.2fMo ", ((Float)memory) / (1024.f*1024.f), ((Float)maxmemory) / (1024.f*1024.f));
	DEBUG_DrawText(value.GetArray(), 10, curY, Color::Grey);
	curY += 20;

	m_iFrameCount++;
	value.SetFrom("%d", m_iFrameCount);
	DEBUG_DrawText(value.GetArray(), 10, curY, Color::Blue);
	curY += 20;

	if (abs(m_fTimeFactor - 1.f) > Float_Eps)
	{
		Str value;
		value.SetFrom("x%.2f", m_fTimeFactor);
		DEBUG_DrawText(value.GetArray(), 10, curY, Color::Yellow);
	}
}

//---------------------------------

void Application::Pause()
{
	LOGGER_Log("m_MainTheme->Pause");
	//if( m_MainTheme.IsValid() )
	//{
	//	m_MainTheme->Pause();
	//	SoundManager::GetInstance().Update();
	//}
	m_bPaused = TRUE;
#ifndef ANDROID
	SoundManager::GetInstance().Pause();
	ResourceManager::GetInstance().OnPause();
#endif
}

//---------------------------------

void Application::Resume()
{
	LOGGER_Log("Application::Resume");
#ifndef ANDROID
	ResourceManager::GetInstance().OnResume();
	SoundManager::GetInstance().Resume();
#endif
	m_Timer.Reset();
	m_bOnResumeNextFrame = TRUE;
	m_bPaused = FALSE;
}

//---------------------------------

void	Application::AddPreloadTexture(const Str& _path)
{
	m_PreloadTextures.AddLastItem(_path);
}

//---------------------------------

void	Application::StartPreload()
{
	Preload(this);
}

//---------------------------------

Bool 	Application::PreloadDone() const
{
	return TRUE;
}

//---------------------------------

void Application::Preload(void* _game)
{
	Application*game = (Application*)_game;
	for(U32 i=0; i<game->m_PreloadTextures.GetCount(); i++)
	{
		ResourceRef resource = ResourceManager::GetInstance().GetResourceFromFile<Texture>(game->m_PreloadTextures[i]);
		if( resource.IsValid() )
			resource->Lock();
		else
		{
			LOGGER_LogError("Unable to preloadtexture  %s !!\n",game->m_PreloadTextures[i].GetArray());
		}
	}
}

//---------------------------------

void Application::UpdateComputeIsIn()
{
	Bool isOn2D = FALSE;
	World::GetInstance().VisitComponents<EntityComponentInputClick>([&](Entity* _pEntity, EntityComponentInputClick* _pClick)
	{
		Bool isIn = _pClick->ComputeIsIn();
		if (isIn && _pClick->Is2D() && !_pEntity->IsHidden())
		{
			isOn2D = TRUE;
			return VisitorReturn::Stop;
		}

		return VisitorReturn::Continue;
	});

	InputManager::GetInstance().SetOn2D(isOn2D);
}

//---------------------------------

Bool Application::HasFocus() const
{
#ifdef USE_EDITOR
	if(m_Editor.IsValid())
		return m_Editor->isActiveWindow();
#endif

#ifdef WIN32
	if(m_Window.IsValid())
		return m_Window->HasFocus();
#endif

	return TRUE;
}

//---------------------------------

void	Application::LaunchCommand(const Name& _command)
{
	SApplicationCommand* cmd = m_Commands.GetItem(_command);
	if (cmd)
	{
		(this->*(cmd->m_pFunction))();
	}
	else
	{
		LOGGER_LogError("Unknown command %s", _command.GetStr().GetArray());
	}
}

//---------------------------------

void	Application::RegisterCommand(const Name& _command, const CommandCallback& _function)
{
	SApplicationCommand newCmd;
	newCmd.m_nName = _command;
	newCmd.m_pFunction = _function;
	m_Commands.AddItem(_command, newCmd);
}