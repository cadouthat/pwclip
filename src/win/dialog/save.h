/*
Interaction to save an entry
by: Connor Douthat
10/24/2015
*/
void SaveDialog()
{
	//Initial prompt is always needed
	char prompt_title[512] = {0};
	snprintf(prompt_title, sizeof(prompt_title), "Save New Entry");
	UserInput prompt(UIF_GENERATE | UIF_NAME | UIF_NEWPASS, prompt_title);
	if(prompt.get())
	{
		//Init new entry
		PWClipEntry entry(&crypto_keys, db, prompt.name());
		//Generate password if requested
		if(prompt.generate())
		{
			char pass[GEN_PASS_SIZE + 1] = {0};
			if(!RandText(pass, GEN_PASS_SIZE))
			{
				ErrorBox("Failed to generate password");
				return;
			}
			if(!SetClipboardText(pass))
			{
				ErrorBox("Failed to set clipboard text");
				return;
			}
			memset(pass, 0, sizeof(pass));
		}
		//Confirm overwrite if value exists
		if(entry.exists())
		{
			char message[512] = {0};
			snprintf(message, sizeof(message), "Are you sure you want to replace '%s'?", entry.name());
			if(IDYES == MessageBox(hwndMain, message, "Overwrite Warning", MB_YESNO))
			{
				if(!entry.remove()) return;
			}
			else return;
		}
		//Update tray on success
		if(entry.save())
		{
			TraySuccessState();
		}
	}
}
