/*
Interaction to change an entry
by: Connor Douthat
2/14/2016
*/
bool RenameEntry(VaultEntry *entry, const char *new_key, bool &name_conflict, void *prompt, bool commit = true);
char *RenameEntryKey(const char *base_orig, const char *base_new, const char *key_orig);

void ChangeEntryDialog(VaultEntry *entry)
{
	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Entry name must be nonempty
	if(!entry->name() || !entry->name()[0]) return;
	int entry_name_len = strlen(entry->name());

	//For single entry, pass to entry form
	if(entry->name()[entry_name_len - 1] != ':')
	{
		CreateEntryDialog(entry);
		return;
	}

	void *prompt = UserInput_new("Rename");

	//Trim trailing separator
	char *prompt_text = strdup(entry->name());
	prompt_text[entry_name_len - 1] = 0;
	UserInput_addField(prompt, UIF_TEXT, "Entry Name", prompt_text);
	free(prompt_text);

	bool success = false;
	bool get_name = true;
	while(get_name && UserInput_get(prompt))
	{
		success = false;
		get_name = false;
		//Build new key
		char *new_name = UserInput_stringValue(prompt, 0);
		char *new_key = (char*)malloc(strlen(new_name) + 2);
		strcpy(new_key, new_name);
		if(*new_key) strcat(new_key, ":");
		free(new_name);
		//Special case - no change
		if(!strcmp(new_key, entry->name()))
		{
			free(new_key);
			break;
		}
		//Find all entries within tree
		std::vector<VaultEntry*> subentries;
		std::vector<char*> new_subkeys;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(entry->getVault()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
		{
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				const char *subkey = (const char*)sqlite3_column_text(stmt, 0);
				if(!strncmp(subkey, entry->name(), entry_name_len))
				{
					//Get sub entry and determine new key
					VaultEntry *subentry = new VaultEntry(entry->getVault(), subkey);
					char *new_subkey = RenameEntryKey(entry->name(), new_key, subkey);
					subentries.push_back(subentry);
					new_subkeys.push_back(new_subkey);
					//Probe rename
					success = RenameEntry(subentry, new_subkey, get_name, prompt, false);
					if(!success) break;
				}
			}
			sqlite3_finalize(stmt);
		}
		if(success)
		{
			for(int i = 0; i < subentries.size(); i++)
			{
				VaultEntry *subentry = subentries[i];
				char *new_subkey = new_subkeys[i];
				//Decrypt entry
				if(subentry->decrypt() && subentry->valuePlain())
				{
					//Commit rename
					success = RenameEntry(subentry, new_subkey, get_name, prompt);
					if(!success) break;
				}
				else
				{
					char subkey_short[512] = {0};
					stringPreview(subentry->name(), subkey_short, sizeof(subkey_short));
					ErrorBox("Failed to decrypt '%s'", subkey_short);
					success = false;
					break;
				}
			}
		}
		//Clean up
		for(int i = 0; i < subentries.size(); i++)
		{
			delete subentries[i];
			delete new_subkeys[i];
		}
	}
	UserInput_delete(prompt);

	if(success)
	{
		//Refresh menu
		MenuReload();
		TrayBalloon("Rename successful.");
	}
}

bool RenameEntry(VaultEntry *entry, const char *new_key, bool &name_conflict, void *prompt, bool commit)
{
	//Replace single entry
	VaultEntry new_entry(entry->getVault(), new_key);
	if(!new_entry.exists())
	{
		if(!commit) return true;
		new_entry.valuePlain(entry->valuePlain());
		if(new_entry.save())
		{
			entry->remove();
			return true;
		}
	}
	else
	{
		char *error_text = (char*)malloc(strlen(new_key) + 64);
		sprintf(error_text, "Entry '%s' already exists", new_key);
		UserInput_setError(prompt, error_text);
		free(error_text);
		name_conflict = true;
	}
	return false;
}

char *RenameEntryKey(const char *base_orig, const char *base_new, const char *key_orig)
{
	char *key = (char*)malloc(strlen(base_new) + strlen(key_orig) + 1);
	strcpy(key, base_new);
	strcat(key, key_orig + strlen(base_orig));
	return key;
}
