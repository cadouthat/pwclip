/*
Acquire user password and start crypto
by: Connor Douthat
10/4/2015
*/
PasswordCipher *CryptoInit(bool allow_blank_pass = false)
{
	char pass[PASSWORD_MAX + 1] = {0};
	//Prompt user for password unless skipped
	if(!flag_skip_pass)
	{
		//Silent password input
		printf("Encryption password: ");
		int r = ScanSilent(pass, sizeof(pass));
		if(r < 0)
		{
			printf("Input cancelled, aborting.\n");
			return NULL;
		}
		if(r == 0 && !allow_blank_pass)
		{
			printf("Password is required, aborting.\n");
			return NULL;
		}
	}
	//Create cipher
	PasswordCipher *cipher = new PasswordCipher(pass);
	//Wipe password
	memset(pass, 0, sizeof(pass));
	return cipher;
}
