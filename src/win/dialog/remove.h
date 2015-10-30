/*
Interaction to remove an entry
by: Connor Douthat
10/24/2015
*/
void RemoveDialog(PWClipEntry *entry)
{
	//Ask user for confirmation
	char message[512] = {0};
	snprintf(message, sizeof(message), "Are you sure you want to delete '%s'?", entry->name());
	if(IDYES == MessageBox(hwnd_main, message, "Confirm Delete", MB_YESNO))
	{
		if(entry->remove())
		{
			//Refresh menu
			MenuReload();
			TrayBalloon("Entry deleted.");
		}
	}
}
