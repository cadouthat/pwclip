/*
C++ bridge for UserInput
by: Connor Douthat
11/10/2015
*/
#import "UserInputObjC.h"

void *UserInput_new(int flags_in, const char *title_in)
{
    return (void*)CFBridgingRetain([[UserInputObjC alloc] init]);
}
void UserInput_delete(void *ui)
{
    CFRelease((CFTypeRef)ui);
}
const char *UserInput_name(void *ui)
{
    return [(__bridge id) ui nameValue];
}
const char *UserInput_pass(void *ui)
{
    return [(__bridge id) ui passValue];
}
void UserInput_setInfo(void *ui, const char *text)
{
    return [(__bridge id) ui setInfo:text];
}
void UserInput_setError(void *ui, const char *text)
{
    return [(__bridge id) ui setError:text];
}
bool UserInput_get(void *ui)
{
    return [(__bridge id) ui get];
}
