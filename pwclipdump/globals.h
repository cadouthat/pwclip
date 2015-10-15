/*
Global variables
by: Connor Douthat
10/15/2015
*/
#define USAGE_TEXT "\nUsage: pwclipdump <output file> [database file]\n\n"

//Arguments
bool flag_skip_pass = false;
char db_path[256] = {0};

//Runtime
sqlite3 *db = NULL;
