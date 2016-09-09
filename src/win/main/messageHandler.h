/*
Handle tray interaction messages
by: Connor Douthat
10/22/2015
*/
LRESULT CALLBACK HandleTrayMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool command_active = false;
	switch(msg)
	{
	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			command_active = true;
			int mapInd = LOWORD(wParam) - MENU_BASE_ID;
			if(mapInd >= 0 && mapInd < menu_map.size())
			{
				MenuItemMeta *meta = menu_map[mapInd];
				if(meta) meta->activate();
			}
			command_active = false;
		}
		break;
	case MSG_TRAYICON:
		switch(lParam)
		{
		case WM_LBUTTONUP:
			command_active = true;
			QuickRecallDialog();
			command_active = false;
			break;
		case WM_RBUTTONUP:
			POINT p;
			GetCursorPos(&p);
			SetForegroundWindow(hwnd);
			if(!command_active) TrackPopupMenu(popup_menu, TPM_BOTTOMALIGN | TPM_RIGHTALIGN, p.x, p.y, 0, hwnd, NULL);
			break;
		}
		break;
	case WM_TIMER:
		switch(wParam)
		{
		case TIMER_WIPE:
			//Wipe clipboard if it hasn't changed
			if(GetClipboardSequenceNumber() == clip_sequence)
			{
				if(!WipeClipboardText()) ErrorBox("Failed to wipe clipboard");
			}
			//Return tray to normal state
			TrayNormalState();
			KillTimer(hwnd, wParam);
			clip_wipe_pending = false;
			break;
		case TIMER_UPDATE_TRAY:
			//Cancel clipboard wipe if contents change
			if(GetClipboardSequenceNumber() != clip_sequence)
			{
				TrayNormalState();
				KillTimer(hwnd, TIMER_WIPE);
				clip_wipe_pending = false;
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
