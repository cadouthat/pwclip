/*
Argument parsing for the core module
by: Connor Douthat
10/2/2015
*/
bool ProcessArguments(int argc, char **argv)
{
	//Enforce max entry name length
	if(strlen(argv[2]) > ENTRY_NAME_MAX)
	{
		printf("Entry name must not exceed %d characters\n", ENTRY_NAME_MAX);
		return false;
	}
	//Optional arguments
	for(int opt = 3; opt < argc; opt++)
	{
		//Last argument might be a database file
		if(opt + 1 == argc && *argv[opt] != '-')
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
			case 'f':
				flag_force = true;
				break;
			case 's':
				crypto_keys->skip_encrypt_pass = true;
				break;
			case 'g':
				flag_generate = true;
				break;
			default:
				printf("Invalid flag: '%s'\n", argv[opt]);
				return false;
			}
		}
	}
	return true;
}
