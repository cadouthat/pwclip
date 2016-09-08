/*
Interaction to load an entry
by: Connor Douthat
10/24/2015
*/
void RecallEntryDialog(VaultEntry *entry)
{
	if(entry->decrypt() && entry->plaintext())
	{
		//Move plaintext to clipboard
		if(SetClipboardText(entry->plaintext()))
		{
			//Update tray on success
			ClipboardWatchStart();
			TrayWipeState();
			TrayBalloon("Entry loaded to clipboard.");
		}
		else ErrorBox("Failed to set clipboard text");
	}
	else ErrorBox("Decryption failed!");
}
