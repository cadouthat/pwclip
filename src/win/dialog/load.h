/*
Interaction to load an entry
by: Connor Douthat
10/24/2015
*/
void LoadDialog(PWClipEntry *entry)
{
	if(db.topKey() && entry->decrypt(db.topKey()) && entry->valuePlain())
	{
		//Move plaintext to clipboard
		if(SetClipboardText(entry->valuePlain()))
		{
			//Update tray on success
			clip_sequence = GetClipboardSequenceNumber();
			TrayWipeState();
			TrayBalloon("Entry loaded to clipboard.");
		}
		else ErrorBox("Failed to set clipboard text");
	}
	else ErrorBox("Decryption failed!");
}
