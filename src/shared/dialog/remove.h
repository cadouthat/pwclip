/*
Interaction to remove an entry
by: Connor Douthat
10/24/2015
*/
void RemoveDialog(VaultEntry *entry)
{
	//Ask user for confirmation
	char key_short[512] = {0};
	stringPreview(entry->name(), key_short, sizeof(key_short));
	if(ConfirmBox("Confirm Delete", "Are you sure you want to delete '%s'?", key_short))
	{
		if(entry->remove())
		{
			//Refresh menu
			MenuReload();
			TrayBalloon("Entry deleted.");
		}
	}
}
