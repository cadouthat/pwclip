/*
Global variables
by: Connor Douthat
10/1/2015
*/
#define USAGE_TEXT "\nUsage: pwclip <action> <entry name> [flags] [database file]\n\n  Actions\n    load\n    save\n    remove\n\n  Flags\n    -k    Keep password in clipboard indefinitely\n    -f    Force overwrite if entry exists\n"

//Global configuration
int clip_wipe_delay = 10;

//Arguments
bool flag_keep = false;
bool flag_force = false;
char db_path[256] = {0};

//Runtime
sqlite3 *db = NULL;
