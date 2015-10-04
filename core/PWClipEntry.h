/*
PWClipEntry class which acts on a password vault entry
by: Connor Douthat
10/1/2015
*/
class PWClipEntry
{
	sqlite3 *db;
	const char *key;
	char *value;

public:
	PWClipEntry(sqlite3 *db_in, const char *key_in)
	{
		db = db_in;
		key = key_in;
		//Find existing value (if any)
		value = NULL;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "SELECT value FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC))
			{
				if(sqlite3_step(stmt) == SQLITE_ROW)
				{
					//Keep internal copy of value
					value = strdup((const char*)sqlite3_column_text(stmt, 0));
				}
			}
			sqlite3_finalize(stmt);
		}
	}
	~PWClipEntry()
	{
		if(value) free(value);
	}
	bool load()
	{
		if(!value)
		{
			printf("Entry not found\n");
			return false;
		}
		if(!SetClipboardText(value))
		{
			printf("Failed to set clipboard text\n");
			return false;
		}
		printf("Entry successfully loaded to clipboard\n");
		return true;
	}
	bool save()
	{
		if(value)
		{
			printf("Entry already exists\n");
			return false;
		}
		if(!(value = GetClipboardText()))
		{
			printf("Failed to get clipboard text\n");
			return false;
		}
		//Attempt insert
		bool insert_ok = false;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(db, "INSERT INTO `entries` (`key`, `value`) VALUES (?, ?)", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC))
			{
				insert_ok = (sqlite3_step(stmt) == SQLITE_DONE);
			}
			sqlite3_finalize(stmt);
		}
		if(!insert_ok)
		{
			printf("Unknown error creating entry\n");
			free(value);
			value = NULL;
			return false;
		}
		printf("Entry successfully saved\n");
		return true;
	}
	bool remove()
	{
		if(!value)
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
		if(!delete_ok)
		{
			printf("Unknown error removing entry\n");
			return false;
		}
		free(value);
		value = NULL;
		printf("Entry successfully removed\n");
		return true;
	}
};
