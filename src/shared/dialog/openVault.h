/*
Interaction to open a vault
by: Connor Douthat
11/1/2015
*/
void OpenVaultDialog(int hist_index, const char *path = NULL)
{
	//Special cases for opening from history
	Vault *from_history = NULL;
	if(!path)
	{
		//Check bounds
		if(hist_index < 0 || hist_index >= vaults.history.size()) return;
		from_history = vaults.history[hist_index];
		//Already open
		if(from_history->isOpen())
		{
			//If not already active, move to top
			if(hist_index)
			{
				vaults.history.erase(vaults.history.begin() + hist_index);
				vaults.history.insert(vaults.history.begin(), from_history);
				MenuReload();
				char message[512] = {0};
				sprintf(message, "Active vault changed to '%s'.", fileNameInPath(from_history->path()));
				TrayBalloon(message);
			}
			return;
		}
		path = from_history->path();
	}
	//Prepare vault object
	Vault *vault = new Vault(path);
	bool needs_init = !vault->exists();
	//Prepare password prompt (first attempt at opening skips prompt)
	void *prompt = UserInput_new(needs_init ? "Create Vault" : "Open Vault");
	if(needs_init)
	{
		UserInput_addField(prompt, UIF_NEWPASS, "New Vault Password");
	}
	else
	{
		UserInput_addField(prompt, UIF_OLDPASS, "Vault Password");
	}
	bool first_pass = true;
	bool result = false;
	while((first_pass && !needs_init) || UserInput_get(prompt))
	{
		//Set vault key from password
		char *value = UserInput_stringValue(prompt, 0);
		vault->key(new PasswordCipher(value));
		memset(value, 0, strlen(value));
		free(value);
		//Attempt opening
		result = vault->open();
		//Abort if successful or fatal
		if(result || vault->fatal() || needs_init) break;
		//Feedback for successive passes
		if(!first_pass) UserInput_setError(prompt, "Incorrect password, please try again.");
		first_pass = false;
	}
	UserInput_delete(prompt);
	//Finish up
	if(result)
	{
		//Move vault into history
		if(from_history)
		{
			delete from_history;
			vaults.history.erase(vaults.history.begin() + hist_index);
		}
		vaults.push(vault);
		MenuReload();
		TrayBalloon("Vault opened.");
	}
	else
	{
		//Discard vault
		delete vault;
	}
}
