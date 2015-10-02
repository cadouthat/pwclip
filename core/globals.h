/*
Global variables
by: Connor Douthat
10/1/2015
*/
#define USAGE_TEXT "Usage: pwclip <action> <password name> [flags] [database file]\n  Actions\n    load\n    save\n    replace\n    delete\n  Flags\n    -k        Keep password in clipboard indefinitely\n"

//Global configuration
int clip_wipe_delay = 10;

//Arguments
bool flag_keep = false;
char db_path[256] = {0};

//Runtime
sqlite3 *db = NULL;
