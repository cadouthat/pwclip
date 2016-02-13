/*
GUI related helper functions (windows implementations)
by: Connor Douthat
10/28/2015
*/
void ErrorBox(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
	MessageBox(hwnd_main, message, ERROR_TITLE, 0);
	va_end(args);
}
bool ConfirmBox(const char *title, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
	bool result = (IDYES == MessageBox(hwnd_main, message, title, MB_YESNO));
	va_end(args);
	return result;
}
void GetFileTypeStrings(int type, OPENFILENAME *ofn)
{
	switch(type)
	{
	case FILE_TYPE_DB:
		ofn->lpstrFilter = "Database Files\0*.db\0All Files\0*.*\0\0";
		ofn->lpstrDefExt = "db";
		break;
	case FILE_TYPE_TXT:
		ofn->lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0\0";
		ofn->lpstrDefExt = "txt";
		break;
	default:
		ofn->lpstrFilter = "All Files\0*.*\0\0";
		ofn->lpstrDefExt = NULL;
	}
}
bool BrowseForOutput(int type, char *out, int out_max)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd_main;
	GetFileTypeStrings(type, &ofn);
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrFile = out;
	ofn.nMaxFile = out_max;
	return GetSaveFileName(&ofn);
}
bool BrowseForInput(int type, char *out, int out_max = 256)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd_main;
	GetFileTypeStrings(type, &ofn);
	ofn.lpstrFile = out;
	ofn.nMaxFile = out_max;
	return GetOpenFileName(&ofn);
}
