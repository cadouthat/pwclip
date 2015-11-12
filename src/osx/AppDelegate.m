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

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode: YES];
    [_statusItem setEnabled: YES];
    [_statusItem setImage: [NSImage imageNamed:@"StatusBarButtonImage"]];
    
    NSMenu *menu = [[NSMenu alloc] init];
    [menu addItemWithTitle:@"Test" action:nil keyEquivalent:@""];
    [menu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    _statusItem.menu = menu;
    
    /*
     //Settings and global init
     char config_path[256] = {0};
     if(LocalUserAppData(APPDATA_NAME, config_path))
     {
     strcat(config_path, "pwclip.ini");
     LoadConfig(config_path);
     }
     vaults.readHistory();
     
     //GUI init
     MenuInit();
     
     //Main message loop
     //
     
     //GUI cleanup
     MenuCleanup();
     
     //GLobal cleanup
     vaults.writeHistory();
     vaults.close();
     */
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
