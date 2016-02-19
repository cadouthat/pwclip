/*
Interaction to create an entry
by: Connor Douthat
10/24/2015
*/
void PasswordGenerationHandler(void *ui, int i_field);
void BuildEntryForm(void *prompt);

void CreateEntryDialog(VaultEntry *modifyEntry = NULL)
{
	static char *last_base = NULL;

	//Make sure vault is open
	if(!vaults.topOpen()) return;

	//Prompt for name
	void *prompt = UserInput_new(modifyEntry ? "Edit Entry" : "Save New Entry");
	BuildEntryForm(prompt);
	if(modifyEntry)
	{
		UserInput_setValue(prompt, 0, modifyEntry->name());
		modifyEntry->decrypt();
		UserInput_setValue(prompt, 1, modifyEntry->valuePlain());
	}
	else if(last_base)
	{
		UserInput_setValue(prompt, 0, last_base);
	}
	while(UserInput_get(prompt))
	{
		//Start building new entry
		char *name = UserInput_stringValue(prompt, 0);
		VaultEntry entry(vaults.top(), name);
		free(name);
		//For creating and renaming, check for conflicts
		if(!modifyEntry || strcmp(entry.name(), modifyEntry->name()))
		{
			if(entry.exists())
			{
				UserInput_setError(prompt, "Entry already exists.");
				continue;
			}
		}
		char *pass = UserInput_stringValue(prompt, 1);
		//For updating, remove old entry
		if(modifyEntry) modifyEntry->remove();
		//Save new entry
		entry.valuePlain(pass);
		if(entry.save())
		{
			MenuReload();
			//Load generated entries to clipboard for reference
			bool loadedToClip = false;
			if(last_generated_pass && !strcmp(pass, last_generated_pass))
			{
				//Move plaintext to clipboard
				if(SetClipboardText(last_generated_pass))
				{
					//Update tray on success
					ClipboardWatchStart();
					TrayWipeState();
					loadedToClip = true;
					TrayBalloon("Entry saved and loaded to clipboard.");
				}
			}
			if(!loadedToClip) TrayBalloon("Entry saved.");
			//Keep track of last submenu created
			if(!modifyEntry)
			{
				if(last_base) free(last_base);
				last_base = strdup(entry.name());
				//Truncate after last separator
				for(int i = strlen(last_base) - 1; i >= 0 && last_base[i] != ':'; i--)
				{
					last_base[i] = 0;
				}
			}
		}
		memset(pass, 0, strlen(pass));
		free(pass);
		break;
	}

	//Cleanup
	UserInput_delete(prompt);
	if(last_generated_pass)
	{
		memset(last_generated_pass, 0, strlen(last_generated_pass));
		free(last_generated_pass);
		last_generated_pass = NULL;
	}
}

void PasswordGenerationHandler(void *ui, int i_field)
{
	//Generate random text
	char *pass = (char*)malloc(generate_length + 1);
	if(!RandText(pass, generate_length, generate_include_symbols))
	{
		ErrorBox("Failed to generate password");
		free(pass);
		return;
	}
	pass[generate_length] = 0;
	//Show preview info
	char preview_text[24] = {0};
	char info_text[128] = {0};
	stringPreview(pass, preview_text, sizeof(preview_text));
	sprintf(info_text, "Generated value '%s'", preview_text);
	UserInput_setInfo(ui, info_text);
	memset(preview_text, 0, sizeof(preview_text));
	memset(info_text, 0, sizeof(info_text));
	//Set password values
	UserInput_setValue(ui, 1, pass);
	//Cache generated password
	if(last_generated_pass) free(last_generated_pass);
	last_generated_pass = pass;
}
void BuildEntryForm(void *prompt)
{
	UserInput_addField(prompt, UIF_TEXT, "Entry Name (e.g. work:email)");
	UserInput_addField(prompt, UIF_NEWPASS, "Entry Password");
	UserInput_addField(prompt, UIF_BUTTON, "Generate Password", (const void*)PasswordGenerationHandler);
}

