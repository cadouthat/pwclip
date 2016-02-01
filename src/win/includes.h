/*
Master include file
by: Connor Douthat
10/22/2015
*/
//Windows system
#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <unistd.h>

//System macros
#define SleepSeconds(x) Sleep(x * 1000)

//Shared
#include "../shared/shared.h"

#include "gui/MenuTree.h"
#include "gui/UserInput.h"

//Windows implementations
#include "globals.h"

#include "gui/UserInput.cpp"
#include "gui/guiConstants.h"
#include "gui/MenuTree.cpp"
#include "gui/guiHelp.h"
#include "gui/tray.h"
#include "gui/menu.h"

#include "util/clipHelp.h"
#include "util/envHelp.h"

#include "dialog/openVault.h"
#include "dialog/masterPass.h"
#include "dialog/save.h"
#include "dialog/load.h"
#include "dialog/remove.h"
#include "dialog/export.h"

#include "trayMessage.h"
