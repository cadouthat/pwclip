/*
Load/initialize databases and manage history
by: Connor Douthat
10/2/2015
*/
class DBManager
{
	bool defaultPath(char *out)
	{
		if(!LocalUserAppData(APPDATA_NAME, out))
		{
			ErrorBox("No suitable location for application data");
			return false;
		}
		strcat(out, "vault.db");
		return true;
	}
	bool historyPath(char *out)
	{
		if(!LocalUserAppData(APPDATA_NAME, out))
		{
			ErrorBox("No suitable location for application data");
			return false;
		}
		strcat(out, "db_list.pref");
		return true;
	}
	void clearHistory(int keep = 0)
	{
		while(history.size() > keep)
		{
			free(history.back());
			history.pop_back();
		}
	}
	void insertHistory(const char *path)
	{
		//Push new value into history
		history.insert(history.begin(), strdup(path));
		//Trim back to limit
		clearHistory(MAX_DB_HIST);
	}
	bool open(char *db_path)
	{
		//Check for existing file
		bool needs_init = !FileExists(db_path);
		//Open database
		sqlite3 *db = NULL;
		if(sqlite3_open(db_path, &db) != SQLITE_OK)
		{
			ErrorBox("Failed to open '%s'", db_path);
			return false;
		}
		//First time setup for new files
		if(needs_init)
		{
			char *err;
			if(SQLITE_OK != sqlite3_exec(db, "CREATE TABLE `entries` (`key` VARCHAR(50) PRIMARY KEY, `value` TEXT NOT NULL, `iv` TEXT NOT NULL)", NULL, NULL, &err))
			{
				//Abort
				ErrorBox("Failed to initialize '%s'", db_path);
				sqlite3_close(db);
				db = NULL;
				DeleteFile(db_path);
			}
		}
		close();
		loaded = db;
		return true;
	}

public:
	sqlite3 *loaded;
	std::vector<char*> history;

	DBManager()
	{
		loaded = NULL;
	}
	~DBManager()
	{
		close();
		clearHistory();
	}
	void close()
	{
		if(loaded)
		{
			sqlite3_close(loaded);
			loaded = NULL;
		}
	}
	void resetHistory()
	{
		//Forget everything but current path
		clearHistory(1);
	}
	bool load()
	{
		if(loaded) return true;
		//Determine history file location
		char data_path[256] = {0};
		if(!historyPath(data_path)) return false;
		//Parse history file if present
		FILE *f_hist = fopen(data_path, "r");
		if(f_hist)
		{
			char line[256] = {0};
			while(fgets(line, sizeof(line), f_hist) && history.size() < MAX_DB_HIST)
			{
				int len = strlen(line);
				if(line[len - 1] == '\n') line[--len] = 0;
				if(line[len - 1] == '\r') line[--len] = 0;
				if(len <= 0) continue;
				//Skip missing files
				if(FileExists(line)) history.push_back(strdup(line));
			}
			fclose(f_hist);
		}
		//Attempt loading in order
		while(history.size() && !open(history.front()))
		{
			free(history.front());
			history.erase(history.begin());
		}
		//Fallback to default if needed
		if(!loaded)
		{
			char def_path[256] = {0};
			if(defaultPath(def_path) && open(def_path))
			{
				insertHistory(def_path);
			}
		}
		return loaded != NULL;
	}
	bool push(char *path)
	{
		if(open(path))
		{
			insertHistory(path);
			return true;
		}
		else return false;
	}
	bool push(int i)
	{
		//Cut out existing entry
		char *tmp = history[i];
		history.erase(history.begin() + i);
		//Open and push to front
		bool result = push(tmp);
		free(tmp);
		return result;
	}
	bool writeHistory()
	{
		char data_path[256] = {0};
		if(!historyPath(data_path)) return false;
		//Overwrite history file
		FILE *f_hist = fopen(data_path, "w");
		if(!f_hist) return false;
		for(int i = 0; i < history.size(); i++)
		{
			fprintf(f_hist, "%s\n", history[i]);
		}
		fclose(f_hist);
		return true;
	}
};
