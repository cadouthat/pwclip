//
//  AppDelegate.h
//  pwclip
//
//  Created by Connor Douthat on 11/8/15.
//  Copyright © 2015 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "UserInput.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property UserInput *activeDialog;

@property bool confirmResult;

- (void)updateMenu;

- (IBAction)openUserInput:(id)sender;
- (IBAction)closeUserInput:(id)sender;
- (IBAction)displayError:(id)sender;
- (IBAction)displayConfirm:(id)sender;

@end
