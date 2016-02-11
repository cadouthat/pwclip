/*
 Menu building implementation for windows
 by: Connor Douthat
 10/29/2015
 */
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <cstdlib>
#include <vector>
#include "../shared/MenuTree.h"

NSMenu *buildMenuTree(MenuTree *root, SEL action) {
    NSMenu *menu = [[NSMenu alloc] init];

    for(int i = 0; i < root->nodes.size(); i++)
    {
        MenuTree *child = root->nodes[i];
        if(child->key)
        {
            NSMenuItem *subItem = [menu addItemWithTitle:[NSString stringWithUTF8String:child->name] action:action keyEquivalent:@""];
            [subItem setRepresentedObject:[NSString stringWithUTF8String:child->key]];
        }
        else
        {
            [menu setSubmenu:buildMenuTree(child, action) forItem:[menu addItemWithTitle:[NSString stringWithUTF8String:child->name] action:NULL keyEquivalent:@""]];
        }
    }
    
    return menu;
}
