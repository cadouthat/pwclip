//
//  AppDelegate.mm
//  pwclip
//
//  Created by Connor Douthat on 11/8/15.
//  Copyright © 2015 Connor Douthat. All rights reserved.
//

#import "AppDelegate.h"
#import "includes.h"
#import "UserInput.h"

@interface AppDelegate ()

@property NSTimer *balloonTimer;

@property (weak) IBOutlet NSWindow *window;
@property NSStatusItem *statusItem;
@property NSMenu *mainMenu;

@end

@implementation AppDelegate

- (void)setNormalTray {
    [_statusItem setImage: [NSImage imageNamed:@"MenuIcon"]];
}

- (IBAction)scheduleClipWipe:(id)sender {
    if(_clipWipeTimer) {
        [_clipWipeTimer invalidate];
    }

    if(!clip_wipe_delay) {
        return;
    }
    _clipWipeTimer = [NSTimer scheduledTimerWithTimeInterval:clip_wipe_delay target:self selector:@selector(wipeClipboard:) userInfo:self repeats:false];
    [_statusItem setImage: [NSImage imageNamed:@"ReadyIcon"]];
}

- (IBAction)wipeClipboard:(id)sender {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    if([pasteboard changeCount] == clipReference) {
        SetClipboardText("");
    }
    _clipWipeTimer = nil;
    [self setNormalTray];
}

- (IBAction)cancelClipWipe:(id)sender {
    if(_clipWipeTimer) {
        [_clipWipeTimer invalidate];
        _clipWipeTimer = nil;
    }
    [self setNormalTray];
}

- (IBAction)displayError:(id)sender {
    [NSApp activateIgnoringOtherApps:YES];

    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:sender];
    [alert setIcon:[NSImage imageNamed:@"ErrorIcon"]];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert runModal];
}

- (IBAction)displayConfirm:(id)sender {
    NSArray *params = sender;
    
    [NSApp activateIgnoringOtherApps:YES];
    
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Okay"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:[params objectAtIndex:0]];
    [alert setInformativeText:[params objectAtIndex:1]];
    [alert setIcon:[NSImage new]];
    _confirmResult = ([alert runModal] == NSAlertFirstButtonReturn);
}

- (IBAction)displaySaveAs:(id)sender {
    NSArray *params = sender;
    
    NSSavePanel* savePanel = [NSSavePanel savePanel];
    
    savePanel.title = [params objectAtIndex:0];
    savePanel.showsResizeIndicator = YES;
    savePanel.showsHiddenFiles = NO;
    savePanel.canCreateDirectories = YES;
    NSString *type = [params objectAtIndex:1];
    savePanel.allowedFileTypes = (type ? @[type] : nil);
    
    _saveAsActive = true;
    _saveAsResult = nil;
    [self updateStatusAction];
    
    [NSApp activateIgnoringOtherApps:YES];
    
    [savePanel beginSheetModalForWindow:_window
                      completionHandler:^(NSInteger result) {
                          if (result==NSModalResponseOK) {
                              NSURL *selection = savePanel.URL;
                              _saveAsResult = [selection.path stringByResolvingSymlinksInPath];
                          }
                          _saveAsActive = false;
                          [mainApp updateStatusAction];
                      }];
}

- (IBAction)displayOpenFile:(id)sender {
    NSArray *params = sender;
    
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    
    openPanel.title = [params objectAtIndex:0];
    openPanel.showsResizeIndicator = YES;
    openPanel.showsHiddenFiles = NO;
    openPanel.canChooseDirectories = NO;
    openPanel.canCreateDirectories = NO;
    openPanel.allowsMultipleSelection = NO;
    NSString *type = [params objectAtIndex:1];
    openPanel.allowedFileTypes = (type ? @[type] : nil);
    
    _openFileActive = true;
    _openFileResult = nil;
    [self updateStatusAction];
    
    [NSApp activateIgnoringOtherApps:YES];
    
    [openPanel beginSheetModalForWindow:_window
                      completionHandler:^(NSInteger result) {
                          if (result==NSModalResponseOK) {
                              NSURL *selection = openPanel.URLs[0];
                              _openFileResult = [selection.path stringByResolvingSymlinksInPath];
                          }
                          _openFileActive = false;
                          [self updateStatusAction];
                      }];
}

- (IBAction)displayBalloon:(id)sender {
    NSArray *params = sender;

    [NSApp activateIgnoringOtherApps:YES];
 
    NSRect statusRect = [[_statusItem valueForKey:@"window"] frame];
    [mainApp setTrayWindow:[[MAAttachedWindow alloc] initWithView:_balloonView
                                                  attachedToPoint:NSMakePoint(NSMidX(statusRect), NSMinY(statusRect))
                                                         inWindow:nil
                                                           onSide:MAPositionBottom
                                                       atDistance:5.0]];
    [_balloonText setTextColor:[_trayWindow borderColor]];
    NSString *text = [params objectAtIndex:0];
    NSRect frame = [_balloonText frame];
    if([text length] < 32) {
        frame.origin.y = -16;
    }
    else {
        frame.origin.y = -8;
    }
    [_balloonText setFrame:frame];
    [_balloonText setStringValue:text];
    [_trayWindow makeKeyAndOrderFront:self];
    
    if(_balloonTimer) {
        [_balloonTimer invalidate];
    }
    _balloonTimer = [NSTimer scheduledTimerWithTimeInterval:[[params objectAtIndex:1] floatValue] target:self selector:@selector(closeBalloon_timer:) userInfo:self repeats:false];
}

- (IBAction)closeBalloon:(id)sender {
    if(_trayWindow) {
        [_trayWindow orderOut:self];
        _trayWindow = nil;
    }
    if(_balloonTimer) {
        [_balloonTimer invalidate];
        _balloonTimer = nil;
    }
}

- (IBAction)closeBalloon_timer:(id)sender {
    _balloonTimer = nil;
    [self closeBalloon:self];
}

- (IBAction)openUserInput:(id)sender {
    _activeDialog = (UserInput*)sender;
    [self updateStatusAction];
    [_activeDialog showWindow:mainApp];
    [NSApp activateIgnoringOtherApps:YES];
}

- (IBAction)addUserInputField:(id)sender {
    [_activeDialog addField:[[sender objectAtIndex:0] intValue] withText:[sender objectAtIndex:1] withValue:[[sender objectAtIndex:2] pointerValue]];
}

- (IBAction)setUserInputInfo:(id)sender {
    [_activeDialog setInfo:sender];
}
- (IBAction)setUserInputError:(id)sender {
    [_activeDialog setError:sender];
}
- (IBAction)setUserInputValue:(id)sender {
    [_activeDialog setFieldValue:[[sender objectAtIndex:0] intValue] withValue:[[sender objectAtIndex:1] pointerValue]];
}
- (IBAction)getUserInputStringValue:(id)sender {
    _userInputString = [_activeDialog fieldStringValue:[sender intValue]];
}
- (IBAction)getUserInputBoolValue:(id)sender {
    _userInputBool = [_activeDialog fieldBoolValue:[sender intValue]];
}
- (IBAction)getUserInputUintValue:(id)sender {
    _userInputUint = [_activeDialog fieldUintValue:[sender intValue]];
}

- (IBAction)closeUserInput:(id) sender {
    [_activeDialog close];
    _activeDialog = NULL;
    [self updateStatusAction];
}

- (IBAction)menuMainDispatch:(id)sender {
    [self performSelectorInBackground:@selector(menuMainDispatch_block:) withObject:sender];
}

- (IBAction)menuWillOpen:(NSMenu *)menu {
    [self closeBalloon:self];
}

- (IBAction)rejectMenuOpen:(id)sender {
    [NSApp activateIgnoringOtherApps:true];
}

- (bool)hasActiveDialog {
    return _activeDialog || _saveAsActive || _openFileActive;
}

- (bool)showActiveDialog {
    if([self hasActiveDialog]) {
        [NSApp activateIgnoringOtherApps:true];
        return true;
    }
    return false;
}

- (void)updateStatusAction {
    if([self hasActiveDialog]) {
        _statusItem.menu = nil;
        [_statusItem setAction:@selector(rejectMenuOpen:)];
    }
    else {
        [_mainMenu setDelegate:self];
        _statusItem.menu = _mainMenu;
        [_statusItem setAction:nil];
    }
}

- (void)updateMenu {
    MenuCleanup();
    _mainMenu = (__bridge NSMenu*)MenuInit();
    [self updateStatusAction];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    mainApp = self;
    
    NSArray *appDataList = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *basePath = [appDataList firstObject];

    strcpy(config_path, [basePath UTF8String]);
    strcat(config_path, "/");
    strcat(config_path, APPDATA_NAME);
    strcat(config_path, "/");
    strcpy(vaults.appDataPath, config_path);
    strcat(config_path, "pwclip.ini");
    LoadConfig(config_path);
    
    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode: YES];
    [_statusItem setEnabled: YES];
    [_statusItem setImage: [NSImage imageNamed:@"MenuIcon"]];
    [_statusItem setTarget:self];

    if(!vaults.readHistory()) {
        TrayBalloon("Welcome to pwclip! To get started, try creating a vault.", 6.0f);
    }
    
    _mainMenu = (__bridge NSMenu*)MenuInit();
    [self updateStatusAction];

    [self performSelectorInBackground:@selector(monitorClipboard:) withObject:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    //Wipe clipboard if pending
    if(_clipWipeTimer) {
        [self wipeClipboard:self];
        [_clipWipeTimer invalidate];
        _clipWipeTimer = nil;
    }
    //GLobal cleanup
    MenuCleanup();
    _mainMenu = nil;

    if(!FileExists(vaults.appDataPath)) {
        [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:vaults.appDataPath] withIntermediateDirectories:YES attributes:nil error:NULL];
    }
    vaults.writeHistory();
    vaults.close();
    SaveConfig(config_path);
    
    mainApp = NULL;
}

- (IBAction)menuMainDispatch_block:(id)sender {
    NSMenuItem *item = sender;
    MenuItemMeta *meta = (MenuItemMeta*)[[item representedObject] pointerValue];
    if(meta) meta->activate();
}

- (IBAction)monitorClipboard:(id)sender {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    while(true) {
        [NSThread sleepForTimeInterval:0.2f];
        if(_clipWipeTimer) {
            if([pasteboard changeCount] != clipReference) {
                [mainApp performSelectorOnMainThread:@selector(cancelClipWipe:) withObject:self waitUntilDone:true];
            }
        }
    }
}

@end
