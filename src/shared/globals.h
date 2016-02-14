/*
Global variables across platforms
by: Connor Douthat
11/5/2015
*/
//Config (default values in config.h)
int clip_wipe_delay = 0;
int generate_length = 0;
bool generate_include_symbols = 0;
bool generate_echo = 0;

//Runtime
char config_path[256] = {0};
VaultManager vaults;
