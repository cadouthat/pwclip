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

@interface AppDelegate : NSObject <NSApplicationDelegate, NSMenuDelegate>

@property NSTimer *clipWipeTimer;

@property IBOutlet NSView *balloonView;
@property IBOutlet NSTextField *balloonText;
@property MAAttachedWindow *trayWindow;

@property UserInput *activeDialog;
@property NSString *userInputString;
@property bool userInputBool;
@property int userInputUint;

@property bool saveAsActive;
@property NSString *saveAsResult;
@property bool openFileActive;
@property NSString *openFileResult;

@property bool confirmResult;

- (bool)showActiveDialog;

- (void)updateMenu;

- (IBAction)menuMainDispatch:(id)sender;

- (IBAction)openUserInput:(id)sender;
- (IBAction)addUserInputField:(id)sender;
- (IBAction)setUserInputInfo:(id)sender;
- (IBAction)setUserInputError:(id)sender;
- (IBAction)setUserInputValue:(id)sender;
- (IBAction)getUserInputStringValue:(id)sender;
- (IBAction)getUserInputBoolValue:(id)sender;
- (IBAction)getUserInputUintValue:(id)sender;
- (IBAction)closeUserInput:(id)sender;

- (IBAction)scheduleClipWipe:(id)sender;
- (IBAction)displayError:(id)sender;
- (IBAction)displayConfirm:(id)sender;
- (IBAction)displaySaveAs:(id)sender;
- (IBAction)displayOpenFile:(id)sender;
- (IBAction)displayBalloon:(id)sender;
- (IBAction)closeBalloon:(id)sender;

@end
