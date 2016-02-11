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

- (IBAction)generate:(id)sender {
    char pass[GEN_PASS_SIZE + 1] = {0};
    if(!RandText(pass, GEN_PASS_SIZE))
    {
        ErrorBox("Failed to generate password");
        return;
    }
    if(!SetClipboardText(pass))
    {
        ErrorBox("Failed to set clipboard text");
        return;
    }
    memset(pass, 0, sizeof(pass));
}

- (IBAction)saveEntry:(id)sender {
    [self performSelectorInBackground:@selector(saveEntry_block:) withObject:sender];
}

- (IBAction)loadEntry:(id)sender {
    VaultEntry entry(vaults.top(), [[sender representedObject] UTF8String]);
    LoadDialog(&entry);
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
    
    if(vaultEnabled) {
        MenuTree tree;
        sqlite3_stmt *stmt;
        if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
        {
            int i = 0;
            while(sqlite3_step(stmt) == SQLITE_ROW)
            {
                //Duplicate key and append menu
                char *key = strdup((const char*)sqlite3_column_text(stmt, 0));
                tree.parse(key, i++);
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
    
    _mainMenu = [[NSMenu alloc] init];
    [_mainMenu setAutoenablesItems:false];
    _menuGenerate = [_mainMenu addItemWithTitle:@"Generate Password" action:@selector(generate:) keyEquivalent:@""];
    _menuSaveEntry = [_mainMenu addItemWithTitle:@"Save from Clipboard" action:@selector(saveEntry:) keyEquivalent:@""];
    _menuLoadEntry = [_mainMenu addItemWithTitle:@"Load to Clipboard" action:NULL keyEquivalent:@""];
    _menuDeleteEntry = [_mainMenu addItemWithTitle:@"Delete Entry" action:NULL keyEquivalent:@""];
    _menuSetMaster = [_mainMenu addItemWithTitle:@"Change Master Password" action:@selector(setMaster:) keyEquivalent:@""];
    _menuExportAll = [_mainMenu addItemWithTitle:@"Raw Export" action:@selector(exportAll:) keyEquivalent:@""];
    _menuOpenVault = [_mainMenu addItemWithTitle:@"Open Vault" action:NULL keyEquivalent:@""];
    _menuCloseVaults = [_mainMenu addItemWithTitle:@"Close All Vaults" action:@selector(closeVaults:) keyEquivalent:@""];
    [_mainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    _statusItem.menu = _mainMenu;
    
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
    
    [self updateMenu];
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

@end
