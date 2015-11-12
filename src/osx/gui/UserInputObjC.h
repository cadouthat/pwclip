/*
Objective C interface for UserInput
by: Connor Douthat
11/10/2015
*/
#import <Foundation/Foundation.h>

@interface UserInputObjC : NSObject

- (const char*) nameValue;
- (const char*) passValue;
- (void) setInfo:(const char*) text;
- (void) setError:(const char*) text;
- (bool) get;

@end
