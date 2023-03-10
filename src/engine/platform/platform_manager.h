#ifndef __PLATFORM_MANAGER_H__
#define __PLATFORM_MANAGER_H__

#ifdef IOS
#include "ViewController.h"

#endif

//------------------------------------------

class PlatformManager
{
	SINGLETON_Declare(PlatformManager);
	DECLARE_SCRIPT_CLASS(PlatformManager)

public:
	PlatformManager();
	~PlatformManager();

	void Update(Float _dTime);

	void Init();
	void SignIn();
	void SignOut();
	void UnlockAchievement(const Str& achievementId);
	void SubmitHighScore(const Str& leaderboardId, S64 score);
	void ShowAchievements();
	void ResetAchievements();
	void ShowLeaderboards();
	void ShowLeaderboard(const Str& leaderboardId);
	void SetIsSignedIn(Bool _signed)			{ m_bIsSignedIn = _signed; }
	void SetSignInProgress(Bool _inProgress)	{ m_bSignInProgress = _inProgress; }
	Bool IsSignInProgress() 					{ return m_bSignInProgress; }

#ifdef WIN32
	void LaunchPurchase()						{ OnEndPurchase(TRUE); }
#elif defined(IOS)
	void LaunchPurchase();
#else
	void LaunchPurchase()						{ m_bMustLaunchPurchase = TRUE; m_bPurchaseInProgress = TRUE; LOGGER_Log("Platform : LaunchPurchase\n");}
#endif
	
	Bool MustLaunchPurchase() const				{ return m_bMustLaunchPurchase; }
	void OnStartPurchase()						{ m_bMustLaunchPurchase = FALSE; LOGGER_Log("Platform : OnStartPurchase\n");}
	void OnEndPurchase(Bool _bSuccess)			{ m_bPurchaseInProgress = FALSE; m_bPreviousPurchaseSuccess = _bSuccess; LOGGER_Log("Platform : OnEndPurchase %s\n",_bSuccess?"SUCCESS":"FAIL");}
	void ResetPurchase() 						{ m_bPreviousPurchaseSuccess = FALSE; LOGGER_Log("Platform : ResetPurchase \n");}
	Bool PurchaseSuccess() const				{ return m_bPreviousPurchaseSuccess; }
	Bool PurchaseInProgress() const				{ return m_bPurchaseInProgress; }

	void SetLoadSaveFromCloudInProgress(Bool _progress)		{ m_bLoadSaveFromCloudInProgress = _progress; }
	Bool IsLoadSaveFromCloudInProgress() const		{ return m_bLoadSaveFromCloudInProgress; }

	void ShowSaveUI();
	void SaveSnapshot(std::vector< uint8_t >& _data);
	void LoadFromSnapshot(std::vector< uint8_t >& _data);

#ifdef ANDROID
	void SetActivity(jobject _activity)	{ m_Activity = _activity; }
#endif
    
#ifdef IOS
	void SetViewController(ViewController* _view)	{ m_ViewController = _view; }
#endif

	struct Monitor
	{
		Vec2f m_vPos = Vec2f::Zero;
		Vec2f m_vSize = Vec2f::Zero;
		Vec2f m_vWorkPos = Vec2f::Zero;
		Vec2f m_vWorkSize = Vec2f::Zero;
		Float m_fDpiScale = 1.f;
		Bool m_bPrimary = FALSE;
	};
	template<class Visitor>
	void   VisitMonitors(Visitor&& _visitor); // (PlatformManager::Monitor) 

protected:
	void FillMonitorList(ArrayOf<Monitor>& _monitors);

	Bool m_bSignInProgress;
	Bool m_bIsSignedIn;
	Bool m_bMustLaunchPurchase;
	Bool m_bPurchaseInProgress;
	Bool m_bPreviousPurchaseSuccess;
	Bool m_bLoadSaveFromCloudInProgress;
#ifdef ANDROID
	Str GetID(const Name& _name);

	static std::unique_ptr<gpg::GameServices> 	m_pGameService;
	gpg::SnapshotMetadata 				m_Snapshot;
	Float 								m_fSnapshotDuration;
	jobject								m_Activity;
#endif
    
 #ifdef IOS
    ViewController* m_ViewController;
#endif
};

template<class Visitor>
void   PlatformManager::VisitMonitors(Visitor&& _visitor)
{
	ArrayOf<Monitor> monitors;
	FillMonitorList(monitors);	
	for (Monitor& monitor : monitors)
		_visitor(monitor);
}

#endif
