/*
Global variables for tray application
by: Connor Douthat
10/22/2015
*/
//Config
int clip_wipe_delay = 10;
bool config_keep_clip = false;
char db_path[256] = {0};

//Runtime
sqlite3 *db = NULL;
KeyManager crypto_keys;
long wipe_clip_timer = 0;

//GUI
HWND hwndMain = NULL;
HMENU popupMenu = NULL;

std::vector<char*> menu_keys;
unsigned int recall_menu_end = 0;
unsigned int encrypt_menu_end = 0;
unsigned int remove_menu_end = 0;
