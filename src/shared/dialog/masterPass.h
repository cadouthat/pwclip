/*
Interaction to change master password
by: Connor Douthat
10/24/2015
*/
bool ReEncryptAll(Vault *vault, PasswordCipher *old_key, PasswordCipher *new_key, bool verbose = true);

void MasterPassDialog()
{
	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Prompt for new password
	void *prompt = UserInput_new("Change Vault Password");
	UserInput_addField(prompt, UIF_NEWPASS, "New Vault Password");
	if(UserInput_get(prompt))
	{
		//Build cipher from new password
		char *value = UserInput_stringValue(prompt, 0);
		PasswordCipher *new_key = new PasswordCipher(value);
		memset(value, 0, strlen(value));
		free(value);
		//Update all entries with new key
		if(ReEncryptAll(vaults.top(), vaults.top()->key(), new_key))
		{
			//Transfer key to vault
			vaults.top()->key(new_key);
			TrayBalloon("Vault password successfully changed.");
		}
		else
		{
			//Roll back changes
			ReEncryptAll(vaults.top(), new_key, vaults.top()->key(), false);
			delete new_key;
		}
	}
	UserInput_delete(prompt);
}

bool ReEncryptAll(Vault *vault, PasswordCipher *old_key, PasswordCipher *new_key, bool verbose)
{
	//Query all entries
	bool result = true;
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "SELECT `key` FROM `entries` ORDER BY `key`", -1, &stmt, NULL))
	{
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			//Load entry
			const char *key = (const char*)sqlite3_column_text(stmt, 0);
			VaultEntry entry(vault, key);
			//Update entry
			if(!entry.reEncrypt(old_key, new_key))
			{
				//Abort
				char key_short[128] = {0};
				stringPreview(key, key_short, sizeof(key_short));
				if(verbose) ErrorBox("Failed to update '%s'", key_short);
				result = false;
				break;
			}
		}
		sqlite3_finalize(stmt);
	}
	return result;
}
