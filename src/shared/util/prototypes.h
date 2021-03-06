/*
Platform defined helper prototypes
by: Connor Douthat
11/5/2015
*/
void ErrorBox(const char *format, ...);
bool ConfirmBox(const char *title, const char *format, ...);
bool BrowseForInput(int type, char *out, int out_max = 256);
bool BrowseForOutput(int type, char *out, int out_max = 256);

bool TrayBalloon(const char *message, float timeout = 2.0f);
bool TrayWipeState();

void ClipboardWatchStart();
bool WipeClipboardText();
bool SetClipboardText(const char *text);
char *GetClipboardText();

class MenuItemMeta;
void *CreateMenuContainer();
void DestroyMenuContainer(void *menu);
void AddMenuItem(void *menu, const char *text, MenuItemMeta *data = NULL, void *subItem = NULL, bool enabled = true, int atIndex = -1);
void CheckMenuItem(void *menu, int index);
bool MenuReload();

void ClipboardWatchStart();
char *GetClipboardText();
bool SetClipboardText(const char *text);

#ifdef __WIN32__
bool CheckStartupValue();
bool SetStartupValue(bool enabled);
#endif
