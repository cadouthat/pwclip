/*
User environment utilities (OSX implementations)
by: Connor Douthat
11/5/2015
*/
bool LocalUserAppData(const char *app_name, char *path_out)
{
	char path[256] = {0};
	//Get local user app data base path
	//
	//Append desired folder name
	strcat(path, "/");
	strcat(path, app_name);
	//Check whether directory exists
	//
	//Return result with trailing slash
	strcat(path, "/");
	strcpy(path_out, path);
	return true;
}
bool FileExists(const char *path)
{
	//
}
