#include "platform_manager.h"

#include "script/flow_script.h"
#include "platform_win32.h"
#include "game.h"	// crado

SINGLETON_Define(PlatformManager);
BEGIN_SCRIPT_CLASS(PlatformManager)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,PlatformManager)
	SCRIPT_VOID_METHOD(Init)
	SCRIPT_VOID_METHOD(SignIn)
	SCRIPT_VOID_METHOD(SignOut)
	SCRIPT_VOID_METHOD1(UnlockAchievement,Str)
	SCRIPT_VOID_METHOD2(SubmitHighScore,Str,S64)
	SCRIPT_VOID_METHOD(ShowAchievements)
	SCRIPT_VOID_METHOD(ShowLeaderboards)
	SCRIPT_VOID_METHOD(ResetAchievements)
	SCRIPT_VOID_METHOD1(ShowLeaderboard,Str)
	SCRIPT_VOID_METHOD(LaunchPurchase)
	SCRIPT_VOID_METHOD(ResetPurchase)
	SCRIPT_RESULT_METHOD(IsSignInProgress,Bool)
	SCRIPT_RESULT_METHOD(PurchaseSuccess,Bool)
	SCRIPT_RESULT_METHOD(PurchaseInProgress,Bool)
	SCRIPT_RESULT_METHOD(IsLoadSaveFromCloudInProgress,Bool)
END_SCRIPT_CLASS

#ifdef ANDROID
struct IDMapping
{
	Name	m_Name;
	Str		m_ID;
};

IDMapping g_Mapping[] = {
  "achievement_discovered","CgkI_ciKufcNEAIQAA",
  "achievement_burned","CgkI_ciKufcNEAIQAQ",
  "achievement_drowned","CgkI_ciKufcNEAIQAg",
  "achievement_suffocated","CgkI_ciKufcNEAIQAw",
  "achievement_transformed","CgkI_ciKufcNEAIQFw",
  "achievement_science_intern","CgkI_ciKufcNEAIQBA",
  "achievement_apprentice","CgkI_ciKufcNEAIQBQ",
  "achievement_scientist","CgkI_ciKufcNEAIQBg",
  "achievement_expert","CgkI_ciKufcNEAIQBw",
  "achievement_professor","CgkI_ciKufcNEAIQFQ",
  "achievement_chaining_star","CgkI_ciKufcNEAIQCA",
  "achievement_diversity","CgkI_ciKufcNEAIQCQ",
  "achievement_ultrices_hyacintho","CgkI_ciKufcNEAIQCg",
  "achievement_purpura_lignum","CgkI_ciKufcNEAIQCw",
  "achievement_aurantiaco_ignis","CgkI_ciKufcNEAIQDA",
  "achievement_tardus_viridis","CgkI_ciKufcNEAIQDQ",
  "achievement_exploda_bulla","CgkI_ciKufcNEAIQDg",
  "achievement_mutatio_colorem","CgkI_ciKufcNEAIQFg",
  "leaderboard_overall_score","CgkI_ciKufcNEAIQDw",
  "leaderboard_total_score_chapter_1","CgkI_ciKufcNEAIQEA",
  "leaderboard_total_score_chapter_2","CgkI_ciKufcNEAIQEQ",
  "leaderboard_total_score_chapter_3","CgkI_ciKufcNEAIQEg",
  "leaderboard_total_score_chapter_4","CgkI_ciKufcNEAIQEw",
  "leaderboard_total_score_chapter_5","CgkI_ciKufcNEAIQGA"
};
#endif

PlatformManager::PlatformManager()
{
	m_bSignInProgress = FALSE;
	m_bMustLaunchPurchase = FALSE;
	m_bPurchaseInProgress = FALSE;
	m_bPreviousPurchaseSuccess = FALSE;
	m_bLoadSaveFromCloudInProgress = FALSE;
	m_bIsSignedIn = FALSE;
#ifdef ANDROID
	m_pGameService = NULL;
	m_Activity = NULL;
	m_fSnapshotDuration = 0.f;
#endif
#ifdef IOS
    m_ViewController = NULL;
#endif
}

//------------------------------------------

PlatformManager::~PlatformManager()
{
}

//------------------------------------------

void PlatformManager::Update(Float _dTime)
{
#ifdef ANDROID
	m_fSnapshotDuration += _dTime;
#endif
}

//------------------------------------------

#ifdef ANDROID
void OnAuthActionStarted(gpg::AuthOperation op)
{
	PlatformManager::GetInstance().SetSignInProgress(TRUE);
	LOGGER_Log("PlatformManager::OnAuthActionStarted\n");
}

//------------------------------------------

void OnAuthActionFinished(gpg::AuthOperation op,gpg::AuthStatus status)
{
	LOGGER_Log("PlatformManager::OnAuthActionFinished\n");

	if( status == gpg::AuthStatus::VALID )
	{
		PlatformManager::GetInstance().SetIsSignedIn(TRUE);
		Game::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->LoadFromFile();
		PlatformManager::GetInstance().SetSignInProgress(FALSE);
		//PlatformManager::GetInstance().ShowSaveUI();
	}
	else
	{
		Str err = "FAILED";
		switch(status)
		{
		case gpg::AuthStatus::ERROR_INTERNAL :					err = "ERROR_INTERNAL"; break;
		case gpg::AuthStatus::ERROR_NOT_AUTHORIZED  :			err = "ERROR_NOT_AUTHORIZED"; break;
		case gpg::AuthStatus::ERROR_VERSION_UPDATE_REQUIRED  :	err = "ERROR_VERSION_UPDATE_REQUIRED"; break;
		case gpg::AuthStatus::ERROR_TIMEOUT : 					err = "ERROR_TIMEOUT"; break;
		}

		LOGGER_Log("-> Failed Status = %s\n",err.GetArray());
		PlatformManager::GetInstance().SetSignInProgress(FALSE);
	}
}

//------------------------------------------

Str PlatformManager::GetID(const Name& _name)
{
	for(S32 i=0; i<_countof(g_Mapping); i++)
	{
		if( g_Mapping[i].m_Name == _name )
			return g_Mapping[i].m_ID;
	}

	return "";
}
#endif

//------------------------------------------

#ifdef ANDROID
std::unique_ptr<gpg::GameServices> PlatformManager::m_pGameService;
#endif

void PlatformManager::Init()
{
#ifdef ANDROID
	LOGGER_Log("PlatformManager::Init\n");
	if (!m_pGameService)
	{
		LOGGER_Log("->Activity %x\n",m_Activity);
		gpg::AndroidPlatformConfiguration platform_configuration;
		platform_configuration.SetActivity(m_Activity);

	    m_pGameService = gpg::GameServices::Builder()
	    	.SetDefaultOnLog(gpg::LogLevel::VERBOSE)
	    	.EnableSnapshots()
			.SetOnAuthActionStarted(&OnAuthActionStarted)
			.SetOnAuthActionFinished(&OnAuthActionFinished)
	        .Create(platform_configuration);

	    LOGGER_Log("->Ok %x\n",m_pGameService.get());
	  }
#endif
}

//------------------------------------------

#ifndef IOS
void PlatformManager::SignIn()
{
#ifdef ANDROID
	LOGGER_Log("PlatformManager::SignIn\n");
	if (m_pGameService && !m_pGameService->IsAuthorized())
	{
		LOGGER_Log("->Ok\n");
		m_pGameService->StartAuthorizationUI();
	}
#endif
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::SignOut()
{
#ifdef ANDROID
	LOGGER_Log("PlatformManager::SignOut\n");
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		LOGGER_Log("->Ok\n");
		m_pGameService->SignOut();
		m_bIsSignedIn = FALSE;
	}
#endif
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::UnlockAchievement(const Str& achievement_id)
{
#ifdef ANDROID
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		m_pGameService->Achievements().Unlock(GetID(achievement_id).GetArray());
	}
#endif
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::SubmitHighScore(const Str& leaderboard_id, S64 score)
{
#ifdef ANDROID
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		m_pGameService->Leaderboards().SubmitScore(GetID(leaderboard_id).GetArray(), score);
	}
#endif
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::ShowAchievements()
{
#ifdef ANDROID
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		m_pGameService->Achievements().ShowAllUI();
	}
#endif
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::ResetAchievements()
{
	// todo
}
#endif

//------------------------------------------

#ifndef IOS
void PlatformManager::ShowLeaderboards()
{
#ifdef ANDROID
	/*ShowSaveUI();
	return;*/

	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		m_pGameService->Leaderboards().ShowAllUI();
	}
#endif
}
#endif

//------------------------------------------

void PlatformManager::ShowLeaderboard(const Str& leaderboard_id)
{
#ifdef ANDROID
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		m_pGameService->Leaderboards().ShowUI(GetID(leaderboard_id).GetArray());
	}
#endif
}

//------------------------------------------

#define ALLOW_CREATE_SNAPSHOT	true
#define ALLOW_DELETE_SNAPSHOT 	true
#define MAX_SNAPSHOTS 4
#define SNAPSHOT_UI_TITLE "Select your game"
#define SNAPTSHOT_DESC "Splode'n'Die save data"
#define SAVE_FILENAME	"snapshotSplodeNDie"

#ifdef ANDROID
std::string GenerateSaveFileName();
#endif

void PlatformManager::ShowSaveUI()
{
#ifdef ANDROID
	__android_log_print(ANDROID_LOG_INFO,"DebugSave","PlatformManager::ShowSaveUI()\n");
	if (m_pGameService && m_pGameService->IsAuthorized())
	{
		__android_log_print(ANDROID_LOG_INFO,"DebugSave","->ShowSelectUIOperationBlocking\n");
		gpg::SnapshotManager::SnapshotSelectUIResponse response = m_pGameService->Snapshots().ShowSelectUIOperationBlocking(gpg::Timeout(24*60*60*1000),ALLOW_CREATE_SNAPSHOT,ALLOW_DELETE_SNAPSHOT,MAX_SNAPSHOTS,SNAPSHOT_UI_TITLE);
		if (IsSuccess(response.status))
		{
			__android_log_print(ANDROID_LOG_INFO,"DebugSave","->response.status = SUCCESS\n");
			if (response.data.Valid())
			{
				__android_log_print(ANDROID_LOG_INFO,"DebugSave","->response.data = VALID\n");
				LOGGER_Log("Description: %s", response.data.Description().c_str());
				LOGGER_Log("FileName %s", response.data.FileName().c_str());
				//Opening the snapshot data

				m_Snapshot = response.data;
				Game::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->LoadFromFile();
				//LoadFromSnapshot();
			}
			else
			{
				__android_log_print(ANDROID_LOG_INFO,"DebugSave","->response.data = NOT VALID\n");
				LOGGER_Log("Creating new snapshot");
				Game::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->SaveToFile();
			}
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO,"DebugSave","->response.status = NOT SUCCESS\n");
			LOGGER_LogError("ShowSelectUIOperation returns an error %s", gpg::DebugString(response.status).c_str());
		}

		__android_log_print(ANDROID_LOG_INFO,"DebugSave","->End\n");
		PlatformManager::GetInstance().SetSignInProgress(FALSE);
	}
#endif
}

//------------------------------------------


void PlatformManager::SaveSnapshot(std::vector< uint8_t >& _data)
{
#ifdef ANDROID
	static std::vector< uint8_t > g_dataToSave;
	std::string fileName;
	if (m_Snapshot.Valid() == false)
	{
		fileName = SAVE_FILENAME;
		LOGGER_Log("Creating new snapshot %s", fileName.c_str());
	}
	else
	{
		fileName = m_Snapshot.FileName();
		LOGGER_Log("Snapshot exist %s", fileName.c_str());
	}

#if 1
	g_dataToSave = _data;
	m_pGameService->Snapshots().Open(fileName,gpg::SnapshotConflictPolicy::LONGEST_PLAYTIME,[this](gpg::SnapshotManager::OpenResponse const & response) 
	{
        LOGGER_Log("Opened file");
		if (IsSuccess(response.status))
		{
			m_Snapshot = response.data;
			gpg::Duration duration = m_Snapshot.PlayedTime();
			duration += gpg::Duration((int)(m_fSnapshotDuration * 1000.f));

			LOGGER_Log("(debugsnap) SaveSnapshot duration = %d, addduration = %d, newduration = %d \n",(S32)m_Snapshot.PlayedTime().count(),(S32)(m_fSnapshotDuration * 1000.f),(S32)duration.count());

			m_fSnapshotDuration = 0.f;

			gpg::SnapshotMetadataChange::Builder builder;
			gpg::SnapshotMetadataChange metadata_change = builder.SetDescription(SNAPTSHOT_DESC).SetPlayedTime(duration).Create();

			gpg::SnapshotManager::CommitResponse commitResponse = m_pGameService->Snapshots().CommitBlocking(m_Snapshot, metadata_change,g_dataToSave);

			if (IsSuccess(commitResponse.status))
			{
				LOGGER_Log("Saved game (Game duration = %d ms)",(S32)duration.count());
			}
			else
			{
				LOGGER_LogError("Saved game failed error: %d", commitResponse.status);
			}
		}
    });
#else
	gpg::SnapshotManager::OpenResponse response = m_pGameService->Snapshots().OpenBlocking(fileName,gpg::SnapshotConflictPolicy::LONGEST_PLAYTIME);
	LOGGER_Log("Opened file");
	if (IsSuccess(response.status))
	{
		m_Snapshot = response.data;
		gpg::Duration duration = m_Snapshot.PlayedTime();
		duration += gpg::Duration((int)(m_fSnapshotDuration * 1000.f));

		LOGGER_Log("(debugsnap) SaveSnapshot duration = %d, addduration = %d, newduration = %d \n",(S32)m_Snapshot.PlayedTime().count(),(S32)(m_fSnapshotDuration * 1000.f),(S32)duration.count());

		m_fSnapshotDuration = 0.f;

		gpg::SnapshotMetadataChange::Builder builder;
		gpg::SnapshotMetadataChange metadata_change = builder.SetDescription(SNAPTSHOT_DESC).SetPlayedTime(duration).Create();

		gpg::SnapshotManager::CommitResponse commitResponse = m_pGameService->Snapshots().CommitBlocking(m_Snapshot, metadata_change,_data);

		if (IsSuccess(commitResponse.status))
		{
			LOGGER_Log("Saved game (Game duration = %d ms)",(S32)duration.count());
		}
		else
		{
			LOGGER_LogError("Saved game failed error: %d", commitResponse.status);
		}
	}
#endif
#endif
}

//------------------------------------------

void PlatformManager::LoadFromSnapshot(std::vector< uint8_t >& _data)
{
#ifdef ANDROID
	if( !m_pGameService )
		return;

#if 1
	m_pGameService->Snapshots().Open(SAVE_FILENAME,gpg::SnapshotConflictPolicy::LONGEST_PLAYTIME,[this](gpg::SnapshotManager::OpenResponse const & response) 
	{
		LOGGER_Log("Opened file\n");
		if (IsSuccess(response.status))
		{
			if (response.data.Valid() == false)
			{
				return;
			}

			LOGGER_Log("Reading file\n");
			m_Snapshot = response.data;

			gpg::Duration duration = m_Snapshot.PlayedTime();
			m_fSnapshotDuration = 0.f;
			LOGGER_Log("(debugsnap) LoadSnapshot duration = %d \n",(S32)duration.count());

			gpg::SnapshotManager::ReadResponse responseRead = m_pGameService->Snapshots().ReadBlocking(response.data);
			if (IsSuccess(responseRead.status))
			{
				//_data = responseRead.data;
				Game::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->LoadFromFile(&responseRead.data);
				LOGGER_Log("Loaded save\n");
			}
		}
	});
#else
	LOGGER_Log("Opening file\n");
	gpg::SnapshotManager::OpenResponse response= m_pGameService->Snapshots().OpenBlocking(SAVE_FILENAME,gpg::SnapshotConflictPolicy::LONGEST_PLAYTIME);
	LOGGER_Log("Opened file\n");
	if (IsSuccess(response.status))
	{
		if (response.data.Valid() == false)
		{
			return;
		}

		LOGGER_Log("Reading file\n");
		m_Snapshot = response.data;

		gpg::Duration duration = m_Snapshot.PlayedTime();
		m_fSnapshotDuration = 0.f;
		LOGGER_Log("(debugsnap) LoadSnapshot duration = %d \n",(S32)duration.count());

		gpg::SnapshotManager::ReadResponse responseRead = m_pGameService->Snapshots().ReadBlocking(response.data);
		if (IsSuccess(responseRead.status))
		{
			Game::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->LoadFromFile(&responseRead.data);
			LOGGER_Log("Loaded save\n");
		}
	}
#endif
#endif
}

/*
 * Generate a unique filename
 */
#ifdef ANDROID
std::string GenerateSaveFileName()
{
	// Generates random filename
	timeval Time;
	gettimeofday(&Time, NULL);

	std::ostringstream str;
	str << "snapshotSplode-" << Time.tv_usec;

	return str.str();
}
#endif

#ifdef WIN32
ArrayOf<PlatformManager::Monitor>* g_pMonitors;

BOOL EnumMonitor(HMONITOR _monitor, HDC, LPRECT, LPARAM)
{
	MONITORINFO info = {};
	info.cbSize = sizeof(MONITORINFO);
	if (!::GetMonitorInfo(_monitor, &info))
		return TRUE;

	PlatformManager::Monitor& monitor = g_pMonitors->AddLastItem();
	monitor.m_vPos = Vec2f((Float)info.rcMonitor.left, (Float)info.rcMonitor.top);
	monitor.m_vSize = Vec2f((Float)(info.rcMonitor.right - info.rcMonitor.left), (Float)(info.rcMonitor.bottom - info.rcMonitor.top));
	monitor.m_vWorkPos = Vec2f((Float)info.rcWork.left, (Float)info.rcWork.top);
	monitor.m_vWorkSize = Vec2f((Float)(info.rcWork.right - info.rcWork.left), (Float)(info.rcWork.bottom - info.rcWork.top));
	monitor.m_fDpiScale = 1.f;// todo
	monitor.m_bPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) != 0);

	return TRUE;
};
#endif

void PlatformManager::FillMonitorList(ArrayOf<PlatformManager::Monitor>& _monitors)
{
#ifdef WIN32
	g_pMonitors = &_monitors;
	::EnumDisplayMonitors(NULL, NULL, EnumMonitor, 0);
#endif
}

