//
//  UserInput.h
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface UserInput : NSWindowController

@property NSString *title;

@property IBOutlet NSTextField *lblInfo;
@property IBOutlet NSTextField *lblError;

@property bool shouldClose;
@property bool submitted;

- (void)setInfo:(NSString*)text;
- (void)setError:(NSString*)text;
- (void)addField:(int)type withText:(NSString*)text withValue:(const void*)value;
- (void)setFieldValue:(int)i_field withValue:(const void*)valueIn;
- (NSString*)fieldStringValue:(int)index;
- (bool)fieldBoolValue:(int)index;
- (int)fieldUintValue:(int)index;

@end
