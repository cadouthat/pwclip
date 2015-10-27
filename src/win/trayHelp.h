/*
Manages tray icon and notification window
by: Connor Douthat
10/22/2015
*/
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
	strcpy(nid.szTip, "pwclip");
	return Shell_NotifyIcon(NIM_ADD, &nid);
}
bool TrayCleanup()
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}
