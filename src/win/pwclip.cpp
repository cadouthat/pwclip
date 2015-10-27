/*
pwclip tray utility (windows)
by: Connor Douthat
10/20/2015
*/
#define _WIN32_WINNT 0x0500
#include "../shared/shared.h"
#include "includes.h"

int main(int argc, char **argv)
{
	//Settings and global init
	if(!(db = OpenDB(db_path)))
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
	if(db) sqlite3_close(db);
	return 0;
}
