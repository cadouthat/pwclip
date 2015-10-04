/*
Database management
by: Connor Douthat
10/2/2015
*/
void CloseDB()
{
	if(db)
	{
		sqlite3_close(db);
		db = NULL;
	}
}
bool OpenDB()
{
	//Close existing db if any
	CloseDB();
	//Default data location
	if(!db_path[0])
	{
		if(!LocalUserAppData("pwclip", db_path))
		{
			printf("No suitable location for application data\n");
			return false;
		}
		strcat(db_path, "vault.db");
	}
	//Check for existing file
	bool needs_init = !FileExists(db_path);
	//Open database
	if(sqlite3_open(db_path, &db) != SQLITE_OK)
	{
		printf("Failed to open '%s'\n", db_path);
		return false;
	}
	//First time setup for new files
	if(needs_init)
	{
		char *err;
		if(SQLITE_OK != sqlite3_exec(db, "CREATE TABLE `entries` (`key` VARCHAR(50) PRIMARY KEY, `value` TEXT NOT NULL)", NULL, NULL, &err))
		{
			//Abort
			printf("Failed to initialize '%s'\n", db_path);
			sqlite3_close(db);
			db = NULL;
			DeleteFile(db_path);
		}
	}
	return true;
}
