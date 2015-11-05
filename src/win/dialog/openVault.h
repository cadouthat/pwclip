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
				TrayBalloon("Active vault changed.");
			}
			return;
		}
		path = from_history->path();
	}
	//Prepare vault object
	Vault *vault = new Vault(path);
	bool needs_init = !vault->exists();
	//Prepare password prompt (first attempt at opening skips prompt)
	UserInput prompt(needs_init ? UIF_NEWPASS : UIF_OLDPASS, needs_init ? "Create Vault" : "Open Vault");
	bool first_pass = true;
	bool result;
	while((first_pass && !needs_init) || prompt.get())
	{
		//Set vault key from password
		vault->key(new PasswordCipher(needs_init ? prompt.newpass() : prompt.oldpass()));
		//Attempt opening
		result = vault->open();
		//Abort if successful or fatal
		if(result || vault->fatal() || needs_init) break;
		//Feedback for successive passes
		if(!first_pass) prompt.setError("Incorrect password, please try again.");
		first_pass = false;
	}
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
