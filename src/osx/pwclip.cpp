/*
OSX entry point
by: Connor Douthat
11/5/2015
*/
#include "includes.h"

int main(int argc, char **argv)
{
	//Settings and global init
	char config_path[256] = {0};
	if(LocalUserAppData(APPDATA_NAME, config_path))
	{
		strcat(config_path, "pwclip.ini");
		LoadConfig(config_path);
	}
	vaults.readHistory();

	//GUI init
	MenuInit();

	//Main message loop
	//

	//GUI cleanup
	MenuCleanup();

	//GLobal cleanup
	vaults.writeHistory();
	vaults.close();
	return 0;
}
