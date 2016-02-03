//
//  UserInput.m
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "UserInput.h"

@interface UserInput ()

@property IBOutlet NSTextField *editConfirm;
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

- (void)windowDidLoad {
    [super windowDidLoad];
    
    [[self window] setTitle:[self title]];
    [[self btnOkay] setAction:@selector(okayPressed:)];
    [[self btnCancel] setAction:@selector(cancelPressed:)];
    
    //Update visibility and layout
}

- (void)windowWillClose:(NSNotification *)notification {
    _shouldClose = true;
}

@end
