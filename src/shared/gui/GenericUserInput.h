/*
User input prompt prototypes
by: Connor Douthat
10/28/2015
*/
typedef void(*UserInputFieldHandler)(void *ui, int i_field);
typedef void(*UserInputFieldEditHandler)(void *ui, int i_field, void *extra);

#define UIF_TEXT 1
#define UIF_OLDPASS 2
#define UIF_NEWPASS 3
#define UIF_TOGGLE 4
#define UIF_UINT 5
#define UIF_BUTTON 6

void *UserInput_new(const char *title_in);
void UserInput_delete(void *ui);
void UserInput_addField(void *ui, int type, const char *text, const void *valueIn = NULL);
void UserInput_onEdit(void *ui, int index, UserInputFieldEditHandler cb, void *extra = NULL);
void UserInput_setInfo(void *ui, const char *text);
void UserInput_setError(void *ui, const char *text);
void UserInput_setValue(void *ui, int index, const void *valueIn);
bool UserInput_get(void *ui);
char *UserInput_stringValue(void *ui, int index);
bool UserInput_boolValue(void *ui, int index);
int UserInput_uintValue(void *ui, int index);
