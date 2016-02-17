/*
Class to manage open vaults and history
by: Connor Douthat
10/2/2015
*/
class VaultManager
{
	bool historyPath(char *out)
	{
		if(!appDataPath[0]) return false;
		strcpy(out, appDataPath);
		strcat(out, "db_list.pref");
		return true;
	}

public:
	char appDataPath[256];
	std::vector<Vault*> history;

	VaultManager()
	{
		appDataPath[0] = 0;
	}
	~VaultManager()
	{
		while(history.size())
		{
			delete history.back();
			history.pop_back();
		}
	}
	Vault *top()
	{
		if(!history.size()) return NULL;
		return history.front();
	}
	bool topOpen()
	{
		if(!top()) return false;
		return top()->isOpen();
	}
	void close(int i)
	{
		if(history[i]->isOpen())
		{
			int first_closed = i;
			while(first_closed < history.size() && history[first_closed]->isOpen()) first_closed++;
			history[i]->close();
			//Move below open vaults
			if(first_closed > i + 1)
			{
				history.insert(history.begin() + first_closed, history[i]);
				history.erase(history.begin() + i);
			}
		}
	}
	int close()
	{
		int num_closed = 0;
		for(int i = 0; i < history.size(); i++)
		{
			if(history[i]->isOpen())
			{
				history[i]->close();
				num_closed++;
			}
		}
		return num_closed;
	}
	void push(Vault *vault)
	{
		//Remove duplicates
		for(int i = 0; i < history.size();)
		{
			if(!strcmp(history[i]->path(), vault->path()))
			{
				delete history[i];
				history.erase(history.begin() + i);
			}
			else i++;
		}
		//Push new value into history
		history.insert(history.begin(), vault);
		//Trim back to limit
		while(history.size() > MAX_DB_HIST)
		{
			delete history.back();
			history.pop_back();
		}
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
				return true;
			}
		}
		return false;
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
