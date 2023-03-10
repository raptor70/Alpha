
COMMAND(Restart)
{
	m_pFlow->ForceNextScript(m_sStartScript);
}

//..

COMMAND(RefreshScript)
{
	m_bAskRefresh = TRUE;
}

//..

COMMAND(TexturesDump)
{
	ResourceManager::GetInstance().Dump<Texture>();
}

//..

COMMAND(DumpSound)
{
	SoundManager::GetInstance().Dump();
}

//..

COMMAND(LaunchProfiler)
{
#ifdef WIN32
#ifdef USE_OPTICK
	system("start " OPTICK_EXE);
#endif
#endif
}

//..

COMMAND(LaunchFrameDebugger)
{
#ifdef WIN32
#ifdef USE_RENDERDOC
	system("start " RENDERDOC_EXE);
#endif
#endif
}

//..

COMMAND(Quit)
{
#ifdef USE_QT
	m_QTApp.quit();
#else
	m_Window->Kill();
#endif
}

//..

COMMAND(Save)
{
	Application::GetInstance().GetFlowScriptManager()->SaveAll();
}

