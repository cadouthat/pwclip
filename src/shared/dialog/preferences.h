/*
Interaction for editing preferences
by: Connor Douthat
2/14/2016
*/
void PreferencesDialog()
{
	void *prompt = UserInput_new("pwclip Preferences");
	UserInput_addField(prompt, UIF_UINT, "Clipboard wipe timer (seconds)", &clip_wipe_delay);
	UserInput_addField(prompt, UIF_UINT, "Generated password length (characters)", &generate_length);
	UserInput_addField(prompt, UIF_TOGGLE, "Include symbols in generated passwords", &generate_include_symbols);
#ifdef __WIN32__
	bool startup = CheckStartupValue();
	UserInput_addField(prompt, UIF_TOGGLE, "Start pwclip with Windows", &startup);
#endif
	if(UserInput_get(prompt))
	{
		clip_wipe_delay = UserInput_uintValue(prompt, 0);
		generate_length = UserInput_uintValue(prompt, 1);
		generate_include_symbols = UserInput_boolValue(prompt, 2);
#ifdef __WIN32__
		if(startup != UserInput_boolValue(prompt, 3))
		{
			if(!SetStartupValue(!startup))
			{
				ErrorBox("Failed to %s starting with Windows", startup ? "disable" : "enable");
			}
		}
#endif
	}
	UserInput_delete(prompt);
}
