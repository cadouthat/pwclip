/*
Windows entry point
by: Connor Douthat
10/20/2015
*/
#include "includes.h"

int main(int argc, char **argv)
{
	//Settings and global init
	char config_path[256] = {0};
	if(LocalUserAppData(APPDATA_NAME, config_path))
	{
		//Set app data path for vaults
		strcpy(vaults.appDataPath, config_path);
		//Get config file path and load it
		strcat(config_path, "pwclip.ini");
		LoadConfig(config_path);
	}
	vaults.readHistory();

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
	SetTimer(hwnd_main, TIMER_UPDATE_TRAY, 500, NULL);
	MSG msg;
	while(GetMessage(&msg, hwnd_main, 0, 0) > 0)
	{
		//Process message
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//GUI cleanup
	TrayCleanup();
	MenuCleanup();

	//GLobal cleanup
	vaults.writeHistory();
	vaults.close();
	return 0;
}
