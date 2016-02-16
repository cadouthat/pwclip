/*
Interaction to replace an entry
by: Connor Douthat
2/14/2016
*/
void ReplaceDialog(VaultEntry *entry)
{
	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Special logic for adding new entry to submenu
	int last_char = strlen(entry->name()) - 1;
	if(last_char >= 0 && entry->name()[last_char] == ':')
	{
		SaveDialog(entry->name());
		return;
	}

	//Store plaintext from clipboard
	char *clip_text = GetClipboardText();
	if(!clip_text)
	{
		ErrorBox("Clipboard has no text");
		return;
	}
	ClipboardWatchStart();

	//Ask user for confirmation
	bool confirm = false;
	if(password_preview)
	{
		char preview[24] = {0};
		stringPreview(clip_text, preview, sizeof(preview));
		char key_short[512] = {0};
		stringPreview(entry->name(), key_short, sizeof(key_short));
		confirm = ConfirmBox("Confirm Replace", "Are you sure you want to overwrite '%s' with value '%s'?", key_short, preview);
		memset(preview, 0, sizeof(preview));
	}
	else
	{
		char key_short[512] = {0};
		stringPreview(entry->name(), key_short, sizeof(key_short));
		confirm = ConfirmBox("Confirm Replace", "Are you sure you want to overwrite '%s' with the current clipboard contents?", key_short);
	}
	if(!confirm) return;

	entry->remove();
	entry->valuePlain(clip_text);
	if(entry->save())
	{
		//Update tray on success
		TrayWipeState();
		TrayBalloon("Entry successfully saved.");
	}

	//Wipe and free plaintext
	memset(clip_text, 0, strlen(clip_text));
	free(clip_text);
}
