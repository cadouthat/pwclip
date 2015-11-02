/*
Interaction to save an entry
by: Connor Douthat
10/24/2015
*/
void SaveDialog()
{
	//Make sure vault is open
	OpenVaultDialog(0);
	if(!db.topDB() || !db.topKey()) return;

	//Store plaintext from clipboard
	char *clip_text = GetClipboardText();
	if(!clip_text)
	{
		ErrorBox("Clipboard has no text");
		return;
	}
	clip_sequence = GetClipboardSequenceNumber();

	//Prompt for name
	UserInput prompt(UIF_NAME, "Save New Entry");
	if(prompt.get())
	{
		PWClipEntry entry(db.topDB(), prompt.name());
		//Confirm overwrite if value exists
		bool result = true;
		if(entry.exists())
		{
			char message[512] = {0};
			snprintf(message, sizeof(message), "Are you sure you want to replace '%s'?", entry.name());
			if(IDYES == MessageBox(hwnd_main, message, "Overwrite Warning", MB_YESNO))
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
			if(entry.save(db.topKey()))
			{
				//Refresh menu
				MenuReload();
				//Update tray on success
				TrayWipeState();
				TrayBalloon("Entry successfully saved.");
			}
		}
	}

	if(clip_text)
	{
		//Wipe and free plaintext
		memset(clip_text, 0, strlen(clip_text));
		free(clip_text);
	}
}
