//
//  UserInput.m
//  pwclip
//
//  Created by Connor Douthat on 2/1/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "UserInput.h"
#import "../shared/gui/GenericUserInput.h"

@interface UserInput ()

@property IBOutlet NSButton *btnCancel;
@property IBOutlet NSButton *btnOkay;
@property NSMutableArray *fieldTypes;
@property NSMutableArray *fieldObjects;

@end

@implementation UserInput

- (IBAction)okayPressed:(id)sender {
    
    id errorField = nil;
    const char *error = NULL;
    
    for(int i = 0; i < [_fieldObjects count]; i++) {
        id field = [_fieldObjects objectAtIndex:i];
        id input = [field objectAtIndex:0];
        int type = [[_fieldTypes objectAtIndex:i] intValue];
        switch(type)
        {
            case UIF_NEWPASS:
            {
                NSString *val1 = [input stringValue];
                NSString *val2 = [[field objectAtIndex:1] stringValue];
                if(![val1 length]) error = "Please enter a value";
                else if(![val1 isEqualToString:val2]) error = "Passwords do not match";
                break;
            }
            case UIF_UINT:
                NSString *val = [input stringValue];
                for(int i = 0; i < [val length]; i++)
                {
                    char c = [val characterAtIndex:i];
                    if(c < '0' || c > '9')
                    {
                        error = "Invalid number";
                        break;
                    }
                }
                break;
        }
        if(error) {
            errorField = input;
            break;
        }
    }

    if(error) {
        [self setError:[NSString stringWithUTF8String:error]];
        [[self window] makeFirstResponder:errorField];
    }
    else {
        _shouldClose = true;
        _submitted = true;
    }
}

- (IBAction)cancelPressed:(id)sender {
    _shouldClose = true;
}

- (IBAction)buttonProxy:(id)sender {
    [self setError:@""];
    NSArray *arr = [sender representedObject];
    NSValue *v = [arr objectAtIndex:0];
    ((void(*)(void*,int))[v pointerValue])((__bridge void*)self, [[arr objectAtIndex:1] intValue]);
}

- (void)updateLayout {
    NSRect frame;
    NSRect windowFrame = [[self window] frame];
    NSInteger xpad = 20;
    NSInteger ypad = 40;
    NSInteger sep = 10;
    NSInteger y = ypad;
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
    for(int i = 0; i < [_fieldTypes count]; i++) {
        id field = [_fieldObjects objectAtIndex:i];
        int type = [[_fieldTypes objectAtIndex:i] intValue];
        frame.size.width = windowFrame.size.width - xpad * 2;
        frame.size.height = 22;
        frame.origin.x = xpad;
        frame.origin.y = windowFrame.size.height - y - frame.size.height;
        switch(type) {
            case UIF_NEWPASS:
                frame.origin.y = windowFrame.size.height - y - frame.size.height * 2 - sep;
                [[field objectAtIndex:1] setFrame:frame];
                frame.origin.y = windowFrame.size.height - y - frame.size.height;
                y += frame.size.height + sep;
                break;
            case UIF_UINT:
                int w;
                w = frame.size.width;
                frame.size.width = w - 40;
                [[field objectAtIndex:1] setFrame:frame];
                frame.size.width = 40;
                frame.origin.x += w - 40;
                break;
            case UIF_BUTTON:
                frame.size.width /= 2;
                frame.origin.x -= 6;
                break;
        }
        [[field objectAtIndex:0] setFrame:frame];
        y += frame.size.height + sep;
    }
    y += sep;
    frame = [_btnOkay frame];
    frame.origin.y = windowFrame.size.height - y - frame.size.height;
    [_btnOkay setFrame:frame];
    frame = [_btnCancel frame];
    frame.origin.y = windowFrame.size.height - y - frame.size.height;
    [_btnCancel setFrame:frame];
    y += ypad;
    windowFrame.size.height = y;
    [[self window] setFrame:windowFrame display:YES];
}

- (void)setInfo:(NSString*)text {
    [_lblInfo setStringValue:text];
    [_lblInfo setHidden:[text length] == 0];
    [self updateLayout];
}

- (void)setError:(NSString*)text {
    [_lblError setStringValue:text];
    [_lblError setHidden:[text length] == 0];
    [self updateLayout];
}

- (void)windowDidLoad {
    [super windowDidLoad];

    _shouldClose = false;
    _fieldTypes = [[NSMutableArray alloc] init];
    _fieldObjects = [[NSMutableArray alloc] init];
    
    [[self window] setTitle:[self title]];
    [[self btnOkay] setAction:@selector(okayPressed:)];
    [[self btnCancel] setAction:@selector(cancelPressed:)];
}

- (void)windowWillClose:(NSNotification *)notification {
    _shouldClose = true;
}

- (void)addField:(int)type withText:(NSString*)text withValue:(const void*)value {
    [_fieldTypes addObject:[NSNumber numberWithInt:type]];
    id field = [[NSMutableArray alloc] init];
    id widget;
    switch(type) {
        case UIF_NEWPASS:
        case UIF_OLDPASS:
            widget = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [widget setPlaceholderString:text];
            [field addObject:widget];
            if(type == UIF_NEWPASS) {
                widget = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                [widget setPlaceholderString:[@"Confirm " stringByAppendingString:text]];
                [field addObject:widget];
            }
            break;
        case UIF_TEXT:
            widget = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [widget setPlaceholderString:text];
            [field addObject:widget];
            break;
        case UIF_UINT:
            widget = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [field addObject:widget];
            widget = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [widget setStringValue:text];
            [widget setBezeled:NO];
            [widget setDrawsBackground:NO];
            [widget setEditable:NO];
            [widget setSelectable:NO];
            [field addObject:widget];
            break;
        case UIF_TOGGLE:
        case UIF_BUTTON:
            widget = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [widget setTitle:text];
            if(type == UIF_TOGGLE) {
                [widget setButtonType:NSSwitchButton];
            }
            else {
                [widget setButtonType:NSMomentaryPushInButton];
                [widget setBezelStyle:NSRoundedBezelStyle];
            }
            [field addObject:widget];
            break;
    }
    [_fieldObjects addObject:field];
    [(NSView*)[field objectAtIndex:0] setAutoresizingMask:NSViewMinXMargin | NSViewMinYMargin];
    [[[self window] contentView] addSubview:[field objectAtIndex:0]];
    if([field count] > 1) {
        [(NSView*)[field objectAtIndex:1] setAutoresizingMask:NSViewMinXMargin | NSViewMinYMargin];
        [[[self window] contentView] addSubview:[field objectAtIndex:1]];
    }
    if(value) {
        [self setFieldValue:((int)[_fieldObjects count] - 1) withValue:value];
    }

    [self updateLayout];

    if([_fieldObjects count] == 1) {
        [[self window] setInitialFirstResponder:[field objectAtIndex:0]];
        [[self window] makeFirstResponder:[field objectAtIndex:0]];
    }
}
- (void)setFieldValue:(int)i_field withValue:(const void*)valueIn {
    int type = [[_fieldTypes objectAtIndex:i_field] intValue];
    id field = [_fieldObjects objectAtIndex:i_field];
    NSTextField *text;
    switch(type) {
        case UIF_NEWPASS:
            [[field objectAtIndex:1] setStringValue:[NSString stringWithUTF8String:(const char*)valueIn]];
        case UIF_OLDPASS:
        case UIF_TEXT:
            text = [field objectAtIndex:0];
            [text setStringValue:[NSString stringWithUTF8String:(const char*)valueIn]];
            [[text currentEditor] setSelectedRange:NSMakeRange([[text stringValue] length], 0)];
            break;
        case UIF_UINT:
            [[field objectAtIndex:0] setIntValue:*(int*)valueIn];
            break;
        case UIF_TOGGLE:
            [[field objectAtIndex:0] setState:(*(bool*)valueIn ? NSOnState : NSOffState)];
            break;
        case UIF_BUTTON:
            [[field objectAtIndex:0] setTarget:self];
            [[field objectAtIndex:0] setAction:@selector(buttonProxy:)];
            [[field objectAtIndex:0] setRepresentedObject:[NSArray arrayWithObjects:[NSValue valueWithPointer:valueIn], [NSNumber numberWithInt:i_field], nil]];
            break;
    }
}
- (NSString*)fieldStringValue:(int)index {
    return [[[_fieldObjects objectAtIndex:index] objectAtIndex:0] stringValue];
}
- (bool)fieldBoolValue:(int)index {
    return ([[[_fieldObjects objectAtIndex:index] objectAtIndex:0] state] == NSOnState);
}
- (int)fieldUintValue:(int)index {
    return [[[_fieldObjects objectAtIndex:index] objectAtIndex:0] intValue];
}

@end
