/*
GUI related helper functions (OSX implementations)
by: Connor Douthat
11/5/2015
*/
void ErrorBox(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
	//
	va_end(args);
}
bool ConfirmBox(const char *title, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsnprintf(message, sizeof(message), format, args);
	//
	va_end(args);
	return result;
}
bool BrowseForOutput(char *out, int out_max)
{
	//
}
bool BrowseForInput(char *out, int out_max)
{
	//
}
