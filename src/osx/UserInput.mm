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
    //
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
    if(_flags & UIF_NAME) {
        [_editName setEnabled:true];
        [_editName setHidden:false];
    }
    if(_flags & UIF_OLDPASS) {
        [_editPassword setEnabled:true];
        [_editPassword setHidden:false];
    }
    if(_flags & UIF_NEWPASS) {
        [_editPassword setEnabled:true];
        [_editPassword setHidden:false];
        [_editConfirm setEnabled:true];
        [_editConfirm setHidden:false];
        [_editPassword setPlaceholderString:@"New Master Password"];
    }
    
    [self updateLayout];
}

- (void)windowWillClose:(NSNotification *)notification {
    _shouldClose = true;
}

@end
