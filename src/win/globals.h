/*
Global variables for windows
by: Connor Douthat
10/22/2015
*/
//Runtime
DWORD clip_sequence;

//GUI
HWND hwnd_main = NULL;
HMENU popup_menu = NULL;
int tray_state = 0;
bool clip_wipe_pending = false;

std::vector<char*> menu_keys;
unsigned int recall_menu_end = 0;
unsigned int remove_menu_end = 0;
unsigned int change_menu_end = 0;
