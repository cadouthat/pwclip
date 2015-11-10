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

#include "util/menuHelp.h"
#include "util/envHelp.h"
#include "util/clipHelp.h"
#include "util/guiHelp.h"

#include "gui/UserInputBridge.h"
