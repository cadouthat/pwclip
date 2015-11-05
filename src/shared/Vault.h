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
	void key(PasswordCipher *key_in)
	{
		if(db_key) delete db_key;
		db_key = key_in;
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
	bool open()
	{
		//Already open
		if(db_handle) return true;
		//Check for existing file
		bool needs_init = !exists();
		//Key will be needed
		if(!db_key) return false;
		//Open database
		if(sqlite3_open(db_path, &db_handle) != SQLITE_OK)
		{
			fatal_flag = true;
			ErrorBox("Failed to open '%s'", db_path);
			return false;
		}
		PWClipEntry meta(db_handle, "__meta__");
		if(needs_init)
		{
			meta.valuePlain(strdup(meta.name()));
			//First time setup for new files
			char *err;
			int schema_result = sqlite3_exec(db_handle, SCHEMA_SQL, NULL, NULL, &err);
			if(schema_result != SQLITE_OK || !meta.save(db_key))
			{
				//Abort and cleanup traces
				close();
				DeleteFile(db_path);
				fatal_flag = true;
				ErrorBox("Failed to initialize '%s'", db_path);
				return false;
			}
		}
		else
		{
			//Key verification
			if(!meta.decrypt(db_key))
			{
				close();
				return false;
			}
		}
		return true;
	}
};
