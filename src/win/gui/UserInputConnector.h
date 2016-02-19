/*
Connectors for generic UserInput functions
by: Connor Douthat
2/18/2016
*/
void *UserInput_new(const char *title_in)
{
	return new UserInput(title_in);
}
void UserInput_delete(void *ui)
{
	delete (UserInput*)ui;
}
void UserInput_addField(void *ui, int type, const char *text, const void *valueIn)
{
	((UserInput*)ui)->addField(type, text, valueIn);
}
void UserInput_setInfo(void *ui, const char *text)
{
	((UserInput*)ui)->setInfo(text);
}
void UserInput_setError(void *ui, const char *text)
{
	((UserInput*)ui)->setError(text);
}
void UserInput_setValue(void *ui, int i_field, const void *valueIn)
{
	((UserInput*)ui)->setValue(i_field, valueIn);
}
bool UserInput_get(void *ui)
{
	return ((UserInput*)ui)->get();
}
char *UserInput_stringValue(void *ui, int index)
{
	UserInputField *f = ((UserInput*)ui)->getField(index);
	return f->stringValue();
}
bool UserInput_boolValue(void *ui, int index)
{
	UserInputField *f = ((UserInput*)ui)->getField(index);
	return f->boolValue();
}
int UserInput_uintValue(void *ui, int index)
{
	UserInputField *f = ((UserInput*)ui)->getField(index);
	return f->uintValue();
}
