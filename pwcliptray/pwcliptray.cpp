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
	//Settings and global init
	if(!OpenDB())
	{
		ErrorBox("Failed to open/create database");
		return 1;
	}
	//GUI init
	MenuInit();
	TrayInit();
	//Main message loop
	MSG msg;
	while(GetMessage(&msg, hwndMain, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//GUI cleanup
	TrayCleanup();
	MenuCleanup();
	//GLobal cleanup
	CloseDB();
	return 0;
}
