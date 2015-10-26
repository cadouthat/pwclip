/*
PWClipEntry class which acts on a password vault entry
by: Connor Douthat
10/1/2015
*/
class PWClipEntry
{
	KeyManager *crypto;
	sqlite3 *db;
	const char *key;
	char *value;
	char *iv;
	char *value_plain;
	unsigned char iv_raw[CRYPTO_BLOCK_SIZE];

	void clearPlaintext()
	{
		if(value_plain)
		{
			//Wipe plaintext before freeing
			memset(value_plain, 0, strlen(value_plain));
			free(value_plain);
			value_plain = NULL;
		}
		memset(iv_raw, 0, sizeof(iv_raw));
	}
	void clearValue()
	{
		if(value)
		{
			free(value);
			value = NULL;
		}
		if(iv)
		{
			free(iv);
			iv = NULL;
		}
	}
	void clear()
	{
		clearPlaintext();
		clearValue();
	}
	bool decrypt(const char *prompt_text = NULL)
	{
		clearPlaintext();
		//Must have existing value to decrypt
		if(!exists())
		{
			ErrorBox("Entry not found");
			return false;
		}
		//Decode IV to binary
		hex2bin(iv, iv_raw, sizeof(iv_raw));
		//Decrypt value
		value_plain = crypto->decrypt(value, iv_raw, prompt_text);
		if(!value_plain)
		{
			ErrorBox("Failed to decrypt value");
			return false;
		}
		return true;
	}
	bool encrypt(const char *prompt_text = NULL)
	{
		clearValue();
		//Encrypt plaintext from clipboard
		value = crypto->encrypt(value_plain, iv_raw, prompt_text);
		//Verify encryption success
		if(!value)
		{
			ErrorBox("Failed to encrypt value");
			return false;
		}
		//Convert IV to hex encoding
		iv = bin2hex(iv_raw, sizeof(iv_raw));
		if(!iv)
		{
			clearValue();
			ErrorBox("Failed to encode IV");
			return false;
		}
		return true;
	}

public:
	PWClipEntry(KeyManager *crypto_in, sqlite3 *db_in, const char *key_in)
	{
		crypto = crypto_in;
		db = db_in;
		key = key_in;
		//Find existing value (if any)
		value = NULL;
		iv = NULL;
		value_plain = NULL;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "SELECT `value`, `iv` FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC))
			{
				if(sqlite3_step(stmt) == SQLITE_ROW)
				{
					//Keep internal copy of value/iv
					value = strdup((const char*)sqlite3_column_text(stmt, 0));
					iv = strdup((const char*)sqlite3_column_text(stmt, 1));
				}
			}
			sqlite3_finalize(stmt);
		}
	}
	~PWClipEntry()
	{
		clear();
	}
	bool exists()
	{
		return (value && iv);
	}
	bool load()
	{
		//Decrypt value to plaintext
		if(!decrypt()) return false;
		//Move plaintext to clipboard
		bool result = SetClipboardText(value_plain);
		clearPlaintext();
		if(!result) ErrorBox("Failed to set clipboard text");
		return result;
	}
	bool save()
	{
		//No implicit overwrite
		if(exists())
		{
			ErrorBox("Entry already exists");
			return false;
		}
		//Make sure plaintext is clean
		clearPlaintext();
		//Store plaintext from clipboard
		value_plain = GetClipboardText();
		if(!value_plain)
		{
			ErrorBox("Failed to get clipboard text");
			return false;
		}
		if(!encrypt()) return false;
		clearPlaintext();
		//Attempt insert
		bool insert_ok = false;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "INSERT INTO `entries` (`key`, `value`, `iv`) VALUES (?, ?, ?)", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 3, iv, -1, SQLITE_STATIC))
			{
				insert_ok = (sqlite3_step(stmt) == SQLITE_DONE);
			}
			sqlite3_finalize(stmt);
		}
		if(!insert_ok)
		{
			//Value not saved, clear internal value
			clear();
			ErrorBox("Unknown error creating entry");
		}
		return insert_ok;
	}
	bool reEncrypt()
	{
		//Decrypt with appropriate prompt
		if(!decrypt("Old encryption password")) return false;
		//Displace original encrypted value for now
		char *value_old = value;
		char *iv_old = iv;
		value = NULL;
		iv = NULL;
		//Re-encrypt with appropriate prompt
		bool result = encrypt("New encryption password");
		clearPlaintext();
		if(result)
		{
			result = false;
			//Attempt update
			sqlite3_stmt *stmt;
			if(SQLITE_OK == sqlite3_prepare_v2(db, "UPDATE `entries` SET `value`=?, `iv`=? WHERE `key`=?", -1, &stmt, NULL))
			{
				if(SQLITE_OK == sqlite3_bind_text(stmt, 1, value, -1, SQLITE_STATIC) &&
					SQLITE_OK == sqlite3_bind_text(stmt, 2, iv, -1, SQLITE_STATIC) &&
					SQLITE_OK == sqlite3_bind_text(stmt, 3, key, -1, SQLITE_STATIC))
				{
					result = (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db));
				}
				sqlite3_finalize(stmt);
			}
			if(!result) ErrorBox("Unknown error updating entry");
		}
		//Cleanup
		if(result)
		{
			//Free original value
			if(value_old) free(value_old);
			if(iv_old) free(iv_old);
		}
		else
		{
			//Rollback value
			clearValue();
			value = value_old;
			iv = iv_old;
		}
		return result;
	}
	bool remove()
	{
		if(!exists())
		{
			ErrorBox("Entry not found");
			return false;
		}
		//Attempt delete
		bool delete_ok = false;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "DELETE FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC))
			{
				delete_ok = (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db));
			}
			sqlite3_finalize(stmt);
		}
		if(delete_ok) clear();
		else ErrorBox("Unknown error removing entry");
		return delete_ok;
	}
};
