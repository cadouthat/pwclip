/*
Most basic pwclip functionality
by: Connor Douthat
10/1/2015
*/
#include <cstdlib>
#include <stdio.h>

#include <sqlite3/sqlite3.h>

#include "clipHelp.h"
#include "envHelp.h"
#include "PWClipEntry.h"
#include "globals.h"

bool ProcessArguments(int argc, char **argv)
{
	//Optional arguments
	for(int opt = 3; opt < argc; opt++)
	{
		//Last argument might be a database file
		if(opt + 1 == argc && file_exists(argv[opt]))
		{
			strcpy(db_path, argv[opt]);
		}
		else
		{
			//All flags are a dash followed by a single character
			if(strlen(argv[opt]) != 2 || *argv[opt] != '-')
			{
				printf("Invalid option: '%s'\n", argv[opt]);
				return false;
			}
			//Check for valid flags (ignore case)
			switch(tolower(argv[opt][1]))
			{
			case 'k':
				flag_keep = true;
				break;
			default:
				printf("Invalid flag: '%s'\n", argv[opt]);
				return false;
			}
		}
	}
	return true;
}

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

bool EntryAction(char *action, char *pw_name, bool *wipe_clip)
{
	PWClipEntry entry(db, pw_name);
	if(!stricmp(action, "load"))
	{
		//Wipe clipboard after successful load
		*wipe_clip = entry.load();
		return *wipe_clip;
	}
	else if(!stricmp(action, "save"))
	{
		//Wipe clipboard after successful save
		*wipe_clip = entry.save();
		return *wipe_clip;
	}
	else if(!stricmp(action, "replace"))
	{
		//Delete existing value before saving
		if(!entry.delete()) return false;
		//Wipe clipboard after successful save
		*wipe_clip = entry.save();
		return *wipe_clip;
	}
	else if(!stricmp(action, "delete"))
	{
		//Clipboard stores nothing for delete
		*wipe_clip = false;
		return entry.delete();
	}
	else
	{
		printf("Invalid action: '%s'\n", action);
		return false;
	}
}

int main(int argc, char **argv)
{
	//Requires action and password name at minimum
	if(argc <= 2)
	{
		printf("%s\n", USAGE_TEXT);
		return 1;
	}

	//Setup
	if(!ProcessArguments(argc, argv)) return 1;
	if(!OpenDB()) return 1;

	//Perform action
	bool wipe_clip = false;
	EntryAction(argv[1], argv[2], &wipe_clip);

	//Cleanup
	CloseDB();
	if(wipe_clip && !flag_keep)
	{
		printf("Clipboard will be wiped shortly..\n");
		sleep(clip_wipe_delay);
		if(!WipeClipboard())
		{
			printf("Warning: failed to wipe clipboard\n");
			return 2;
		}
	}
	return 0;
}
