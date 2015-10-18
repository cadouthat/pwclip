/*
Global variables
by: Connor Douthat
10/15/2015
*/
#define USAGE_TEXT "\nUsage: pwclipdump <output file> [database file]\n\n"

//Arguments
char db_path[256] = {0};

//Runtime
sqlite3 *db = NULL;
