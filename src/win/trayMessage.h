/*
Handle tray interaction messages
by: Connor Douthat
10/22/2015
*/
LRESULT CALLBACK HandleTrayMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			int id = LOWORD(wParam);
			if(id >= TRAY_KEY && id - TRAY_KEY < menu_keys.size())
			{
				id -= TRAY_KEY;
				char *name = menu_keys[id];
				PWClipEntry entry(&crypto_keys, db, name);
				if(id < recall_menu_end)
				{
					LoadDialog(&entry);
				}
				else if(id < encrypt_menu_end)
				{
					EncryptDialog(&entry);
				}
				else if(id < remove_menu_end)
				{
					RemoveDialog(&entry);
				}
			}
			else switch(id)
			{
			case TRAY_SAVE:
				SaveDialog();
				break;
			case TRAY_SETTINGS:
				SettingsDialog();
				break;
			case TRAY_EXPORT:
				ExportDialog();
				break;
			case TRAY_EXIT:
				DestroyWindow(hwnd);
				break;
			}
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
			TrackPopupMenu(popupMenu, TPM_BOTTOMALIGN | TPM_RIGHTALIGN, p.x, p.y, 0, hwnd, NULL);
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
