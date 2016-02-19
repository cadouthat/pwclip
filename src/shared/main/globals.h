/*
Global variables across platforms
by: Connor Douthat
11/5/2015
*/
//Config (default values in config.h)
int clip_wipe_delay = 0;
int generate_length = 0;
bool generate_include_symbols = 0;

//Runtime
VaultManager vaults;
void *main_menu = NULL;
std::vector<MenuTree*> dynamic_menus;
std::vector<MenuItemMeta*> global_menu_cache;
char *last_generated_pass = NULL;
