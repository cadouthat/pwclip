//
//  MenuBalloonController.m
//  pwclip
//
//  Created by Connor Douthat on 2/11/16.
//  Copyright © 2016 Connor Douthat. All rights reserved.
//

#import "MenuBalloonController.h"

@interface MenuBalloonController ()

@end

@implementation MenuBalloonController

- (void)mouseDown:(NSEvent *)theEvent {
    [[[NSApplication sharedApplication] delegate] performSelector:@selector(closeBalloon:) withObject:self];
}

@end
