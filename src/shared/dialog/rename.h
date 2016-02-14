/*
Interaction to rename an entry
by: Connor Douthat
2/14/2016
*/
bool RenameEntry(VaultEntry *entry, const char *new_key, bool &name_conflict, char *&error_text, void *prompt, bool commit = true);
char *RenameEntryKey(const char *base_orig, const char *base_new, const char *key_orig);

void RenameDialog(VaultEntry *entry)
{
	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Entry name must be nonempty
	if(!entry->name() || !entry->name()[0]) return;

	//For single entry, decrypt early
	if(entry->exists())
	{
		if(!entry->decrypt() || !entry->valuePlain())
		{
			ErrorBox("Decrypt failed.");
			return;
		}
	}

	//Parse base key and current name
	bool is_tree = false;
	char *base_key = strdup(entry->name());
	char *cur_name = NULL;
	int delim = strlen(base_key) - 1;
	if(base_key[delim] == ':')
	{
		//Trim trailing separator
		base_key[delim--] = 0;
		is_tree = true;
	}
	//Find last separator
	while(delim >= 0 && base_key[delim] != ':') delim--;
	if(delim >= 0)
	{
		//Split at separator
		cur_name = strdup(base_key + delim + 1);
		base_key[delim + 1] = 0;
	}
	else
	{
		//Name is at root
		cur_name = strdup(base_key);
		base_key[0] = 0;
	}

	//Prompt for new name
	void *prompt = UserInput_new(UIF_NAME, "Rename");

	char *info_text = (char*)malloc(strlen(cur_name) + 64);
	sprintf(info_text, "Enter new name for '%s'", cur_name);
	UserInput_setInfo(prompt, info_text);
	free(info_text);

	char *error_text = NULL;
	char *new_key = NULL;
	bool success = false;
	while(UserInput_get(prompt))
	{
		bool name_conflict = false;
		//Build new key
		const char *replace_name = UserInput_name(prompt);
		if(new_key) free(new_key);
		new_key = (char*)malloc(strlen(base_key) + strlen(replace_name) + 2);
		strcpy(new_key, base_key);
		strcat(new_key, replace_name);
		if(is_tree) strcat(new_key, ":");
		//Execute replacement
		if(is_tree)
		{
			//To replace trees, get entries starting with tree name
			sqlite3_stmt *stmt;
			if(SQLITE_OK == sqlite3_prepare_v2(entry->getVault()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' AND `key` LIKE ?||'%' ORDER BY `key`", -1, &stmt, NULL))
			{
				if(SQLITE_OK == sqlite3_bind_text(stmt, 1, entry->name(), -1, SQLITE_STATIC))
				{
					//Verification loop
					while(sqlite3_step(stmt) == SQLITE_ROW)
					{
						const char *subkey = (const char*)sqlite3_column_text(stmt, 0);
						VaultEntry subentry(entry->getVault(), subkey);
						//Validate rename
						char *new_subkey = RenameEntryKey(entry->name(), new_key, subkey);
						success = RenameEntry(&subentry, new_subkey, name_conflict, error_text, prompt, false);
						free(new_subkey);
						if(!success) break;
					}
					if(success)
					{
						//Commit loop
						sqlite3_reset(stmt);
						while(sqlite3_step(stmt) == SQLITE_ROW)
						{
							const char *subkey = (const char*)sqlite3_column_text(stmt, 0);
							VaultEntry subentry(entry->getVault(), subkey);
							//Decrypt entry
							if(subentry.decrypt() && subentry.valuePlain())
							{
								//Commit rename
								char *new_subkey = RenameEntryKey(entry->name(), new_key, subkey);
								success = RenameEntry(&subentry, new_subkey, name_conflict, error_text, prompt);
								free(new_subkey);
							}
							else
							{
								char subkey_short[512] = {0};
								stringPreview(subkey, subkey_short, sizeof(subkey_short));
								ErrorBox("Failed to decrypt '%s'", subkey_short);
								success = false;
							}
							if(!success) break;
						}
					}
				}
				sqlite3_finalize(stmt);
			}
		}
		else
		{
			//Replace single entry
			success = RenameEntry(entry, new_key, name_conflict, error_text, prompt);
		}
		if(!name_conflict) break;
	}
	UserInput_delete(prompt);
	if(new_key) free(new_key);
	if(cur_name) free(cur_name);
	if(base_key) free(base_key);
	if(error_text) free(error_text);

	if(success)
	{
		//Refresh menu
		MenuReload();
		TrayBalloon("Rename successful.");
	}
}

bool RenameEntry(VaultEntry *entry, const char *new_key, bool &name_conflict, char *&error_text, void *prompt, bool commit)
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
		if(error_text) free(error_text);
		error_text = (char*)malloc(strlen(new_key) + 64);
		sprintf(error_text, "Entry '%s' already exists", new_key);
		UserInput_setError(prompt, error_text);
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
