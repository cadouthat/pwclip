//
//  UserInput.m
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "UserInput.h"
#import "../shared/UserInput.h"

@interface UserInput ()

@property IBOutlet NSButton *btnCancel;
@property IBOutlet NSButton *btnOkay;

@end

@implementation UserInput

- (IBAction)okayPressed:(id)sender {
    _shouldClose = true;
    _submitted = true;
}

- (IBAction)cancelPressed:(id)sender {
    _shouldClose = true;
}

- (void)updateLayout {
    NSRect frame;
    NSRect windowFrame = [[self window] frame];
    NSInteger pad = 40;
    NSInteger sep = 10;
    NSInteger y = pad;
    if(![_lblInfo isHidden]) {
        frame = [_lblInfo frame];
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        [_lblInfo setFrame:frame];
        y += frame.size.height + sep;
    }
    if(![_lblError isHidden]) {
        frame = [_lblError frame];
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        [_lblError setFrame:frame];
        y += frame.size.height + sep;
    }
    if(![_editName isHidden]) {
        frame = [_editName frame];
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        [_editName setFrame:frame];
        y += frame.size.height + sep;
    }
    if(![_editPassword isHidden]) {
        frame = [_editPassword frame];
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        [_editPassword setFrame:frame];
        y += frame.size.height + sep;
    }
    if(![_editConfirm isHidden]) {
        frame = [_editConfirm frame];
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        [_editConfirm setFrame:frame];
        y += frame.size.height + sep;
    }
    y += sep;
    frame = [_btnCancel frame];
    frame.origin.y = windowFrame.size.height - y - frame.size.height;
    [_btnCancel setFrame:frame];
    frame = [_btnOkay frame];
    frame.origin.y = windowFrame.size.height - y - frame.size.height;
    [_btnOkay setFrame:frame];
    y += pad;
    windowFrame.size.height = y;
    [[self window] setFrame:windowFrame display:YES];
}

- (void)setInfo:(NSString*)text {
    [_lblInfo setStringValue:text];
    [_lblInfo setHidden:false];
    [self updateLayout];
}

- (void)setError:(NSString*)text {
    [_lblError setStringValue:text];
    [_lblError setHidden:false];
    [self updateLayout];
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    [[self window] setTitle:[self title]];
    [[self btnOkay] setAction:@selector(okayPressed:)];
    [[self btnCancel] setAction:@selector(cancelPressed:)];
    
    //Update visibility and layout
    bool hasFocus = false;
    if(_flags & UIF_NAME) {
        [_editName setEnabled:true];
        [_editName setHidden:false];
        if(!hasFocus) {
            [_editName becomeFirstResponder];
            hasFocus = true;
        }
    }
    if(_flags & UIF_OLDPASS) {
        [_editPassword setEnabled:true];
        [_editPassword setHidden:false];
        if(!hasFocus) {
            [_editPassword becomeFirstResponder];
            hasFocus = true;
        }
    }
    if(_flags & UIF_NEWPASS) {
        [_editPassword setEnabled:true];
        [_editPassword setHidden:false];
        [_editConfirm setEnabled:true];
        [_editConfirm setHidden:false];
        [_editPassword setPlaceholderString:@"New Master Password"];
        if(!hasFocus) {
            [_editPassword becomeFirstResponder];
            hasFocus = true;
        }
    }
    
    [self updateLayout];
}

- (void)windowWillClose:(NSNotification *)notification {
    _shouldClose = true;
}

@end
