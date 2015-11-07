/*
Platform defined helper prototypes
by: Connor Douthat
11/5/2015
*/
void ErrorBox(const char *format, ...);
bool ConfirmBox(const char *title, const char *format, ...);

bool BrowseForOutput(char *out, int out_max = 256);
bool BrowseForInput(char *out, int out_max = 256);

bool LocalUserAppData(const char *app_name, char *path_out);
bool FileExists(const char *path);

void ClipboardWatchStart();
bool WipeClipboardText();
bool SetClipboardText(const char *text);
char *GetClipboardText();

bool TrayBalloon(const char *message);
bool TrayWipeState();

bool MenuReload();
