/*
Acquire user password and start crypto
by: Connor Douthat
10/4/2015
*/
PasswordCipher *CryptoInit()
{
	char pass[PASSWORD_MAX + 1] = {0};
	//Prompt user for password unless skipped
	if(!flag_skip_pass)
	{
		//Silent password input
		printf("Encryption password: ");
		if(ScanSilent(pass, sizeof(pass)) <= 0)
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
