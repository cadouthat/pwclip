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

@property (weak) IBOutlet NSWindow *window;
@property NSStatusItem *statusItem;
@property NSMenu *mainMenu;
@property NSMenuItem *menuGenerate;
@property NSMenuItem *menuSaveEntry;
@property NSMenuItem *menuLoadEntry;
@property NSMenuItem *menuDeleteEntry;
@property NSMenuItem *menuSetMaster;
@property NSMenuItem *menuExportAll;
@property NSMenuItem *menuOpenVault;
@property NSMenuItem *menuCloseVaults;

@end

@implementation AppDelegate

- (IBAction)displayError:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:sender];
    [alert setIcon:[NSImage imageNamed:@"ErrorIcon"]];
    [alert runModal];
}

- (IBAction)generate:(id)sender {
}

- (IBAction)saveEntry:(id)sender {
}

- (IBAction)loadEntry:(id)sender {
}

- (IBAction)deleteEntry:(id)sender {
}

- (IBAction)setMaster:(id)sender {
}

- (IBAction)exportAll:(id)sender {
}

- (IBAction)openUserInput:(id)sender {
    _activeDialog = (UserInput*)sender;
    [_activeDialog showWindow:mainApp];
}

- (IBAction)closeUserInput:(id) sender {
    [_activeDialog close];
    _activeDialog = NULL;
}

- (IBAction)openVault:(id)sender {
    [self performSelectorInBackground:@selector(openVault_block:) withObject:sender];
}

- (IBAction)openVault_block:(id)sender {
    NSMenuItem *item = (NSMenuItem*)sender;
    NSNumber *indObj = [item representedObject];
    int ind = [indObj intValue];
    if(ind >= 0) {
        OpenVaultDialog(ind);
    }
    else ErrorBox("Need browse dialog..");
}

- (IBAction)closeVaults:(id)sender {
}

- (void)updateVaultList {
    NSMenu *subMenu = [[NSMenu alloc] init];
    
    for(int i = 0; i < vaults.history.size(); i++)
    {
        [[subMenu addItemWithTitle:[NSString stringWithUTF8String:vaults.history[i]->path()] action:@selector(openVault:) keyEquivalent:@""] setRepresentedObject:[NSNumber numberWithInt:i]];
    }
    [[subMenu addItemWithTitle:@"Other..." action:@selector(openVault:) keyEquivalent:@""] setRepresentedObject:[NSNumber numberWithInt:-1]];
    
    [_mainMenu setSubmenu:subMenu forItem:_menuOpenVault];
    [_menuOpenVault setEnabled:true];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    mainApp = self;

    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode: YES];
    [_statusItem setEnabled: YES];
    [_statusItem setImage: [NSImage imageNamed:@"MenuIcon"]];
    
    _mainMenu = [[NSMenu alloc] init];
    [_mainMenu setAutoenablesItems:false];
    _menuGenerate = [_mainMenu addItemWithTitle:@"Generate Password" action:@selector(generate:) keyEquivalent:@""];
    _menuSaveEntry = [_mainMenu addItemWithTitle:@"Save from Clipboard" action:@selector(saveEntry:) keyEquivalent:@""];
    _menuLoadEntry = [_mainMenu addItemWithTitle:@"Load to Clipboard" action:NULL keyEquivalent:@""];
    _menuDeleteEntry = [_mainMenu addItemWithTitle:@"Delete Entry" action:@selector(deleteEntry:) keyEquivalent:@""];
    _menuSetMaster = [_mainMenu addItemWithTitle:@"Change Master Password" action:@selector(setMaster:) keyEquivalent:@""];
    _menuExportAll = [_mainMenu addItemWithTitle:@"Raw Export" action:@selector(exportAll:) keyEquivalent:@""];
    _menuOpenVault = [_mainMenu addItemWithTitle:@"Open Vault" action:@selector(openVault:) keyEquivalent:@""];
    _menuCloseVaults = [_mainMenu addItemWithTitle:@"Close All Vaults" action:@selector(closeVaults:) keyEquivalent:@""];
    [_mainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    _statusItem.menu = _mainMenu;
    
    [_menuSaveEntry setEnabled:false];
    [_menuLoadEntry setEnabled:false];
    [_menuDeleteEntry setEnabled:false];
    [_menuSetMaster setEnabled:false];
    [_menuExportAll setEnabled:false];
    [_menuCloseVaults setEnabled:false];
    
    NSArray *appDataList = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *basePath = [appDataList firstObject];
    char config_path[256] = {0};
    strcpy(config_path, [basePath UTF8String]);
    strcat(config_path, "/");
    strcat(config_path, APPDATA_NAME);
    strcat(config_path, "/");
    strcpy(vaults.appDataPath, config_path);
    strcat(config_path, "pwclip.ini");
    LoadConfig(config_path);
    vaults.readHistory();
    
    [self updateVaultList];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    //GLobal cleanup
    if(!FileExists(vaults.appDataPath)) {
        [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:vaults.appDataPath] withIntermediateDirectories:YES attributes:nil error:NULL];
    }
    vaults.writeHistory();
    vaults.close();
    
    mainApp = NULL;
}

@end
