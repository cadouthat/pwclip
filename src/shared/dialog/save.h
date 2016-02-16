/*
Interaction to save an entry
by: Connor Douthat
10/24/2015
*/
void SaveDialog(const char *base = NULL)
{
	//Make sure vault is open
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
	void *prompt = UserInput_new("Save New Entry");
	UserInput_addField(prompt, UIF_TEXT, "Entry Name", base);
	char preview_text[24] = {0};
	char info_text[128] = {0};
	if(password_preview)
	{
		stringPreview(clip_text, preview_text, sizeof(preview_text));
		sprintf(info_text, "Creating new entry with value '%s'", preview_text);
		UserInput_setInfo(prompt, info_text);
	}
	if(UserInput_get(prompt))
	{
		char *name = UserInput_stringValue(prompt, 0);
		VaultEntry entry(vaults.top(), name);
		//Confirm overwrite if value exists
		bool result = true;
		if(entry.exists())
		{
			char key_short[512] = {0};
			stringPreview(entry.name(), key_short, sizeof(key_short));
			if(ConfirmBox("Overwrite Warning", "Are you sure you want to replace '%s'?", key_short))
			{
				if(!entry.remove())
				{
					ErrorBox("Failed to remove existing entry!");
					result = false;
				}
			}
			else result = false;
		}
		if(result)
		{
			entry.valuePlain(clip_text);
			if(entry.save())
			{
				//Refresh menu
				MenuReload();
				//Update tray on success
				TrayWipeState();
				TrayBalloon("Entry successfully saved.");
			}
		}
		free(name);
	}
	UserInput_delete(prompt);
	memset(info_text, 0, sizeof(info_text));
	memset(preview_text, 0, sizeof(preview_text));

	//Wipe and free plaintext
	memset(clip_text, 0, strlen(clip_text));
	free(clip_text);
}
