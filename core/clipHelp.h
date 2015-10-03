/*
Clipboard utilities
by: Connor Douthat
10/1/2015
*/
bool WipeClipboard()
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
