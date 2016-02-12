//
//  AppDelegate.h
//  pwclip
//
//  Created by Connor Douthat on 11/8/15.
//  Copyright © 2015 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAAttachedWindow.h"
#import "UserInput.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property NSTimer *clipWipeTimer;

@property IBOutlet NSView *balloonView;
@property IBOutlet NSTextField *balloonText;

@property UserInput *activeDialog;
@property MAAttachedWindow *trayWindow;

@property bool saveAsCompleted;
@property NSString *saveAsResult;
@property bool confirmResult;

- (void)updateMenu;

- (IBAction)scheduleClipWipe:(id)sender;
- (IBAction)openUserInput:(id)sender;
- (IBAction)closeUserInput:(id)sender;
- (IBAction)displayError:(id)sender;
- (IBAction)displayConfirm:(id)sender;
- (IBAction)displaySaveAs:(id)sender;
- (IBAction)displayBalloon:(id)sender;
- (IBAction)closeBalloon:(id)sender;

@end
