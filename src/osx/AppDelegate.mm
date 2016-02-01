//
//  AppDelegate.m
//  pwclip
//
//  Created by Connor Douthat on 11/8/15.
//  Copyright © 2015 Connor Douthat. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property NSStatusItem *statusItem;
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

- (IBAction)openVault:(id)sender {
}

- (IBAction)closeVaults:(id)sender {
}

- (void)updateVaultList {
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode: YES];
    [_statusItem setEnabled: YES];
    [_statusItem setImage: [NSImage imageNamed:@"MenuIcon"]];
    
    NSMenu *menu = [[NSMenu alloc] init];
    [menu setAutoenablesItems:false];
    _menuGenerate = [menu addItemWithTitle:@"Generate Password" action:@selector(generate:) keyEquivalent:@""];
    _menuSaveEntry = [menu addItemWithTitle:@"Save from Clipboard" action:@selector(saveEntry:) keyEquivalent:@""];
    _menuLoadEntry = [menu addItemWithTitle:@"Load to Clipboard" action:@selector(loadEntry:) keyEquivalent:@""];
    _menuDeleteEntry = [menu addItemWithTitle:@"Delete Entry" action:@selector(deleteEntry:) keyEquivalent:@""];
    _menuSetMaster = [menu addItemWithTitle:@"Change Master Password" action:@selector(setMaster:) keyEquivalent:@""];
    _menuExportAll = [menu addItemWithTitle:@"Raw Export" action:@selector(exportAll:) keyEquivalent:@""];
    _menuOpenVault = [menu addItemWithTitle:@"Open Vault" action:@selector(openVault:) keyEquivalent:@""];
    _menuCloseVaults = [menu addItemWithTitle:@"Close All Vaults" action:@selector(closeVaults:) keyEquivalent:@""];
    [menu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    _statusItem.menu = menu;
    
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
}

@end
