/*
Primary actions exposed in the core module
by: Connor Douthat
10/2/2015
*/
bool EntryAction(char *action, char *pw_name, bool *wipe_clip)
{
	PWClipEntry entry(db, pw_name);
	if(!stricmp(action, "load"))
	{
		//Wipe clipboard after successful load
		*wipe_clip = entry.load();
		return *wipe_clip;
	}
	else if(!stricmp(action, "save"))
	{
		//Wipe clipboard after successful save
		*wipe_clip = entry.save();
		return *wipe_clip;
	}
	else if(!stricmp(action, "replace"))
	{
		//Remove existing value before saving
		if(!entry.remove()) return false;
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
