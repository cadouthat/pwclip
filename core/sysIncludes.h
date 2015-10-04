/*
System and library includes
by: Connor Douthat
10/2/2015
*/
//Windows
#ifdef __WIN32__
#include <windows.h>
#include <shlobj.h>
#endif

//Standard
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>

//Third party
#include <sqlite3/sqlite3.h>

#ifdef __WIN32__
//Windows macros
#define SleepSeconds(x) Sleep(x * 1000)
#else
//Non-windows macros
#define SleepSeconds(x) usleep(x * 1000000)
#endif