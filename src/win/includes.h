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
#include <string>

//System macros
#define EM_SETCUEBANNER 0x1501
#define SleepSeconds(x) Sleep(x * 1000)

//Shared
#include "../shared/shared.h"

//Windows implementations
#include "main/def.h"
#include "main/globals.h"

#include "gui/UserInputField.h"
#include "gui/UserInput.h"
#include "gui/UserInputConnector.h"
#include "gui/menuHelp.h"
#include "gui/guiHelp.h"
#include "gui/trayHelp.h"

#include "util/clipHelp.h"
#include "util/envHelp.h"

#include "main/messageHandler.h"
