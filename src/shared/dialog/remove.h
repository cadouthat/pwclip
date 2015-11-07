/*
Interaction to remove an entry
by: Connor Douthat
10/24/2015
*/
void RemoveDialog(VaultEntry *entry)
{
	//Ask user for confirmation
	if(ConfirmBox("Confirm Delete", "Are you sure you want to delete '%s'?", entry->name()))
	{
		if(entry->remove())
		{
			//Refresh menu
			MenuReload();
			TrayBalloon("Entry deleted.");
		}
	}
}
