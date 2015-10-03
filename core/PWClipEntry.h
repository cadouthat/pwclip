/*
PWClipEntry class which acts on a password vault entry
by: Connor Douthat
10/1/2015
*/
class PWClipEntry
{
	sqlite3 *db;
	char *key;

public:
	PWClipEntry(sqlite3 *db_in, char *key_in)
	{
		db = db_in;
		key = key_in;
	}
	bool load()
	{
		//
		return false;
	}
	bool save()
	{
		//
		return false;
	}
	bool remove()
	{
		//
		return false;
	}
};
