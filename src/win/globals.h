/*
Global variables for tray application
by: Connor Douthat
10/22/2015
*/
//Config
int clip_wipe_delay = 10;
bool config_keep_clip = false;
std::vector<char[256]> db_hist;

//Runtime
DBManager db;
KeyManager crypto_keys;
DWORD clip_sequence;

//GUI
HWND hwnd_main = NULL;
HMENU popup_menu = NULL;
int tray_state = 0;

std::vector<char*> menu_keys;
unsigned int recall_menu_end = 0;
unsigned int encrypt_menu_end = 0;
unsigned int remove_menu_end = 0;
