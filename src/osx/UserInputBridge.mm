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

void *UserInput_new(const char *title_in) {
    UserInput *dlg = [[UserInput alloc] initWithWindowNibName:@"UserInput"];
    [dlg setTitle:[NSString stringWithUTF8String:title_in]];
    [mainApp performSelectorOnMainThread:@selector(openUserInput:) withObject:dlg waitUntilDone:true];
    return (__bridge void*)mainApp;
}
void UserInput_delete(void *ui) {
    [mainApp performSelectorOnMainThread:@selector(closeUserInput:) withObject:NULL waitUntilDone:false];
}
void UserInput_setInfo(void *ui, const char *text) {
    [mainApp performSelectorOnMainThread:@selector(setUserInputInfo:) withObject:[NSString stringWithUTF8String:text] waitUntilDone:true];
}
void UserInput_setError(void *ui, const char *text) {
    [mainApp performSelectorOnMainThread:@selector(setUserInputError:) withObject:[NSString stringWithUTF8String:text] waitUntilDone:true];
}
bool UserInput_get(void *ui) {
    while(![[mainApp activeDialog] shouldClose]) {
        [NSThread sleepForTimeInterval:0.2f];
    }
    bool result = [[mainApp activeDialog] submitted];
    [[mainApp activeDialog] setShouldClose:false];
    [[mainApp activeDialog] setSubmitted:false];
    return result;
}
void UserInput_addField(void *ui, int type, const char *text, const void *valueIn)
{
    [mainApp performSelectorOnMainThread:@selector(addUserInputField:) withObject:[NSArray arrayWithObjects:[NSNumber numberWithInt:type], [NSString stringWithUTF8String:text], [NSValue valueWithPointer:valueIn], nil] waitUntilDone:true];
}
void UserInput_setValue(void *ui, int i_field, const void *valueIn)
{
    [mainApp performSelectorOnMainThread:@selector(setUserInputValue:) withObject:[NSArray arrayWithObjects:[NSNumber numberWithInt:i_field], [NSValue valueWithPointer:valueIn], nil] waitUntilDone:true];
}
char *UserInput_stringValue(void *ui, int index)
{
    [mainApp performSelectorOnMainThread:@selector(getUserInputStringValue:) withObject:[NSNumber numberWithInt:index] waitUntilDone:true];
    return strdup([[mainApp userInputString] UTF8String]);
}
bool UserInput_boolValue(void *ui, int index)
{
    [mainApp performSelectorOnMainThread:@selector(getUserInputBoolValue:) withObject:[NSNumber numberWithInt:index] waitUntilDone:true];
    return [mainApp userInputBool];
}
int UserInput_uintValue(void *ui, int index)
{
    [mainApp performSelectorOnMainThread:@selector(getUserInputUintValue:) withObject:[NSNumber numberWithInt:index] waitUntilDone:true];
    return [mainApp userInputUint];
}
