/*
Cross-platform environment helpers
by: Connor Douthat
1/31/2016
*/
bool FileExists(const char *path)
{
	return (0 == access(path, F_OK));
}
const char *fileNameInPath(const char *path)
{
	if(!path[0]) return path;
	int i = strlen(path) - 1;
	while(i > 0 && path[i - 1] != '/' && path[i - 1] != '\\') i--;
	return path + i;
}
