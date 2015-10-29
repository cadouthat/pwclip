/*
Interaction to re-encrypt an entry
by: Connor Douthat
10/24/2015
*/
void EncryptDialog(PWClipEntry *entry)
{
	//Attempt pre-decryption with known keys
	bool need_pass = !entry->decrypt();
	if(entry->fatal()) return;
	//Prompt for new password, and old password if needed
	char prompt_title[512] = {0};
	snprintf(prompt_title, sizeof(prompt_title), "Recrypt '%s'", entry->name());
	UserInput prompt(UIF_NEWPASS | (need_pass ? UIF_OLDPASS : 0), prompt_title);
	while(prompt.get())
	{
		//Process prompt input
		if(need_pass) crypto_keys.nextDecrypt(new PasswordCipher(prompt.oldpass()));
		crypto_keys.nextEncrypt(new PasswordCipher(prompt.newpass()));
		//Attempt recrypt (until fatal or successful)
		if(!entry->reEncrypt())
		{
			if(entry->fatal()) break;
			//Feedback for next attempt
			prompt.setError("Decryption failed, please try again.");
		}
		else break;
	}
}
