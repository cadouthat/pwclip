/*
Interaction to change master password
by: Connor Douthat
10/24/2015
*/
bool ReEncryptAll(PasswordCipher *old_key, PasswordCipher *new_key, bool verbose = true);

void MasterPassDialog()
{
	//Make sure vault is open
	OpenVaultDialog(0);
	if(!db.topDB() || !db.topKey()) return;

	//Prompt for new password
	UserInput prompt(UIF_NEWPASS, "Change Master Password");
	if(prompt.get())
	{
		//Update all entries with new key
		PasswordCipher *new_key = new PasswordCipher(prompt.newpass());
		if(ReEncryptAll(db.topKey(), new_key))
		{
			//Transfer key to vault
			db.topKey(new_key);
			TrayBalloon("Master password successfully changed.");
		}
		else
		{
			//Roll back changes
			ReEncryptAll(new_key, db.topKey(), false);
			delete new_key;
		}
	}
}

bool ReEncryptAll(PasswordCipher *old_key, PasswordCipher *new_key, bool verbose)
{
	//Query all entries
	bool result = true;
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(db.topDB(), "SELECT `key` FROM `entries` ORDER BY `key`", -1, &stmt, NULL))
	{
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			//Load entry
			const char *key = (const char*)sqlite3_column_text(stmt, 0);
			PWClipEntry entry(db.topDB(), key);
			//Update entry
			if(!entry.reEncrypt(old_key, new_key))
			{
				//Abort
				if(verbose) ErrorBox("Failed to update '%s'", key);
				result = false;
				break;
			}
		}
		sqlite3_finalize(stmt);
	}
	return result;
}
