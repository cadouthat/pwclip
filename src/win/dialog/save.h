/*
Interaction to save an entry
by: Connor Douthat
10/24/2015
*/
void SaveDialog()
{
	//Store plaintext from clipboard
	char *clip_text = GetClipboardText();
	if(!clip_text)
	{
		ErrorBox("Clipboard has no text");
		return;
	}
	clip_sequence = GetClipboardSequenceNumber();
	//Initial prompt is always needed
	char prompt_title[512] = {0};
	snprintf(prompt_title, sizeof(prompt_title), "Save New Entry");
	UserInput prompt(UIF_NAME | UIF_NEWPASS, prompt_title);
	if(prompt.get())
	{
		crypto_keys.nextEncrypt(new PasswordCipher(prompt.newpass()));
		//Init new entry and assign ownership of plaintext
		PWClipEntry entry(&crypto_keys, db.loaded, prompt.name());
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
			entry.valuePlain(clip_text);
			clip_text = NULL;
			if(entry.save())
			{
				//Refresh menu
				MenuReload();
				//Update tray on success
				TraySuccessState();
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
