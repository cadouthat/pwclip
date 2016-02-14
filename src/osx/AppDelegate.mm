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
@property NSMenuItem *menuGenerate;
@property NSMenuItem *menuSaveEntry;
@property NSMenuItem *menuLoadEntry;
@property NSMenuItem *menuDeleteEntry;
@property NSMenuItem *menuSetMaster;
@property NSMenuItem *menuExportAll;
@property NSMenuItem *menuVaults;
@property NSMenuItem *menuOpenVault;
@property NSMenuItem *menuSwitchVault;
@property NSMenuItem *menuCloseVaults;

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
    SetClipboardText("");
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
    
    _saveAsCompleted = false;
    _saveAsResult = nil;
    
    [NSApp activateIgnoringOtherApps:YES];
    
    [savePanel beginSheetModalForWindow:_window
                      completionHandler:^(NSInteger result) {
                          if (result==NSModalResponseOK) {
                              NSURL *selection = savePanel.URL;
                              _saveAsResult = [selection.path stringByResolvingSymlinksInPath];
                          }
                          _saveAsCompleted = true;
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
    _balloonTimer = [NSTimer scheduledTimerWithTimeInterval:[[params objectAtIndex:1] floatValue] target:self selector:@selector(closeBalloon:) userInfo:self repeats:false];
}

- (IBAction)closeBalloon:(id)sender {
    [_trayWindow orderOut:self];
    _trayWindow = nil;
    if(_balloonTimer) {
        [_balloonTimer invalidate];
        _balloonTimer = nil;
    }
}

- (IBAction)generate:(id)sender {
    GenerateDialog();
}

- (IBAction)saveEntry:(id)sender {
    [self performSelectorInBackground:@selector(saveEntry_block:) withObject:sender];
}

- (IBAction)loadEntry:(id)sender {
    VaultEntry entry(vaults.top(), [[sender representedObject] UTF8String]);
    LoadDialog(&entry);
}

- (IBAction)deleteEntry:(id)sender {
    VaultEntry entry(vaults.top(), [[sender representedObject] UTF8String]);
    RemoveDialog(&entry);
}

- (IBAction)setMaster:(id)sender {
    [self performSelectorInBackground:@selector(setMaster_block:) withObject:sender];
}

- (IBAction)exportAll:(id)sender {
    [self performSelectorInBackground:@selector(exportAll_block:) withObject:sender];
}

- (IBAction)openUserInput:(id)sender {
    _activeDialog = (UserInput*)sender;
    [_activeDialog showWindow:mainApp];
    [NSApp activateIgnoringOtherApps:YES];
}

- (IBAction)closeUserInput:(id) sender {
    [_activeDialog close];
    _activeDialog = NULL;
}

- (IBAction)openVault:(id)sender {
    NSMenuItem *item = (NSMenuItem*)sender;
    NSNumber *indObj = [item representedObject];
    if([indObj intValue] >= 0) {
        [self performSelectorInBackground:@selector(openVault_block:) withObject:indObj];
    }
    else {
        NSOpenPanel* openPanel = [NSOpenPanel openPanel];
        
        openPanel.title = @"Choose a vault file";
        openPanel.showsResizeIndicator = YES;
        openPanel.showsHiddenFiles = NO;
        openPanel.canChooseDirectories = NO;
        openPanel.canCreateDirectories = YES;
        openPanel.allowsMultipleSelection = NO;
        openPanel.allowedFileTypes = @[@"db"];
        
        [NSApp activateIgnoringOtherApps:YES];

        [openPanel beginSheetModalForWindow:_window
                          completionHandler:^(NSInteger result) {
                              if (result==NSModalResponseOK) {
                                  NSURL *selection = openPanel.URLs[0];
                                  NSString* path = [selection.path stringByResolvingSymlinksInPath];
                                  [self performSelectorInBackground:@selector(openVault_block:) withObject:path];
                              }
                          }];
    }
}

- (IBAction)createVault:(id)sender {
    [self performSelectorInBackground:@selector(createVault_block:) withObject:self];
}

- (IBAction)closeVaults:(id)sender {
    vaults.close();
    [self updateMenu];
}

- (void)updateMenu {
    NSMenu *subMenu = [[NSMenu alloc] init];
    
    for(int i = 0; i < vaults.history.size(); i++)
    {
        NSMenuItem *subItem = [subMenu addItemWithTitle:[NSString stringWithUTF8String:vaults.history[i]->path()] action:@selector(openVault:) keyEquivalent:@""];
        [subItem setRepresentedObject:[NSNumber numberWithInt:i]];
        if(i == 0 && vaults.topOpen()) {
            [subItem setState:NSOnState];
        }
    }
    [[subMenu addItemWithTitle:@"Other..." action:@selector(openVault:) keyEquivalent:@""] setRepresentedObject:[NSNumber numberWithInt:-1]];
    
    [_mainMenu setSubmenu:subMenu forItem:_menuOpenVault];
    
    bool vaultEnabled = vaults.topOpen();
    [_menuSaveEntry setEnabled:vaultEnabled];
    [_menuLoadEntry setEnabled:vaultEnabled];
    [_menuDeleteEntry setEnabled:vaultEnabled];
    [_menuSetMaster setEnabled:vaultEnabled];
    [_menuExportAll setEnabled:vaultEnabled];
    [_menuCloseVaults setEnabled:vaultEnabled];
    [_menuSwitchVault setEnabled:vaultEnabled];
    
    if(vaultEnabled) {
        MenuTree tree(GenerateMenuValue);
        sqlite3_stmt *stmt;
        if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
        {
            int i = 0;
            while(sqlite3_step(stmt) == SQLITE_ROW)
            {
                //Duplicate key and append menu
                const char *key = (const char*)sqlite3_column_text(stmt, 0);
                tree.parse(key);
            }
            sqlite3_finalize(stmt);
        }
        [_mainMenu setSubmenu:buildMenuTree(&tree, @selector(deleteEntry:)) forItem:_menuDeleteEntry];
        [_mainMenu setSubmenu:buildMenuTree(&tree, @selector(loadEntry:)) forItem:_menuLoadEntry];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    mainApp = self;

    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode: YES];
    [_statusItem setEnabled: YES];
    [_statusItem setImage: [NSImage imageNamed:@"MenuIcon"]];
    
    NSMenu *vaultMenu = [[NSMenu alloc] init];
    [vaultMenu setAutoenablesItems:false];
    [vaultMenu addItemWithTitle:@"Create Vault" action:@selector(createVault:) keyEquivalent:@""];
    _menuOpenVault = [vaultMenu addItemWithTitle:@"Open Vault" action:NULL keyEquivalent:@""];
    _menuSwitchVault = [vaultMenu addItemWithTitle:@"Switch Vault" action:NULL keyEquivalent:@""];
    _menuCloseVaults = [vaultMenu addItemWithTitle:@"Close All Vaults" action:@selector(closeVaults:) keyEquivalent:@""];
    
    _mainMenu = [[NSMenu alloc] init];
    [_mainMenu setAutoenablesItems:false];
    _menuGenerate = [_mainMenu addItemWithTitle:@"Generate Password" action:@selector(generate:) keyEquivalent:@""];
    _menuSaveEntry = [_mainMenu addItemWithTitle:@"Save from Clipboard" action:@selector(saveEntry:) keyEquivalent:@""];
    _menuLoadEntry = [_mainMenu addItemWithTitle:@"Load to Clipboard" action:NULL keyEquivalent:@""];
    _menuDeleteEntry = [_mainMenu addItemWithTitle:@"Delete Entry" action:NULL keyEquivalent:@""];
    _menuExportAll = [_mainMenu addItemWithTitle:@"Export Raw Entries" action:@selector(exportAll:) keyEquivalent:@""];
    _menuSetMaster = [_mainMenu addItemWithTitle:@"Change Master Password" action:@selector(setMaster:) keyEquivalent:@""];
    _menuVaults = [_mainMenu addItemWithTitle:@"Vaults" action:NULL keyEquivalent:@""];
    [_mainMenu setSubmenu:vaultMenu forItem:_menuVaults];
    [_mainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    _statusItem.menu = _mainMenu;
    
    NSArray *appDataList = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *basePath = [appDataList firstObject];

    strcpy(config_path, [basePath UTF8String]);
    strcat(config_path, "/");
    strcat(config_path, APPDATA_NAME);
    strcat(config_path, "/");
    strcpy(vaults.appDataPath, config_path);
    strcat(config_path, "pwclip.ini");
    LoadConfig(config_path);
    if(!vaults.readHistory()) {
        TrayBalloon("Welcome to pwclip! To get started, try creating a vault.", 6.0f);
    }
    
    [self updateMenu];
    
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
    if(!FileExists(vaults.appDataPath)) {
        [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:vaults.appDataPath] withIntermediateDirectories:YES attributes:nil error:NULL];
    }
    vaults.writeHistory();
    vaults.close();
    SaveConfig(config_path);
    
    mainApp = NULL;
}

- (IBAction)createVault_block:(id)sender {
    char path[256] = {0};
    if(BrowseForOutput(FILE_TYPE_DB, path)) {
        [self openVault_block:[NSString stringWithUTF8String:path]];
    }
}

- (IBAction)exportAll_block:(id)sender {
    ExportDialog();
}

- (IBAction)setMaster_block:(id)sender {
    MasterPassDialog();
}

- (IBAction)saveEntry_block:(id)sender {
    SaveDialog();
}

- (IBAction)openVault_block:(id)sender {
    if([sender isKindOfClass:[NSNumber class]]) {
        OpenVaultDialog([sender intValue]);
    }
    else {
        OpenVaultDialog(-1, [sender UTF8String]);
    }
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
