/*
pwclip tray utility (windows)
by: Connor Douthat
10/20/2015
*/
#define _WIN32_WINNT 0x0500
#include "../core/sysIncludes.h"
#include "includes.h"

int main(int argc, char **argv)
{
	//Create tray icon and listen for messages
	MenuInit();
	TrayInit();
	MSG msg;
	while(GetMessage(&msg, hwndMain, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//Cleanup
	TrayCleanup();
	MenuCleanup();
	return 0;
}
