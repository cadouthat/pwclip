/*
Interaction for editing preferences
by: Connor Douthat
2/14/2016
*/
void PreferencesDialog()
{
	void *prompt = UserInput_new("Edit Preferences");
	UserInput_addField(prompt, UIF_UINT, "Clipboard wipe timer", &clip_wipe_delay);
	UserInput_addField(prompt, UIF_UINT, "Generate length", &generate_length);
	UserInput_addField(prompt, UIF_TOGGLE, "Generate with symbols", &generate_include_symbols);
	UserInput_addField(prompt, UIF_TOGGLE, "Preview new passwords", &password_preview);
	if(UserInput_get(prompt))
	{
		clip_wipe_delay = UserInput_uintValue(prompt, 0);
		generate_length = UserInput_uintValue(prompt, 1);
		generate_include_symbols = UserInput_boolValue(prompt, 2);
		password_preview = UserInput_boolValue(prompt, 3);
	}
	UserInput_delete(prompt);
}
