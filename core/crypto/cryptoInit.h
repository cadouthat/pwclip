/*
Acquire user password and start crypto
by: Connor Douthat
10/4/2015
*/
PasswordCipher *CryptoInit(bool for_encrypt, bool skip_pass = false)
{
	char pass[PASSWORD_MAX + 1] = {0};
	char confirm[PASSWORD_MAX + 1] = {0};
	//Prompt user for password unless skipped
	if(!skip_pass)
	{
		int pass_len;
		do
		{
			//Silent password input
			printf("Encryption password: ");
			pass_len = ScanSilent(pass, sizeof(pass));
			if(pass_len < 0)
			{
				printf("Input cancelled, aborting.\n");
				return NULL;
			}
			if(for_encrypt)
			{
				if(!pass_len)
				{
					printf("Password is required.\n");
					continue;
				}
				//Silent password confirm
				printf("Confirm password: ");
				pass_len = ScanSilent(confirm, sizeof(confirm));
				if(pass_len < 0)
				{
					printf("Input cancelled, aborting.\n");
					return NULL;
				}
				if(strcmp(pass, confirm))
				{
					printf("Passwords did not match. Please try again.\n");
					pass_len = 0;
					continue;
				}
			}
		}while(!pass_len && for_encrypt);
	}
	//Create cipher
	PasswordCipher *cipher = new PasswordCipher(pass);
	//Wipe password
	memset(pass, 0, sizeof(pass));
	memset(confirm, 0, sizeof(confirm));
	return cipher;
}
