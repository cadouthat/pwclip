/*
Primary actions exposed in the core module
by: Connor Douthat
10/2/2015
*/
bool EntryAction(char *action, char *pw_name, bool *wipe_clip)
{
	PWClipEntry entry(crypto_keys, db, pw_name);
	if(!stricmp(action, "load"))
	{
		//Wipe clipboard after successful load
		*wipe_clip = entry.load();
		return *wipe_clip;
	}
	else if(!stricmp(action, "save"))
	{
		//If the generate flag was set, store a random password in the clipboard
		if(flag_generate)
		{
			char pass[GEN_PASS_SIZE + 1] = {0};
			if(!RandText(pass, GEN_PASS_SIZE))
			{
				printf("Failed to generate password\n");
				return false;
			}
			if(!SetClipboardText(pass))
			{
				printf("Failed to set clipboard text\n");
				return false;
			}
			memset(pass, 0, sizeof(pass));
			printf("Clipboard loaded with random password\n");
		}
		//If the force flag was set, any existing entry is removed before saving
		if(flag_force && entry.exists())
		{
			if(!entry.remove()) return false;
		}
		//Wipe clipboard after successful save
		*wipe_clip = entry.save();
		return *wipe_clip;
	}
	else if(!stricmp(action, "remove"))
	{
		//Clipboard stores nothing for remove
		*wipe_clip = false;
		return entry.remove();
	}
	else
	{
		printf("Invalid action: '%s'\n", action);
		return false;
	}
}
