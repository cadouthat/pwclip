/*
Platform defined helper prototypes
by: Connor Douthat
11/5/2015
*/
#define FILE_TYPE_DB 1
#define FILE_TYPE_TXT 2

void ErrorBox(const char *format, ...);
bool ConfirmBox(const char *title, const char *format, ...);
bool BrowseForOutput(int type, char *out, int out_max = 256);

bool TrayBalloon(const char *message);
bool TrayWipeState();

void ClipboardWatchStart();
bool WipeClipboardText();
bool SetClipboardText(const char *text);
char *GetClipboardText();

bool MenuReload();

void ClipboardWatchStart();
char *GetClipboardText();
bool SetClipboardText(const char *text);
