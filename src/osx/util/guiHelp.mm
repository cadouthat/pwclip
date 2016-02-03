/*
GUI related helper functions (OSX implementations)
by: Connor Douthat
11/5/2015
*/
#import <Cocoa/Cocoa.h>

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


bool TrayBalloon(const char *message) {
    ErrorBox("%s", message);
    return true;
}

bool MenuReload() {
    [mainApp updateVaultList];
    return true;
}
