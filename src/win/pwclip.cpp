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
		//Process timers
		if(wipe_clip_timer && wipe_clip_timer < GetTickCount())
		{
			//Wipe clipboard if configured
			if(!config_keep_clip)
			{
				if(!WipeClipboardText()) ErrorBox("Failed to wipe clipboard");
			}
			//Return tray to normal state
			TrayNormalState();
			wipe_clip_timer = 0;
		}
		//Process message
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
