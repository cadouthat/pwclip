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

std::vector<MenuItemMeta*> menu_map;
