/*
Most basic pwclip functionality
by: Connor Douthat
10/1/2015
*/
#include "sysIncludes.h"
#include "includes.h"

int main(int argc, char **argv)
{
	//Requires action and password name at minimum
	if(argc <= 2)
	{
		printf("%s\n", USAGE_TEXT);
		return 1;
	}

	//Setup
	if(!ProcessArguments(argc, argv)) return 1;
	if(!OpenDB()) return 1;

	//Perform action
	bool wipe_clip = false;
	EntryAction(argv[1], argv[2], &wipe_clip);

	//Cleanup
	CloseDB();
	if(wipe_clip && !flag_keep)
	{
		printf("The clipboard will be wiped shortly..\n");
		SleepSeconds(clip_wipe_delay);
		if(!WipeClipboardText())
		{
			printf("Warning: failed to wipe clipboard\n");
			return 2;
		}
	}
	return 0;
}
