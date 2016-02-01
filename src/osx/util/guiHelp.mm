/*
GUI related helper functions (OSX implementations)
by: Connor Douthat
11/5/2015
*/
#import <Cocoa/Cocoa.h>

void ErrorBox(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:[NSString stringWithUTF8String:message]];
    [alert setIcon:[NSImage imageNamed:@"ErrorIcon"]];
    [alert runModal];
	va_end(args);
}
