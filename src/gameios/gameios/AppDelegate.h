//
//  AppDelegate.h
//  TestOpenglGame
//
//  Created by Raptor on 06/05/14.
//  Copyright (c) 2014 Raptor. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate, SKPaymentTransactionObserver>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) NSMetadataQuery *query;

@end
