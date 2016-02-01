/*
Cross-platform environment helpers
by: Connor Douthat
1/31/2016
*/

bool FileExists(const char *path)
{
	return (0 == access(path, F_OK));
}
