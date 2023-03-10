//
//  AppDelegate.m
//  TestOpenglGame
//
//  Created by Raptor on 06/05/14.
//  Copyright (c) 2014 Raptor. All rights reserved.
//

#import "AppDelegate.h"
#include "game.h"
#import "platform_manager.h"
#import "ViewController.h"
#import "View.h"

@implementation AppDelegate

@synthesize query = _query;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Load save from cloud
	/*NSMetadataQuery* query = [[NSMetadataQuery alloc] init];
    _query = query;
    [query setSearchScopes:[NSArray arrayWithObject:NSMetadataQueryUbiquitousDocumentsScope]];
	NSPredicate *pred = [NSPredicate predicateWithFormat:@"%K == %@", NSMetadataItemFSNameKey, @"game.sav"];
    [query setPredicate:pred];
	[[NSNotificationCenter defaultCenter]
     addObserver:self
     selector:@selector(queryDidFinishGathering:)
     name:NSMetadataQueryDidFinishGatheringNotification
     object:query];
	[query startQuery];
    
    if( [query isStarted])
        CPlatformManager::GetInstance().SetLoadSaveFromCloudInProgress(true);*/
		    
	return YES;
}


- (void)queryDidFinishGathering:(NSNotification *)notification {
    
    NSMetadataQuery *query = [notification object];
    [query disableUpdates];
    [query stopQuery];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:NSMetadataQueryDidFinishGatheringNotification
                                                  object:query];
    
    _query = nil;
    
	if( CGame::GetInstance().GetFlowScriptManager().IsValid() && CGame::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams())
	{
		CGame::GetInstance().GetFlowScriptManager()->GetCurrentScriptParams()->LoadFromFile();
	}
    
	CPlatformManager::GetInstance().SetLoadSaveFromCloudInProgress(false);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	CGame::GetInstance().Pause();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    CGame::GetInstance().Resume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	CGame::GetInstance().Release();
}

@end
