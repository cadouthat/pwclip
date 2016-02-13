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
			int id = LOWORD(wParam);
			if(id >= TRAY_KEY && id - TRAY_KEY < menu_keys.size())
			{
				//Shouldn't be here without vault open
				if(!vaults.topOpen()) break;
				//Initialize selected entry
				id -= TRAY_KEY;
				char *name = menu_keys[id];
				VaultEntry entry(vaults.top(), name);
				//Determine appropriate action
				if(id < recall_menu_end)
				{
					LoadDialog(&entry);
				}
				else if(id < remove_menu_end)
				{
					RemoveDialog(&entry);
				}
			}
			else if(id >= TRAY_SWITCH_DB && id - TRAY_SWITCH_DB < MAX_DB_HIST)
			{
				//Open db from history
				OpenVaultDialog(id - TRAY_SWITCH_DB);
			}
			else switch(id)
			{
				char db_path[256];
			case TRAY_CREATE_DB:
				//Browse for db to open
				memset(db_path, 0, sizeof(db_path));
				if(BrowseForOutput(FILE_TYPE_DB, db_path))
				{
					//Open selected db
					OpenVaultDialog(0, db_path);
				}
				break;
			case TRAY_BROWSE_DB:
				//Browse for db to open
				memset(db_path, 0, sizeof(db_path));
				if(BrowseForInput(FILE_TYPE_DB, db_path))
				{
					//Open selected db
					OpenVaultDialog(0, db_path);
				}
				break;
			case TRAY_CLOSE_DB:
				vaults.close();
				MenuReload();
				break;
			case TRAY_SET_MASTER:
				MasterPassDialog();
				break;
			case TRAY_GENERATE:
				GeneratePassword();
				break;
			case TRAY_SAVE:
				SaveDialog();
				break;
			case TRAY_EXPORT:
				ExportDialog();
				break;
			case TRAY_EXIT:
				DestroyWindow(hwnd);
				break;
			}
			command_active = false;
		}
		break;
	case MSG_TRAYICON:
		switch(lParam)
		{
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
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
			break;
		case TIMER_UPDATE_TRAY:
			//Cancel clipboard wipe if contents change
			if(GetClipboardSequenceNumber() != clip_sequence)
			{
				TrayNormalState();
				KillTimer(hwnd, TIMER_WIPE);
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
