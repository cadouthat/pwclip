/*
Interaction to save an entry
by: Connor Douthat
10/24/2015
*/
void SaveDialog()
{
	//Make sure vault is open
	OpenVaultDialog(0);
	if(!vaults.topOpen()) return;

	//Store plaintext from clipboard
	char *clip_text = GetClipboardText();
	if(!clip_text)
	{
		ErrorBox("Clipboard has no text");
		return;
	}
	ClipboardWatchStart();

	//Prompt for name
	void *prompt = UserInput_new(UIF_NAME, "Save New Entry");
	if(UserInput_get(prompt))
	{
		VaultEntry entry(vaults.top(), UserInput_name(prompt));
		//Confirm overwrite if value exists
		bool result = true;
		if(entry.exists())
		{
			if(ConfirmBox("Overwrite Warning", "Are you sure you want to replace '%s'?", entry.name()))
			{
				if(!entry.remove()) result = false;
			}
			else result = false;
		}
		if(result)
		{
			//Assign ownership of plaintext
			entry.valuePlain(clip_text);
			clip_text = NULL;
			if(entry.save())
			{
				//Refresh menu
				MenuReload();
				//Update tray on success
				TrayWipeState();
				TrayBalloon("Entry successfully saved.");
			}
		}
	}
	UserInput_delete(prompt);

	if(clip_text)
	{
		//Wipe and free plaintext
		memset(clip_text, 0, strlen(clip_text));
		free(clip_text);
	}
}
