//
//  UserInput.h
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface UserInput : NSWindowController

@property int flags;
@property NSString *title;

@property IBOutlet NSTextField *lblInfo;
@property IBOutlet NSTextField *lblError;
@property IBOutlet NSTextField *editName;
@property IBOutlet NSTextField *editPassword;
@property IBOutlet NSTextField *editConfirm;

@property bool shouldClose;
@property bool submitted;

- (void)setInfo:(NSString*)text;
- (void)setError:(NSString*)text;

@end
