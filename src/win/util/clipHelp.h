/*
Clipboard utilities (windows implementations)
by: Connor Douthat
10/1/2015
*/
void ClipboardWatchStart()
{
	clip_sequence = GetClipboardSequenceNumber();
}
bool WipeClipboardText()
{
	if(!OpenClipboard(NULL)) return false;
	bool foundAndErased = false;
	HANDLE hClip = GetClipboardData(CF_TEXT);
	if(hClip)
	{
		LPVOID clipData = GlobalLock(hClip);
		if(clipData)
		{
			SIZE_T clipSize = GlobalSize(hClip);
			if(clipSize)
			{
				//Write in place, hopefully destroying sensitive data
				memset(clipData, 0, clipSize);
				foundAndErased = true;
			}
			GlobalUnlock(hClip);
		}
		EmptyClipboard();
	}
	CloseClipboard();
	return foundAndErased;
}
bool SetClipboardText(const char *text)
{
	if(!OpenClipboard(NULL)) return false;
	EmptyClipboard();
	//Allocate new clipboard data
	unsigned int text_len = strlen(text);
	HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE, text_len + 1);
	if(!hClip)
	{
		CloseClipboard();
		return false;
	}
	//Lock for writing
	LPVOID clipData = GlobalLock(hClip);
	if(!clipData)
	{
		GlobalFree(hClip);
		CloseClipboard();
		return false;
	}
	//Copy text and finish up
	strcpy((char*)clipData, text);
	GlobalUnlock(hClip);
	hClip = SetClipboardData(CF_TEXT, hClip);
	CloseClipboard();
	return (hClip != NULL);
}
char *GetClipboardText()
{
	if(!OpenClipboard(NULL)) return NULL;
	char *clipCopy = NULL;
	HANDLE hClip = GetClipboardData(CF_TEXT);
	if(hClip)
	{
		LPVOID clipData = GlobalLock(hClip);
		if(clipData)
		{
			SIZE_T clipSize = GlobalSize(hClip);
			if(clipSize)
			{
				//Copy to new string
				clipCopy = (char*)malloc(clipSize + 1);
				memcpy(clipCopy, clipData, clipSize);
				clipCopy[clipSize] = 0;
			}
			GlobalUnlock(hClip);
		}
	}
	CloseClipboard();
	return clipCopy;
}
