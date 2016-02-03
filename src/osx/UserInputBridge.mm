//
//  UserInput.mm
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import "UserInput.h"
#import "globals.h"

void *UserInput_new(int flags_in, const char *title_in) {
    UserInput *dlg = [[UserInput alloc] initWithWindowNibName:@"UserInput"];
    [dlg setFlags:flags_in];
    [dlg setTitle:[NSString stringWithUTF8String:title_in]];
    [mainApp performSelectorOnMainThread:@selector(openUserInput:) withObject:dlg waitUntilDone:false];
    return (__bridge void*)mainApp;
}
void UserInput_delete(void *ui) {
    [mainApp performSelectorOnMainThread:@selector(closeUserInput:) withObject:NULL waitUntilDone:false];
}
const char *UserInput_name(void *ui){
    return [[[[mainApp activeDialog] editName] stringValue] UTF8String];
}
const char *UserInput_pass(void *ui) {
    return [[[[mainApp activeDialog] editPassword] stringValue] UTF8String];
}
void UserInput_setInfo(void *ui, const char *text) {
    [[[mainApp activeDialog] lblInfo] setStringValue:[NSString stringWithUTF8String:text]];
}
void UserInput_setError(void *ui, const char *text) {
    [[[mainApp activeDialog] lblError] setStringValue:[NSString stringWithUTF8String:text]];
}
bool UserInput_get(void *ui) {
    while(![[mainApp activeDialog] shouldClose]) {
        [NSThread sleepForTimeInterval:0.2f];
    }
    bool result = [[mainApp activeDialog] submitted];
    [[mainApp activeDialog] setShouldClose:false];
    [[mainApp activeDialog] setSubmitted:false];
    return result;;
}
