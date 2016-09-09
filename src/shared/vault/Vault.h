/*
Class to represent a pwclip vault
by: Connor Douthat
11/1/2015
*/
class Vault
{
	bool fatal_flag;
	char *db_path;
	sqlite3 *db_handle;
	PasswordCipher *db_key;
	unsigned char db_salt[CRYPTO_SALT_SIZE];

public:
	Vault(const char *db_path_in)
	{
		memset(this, 0, sizeof(*this));
		db_path = strdup(db_path_in);
	}
	~Vault()
	{
		close();
		if(db_path)
		{
			free(db_path);
			db_path = NULL;
		}
	}
	const char *path() { return db_path ? db_path : ""; }
	bool exists() { return FileExists(db_path); }
	bool isOpen() { return (db_handle && db_key); }
	sqlite3 *db() { return db_handle; }
	PasswordCipher *key() { return db_key; }
	unsigned char *salt() { return db_salt; }
	bool key(PasswordCipher *key_in)
	{
		if(db_key) delete db_key;
		db_key = key_in;

		VaultEntry meta(this, "__meta__");
		if(!meta.exists())
		{
			//Initialize new key
			meta.plaintext(meta.name());
			if(meta.save()) return true;
			else fatal_flag = true;
		}
		else
		{
			//Key verification
			if(meta.decrypt()) return true;
		}
		close();
		return false;
	}
	bool fatal()
	{
		if(fatal_flag)
		{
			fatal_flag = false;
			return true;
		}
		else return false;
	}
	void close()
	{
		if(db_handle)
		{
			sqlite3_close(db_handle);
			db_handle = NULL;
		}
		if(db_key)
		{
			delete db_key;
			db_key = NULL;
		}
	}
	bool setSalt(unsigned char *salt_in)
	{
		VaultEntry meta_salt(this, "__meta__salt");
		if(meta_salt.exists() && !meta_salt.remove())
		{
			return false;
		}
		//Hex encode salt
		char *salt_hex = bin2hex(salt_in, CRYPTO_SALT_SIZE);
		meta_salt.ciphertext(salt_hex);
		free(salt_hex);
		//Save salt
		if(!meta_salt.save()) return false;
		//Copy salt
		memcpy(db_salt, salt_in, CRYPTO_SALT_SIZE);
		return true;
	}
	bool genSalt()
	{
		//Generate random salt
		unsigned char tmp_salt[CRYPTO_SALT_SIZE];
		if(RAND_bytes(tmp_salt, sizeof(tmp_salt)))
		{
			//Save new salt
			return setSalt(tmp_salt);
		}
		return false;
	}
	void peek(std::vector<char*> *names_out)
	{
		if(!db_handle && (!exists() || !open()))
		{
			return;
		}
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db_handle, "SELECT `key` FROM `entries` WHERE `key` NOT LIKE '__meta__%' ORDER BY `key`", -1, &stmt, NULL))
		{
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				names_out->push_back(strdup((const char*)sqlite3_column_text(stmt, 0)));
			}
			sqlite3_finalize(stmt);
		}
	}
	bool open()
	{
		//Already open
		if(db_handle) return true;
		//Check for existing file
		bool needs_init = !exists();
		//Open database
		if(sqlite3_open(db_path, &db_handle) != SQLITE_OK)
		{
			fatal_flag = true;
			ErrorBox("Failed to open '%s'", db_path);
			return false;
		}
		//First time setup for new files
		if(needs_init)
		{
			//Init schema
			char *err;
			int schema_result = sqlite3_exec(db_handle, SCHEMA_SQL, NULL, NULL, &err);
			if(schema_result == SQLITE_OK)
			{
				if(genSalt()) needs_init = false;
			}
			if(needs_init)
			{
				//Abort and cleanup traces
				close();
				remove(db_path);
				fatal_flag = true;
				ErrorBox("Failed to initialize '%s'", db_path);
				return false;
			}
		}
		else
		{
			//Load salt
			VaultEntry meta_salt(this, "__meta__salt");
			if(!meta_salt.exists() || !meta_salt.ciphertext() ||
				hex2bin(meta_salt.ciphertext(), db_salt, sizeof(db_salt)) != sizeof(db_salt))
			{
				close();
				fatal_flag = true;
				return false;
			}
		}
		return true;
	}
};
