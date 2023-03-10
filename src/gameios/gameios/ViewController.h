//
//  ViewController.h
//  TestOpenglGame
//
//  Created by Raptor on 06/05/14.
//  Copyright (c) 2014 Raptor. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <StoreKit/StoreKit.h>
#import <GameKit/GameKit.h>
#import "../game/game.h"

@interface ViewController : UIViewController <GKGameCenterControllerDelegate,SKProductsRequestDelegate,SKPaymentTransactionObserver>

// UPDATE
@property (nonatomic, readonly) NSTimeInterval timeSinceLastDraw;
@property (nonatomic) NSUInteger interval;
@property (nonatomic, getter=isPaused) BOOL paused;
@property (nonatomic, readonly, getter=isRunning) BOOL running;

// IAP
@property (nonatomic, strong) SKProduct *mainProduct;
@property (strong, nonatomic) SKProductsRequest *request;

// UPDATE
- (void)dispatchGameLoop;
- (void)stopGameLoop;

// IAP
-(void)LaunchPurchase;

@end
