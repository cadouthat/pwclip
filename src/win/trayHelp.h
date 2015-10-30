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
	hwnd_main = CreateWindow(notifyClass.lpszClassName,
		"",
		0,
		0,
		0,
		CW_USEDEFAULT,
		0,
		HWND_DESKTOP,
		NULL,
		hModule,
		NULL);
	if(!hwnd_main) return false;
	//Create tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd_main;
	nid.uFlags = NIF_TIP | NIF_MESSAGE | NIF_ICON;
	nid.uCallbackMessage = MSG_TRAYICON;
	nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	strcpy(nid.szTip, TRAY_TOOLTIP);
	return Shell_NotifyIcon(NIM_ADD, &nid);
}
bool TrayBalloon(const char *message)
{
	//Update tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd_main;
	nid.uFlags = NIF_INFO;
	nid.uTimeout = 5000;
	strcpy(nid.szInfo, message);
	strcpy(nid.szInfoTitle, TRAY_TOOLTIP);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}
bool TrayNormalState()
{
	if(tray_state == TRAY_STATE_NORMAL) return true;
	tray_state = TRAY_STATE_NORMAL;
	//Update tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd_main;
	nid.uFlags = NIF_TIP | NIF_ICON;
	nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	strcpy(nid.szTip, TRAY_TOOLTIP);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}
bool TraySuccessState()
{
	//Set timer to wipe clipboard
	KillTimer(hwnd_main, TIMER_WIPE);
	SetTimer(hwnd_main, TIMER_WIPE, clip_wipe_delay * 1000, NULL);
	if(tray_state == TRAY_STATE_WIPE) return true;
	tray_state = TRAY_STATE_WIPE;
	//Update tray icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd_main;
	nid.uFlags = NIF_TIP | NIF_ICON;
	nid.hIcon = LoadIcon(NULL, IDI_WARNING);
	if(!config_keep_clip)
	{
		strcpy(nid.szTip, "The clipboard will be wiped shortly..");
	}
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}
bool TrayCleanup()
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd_main;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}
