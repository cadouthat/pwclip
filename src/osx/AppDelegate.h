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

- (void)updateVaultList;

- (IBAction)openUserInput:(id)sender;
- (IBAction)closeUserInput:(id)sender;
- (IBAction)displayError:(id)sender;

@end