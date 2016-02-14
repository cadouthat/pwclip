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
bool DumpResource(const char *name, const char *dest)
{
	HRSRC res = FindResource(NULL, name, "BINARY");
	if(res)
	{
		HGLOBAL res_loaded = LoadResource(NULL, res);
		if(res_loaded)
		{
			LPVOID res_data = LockResource(res_loaded);
			if(res_data)
			{
				int res_size = SizeofResource(NULL, res);
				HANDLE destFile = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				if(destFile != INVALID_HANDLE_VALUE)
				{
					DWORD written = 0;
					WriteFile(destFile, res_data, res_size, &written, NULL);
					SetEndOfFile(destFile);
					CloseHandle(destFile);
					return written == res_size;
				}
			}
		}
	}
	return false;
}
