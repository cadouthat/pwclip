/*
Load/initialize vaults and manage history
by: Connor Douthat
10/2/2015
*/
class VaultManager
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

public:
	std::vector<Vault*> history;

	VaultManager() {}
	~VaultManager()
	{
		while(history.size())
		{
			delete history.back();
			history.pop_back();
		}
	}
	sqlite3 *topDB()
	{
		if(!history.size()) return NULL;
		return history.front()->db();
	}
	PasswordCipher *topKey()
	{
		if(!history.size()) return NULL;
		return history.front()->key();
	}
	void close()
	{
		for(int i = 0; i < history.size(); i++)
		{
			history[i]->close();
		}
	}
	bool open(const char *path, PasswordCipher *key, bool *fatal)
	{
		//Populate new vault object
		Vault *tmp = new Vault(path);
		tmp->key(key);
		//Attempt opening
		if(!tmp->open())
		{
			//Abort and discard
			*fatal = tmp->fatal();
			delete tmp;
			return false;
		}
		//Push new value into history
		history.insert(history.begin(), tmp);
		//Trim back to limit
		while(history.size() > MAX_DB_HIST)
		{
			delete history.back();
			history.pop_back();
		}
		return true;
	}
	bool open(int i, PasswordCipher *key, bool *fatal)
	{
		//Get vault from history
		Vault *tmp = history[i];
		//If not already open
		if(!tmp->db())
		{
			//Attempt opening
			tmp->key(key);
			if(!tmp->open())
			{
				//Abort
				*fatal = tmp->fatal();
				return false;
			}
		}
		//Move to front
		history.erase(history.begin() + i);
		history.insert(history.begin(), tmp);
		return true;
	}
	bool readHistory()
	{
		//Determine history file location
		char data_path[256] = {0};
		if(historyPath(data_path))
		{
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
					if(FileExists(line)) history.push_back(new Vault(line));
				}
				fclose(f_hist);
			}
		}
		//No history found, use default location
		if(!history.size())
		{
			char def_path[256] = {0};
			if(defaultPath(def_path))
			{
				history.push_back(new Vault(def_path));
			}
		}
		return history.size();
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
			fprintf(f_hist, "%s\n", history[i]->path());
		}
		fclose(f_hist);
		return true;
	}
};