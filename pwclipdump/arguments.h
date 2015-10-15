/*
Argument parsing for pwclipdump
by: Connor Douthat
10/15/2015
*/
bool ProcessArguments(int argc, char **argv)
{
	if(argc > 3)
	{
		printf("Too many arguments\n");
		return false;
	}
	if(argc > 2) strcpy(db_path, argv[2]);
	return true;
}
