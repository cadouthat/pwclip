/*
Master include file
by: Connor Douthat
11/5/2015
*/
//System macros
#define SleepSeconds(x) usleep(x * 1000000)

//Shared
#include "../shared/shared.h"

//OSX implementations
#include "globals.h"

void TestHybrid()
{
    ErrorBox("Hello");
}
