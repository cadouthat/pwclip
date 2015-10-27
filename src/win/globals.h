/*
Global variables for tray application
by: Connor Douthat
10/22/2015
*/
char db_path[256] = {0};
sqlite3 *db = NULL;

HWND hwndMain = NULL;
HMENU popupMenu = NULL;

std::vector<char*> menu_keys;
unsigned int recall_menu_end = 0;
unsigned int encrypt_menu_end = 0;
unsigned int remove_menu_end = 0;
