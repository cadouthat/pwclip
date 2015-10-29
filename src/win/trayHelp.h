/*
Manages tray icon and notification window
by: Connor Douthat
10/22/2015
*/
LRESULT CALLBACK HandleTrayMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool TrayInit()
{
	HMODULE hModule = GetModuleHandle(NULL);
	//Register class for notifications
	WNDCLASS notifyClass;
	ZeroMemory(&notifyClass, sizeof(notifyClass));
	notifyClass.lpfnWndProc = HandleTrayMessage;
	notifyClass.hInstance = hModule;
	notifyClass.lpszClassName = "TrayNotifyWindow";
	if(!RegisterClass(&notifyClass)) return false;
	//Create hidden window for notifications
	hwndMain = CreateWindow(notifyClass.lpszClassName,
		"",
		0,
		0,
		0,
		CW_USEDEFAULT,
		0,
		HWND_MESSAGE,
		NULL,
		hModule,
		NULL);
	if(!hwndMain) return false;
	//Create tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uFlags = NIF_TIP | NIF_MESSAGE | NIF_ICON;
	nid.uCallbackMessage = MSG_TRAYICON;
	nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	strcpy(nid.szTip, TRAY_TOOLTIP);
	return Shell_NotifyIcon(NIM_ADD, &nid);
}
bool TrayNormalState()
{
	//Update tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uFlags = NIF_TIP | NIF_ICON;
	nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	strcpy(nid.szTip, TRAY_TOOLTIP);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}
bool TraySuccessState()
{
	//Set timer to wipe clipboard
	wipe_clip_timer = GetTickCount() + clip_wipe_delay * 1000;
	//Update tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uFlags = NIF_TIP | NIF_ICON;
	nid.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	if(config_keep_clip)
	{
		strcpy(nid.szTip, "Success!");
	}
	else
	{
		strcpy(nid.szTip, "Success! The clipboard will be wiped shortly..");
	}
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}
bool TrayCleanup()
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}
