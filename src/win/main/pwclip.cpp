/*
Windows entry point
by: Connor Douthat
10/20/2015
*/
#include "../includes.h"

int main(int argc, char **argv)
{
	//Settings and global init
	HANDLE instanceMutex = CreateMutex(NULL, true, "_pwclip_tray_instance");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if(instanceMutex) CloseHandle(instanceMutex);
		ErrorBox("pwclip is already running");
		return 1;
	}

	char config_path[256] = {0};
	if(LocalUserAppData(APPDATA_NAME, config_path))
	{
		//Set app data path for vaults
		strcpy(vaults.appDataPath, config_path);
		//Get config file path and load it
		strcat(config_path, "pwclip.ini");
		LoadConfig(config_path);
	}
	bool skipWelcome = vaults.readHistory();

	//Find and repair stale startup value
	CheckStartupValue();

	//GUI init
	INITCOMMONCONTROLSEX icex = {0};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	if (!InitCommonControlsEx(&icex))
	{
		ErrorBox("Failed to initialize common controls, cannot create window");
		return 1;
	}
	popup_menu = (HMENU)MenuInit();
	TrayInit();

	//Welcome message for new users
	if(!skipWelcome)
	{
		TrayBalloon("Welcome to pwclip! To get started, try creating a vault.");
	}

	//Main message loop
	SetTimer(hwnd_main, TIMER_UPDATE_TRAY, 500, NULL);
	MSG msg;
	while(GetMessage(&msg, hwnd_main, 0, 0) > 0)
	{
		//Process message
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//Wipe clipboard if still owned
	if(clip_wipe_pending)
	{
		if(GetClipboardSequenceNumber() == clip_sequence)
		{
			WipeClipboardText();
		}
	}

	//GUI cleanup
	TrayCleanup();
	MenuCleanup();

	//GLobal cleanup
	vaults.writeHistory();
	vaults.close();
	SaveConfig(config_path);
	CloseHandle(instanceMutex);
	return 0;
}
