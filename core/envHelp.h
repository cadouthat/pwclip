/*
User environment utilities
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
bool FileExists(const char *path)
{
	DWORD attribs = GetFileAttributes(path);
	if(attribs == INVALID_FILE_ATTRIBUTES) return false;
	if(attribs & FILE_ATTRIBUTE_DIRECTORY) return false;
	return true;
}
bool RandText(char *out, int len)
{
	const char *char_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	int char_set_size = strlen(char_set);
	unsigned char *u_out = (unsigned char*)out;
	if(!RAND_bytes(u_out, len)) return false;
	for(int i = 0; i < len; i++)
	{
		out[i] = char_set[u_out[i] % char_set_size];
	}
	return true;
}
int ScanSilent(char *buf, int buf_size)
{
	int len = 0;
	int ch = 0;
	do
	{
		if((ch = getch()) >= 0)
		{
			switch(ch)
			{
				//Multi-byte (ignored)
				case 0:
				case 224:
					//Skip extra byte
					getch();
					ch = 1;
					break;

				//Cancel input
				case 3: //ETX
				case 27: //ESC
					buf[0] = 0;
					return -1;

				//End of input
				case '\r':
				case '\n':
				case EOF:
					ch = 0;
					break;

				//Backspace removes last char
				case 8: //BS
					if(len > 0)
					{
						len--;
						buf[len] = 0;
					}
					break;

				//Append regular character
				default:
					buf[len] = ch;
					len++;
					break;
			}
		}
	}while(ch > 0 && len < buf_size - 1);
	printf("\n");
	//Terminate string
	buf[len] = 0;
	return len;
}
