/*
 Menu building implementation for windows
 by: Connor Douthat
 10/29/2015
 */
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "globals.h"

class MenuItemMeta;

void MenuExit(const char *key)
{
    [NSApp terminate:mainApp];
}

void *CreateMenuContainer()
{
    NSMenu *menu = [[NSMenu alloc] init];
    [menu setAutoenablesItems:false];
    return (__bridge_retained void*)menu;
}
void AddMenuItem(void *pmenu, const char *text, MenuItemMeta *data, void *subItem, bool enabled, int atIndex)
{
    NSMenu *menu = (__bridge NSMenu*)pmenu;
    NSMenuItem *item = nil;
    SEL sel = subItem ? nil : @selector(menuMainDispatch:);
    if(atIndex < 0) {
        item = [menu addItemWithTitle:[NSString stringWithUTF8String:text] action:sel keyEquivalent:@""];
    }
    else {
        item = [menu insertItemWithTitle:[NSString stringWithUTF8String:text] action:sel keyEquivalent:@"" atIndex:atIndex];
    }
    if(subItem) {
        [menu setSubmenu:(__bridge id)subItem forItem:item];
    }
    else if(data) {
        [item setRepresentedObject:[NSValue valueWithPointer:data]];
    }
    else enabled = false;
    [item setEnabled:enabled];
}
void CheckMenuItem(void *menu, int index)
{
    [[(__bridge NSMenu*)menu itemAtIndex:index] setState:NSOnState];
}
void DestroyMenuContainer(void *pmenu)
{
    //Handled by ARC
}
bool MenuReload()
{
    [mainApp updateMenu];
    return true;
}
