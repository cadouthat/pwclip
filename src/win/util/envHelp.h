/*
User environment utilities (windows implementations)
by: Connor Douthat
10/1/2015
*/
bool LocalUserAppData(const char *app_name, char *path_out)
{
	char path[256] = {0};
	//Get local user app data base path
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
	if(!path[0]) return false;
	//Append desired folder name
	strcat(path, "\\");
	strcat(path, app_name);
	DWORD pathAttribs = GetFileAttributes(path);
	//Check whether directory exists
	if(pathAttribs == INVALID_FILE_ATTRIBUTES || (pathAttribs & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		//Attempt to create directory
		if(!CreateDirectory(path, NULL)) return false;
	}
	//Return result with trailing backslash
	strcat(path, "\\");
	strcpy(path_out, path);
	return true;
}
bool GeneratePassword()
{
	char pass[GEN_PASS_SIZE + 1] = {0};
	if(!RandText(pass, GEN_PASS_SIZE))
	{
		ErrorBox("Failed to generate password");
		return false;
	}
	if(!SetClipboardText(pass))
	{
		ErrorBox("Failed to set clipboard text");
		return false;
	}
	memset(pass, 0, sizeof(pass));
	return true;
}
