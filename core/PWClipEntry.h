/*
PWClipEntry class which acts on a password vault entry
by: Connor Douthat
10/1/2015
*/
class PWClipEntry
{
	PasswordCipher *crypto;
	sqlite3 *db;
	const char *key;
	char *value;
	char *iv;

	bool getCrypto()
	{
		if(!crypto && !(crypto = CryptoInit()))
		{
			printf("Crypto init failed\n");
			return false;
		}
		return true;
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

public:
	PWClipEntry(sqlite3 *db_in, const char *key_in)
	{
		crypto = NULL;
		db = db_in;
		key = key_in;
		//Find existing value (if any)
		value = NULL;
		iv = NULL;
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
		clearValue();
		if(crypto) delete crypto;
	}
	bool exists()
	{
		return (value && iv);
	}
	bool load()
	{
		if(!exists())
		{
			printf("Entry not found\n");
			return false;
		}
		//Decode IV to binary
		unsigned char iv_raw[CRYPTO_BLOCK_SIZE];
		hex2bin(iv, iv_raw, sizeof(iv_raw));
		//Decrypt value
		if(!getCrypto()) return false;
		char *value_plain = crypto->decrypt(value, iv_raw);
		if(!value_plain)
		{
			printf("Failed to decrypt value\n");
			return false;
		}
		//Move to clipboard
		if(!SetClipboardText(value_plain))
		{
			printf("Failed to set clipboard text\n");
			return false;
		}
		//Wipe and free plaintext
		memset(value_plain, 0, strlen(value_plain));
		free(value_plain);
		printf("Entry successfully loaded to clipboard\n");
		return true;
	}
	bool save()
	{
		if(exists())
		{
			printf("Entry already exists\n");
			return false;
		}
		char *value_plain = GetClipboardText();
		if(!value_plain)
		{
			printf("Failed to get clipboard text\n");
			return false;
		}
		//Encrypt plaintext from clipboard
		if(!getCrypto()) return false;
		unsigned char iv_raw[CRYPTO_BLOCK_SIZE];
		value = crypto->encrypt(value_plain, iv_raw);
		//Wipe and free plaintext
		memset(value_plain, 0, strlen(value_plain));
		free(value_plain);
		//Verify encryption success
		if(!value)
		{
			printf("Failed to encrypt value\n");
			return false;
		}
		//Convert IV to hex encoding
		iv = bin2hex(iv_raw, sizeof(iv_raw));
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
			printf("Unknown error creating entry\n");
			clearValue();
			return false;
		}
		printf("Entry successfully saved\n");
		return true;
	}
	bool remove()
	{
		if(!exists())
		{
			printf("Entry not found\n");
			return false;
		}
		//Attempt delete
		bool delete_ok = false;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "DELETE FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC))
			{
				delete_ok = (sqlite3_step(stmt) == SQLITE_DONE);
			}
			sqlite3_finalize(stmt);
		}
		if(delete_ok)
		{
			clearValue();
			printf("Entry successfully removed\n");
		}
		else printf("Unknown error removing entry\n");
		return delete_ok;
	}
};
