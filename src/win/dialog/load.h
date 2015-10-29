/*
Interaction to load an entry
by: Connor Douthat
10/24/2015
*/
void LoadDialog(PWClipEntry *entry)
{
	char prompt_title[512] = {0};
	snprintf(prompt_title, sizeof(prompt_title), "Load '%s'", entry->name());
	UserInput prompt(UIF_OLDPASS, prompt_title);
	//Attempt to load entry with known keys
	bool first_attempt = true;
	bool result = false;
	while(!(result = entry->load()) && !entry->fatal())
	{
		//Give user feedback on failed attempts
		if(!first_attempt)
		{
			prompt.setError("Decryption failed, please try again.");
		}
		//Prompt for new password (until user cancels)
		if(prompt.get())
		{
			//Add pending key
			crypto_keys.nextDecrypt(new PasswordCipher(prompt.oldpass()));
			first_attempt = false;
		}
		else break;
	}
	//Update tray on success
	if(result)
	{
		TraySuccessState();
	}
}
