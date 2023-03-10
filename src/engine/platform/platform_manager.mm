#include "platform_manager.h"
#import "GameKit/GameKit.h"
#import "AppDelegate.h"

//------------------------------------------

void CPlatformManager::SignIn()
{
	m_bSignInProgress = TRUE;
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error)
	{
        if (viewController != nil)
        {
			[m_ViewController presentViewController:viewController animated:YES completion:nil];
			LOGGER_Log("SignOut need viewController\n");
        }
        else 
		{
			m_bSignInProgress = FALSE;
			if (localPlayer.isAuthenticated)
			{
				LOGGER_Log("SignIn\n");
				m_bIsSignedIn = TRUE;
			}
			else
			{
				LOGGER_Log("SignOut\n");
				m_bIsSignedIn = FALSE;
			}
		}
    };
}

//------------------------------------------

void CPlatformManager::SignOut()
{
	m_bIsSignedIn = FALSE;
}

//------------------------------------------

void CPlatformManager::UnlockAchievement(const Str& achievement_id)
{
	if (m_bIsSignedIn)
	{
		NSString* identifier = [NSString stringWithUTF8String:achievement_id.GetArray()];
		GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier: identifier];
		if (achievement )
		{
			achievement.percentComplete = 100;
			achievement.showsCompletionBanner = NO;
			[achievement reportAchievementWithCompletionHandler:^(NSError *error)
			{
				if (error != nil)
				{
				  NSLog(@"Error in reporting achievements: %@", error);
				}
			}];
		}
	}
}

//------------------------------------------

void CPlatformManager::ShowAchievements()
{
	if (m_bIsSignedIn)
	{
		GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
		if (gameCenterController != nil)
		{
			gameCenterController.gameCenterDelegate = m_ViewController;
			gameCenterController.viewState = GKGameCenterViewControllerStateAchievements;
			[m_ViewController presentViewController: gameCenterController animated: YES completion:nil];
		}
	}
}

//------------------------------------------

void CPlatformManager::ShowLeaderboards()
{
	if (m_bIsSignedIn)
	{
		GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
		if (gameCenterController != nil)
		{
			gameCenterController.gameCenterDelegate = m_ViewController;
			gameCenterController.viewState = GKGameCenterViewControllerStateLeaderboards;
			[m_ViewController presentViewController: gameCenterController animated: YES completion:nil];
		}
	}
}

//------------------------------------------

void CPlatformManager::ResetAchievements()
{
	if (m_bIsSignedIn)
	{
		[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError *error) 
		 {
			/*if (!error) {
				 //[storedAchievements release];
				//storedAchievements = [[NSMutableDictionary alloc] init];

				// overwrite any previously stored file
				 //[self writeStoredAchievements];              
			} else {
				// Error clearing achievements. 
			 }*/
		 }];
	}
}

//------------------------------------------

void CPlatformManager::SubmitHighScore(const Str& leaderboard_id, S64 score)
{
	if (m_bIsSignedIn)
	{
		GKScore *scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier: [NSString stringWithUTF8String:leaderboard_id.GetArray()]];
		scoreReporter.value = score;
		scoreReporter.context = 0;
	 
		[scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
            //todo
        }];
		
		/*NSArray *scores = @[scoreReporter];
		[GKLeaderboard reportScores:scores withCompletionHandler:^(NSError *error) {
		//Do something interesting here.
		}];*/
	}
}

//------------------------------------------

void CPlatformManager::LaunchPurchase()
{
	m_bPurchaseInProgress = TRUE;
	OnStartPurchase();
    [m_ViewController LaunchPurchase];
}

