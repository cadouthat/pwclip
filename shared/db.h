/*
Database management
by: Connor Douthat
10/2/2015
*/
sqlite3 *OpenDB(char *db_path)
{
	//Default data location
	if(!db_path[0])
	{
		if(!LocalUserAppData("pwclip", db_path))
		{
			fprintf(stderr, "No suitable location for application data\n");
			return NULL;
		}
		strcat(db_path, "vault.db");
	}
	//Check for existing file
	bool needs_init = !FileExists(db_path);
	//Open database
	sqlite3 *db = NULL;
	if(sqlite3_open(db_path, &db) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to open '%s'\n", db_path);
		return NULL;
	}
	//First time setup for new files
	if(needs_init)
	{
		char *err;
		if(SQLITE_OK != sqlite3_exec(db, "CREATE TABLE `entries` (`key` VARCHAR(50) PRIMARY KEY, `value` TEXT NOT NULL, `iv` TEXT NOT NULL)", NULL, NULL, &err))
		{
			//Abort
			fprintf(stderr, "Failed to initialize '%s'\n", db_path);
			sqlite3_close(db);
			db = NULL;
			DeleteFile(db_path);
		}
	}
	return db;
}
