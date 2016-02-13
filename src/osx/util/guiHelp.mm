/*
GUI related helper functions (OSX implementations)
by: Connor Douthat
11/5/2015
*/
#import <Cocoa/Cocoa.h>
#import "MAAttachedWindow.h"

#import "globals.h"

void ErrorBox(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
    [mainApp performSelectorOnMainThread:@selector(displayError:) withObject:[NSString stringWithUTF8String:message] waitUntilDone:true];
	va_end(args);
}

bool ConfirmBox(const char *title, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char message[1024] = {0};
    vsnprintf(message, sizeof(message), format, args);
    [mainApp performSelectorOnMainThread:@selector(displayConfirm:) withObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:title], [NSString stringWithUTF8String:message], nil] waitUntilDone:true];
    va_end(args);
    return [mainApp confirmResult];
}

bool TrayBalloon(const char *message) {
    [mainApp performSelectorOnMainThread:@selector(displayBalloon:) withObject:[NSString stringWithUTF8String:message] waitUntilDone:false];
    return true;
}

bool MenuReload() {
    [mainApp updateMenu];
    return true;
}

bool BrowseForOutput(int type, char *out, int out_max)
{
    NSString *ext = nil;
    switch(type)
    {
    case FILE_TYPE_DB:
        ext = @"db";
        break;
    case FILE_TYPE_TXT:
        ext = @"txt";
        break;
    }

    [mainApp performSelectorOnMainThread:@selector(displaySaveAs:) withObject:[NSArray arrayWithObjects:@"Choose export destination", @"txt", nil] waitUntilDone:true];
    while(![mainApp saveAsCompleted]) {
        [NSThread sleepForTimeInterval:0.2f];
    }
    if([mainApp saveAsResult]) {
        strncpy(out, [[mainApp saveAsResult] UTF8String], out_max);
        return true;
    }
    else return false;
}

bool TrayWipeState()
{
    [mainApp scheduleClipWipe:mainApp];
    return [mainApp clipWipeTimer] != nil;
}

void ClipboardWatchStart()
{
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    clipReference = [pasteboard changeCount];
}

char *GetClipboardText()
{
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    return strdup([[pasteboard stringForType:NSPasteboardTypeString] UTF8String]);
}

bool SetClipboardText(const char *text)
{
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    if(!pasteboard) return false;
    [pasteboard clearContents];
    [pasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:mainApp];
    return (YES == [pasteboard setString:[NSString stringWithUTF8String:text] forType:NSPasteboardTypeString]);
}
