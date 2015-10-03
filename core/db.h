/*
Database management
by: Connor Douthat
10/2/2015
*/
bool OpenDB()
{
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
	//Open database
	sqlite3 *db = NULL;
	if(sqlite3_open(db_path, &db) != SQLITE_OK)
	{
		printf("Failed to open '%s'\n", db_path);
		return false;
	}
	return true;
}
void CloseDB()
{
	if(db)
	{
		sqlite3_close(db);
		db = NULL;
	}
}
