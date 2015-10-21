/*
pwclip tray utility (windows)
by: Connor Douthat
10/20/2015
*/
#include "../core/sysIncludes.h"
#include "sysIncludes.h"
#include "includes.h"

int main(int argc, char **argv)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uFlags = NIF_ICON;
	nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	Shell_NotifyIcon(NIM_ADD, &nid);
	Sleep(10000);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	return 0;
}
