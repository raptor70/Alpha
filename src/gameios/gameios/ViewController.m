//
//  ViewController.m
//  TestOpenglGame
//
//  Created by Raptor on 06/05/14.
//  Copyright (c) 2014 Raptor. All rights reserved.
//

#import "ViewController.h"
#import "View.h"
#import "input_manager.h"
#import "platform_manager.h"
#import "renderer.h"

@interface ViewController () {
    CGame _game;
@private
    CADisplayLink* _timer;
    BOOL _lastDrawOccurred;
    CFTimeInterval _timeSinceLastDrawPreviousTime;
    BOOL _gameLoopPaused;
}

- (void)setupGL;
- (void)tearDownGL;
@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    
    CPlatformManager::GetInstance().SetViewController(self);

    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    View *view = (View *)self.view;
	[view setNOGLContext];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
		
		View *view = (View *)self.view;
		[view setNOGLContext];
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    View *view = (View *)self.view;
    view = [view initWithFrame:self.view.bounds];
	[view setGLContext];
	CDriverMetal::g_pView = view;
	CRenderer::m_bUseMetal = view.useMetal;
	_game.Init(0,0);

    CGSize drawableSize = view.bounds.size;
    drawableSize.width  *= view.contentScaleFactor;
    drawableSize.height *= view.contentScaleFactor;
    _game.SetWindowParams(drawableSize.width,drawableSize.height,"Sob",TRUE);
}

- (void)tearDownGL
{
    View *view = (View *)self.view;
	[view setGLContext];
	
    _game.Release();
}

- (void) dispatchGameLoop
{
    // create a game loop timer using a dispatch source and fire it on the game loop queue
    _timer = [CADisplayLink displayLinkWithTarget:self
                                         selector:@selector(gameloop)];
    
    _timer.frameInterval = _interval;
    
    [_timer addToRunLoop:[NSRunLoop mainRunLoop]
                 forMode:NSDefaultRunLoopMode];
    
    _running = YES;
    return;
}

// the main game loop called by the timer above
- (void) gameloop
{
    if(!_lastDrawOccurred)
    {
        // set up timing data for display since this is the first time through this loop
        _timeSinceLastDraw             = 0.0;
        _timeSinceLastDrawPreviousTime = CACurrentMediaTime();
        _lastDrawOccurred              = YES;
    }
    else
    {
        // figure out the time since we last we drew
        CFTimeInterval currentTime = CACurrentMediaTime();
        
        _timeSinceLastDraw = currentTime - _timeSinceLastDrawPreviousTime;
        
        // keep track of the time interval between draws
        _timeSinceLastDrawPreviousTime = currentTime;
    }
    
	View* view = (View *)self.view;
	[view beginrender];
	
    Float fTime = _timeSinceLastDraw;
	if(fTime>0.1f)
		fTime=0.1f;
    _game.DoFrame(fTime);
	
	[view endrender];
}

- (void) stopGameLoop
{
    // must not be suspended before cancelling
    if(_timer)
    {
        _running = NO;
        
        [_timer invalidate];
    }
}

- (void) setPaused:(BOOL)pause
{
    // calls to dispatch_resume dispatch_suspend must be balanced, so if this is same value just return
    if(_gameLoopPaused == pause)
    {
        return;
    }
    
    if(_timer)
    {
        // inform the delegate we are about to pause
        /*[_delegate renderViewController:self
                              willPause:pause];*/
        
        if(pause == YES)
        {
            _gameLoopPaused = pause;
            _timer.paused   = YES;
            
            // ask the view to release textures until its resumed
            //[(AAPLView *)self.view releaseTextures];
        }
        else
        {
            _gameLoopPaused = pause;
            _timer.paused   = NO;
        }
    }
}

- (BOOL)isPaused
{
    return _gameLoopPaused;
}
 
- (void)didEnterBackground:(NSNotification*)notification
{
    [self setPaused:YES];
}
 
- (void)willEnterForeground:(NSNotification*)notification
{
    [self setPaused:NO];
}
 
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    // run the game loop
    [self dispatchGameLoop];
}
 
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    // end the gameloop
    [self stopGameLoop];
}


class TouchId
{
public:
	TouchId() { m_ptr = NULL; }
	UITouch* m_ptr;
};

const int max_touch = 11;
TouchId g_touchids[max_touch];

int GetTouchId(UITouch* _touch)
{
	for(int i=0; i<max_touch;i++)
	{
		if( g_touchids[i].m_ptr == _touch )
			return i;
	}

	return -1;
}

int AddTouchId(UITouch* _touch)
{
	for(int i=0; i<max_touch;i++)
	{
		if( g_touchids[i].m_ptr == NULL )
		{
			g_touchids[i].m_ptr = _touch;
			return i;
		}
	}

	return -1;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	LOGGER_Log("touchesBegan\n");
	for (UITouch *touch in touches) 
	{
		int id = AddTouchId(touch);
		if( id >= 0 )
		{
			CGPoint touchLocation = [touch locationInView:touch.view];
            touchLocation.x *= self.view.contentScaleFactor;
            touchLocation.y *= self.view.contentScaleFactor;
			CInputManager::GetInstance().GetMouse().UpdateValue(id,true,Vec2i(touchLocation.x,touchLocation.y));
		}
	}
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	LOGGER_Log("touchesMoved\n");	
	for (UITouch *touch in touches) 
	{
		int id = GetTouchId(touch);
		if( id < 0 )
			id = AddTouchId(touch);

		if( id >= 0 )
		{
			CGPoint touchLocation = [touch locationInView:touch.view];
            touchLocation.x *= self.view.contentScaleFactor;
            touchLocation.y *= self.view.contentScaleFactor;
			CInputManager::GetInstance().GetMouse().UpdateValue(id,true,Vec2i(touchLocation.x,touchLocation.y));
		}
	}
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	LOGGER_Log("touchesEnded\n");	
	for (UITouch *touch in touches) 
	{
		int id = GetTouchId(touch);
		if( id >= 0 )
		{
			g_touchids[id].m_ptr = NULL;
			CInputManager::GetInstance().GetMouse().UpdateValue(id,false,Vec2i(-1,-1));
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent*)event 
{ 
	LOGGER_Log("touchesCancelled\n");	
	for (UITouch *touch in touches) 
	{
		int id = GetTouchId(touch);
		if( id >= 0 )
		{
			g_touchids[id].m_ptr = NULL;
			CInputManager::GetInstance().GetMouse().UpdateValue(0,false,Vec2i(-1,-1));
		}
	}
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(void)LaunchPurchase {
    NSArray * currentProducts = @[@"purchase_unlock"];
    self.request = [[SKProductsRequest alloc]initWithProductIdentifiers:[NSSet setWithArray:currentProducts]];
    self.request.delegate = self;
    [self.request start];
}

-(void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    NSLog(@"error%@",error);
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    // invalid products
    for (NSString *invalidIIdentifier in response.invalidProductIdentifiers) {
        NSLog(@"The following ID is invalid: %@", invalidIIdentifier);
    }
    
    // grab a reference for later
    self.mainProduct = [response.products objectAtIndex:0];
    
    // display UI - if user can make payments
    if ([SKPaymentQueue canMakePayments]) {
        // display store UI
        [self displayStoreUIwithProduct:[response.products objectAtIndex:0]];
    } else {
        // display can't buy UI
        [self cantBuyAnything];
    }
}

//------------------------------------------

- (void)displayStoreUIwithProduct:(SKProduct *)product {
    
    // display local currency
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc]init];
    [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [formatter setLocale:product.priceLocale];
    NSString *price = [NSString stringWithFormat:@"Buy for %@", [formatter stringFromNumber:product.price]];
    
    // a UIAlertView brings up the purchase option
    NSString* title = @"Unlock the next level";//product.localizedTitle;
    NSString* desc = @"Do you want to unlock the next level ?";//product.localizedDescription;
    UIAlertView *storeUI = [[UIAlertView alloc]initWithTitle:title message:desc delegate:self cancelButtonTitle:@"Perhaps Later" otherButtonTitles:price, nil];
    [storeUI show];
    
}

//------------------------------------------

- (void)cantBuyAnything {
    
    // tell user that payments are switched off
    UIAlertView *noStoreAlert = [[UIAlertView alloc]initWithTitle:nil message:@"Your device does not allow in-app purchases at this time." delegate:self cancelButtonTitle:@"I'll check my Settings" otherButtonTitles:nil, nil];
    [noStoreAlert show];
}

//------------------------------------------

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
if(buttonIndex == 0)
	// cancel
	CPlatformManager::GetInstance().OnEndPurchase(FALSE);
else
	// Some code
	[self makeThePurchase];
}

//------------------------------------------

- (BOOL)makeThePurchase {
    
    // take payment for our product
    SKPayment *payment = [SKPayment paymentWithProduct:self.mainProduct];
    [[SKPaymentQueue defaultQueue]addPayment:payment];
    
    return YES;
}

//------------------------------------------

#pragma mark - StoreKit Observer

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions {
    
    for (SKPaymentTransaction *transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased: {
                
				NSLog(@"InappDebug : Purchased %@",transaction);
                // user has purchased
                [self saveTransactionReceipt:transaction];
                CPlatformManager::GetInstance().OnEndPurchase(TRUE);
                
                // let App Store know we're done
                [[SKPaymentQueue defaultQueue]finishTransaction:transaction];
                break;
            }
                
            case SKPaymentTransactionStateFailed: {
				NSLog(@"InappDebug : Failed %@ (%d)", [transaction.error description], [transaction.error code]);
                CPlatformManager::GetInstance().OnEndPurchase(FALSE);
                break;
            }
                
            case SKPaymentTransactionStateRestored: {
				NSLog(@"InappDebug : Restored %@",transaction);
                // purchase has been restored
                break;
            }
                
                
            case SKPaymentTransactionStatePurchasing: {
				NSLog(@"InappDebug : Purchasing %@",transaction);
                // currently purchasing
                break;
            }
                
            default:
                break;
        }
    }
}

//------------------------------------------

- (void)saveTransactionReceipt:(SKPaymentTransaction *)transaction {
    NSLog(@"InappDebug : saveTransactionReceipt %@",transaction);
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *receiptID = transaction.transactionIdentifier;
    NSArray *storedReceipts = [defaults arrayForKey:@"receipts"];
    
    if (!storedReceipts) {
        // store the first receipt
        [defaults setObject:@[receiptID] forKey:@"receipts"];
    } else {
        // add a receipt to the array
        NSArray *updatedReceipts = [storedReceipts arrayByAddingObject:receiptID];
        [defaults setObject:updatedReceipts forKey:@"receipts"];
    }
    
    [defaults synchronize];
}

@end
