/*
Interaction to change master password
by: Connor Douthat
10/24/2015
*/
bool ReEncryptAll(Vault *vault, PasswordCipher *old_key, PasswordCipher *new_key, bool verbose = true);

void ChangeVaultPassDialog()
{
	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Prompt for new password
	void *prompt = UserInput_new("Change Vault Password");
	UserInput_addField(prompt, UIF_NEWPASS, "New Vault Password");
	if(UserInput_get(prompt))
	{
		//Keep old salt on hand
		unsigned char old_salt[CRYPTO_SALT_SIZE];
		memcpy(old_salt, vaults.top()->salt(), sizeof(old_salt));
		//Generate fresh salt
		if(vaults.top()->genSalt())
		{
			//Build cipher from new password
			char *value = UserInput_stringValue(prompt, 0);
			PasswordCipher *new_key = new PasswordCipher(value, vaults.top()->salt());
			memset(value, 0, strlen(value));
			free(value);
			//Update all entries with new key
			if(ReEncryptAll(vaults.top(), vaults.top()->key(), new_key))
			{
				//Transfer key to vault
				vaults.top()->key(new_key);
				TrayBalloon("Vault password changed.");
			}
			else
			{
				//Roll back changes
				vaults.top()->setSalt(old_salt);
				ReEncryptAll(vaults.top(), new_key, vaults.top()->key(), false);
				delete new_key;
			}
		}
	}
	UserInput_delete(prompt);
}

bool ReEncryptAll(Vault *vault, PasswordCipher *old_key, PasswordCipher *new_key, bool verbose)
{
	if(!old_key || !new_key) return false;
	//Query all entries
	bool result = true;
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "SELECT `key` FROM `entries` WHERE `key` NOT LIKE '__meta__salt' ORDER BY `key`", -1, &stmt, NULL))
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
