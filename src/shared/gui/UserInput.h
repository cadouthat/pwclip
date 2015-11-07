/*
User input prompt prototypes
by: Connor Douthat
10/28/2015
*/
#define UIF_NAME 1
#define UIF_OLDPASS 2
#define UIF_NEWPASS 4

void *UserInput_new(int flags_in, const char *title_in);
void UserInput_delete(void *ui);
const char *UserInput_name(void *ui);
const char *UserInput_pass(void *ui);
void UserInput_setInfo(void *ui, const char *text);
void UserInput_setError(void *ui, const char *text);
bool UserInput_get(void *ui);
