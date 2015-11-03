/*
Interaction to open a vault
by: Connor Douthat
11/1/2015
*/
void OpenVaultDialog(int hist_index, const char *path = NULL)
{
	//Prepare password prompt (first attempt skips prompt)
	UserInput prompt(UIF_OLDPASS, "Enter Master Password");
	bool first_pass = true;
	bool fatal, result;
	do
	{
		//Generate key for successive passes
		PasswordCipher *key = first_pass ? NULL : (new PasswordCipher(prompt.oldpass()));
		//Attempt opening (key is consumed)
		if(path) result = db.open(path, key, &fatal);
		else result = db.open(hist_index, key, &fatal);
		//Abort if successful or fatal
		if(result || fatal) break;
		//Feedback for successive passes
		if(!first_pass) prompt.setError("Incorrect password, please try again.");
		first_pass = false;
	} while(prompt.get());
	//Finish up if successful
	if(result)
	{
		TrayBalloon("Active vault changed.");
		MenuReload();
	}
}
