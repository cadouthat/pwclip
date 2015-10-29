/*
pwclip tray utility (windows)
by: Connor Douthat
10/20/2015
*/
#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
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
	INITCOMMONCONTROLSEX icex = {0};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	if (!InitCommonControlsEx(&icex))
	{
		ErrorBox("Failed to initialize common controls, cannot create window");
		return 1;
	}
	MenuInit();
	TrayInit();
	//Main message loop
	MSG msg;
	while(GetMessage(&msg, hwnd_main, 0, 0) > 0)
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
